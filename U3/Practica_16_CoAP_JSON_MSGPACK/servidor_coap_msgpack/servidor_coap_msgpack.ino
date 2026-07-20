#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include <ESPmDNS.h>

const char* ssid = "Fabricio's A56";
const char* password = "holamundo100";

WiFiUDP udp;
Coap coap(udp);

void callback_sensor(CoapPacket &packet, IPAddress ip, int port)
{
    Serial.print("Recibidos ");
    Serial.print(packet.payloadlen);
    Serial.println(" bytes de MessagePack");

    coap.sendResponse(ip, port, packet.messageid, "OK");
}

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    Serial.print("Conectando");

    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi conectado");
    Serial.print("IP del servidor: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp32-servidor"))
    {
        Serial.println("mDNS iniciado: esp32-servidor.local");
    }

    coap.server(callback_sensor, "sensores");
    coap.start();

    Serial.println("Servidor CoAP iniciado.");
}

void loop()
{
    coap.loop();
}
