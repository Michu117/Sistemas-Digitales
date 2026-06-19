#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo miServo;

// Pines
const int pinPIR = 2;
const int pinBoton = 3;
const int pinServo = 9;
const int pinRelay = 10;
const int ledVerde = 5;
const int ledRojo = 6;
const int ledAmarillo = 7;

// ====== ESTRUCTURAS ======
typedef struct
{
    bool piezaDetectada;
    bool paro;
} DatosSensor;

enum Accion
{
    ESPERA,
    PROCESAR,
    PARO
};

// ====== COLAS ======
QueueHandle_t colaSensores;
QueueHandle_t colaAcciones;

// ====== TAREAS ======
void TaskAdquisicion(void *pvParameters);
void TaskProcesamiento(void *pvParameters);
void TaskActuacion(void *pvParameters);

void setup()
{
    lcd.init();
    lcd.backlight();

    pinMode(pinPIR, INPUT);
    pinMode(pinBoton, INPUT_PULLUP);

    pinMode(pinRelay, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    pinMode(ledRojo, OUTPUT);
    pinMode(ledAmarillo, OUTPUT);

    digitalWrite(pinRelay, LOW);

    miServo.attach(pinServo);
    miServo.write(0);

    colaSensores = xQueueCreate(1, sizeof(DatosSensor));
    colaAcciones = xQueueCreate(1, sizeof(int));

    xTaskCreate(TaskAdquisicion, "Sensor", 128, NULL, 3, NULL);
    xTaskCreate(TaskProcesamiento, "Proceso", 128, NULL, 2, NULL);
    xTaskCreate(TaskActuacion, "Actua", 256, NULL, 1, NULL);
}

void loop()
{
}

//=================================================
void TaskAdquisicion(void *pvParameters)
{
    DatosSensor datos;

    while (1)
    {
        // Si tu sensor funciona al revés, cambia por:
        // datos.piezaDetectada = !digitalRead(pinPIR);
        datos.piezaDetectada = digitalRead(pinPIR);

        datos.paro = (digitalRead(pinBoton) == LOW);

        xQueueOverwrite(colaSensores, &datos);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

//=================================================
void TaskProcesamiento(void *pvParameters)
{
    DatosSensor datos;
    int accion;

    static bool paroActivado = false;
    static bool botonAnterior = false;

    while (1)
    {
        if (xQueueReceive(colaSensores, &datos, portMAX_DELAY) == pdTRUE)
        {
            // Detectar pulsación única
            if (datos.paro && !botonAnterior)
            {
                paroActivado = !paroActivado;
            }

            botonAnterior = datos.paro;

            if (paroActivado)
            {
                accion = PARO;
            }
            else if (datos.piezaDetectada)
            {
                accion = PROCESAR;
            }
            else
            {
                accion = ESPERA;
            }

            xQueueOverwrite(colaAcciones, &accion);
        }
    }
}

//=================================================
void TaskActuacion(void *pvParameters)
{
    int accion;
    int accionAnterior = -1;

    while (1)
    {
        if (xQueueReceive(colaAcciones, &accion, portMAX_DELAY) == pdTRUE)
        {
            if (accion != accionAnterior)
            {
                accionAnterior = accion;

                switch (accion)
                {
                    case PARO:

                        digitalWrite(pinRelay, LOW);
                        miServo.write(0);

                        digitalWrite(ledVerde, LOW);
                        digitalWrite(ledAmarillo, LOW);
                        digitalWrite(ledRojo, HIGH);

                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("!! PARO !!");
                        lcd.setCursor(0, 1);
                        lcd.print("Sistema OFF");

                        break;

                    case PROCESAR:

                        digitalWrite(ledVerde, LOW);
                        digitalWrite(ledRojo, LOW);
                        digitalWrite(ledAmarillo, HIGH);

                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("Pieza Detect.");
                        lcd.setCursor(0, 1);
                        lcd.print("Procesando");

                        digitalWrite(pinRelay, HIGH);

                        vTaskDelay(pdMS_TO_TICKS(500));

                        miServo.write(90);

                        vTaskDelay(pdMS_TO_TICKS(2000));

                        miServo.write(0);

                        vTaskDelay(pdMS_TO_TICKS(1000));

                        digitalWrite(pinRelay, LOW);

                        accionAnterior = -1; // permite volver a mostrar ESPERA

                        break;

                    case ESPERA:

                        digitalWrite(pinRelay, LOW);

                        digitalWrite(ledVerde, HIGH);
                        digitalWrite(ledRojo, LOW);
                        digitalWrite(ledAmarillo, LOW);

                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("Linea Activa");
                        lcd.setCursor(0, 1);
                        lcd.print("Esperando...");

                        break;
                }
            }
        }
    }
}