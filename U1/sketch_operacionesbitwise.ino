#define LED_PIN 13

byte estadoLed = 0;
int contador = 0;
long numeroLargo = 123456;
float valorDecimal = 3.14;
bool bandera = true;

void printBin8(byte valor) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((valor >> i) & 1);
  }
  Serial.println();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  Serial.println("AND:");
  printBin8(0b11001100 & 0b10101010);

  Serial.println("OR:");
  printBin8(0b11001100 | 0b10101010);

  Serial.println("XOR:");
  printBin8(0b11001100 ^ 0b10101010);

  Serial.println("NOT:");
  printBin8(~0b11001100);

  Serial.println("SHIFT:");
  printBin8(1 << 2);

  estadoLed = estadoLed | (1 << 0);

  Serial.print("Long: ");
  Serial.println(numeroLargo);

  Serial.print("Float: ");
  Serial.println(valorDecimal);

  Serial.print("Bool: ");
  Serial.println(bandera);
}

void loop() {
  estadoLed = estadoLed ^ 0b00000001;

  if ((estadoLed & 1) == 1) {
    digitalWrite(LED_PIN, HIGH);
    bandera = true;
  } else {
    digitalWrite(LED_PIN, LOW);
    bandera = false;
  }

  delay(500);

  contador = (contador + 1) % 8;
  byte resultadoShift = (1 << contador);

  Serial.print("Shift: ");
  printBin8(resultadoShift);

  delay(500);
}