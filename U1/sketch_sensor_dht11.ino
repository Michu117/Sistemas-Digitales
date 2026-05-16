#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

//==================================================
// CONFIGURACIÓN DE HARDWARE
//==================================================
const uint8_t PIN_DHT       = 2;
const uint8_t PIN_LED_AZUL  = 3;
const uint8_t PIN_LED_VERDE = 4;
const uint8_t PIN_LED_ROJO  = 5;

#define DHTTYPE DHT11

// Dirección I2C del LCD (0x27 o 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(PIN_DHT, DHTTYPE);

//==================================================
// CONSTANTES DEL SISTEMA
//==================================================
const float TEMP_FRIO_MAX    = 20.0;
const float TEMP_NORMAL_MAX  = 30.0;

// Períodos de ejecución (ms)
const unsigned long PERIODO_LECTURA = 2000;
const unsigned long PERIODO_LCD     = 500;

//==================================================
// TIPOS DE DATOS
//==================================================
enum EstadoTemperatura {
  FRIO,
  NORMAL,
  CALIENTE
};

//==================================================
// VARIABLES GLOBALES
//==================================================
float temperaturaActual = 0.0;
float temperaturaMinima = 1000.0;
float temperaturaMaxima = -1000.0;

EstadoTemperatura estadoActual = FRIO;

// Temporizadores
unsigned long tiempoAnteriorLectura = 0;
unsigned long tiempoAnteriorLCD     = 0;

//==================================================
// FUNCIONES AUXILIARES
//==================================================
const char* obtenerTextoEstado(EstadoTemperatura estado) {
  switch (estado) {
    case FRIO:       return "FRIO";
    case NORMAL:     return "NORMAL";
    case CALIENTE:   return "CALIENTE";
  }
  return "";
}

//--------------------------------------------------
void inicializarHardware() {
  pinMode(PIN_LED_AZUL, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);

  digitalWrite(PIN_LED_AZUL, LOW);
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);

  dht.begin();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Sistema Temp");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando");
}

//--------------------------------------------------
bool leerTemperatura() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    return false;
  }

  temperaturaActual = temp;

  if (temperaturaActual < temperaturaMinima) {
    temperaturaMinima = temperaturaActual;
  }

  if (temperaturaActual > temperaturaMaxima) {
    temperaturaMaxima = temperaturaActual;
  }

  return true;
}

//--------------------------------------------------
void determinarEstado() {
  if (temperaturaActual < TEMP_FRIO_MAX) {
    estadoActual = FRIO;
  }
  else if (temperaturaActual <= TEMP_NORMAL_MAX) {
    estadoActual = NORMAL;
  }
  else {
    estadoActual = CALIENTE;
  }
}

//--------------------------------------------------
void actualizarLEDs() {
  digitalWrite(PIN_LED_AZUL,  estadoActual == FRIO);
  digitalWrite(PIN_LED_VERDE, estadoActual == NORMAL);
  digitalWrite(PIN_LED_ROJO,  estadoActual == CALIENTE);
}

//--------------------------------------------------
void mostrarLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperaturaActual, 2);
  lcd.print((char)223);
  lcd.print("C ");

  if (estadoActual == FRIO) {
    lcd.print("FRIO");
  }
  else if (estadoActual == NORMAL) {
    lcd.print("NORMAL");
  }
  else {
    lcd.print("CALOR");
  }

  lcd.setCursor(0, 1);
  lcd.print("Mn:");
  lcd.print(temperaturaMinima, 2);
  lcd.print(" Mx:");
  lcd.print(temperaturaMaxima, 2);
}

//--------------------------------------------------
void tareaLecturaSensor() {
  if (millis() - tiempoAnteriorLectura >= PERIODO_LECTURA) {
    tiempoAnteriorLectura = millis();

    if (leerTemperatura()) {
      determinarEstado();
    }

    actualizarLEDs();
  }
}

//--------------------------------------------------
void tareaActualizarLCD() {
  if (millis() - tiempoAnteriorLCD >= PERIODO_LCD) {
    tiempoAnteriorLCD = millis();
    mostrarLCD();
  }
}

//==================================================
// SETUP
//==================================================
void setup() {
  inicializarHardware();
}

//==================================================
// LOOP PRINCIPAL
//==================================================
void loop() {
  tareaLecturaSensor();
  tareaActualizarLCD();
}