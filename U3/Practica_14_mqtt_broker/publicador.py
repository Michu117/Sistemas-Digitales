import paho.mqtt.client as mqtt
import random
import time

# ==========================
# Configuración MQTT
# ==========================
BROKER = "michu117-pc.local"  
PUERTO = 1883

TOPICO_TEMP = "laboratorio/temperatura"
TOPICO_LED = "laboratorio/led"

cliente = mqtt.Client()

# Variable para alternar ON y OFF
estado_led = False

try:
    cliente.connect(BROKER, PUERTO, 60)
    print(f"Conectado al broker MQTT: {BROKER}:{PUERTO}")

    while True:

        # Temperatura simulada
        temperatura = round(random.uniform(20.0, 35.0), 1)

        # Publicar temperatura
        cliente.publish(TOPICO_TEMP, str(temperatura))

        # Alternar estado del LED
        estado_led = not estado_led
        mensaje_led = "ON" if estado_led else "OFF"

        # Publicar comando del LED
        cliente.publish(TOPICO_LED, mensaje_led)

        print(f"Temperatura enviada: {temperatura} °C")
        print(f"Comando LED enviado: {mensaje_led}")
        print("-" * 40)

        time.sleep(2)

except KeyboardInterrupt:
    print("\nPublicador detenido.")
    cliente.disconnect()

except Exception as e:
    print("Error:", e)