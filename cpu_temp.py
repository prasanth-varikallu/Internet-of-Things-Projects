import paho.mqtt.client as mqtt
import time,resource,os

client = mqtt.Client()
client.connect("172.20.10.6", 1883, 60)

try:
    
    while True:
        temp = int(open('/sys/class/thermal/thermal_zone0/temp').read()) / 1e3  # Get Raspberry Pi CPU temp
        client.publish("rInfo", (str(temp)))
        time.sleep(3)
except KeyboardInterrupt:
    exit
