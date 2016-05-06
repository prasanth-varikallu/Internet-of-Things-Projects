import paho.mqtt.client as mqtt

import RPi.GPIO
import time

RPi.GPIO.setmode(RPi.GPIO.BCM)
RPi.GPIO.setup(18, RPi.GPIO.OUT)

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("Buzzer")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    RPi.GPIO.output(18, True)
    time.sleep(3)
    RPi.GPIO.output(18, False)


try:
    client = mqtt.Client()
    client.connect("172.20.10.6", 1883, 60)
    client.on_connect = on_connect
    client.on_message = on_message
    client.loop_forever()
except KeyboardInterrupt:
    RPi.GPIO.cleanup(18)
