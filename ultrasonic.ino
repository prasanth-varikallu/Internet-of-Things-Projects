#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSID";
const char* password = "PWD";
IPAddress server(172,20,10,6);
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

const int pwPin = 13;

//variables needed to store values
long pulse, inches, cm;

void setup()
{
  //This opens up a serial connection to shoot the results back to the PC console
  Serial.begin(9600);

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

  client.setServer(server, 1883);

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
       } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
   }

   
}

void loop()
{
  pinMode(pwPin, INPUT);

  //Used to read in the pulse that is being sent by the MaxSonar device.
  //Pulse Width representation with a scale factor of 147 uS per Inch.
  client.loop();

  pulse = pulseIn(pwPin, HIGH);
  //147uS per inch
  inches = pulse / 147;
  //change inches to centimetres
  cm = inches * 2.54;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  if (inches < 12) {
    snprintf (msg, 10, "yes");
    client.publish("Buzzer", msg);
  }
  delay(1000);
}
