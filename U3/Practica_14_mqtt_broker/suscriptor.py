import paho.mqtt.client as mqtt

# ==========================
# Configuración MQTT
# ==========================
BROKER = "michu117-pc.local" 
PUERTO = 1883

TOPICO_TEMP = "laboratorio/temperatura"
TOPICO_LED = "laboratorio/led"

# ==========================
# Callbacks
# ==========================
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"Conectado al broker MQTT: {BROKER}:{PUERTO}")

        client.subscribe(TOPICO_TEMP)
        client.subscribe(TOPICO_LED)

        print("Suscrito a los tópicos.")
        print("-" * 40)
    else:
        print("Error de conexión:", rc)


def on_message(client, userdata, msg):

    mensaje = msg.payload.decode()

    if msg.topic == TOPICO_TEMP:
        print(f"Temperatura recibida: {mensaje} °C")

    elif msg.topic == TOPICO_LED:
        print(f"Comando LED recibido: {mensaje}")

    print("-" * 40)


# ==========================
# Cliente MQTT
# ==========================
cliente = mqtt.Client()

cliente.on_connect = on_connect
cliente.on_message = on_message

try:
    cliente.connect(BROKER, PUERTO, 60)
    cliente.loop_forever()

except KeyboardInterrupt:
    print("\nSuscriptor detenido.")
    cliente.disconnect()

except Exception as e:
    print("Error:", e)