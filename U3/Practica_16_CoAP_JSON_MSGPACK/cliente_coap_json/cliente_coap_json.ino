#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

const char* ssid = "Fabricio's A56";
const char* password = "holamundo100";

IPAddress serverIP(10, 206, 255, 223);

WiFiUDP udp;
Coap coap(udp);

unsigned long lastSend = 0;
const unsigned long sendInterval = 5000;
bool waitingResponse = false;
unsigned long responseStart = 0;

void callback_response(CoapPacket &packet, IPAddress ip, int port)
{
    unsigned long tiempo = millis() - responseStart;
    Serial.print("Respuesta del servidor (");

    if (packet.payloadlen > 0)
    {
        char payload[packet.payloadlen + 1];
        memcpy(payload, packet.payload, packet.payloadlen);
        payload[packet.payloadlen] = '\0';

        Serial.print(packet.payloadlen);
        Serial.print(" bytes, ");
        Serial.print(tiempo);
        Serial.print(" ms): ");
        Serial.println(payload);
    }
    else
    {
        Serial.print(packet.payloadlen);
        Serial.print(" bytes, ");
        Serial.print(tiempo);
        Serial.println(" ms): Sin respuesta");
    }
}

void setup()
{
    Serial.begin(9600);

    WiFi.begin(ssid, password);

    Serial.print("Conectando");

    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi conectado");
    Serial.print("IP Cliente: ");
    Serial.println(WiFi.localIP());

    Serial.print("Resolviendo esp32-servidor.local... ");
    if (MDNS.begin("cliente-esp32"))
    {
        for (int i = 0; i < 5; i++)
        {
            IPAddress ip = MDNS.queryHost("esp32-servidor");
            if (ip.toString() != "0.0.0.0")
            {
                serverIP = ip;
                break;
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        if (serverIP.toString() != "0.0.0.0")
        {
            Serial.print("resuelto: ");
            Serial.println(serverIP);
        }
        else
        {
            Serial.println("no resuelto, usando IP estatica");
        }
    }
    else
    {
        Serial.println("mDNS no disponible, usando IP estatica");
    }

    coap.response(callback_response);
    coap.start();
}

void loop()
{
    coap.loop();

    if (waitingResponse)
    {
        if (millis() - responseStart >= 1000)
            waitingResponse = false;
    }
    else if (millis() - lastSend >= sendInterval)
    {
        lastSend = millis();

        StaticJsonDocument<200> doc;

        doc["id"] = "ESP32_01";
        doc["temperatura"] = random(200, 350) / 10.0;
        doc["humedad"] = random(400, 900) / 10.0;
        doc["estado"] = "Activo";

        char buffer[200];
        size_t len = serializeJson(doc, buffer);

        Serial.print("Enviando JSON (");
        Serial.print(len);
        Serial.print(" bytes): ");
        Serial.println(buffer);

        coap.send(
            serverIP,
            5683,
            "sensores",
            COAP_CON,
            COAP_POST,
            NULL,
            0,
            (uint8_t*)buffer,
            len,
            COAP_APPLICATION_JSON
        );

        responseStart = millis();
        waitingResponse = true;
    }
}
