#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <DHT.h>

//====================== WiFi ======================
const char* ssid = "Internet_UNL"; 
const char* password = "UNL1859WiFi"; 

//====================== MQTT ======================
IPAddress mqtt_server;
const int mqtt_port = 1883;

const char* topicTemp = "laboratorio/temperatura";
const char* topicLed  = "laboratorio/led";
const char* clientId  = "ESP32-sensor";

//====================== Pines =====================
#define DHTPIN 4
#define DHTTYPE DHT11

#define LED 2

//====================== Objetos ===================
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

//====================== Variables =================
bool ledState = false;

SemaphoreHandle_t xLedSemaphore;
QueueHandle_t xTempQueue;

//==================================================
// Conexión WiFi
//==================================================
void conectarWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  MDNS.begin("esp32");

  for (int i = 0; i < 5; i++) {

    mqtt_server = MDNS.queryHost("michu117-pc");

    if (mqtt_server != IPAddress(0,0,0,0))
      break;

    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  if (mqtt_server == IPAddress(0,0,0,0)) {

    mqtt_server = IPAddress(192,168,100,161);

  }

  Serial.print("Broker MQTT: ");
  Serial.println(mqtt_server);

}

//==================================================
// Callback MQTT
//==================================================
void callback(char* topic, byte* payload, unsigned int length) {

  String mensaje = "";

  for (int i = 0; i < length; i++)
    mensaje += (char)payload[i];

  Serial.print("Mensaje recibido: ");
  Serial.println(mensaje);

  if (String(topic) == topicLed) {

    if (mensaje == "ON")
      ledState = true;

    else if (mensaje == "OFF")
      ledState = false;

    xSemaphoreGive(xLedSemaphore);

  }

}

//==================================================
// Conectar MQTT
//==================================================
void conectarMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando MQTT...");

    if (client.connect(clientId)) {

      Serial.println("Conectado");

      client.subscribe(topicLed);

    } else {

      Serial.print("Error: ");
      Serial.println(client.state());

      vTaskDelay(pdMS_TO_TICKS(2000));

    }

  }

}

//==================================================
// Tarea Sensor
//==================================================
void vTaskSensor(void *pvParameters) {

  float temperatura;

  while (true) {

    temperatura = dht.readTemperature();

    if (!isnan(temperatura)) {

      xQueueSend(xTempQueue, &temperatura, 0);

    }
    else {

      Serial.println("Error leyendo DHT11");

    }

    vTaskDelay(pdMS_TO_TICKS(2000));

  }

}

//==================================================
// Tarea MQTT
//==================================================
void vTaskMQTT(void *pvParameters) {

  char buffer[10];
  float temperatura;

  while (true) {

    if (!client.connected())
      conectarMQTT();

    client.loop();

    if (xQueueReceive(xTempQueue, &temperatura, 0) == pdTRUE && client.connected()) {

      dtostrf(temperatura, 4, 1, buffer);

      if (client.publish(topicTemp, buffer)) {

        Serial.print("Temperatura enviada: ");
        Serial.print(temperatura);
        Serial.println(" °C");

      }

    }

    vTaskDelay(pdMS_TO_TICKS(20));

  }

}

//==================================================
// Tarea LED
//==================================================
void vTaskLED(void *pvParameters) {

  while (true) {

    if (xSemaphoreTake(xLedSemaphore, portMAX_DELAY) == pdTRUE) {

      digitalWrite(LED, ledState);

      Serial.print("LED ");

      if (ledState)
        Serial.println("ENCENDIDO");
      else
        Serial.println("APAGADO");

    }

  }

}

//==================================================
void setup() {

  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  digitalWrite(LED, LOW);

  dht.begin();

  conectarWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  xLedSemaphore = xSemaphoreCreateBinary();
  xTempQueue = xQueueCreate(1, sizeof(float));

  xTaskCreatePinnedToCore(
      vTaskSensor,
      "Sensor",
      4096,
      NULL,
      1,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      vTaskMQTT,
      "MQTT",
      4096,
      NULL,
      1,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      vTaskLED,
      "LED",
      2048,
      NULL,
      1,
      NULL,
      1);

  Serial.println("==================================");
  Serial.println("Sistema iniciado");
  Serial.println("Publicando en:");
  Serial.println(topicTemp);
  Serial.println("Escuchando:");
  Serial.println(topicLed);
  Serial.println("==================================");

}

//==================================================
void loop() {

}
