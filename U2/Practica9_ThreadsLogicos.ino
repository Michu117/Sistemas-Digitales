// PRACTICA 9 - Procesos y Threads Lógicos
// Pines
const int sensorPin = A0;
const int botonPin = 2;
const int buzzerPin = 6;
const int ledRojo = 7;
const int ledVerde = 8;

// Variables de tiempo
unsigned long tiempoHeartbeat = 0;
unsigned long tiempoTelemetria = 0;
unsigned long tiempoAlarma = 0;

// Intervalos
const unsigned long intervaloHeartbeat = 500;
const unsigned long intervaloTelemetria = 2000;
const unsigned long intervaloAlarma = 300;

// Estados
bool estadoLedVerde = false;
bool estadoAlarma = false;
bool alarmaSilenciada = false;
bool estadoAlarmaVisual = false;

float temperatura = 0.0;

void setup()
{
    pinMode(ledVerde, OUTPUT);
    pinMode(ledRojo, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(botonPin, INPUT);

    Serial.begin(9600);

    Serial.println("=================================");
    Serial.println("Sistema iniciado");
    Serial.println("=================================");
}

void loop()
{
    unsigned long tiempoActual = millis();

    // THREAD 1 - HEARTBEAT
    if (tiempoActual - tiempoHeartbeat >= intervaloHeartbeat)
    {
        tiempoHeartbeat = tiempoActual;

        estadoLedVerde = !estadoLedVerde;
        digitalWrite(ledVerde, estadoLedVerde);
    }

    // THREAD 2 - TELEMETRIA
    if (tiempoActual - tiempoTelemetria >= intervaloTelemetria)
    {
        tiempoTelemetria = tiempoActual;

        int lecturaADC = analogRead(sensorPin);

        // TMP36
        float voltaje = lecturaADC * (5.0 / 1023.0);
        temperatura = (voltaje - 0.5) * 100.0;

        Serial.print("Temperatura: ");
        Serial.print(temperatura);
        Serial.println(" °C");

        // Activar alarma si supera 30°C
        if (temperatura > 30.0)
        {
            estadoAlarma = true;
        }
        else
        {
            estadoAlarma = false;
            alarmaSilenciada = false;

            digitalWrite(ledRojo, LOW);
            noTone(buzzerPin);
        }
    }

    // THREAD 3 - ALARMA TERMICA
    if (estadoAlarma && !alarmaSilenciada)
    {
        if (tiempoActual - tiempoAlarma >= intervaloAlarma)
        {
            tiempoAlarma = tiempoActual;

            estadoAlarmaVisual = !estadoAlarmaVisual;

            digitalWrite(ledRojo, estadoAlarmaVisual);

            if (estadoAlarmaVisual)
            {
                tone(buzzerPin, 1000);
            }
            else
            {
                noTone(buzzerPin);
            }
        }
    }

    // THREAD 4 - PULSADOR ACKNOWLEDGE
    if (estadoAlarma && digitalRead(botonPin) == HIGH)
    {
        alarmaSilenciada = true;

        digitalWrite(ledRojo, LOW);
        noTone(buzzerPin);

        Serial.println("ALARMA SILENCIADA POR OPERADOR");

        while (digitalRead(botonPin) == HIGH)
        {
            // Espera liberación del botón
        }
    }
}
