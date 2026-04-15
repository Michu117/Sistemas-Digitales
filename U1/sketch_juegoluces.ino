#define BOTON 8

int leds[] = {2,3,4,5,6,7};
int patronActual = 0;
bool estadoBoton = 0;
bool ultimoEstado = 0;

void setup() {
  for(int i=0; i<6; i++){
    pinMode(leds[i], OUTPUT);
  }
  pinMode(BOTON, INPUT);
}

void loop() {
  leerBoton();

  switch(patronActual){
    case 0: patronSecuencia(); break;
    case 1: patronPersecucion(); break;
    case 2: patronParpadeo(); break;
    case 3: patronAleatorio(); break;
    case 4: patronOnda(); break;
  }
}

// Boton
void leerBoton(){
  estadoBoton = digitalRead(BOTON);

  if(estadoBoton == HIGH && ultimoEstado == LOW){
    patronActual++;
    if(patronActual > 4) patronActual = 0;
    delay(200); // debounce
  }

  ultimoEstado = estadoBoton;
}

// P0: Secuencia
void patronSecuencia(){
  for(int i=0; i<6; i++){
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }
}

// P1: Persecución
void patronPersecucion(){
  for(int i=0; i<6; i++){
    digitalWrite(leds[i], HIGH);
    delay(100);
    digitalWrite(leds[i], LOW);
  }
  for(int i=4; i>=0; i--){
    digitalWrite(leds[i], HIGH);
    delay(100);
    digitalWrite(leds[i], LOW);
  }
}

// P2: Parpadeo
void patronParpadeo(){
  for(int i=0; i<6; i++){
    digitalWrite(leds[i], HIGH);
  }
  delay(300);
  for(int i=0; i<6; i++){
    digitalWrite(leds[i], LOW);
  }
  delay(300);
}

// P3: Aleatorio
void patronAleatorio(){
  int led = random(0,6);
  digitalWrite(leds[led], HIGH);
  delay(100);
  digitalWrite(leds[led], LOW);
}

// P4: Onda
void patronOnda(){
  for(int i=0; i<6; i++){
    digitalWrite(leds[i], HIGH);
    delay(100);
  }
  for(int i=0; i<6; i++){
    digitalWrite(leds[i], LOW);
    delay(100);
  }
}