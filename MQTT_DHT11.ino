#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 5 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Update these with values suitable for your network.

const char* ssid = "Sandy_AP";
const char* password = "sandeep259";
IPAddress server(192,168,43,1);
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int h;
int t;
char s = 'a';

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == 'h') {
    s = 'h';   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else if ((char)payload[0] == 't') {
   s = 't'; // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);
}

void loop() {
delay(1000);
  if (!client.connected()) {
    reconnect();
  }

  h = dht.readHumidity();
  t = dht.readTemperature(true);
  
  client.loop();
  if (s == 'h') 
  snprintf (msg, 75, "Humidity %ld%%",h);
  else if (s == 't')
  snprintf (msg, 75, "Temperature #%d C",t);
  //Serial.println(msg);
  else
    snprintf (msg, 75, "Humidity %ld%% and Temperature #%d C",h,t);
  client.publish("outTopic", msg);
  client.subscribe("inTopic",0);
}
