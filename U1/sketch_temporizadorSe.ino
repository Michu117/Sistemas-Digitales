const int pin555 = 2;
const int pinFF  = 3;

// Variables canal 1 (555)
int estadoActual1 = LOW;
int estadoAnterior1 = LOW;

unsigned long tiempoAnterior1 = 0;
unsigned long periodo1 = 0;

unsigned long tiempoCambio1 = 0;
unsigned long tHigh1 = 0;
unsigned long tLow1 = 0;

// Variables canal 2 (Flip-Flop)
int estadoActual2 = LOW;
int estadoAnterior2 = LOW;

unsigned long tiempoAnterior2 = 0;
unsigned long periodo2 = 0;

unsigned long tiempoCambio2 = 0;
unsigned long tHigh2 = 0;
unsigned long tLow2 = 0;

// Control de impresión
unsigned long ultimoPrint = 0;

void setup() {
  pinMode(pin555, INPUT);
  pinMode(pinFF, INPUT);

  Serial.begin(9600);
}

void loop() {
  unsigned long tiempoActual = millis();

  // CANAL 1 (555)
  estadoActual1 = digitalRead(pin555);

  // Detectar flanco ascendente
  if (estadoActual1 == HIGH && estadoAnterior1 == LOW) {
    periodo1 = tiempoActual - tiempoAnterior1;
    tiempoAnterior1 = tiempoActual;
  }

  // Medición HIGH/LOW
  if (estadoActual1 != estadoAnterior1) {
    if (estadoActual1 == HIGH) {
      tLow1 = tiempoActual - tiempoCambio1;
    } else {
      tHigh1 = tiempoActual - tiempoCambio1;
    }
    tiempoCambio1 = tiempoActual;
  }

  estadoAnterior1 = estadoActual1;

  // CANAL 2 (Flip-Flop)
  estadoActual2 = digitalRead(pinFF);

  // Detectar flanco ascendente
  if (estadoActual2 == HIGH && estadoAnterior2 == LOW) {
    periodo2 = tiempoActual - tiempoAnterior2;
    tiempoAnterior2 = tiempoActual;
  }

  // Medición HIGH/LOW
  if (estadoActual2 != estadoAnterior2) {
    if (estadoActual2 == HIGH) {
      tLow2 = tiempoActual - tiempoCambio2;
    } else {
      tHigh2 = tiempoActual - tiempoCambio2;
    }
    tiempoCambio2 = tiempoActual;
  }

  estadoAnterior2 = estadoActual2;

  // CÁLCULOS
  float freq1 = 0;
  float duty1 = 0;

  if (periodo1 > 0) {
    freq1 = 1000.0 / periodo1;
  }

  if ((tHigh1 + tLow1) > 0) {
    duty1 = (float)tHigh1 / (tHigh1 + tLow1) * 100.0;
  }

  float freq2 = 0;
  float duty2 = 0;

  if (periodo2 > 0) {
    freq2 = 1000.0 / periodo2;
  }

  if ((tHigh2 + tLow2) > 0) {
    duty2 = (float)tHigh2 / (tHigh2 + tLow2) * 100.0;
  }

  // IMPRESIÓN (cada 1 segundo)
  if (tiempoActual - ultimoPrint >= 1000) {
    ultimoPrint = tiempoActual;

    Serial.println("====== SENAL 555 ======");
    Serial.print("Periodo (ms): "); Serial.println(periodo1);
    Serial.print("Frecuencia (Hz): "); Serial.println(freq1);
    Serial.print("Duty (%): "); Serial.println(duty1);

    Serial.println();

    Serial.println("====== FLIP-FLOP ======");
    Serial.print("Periodo (ms): "); Serial.println(periodo2);
    Serial.print("Frecuencia (Hz): "); Serial.println(freq2);
    Serial.print("Duty (%): "); Serial.println(duty2);

    Serial.println("\n------------------------\n");
  }
  
// SERIAL PLOTTER
Serial.print(estadoActual1);
Serial.print(",");
Serial.println(estadoActual2);
}