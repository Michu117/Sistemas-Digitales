#include <WiFi.h>
#include <WebServer.h>

// Datos de la red WiFi
const char* ssid = "Internet_UNL";     // Internet_UNL - Fabricio's A56 - MoranSanchez
const char* password = "UNL1859WiFi";  // UNL1859WiFi - holamundo100 - 0702594508

WebServer server(80);

// Pines de los LEDs
const int LED1 = 2;
const int LED2 = 4;
const int LED3 = 5;

// Página HTML
String paginaHTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Control de LEDs ESP32</title>
<style>
body{
    font-family: Arial, sans-serif;
    background:#f2f2f2;
    text-align:center;
    margin-top:40px;
}
h1{
    color:#1565C0;
}
button{
    width:120px;
    padding:12px;
    margin:5px;
    font-size:16px;
    border:none;
    border-radius:8px;
    cursor:pointer;
}
.on{
    background:green;
    color:white;
}
.off{
    background:red;
    color:white;
}
.led{
    margin:20px;
}
</style>
</head>
<body>

<h1>Servidor Web ESP32</h1>

<div class="led">
<h2>LED 1</h2>
<button class="on" onclick="location.href='/led1/on'">Encender</button>
<button class="off" onclick="location.href='/led1/off'">Apagar</button>
</div>

<div class="led">
<h2>LED 2</h2>
<button class="on" onclick="location.href='/led2/on'">Encender</button>
<button class="off" onclick="location.href='/led2/off'">Apagar</button>
</div>

<div class="led">
<h2>LED 3</h2>
<button class="on" onclick="location.href='/led3/on'">Encender</button>
<button class="off" onclick="location.href='/led3/off'">Apagar</button>
</div>

</body>
</html>
)rawliteral";

// Página principal
void handleRoot() {
  server.send(200, "text/html", paginaHTML);
}

// Redireccionar al inicio
void regresar() {
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {

  Serial.begin(9600);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  Serial.println("Conectando a la red WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Página principal
  server.on("/", handleRoot);

  // LED 1
  server.on("/led1/on", []() {
    digitalWrite(LED1, HIGH);
    regresar();
  });

  server.on("/led1/off", []() {
    digitalWrite(LED1, LOW);
    regresar();
  });

  // LED 2
  server.on("/led2/on", []() {
    digitalWrite(LED2, HIGH);
    regresar();
  });

  server.on("/led2/off", []() {
    digitalWrite(LED2, LOW);
    regresar();
  });

  // LED 3
  server.on("/led3/on", []() {
    digitalWrite(LED3, HIGH);
    regresar();
  });

  server.on("/led3/off", []() {
    digitalWrite(LED3, LOW);
    regresar();
  });

  server.begin();

  Serial.println("Servidor HTTP iniciado");
  Serial.print("Abre en tu navegador: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}