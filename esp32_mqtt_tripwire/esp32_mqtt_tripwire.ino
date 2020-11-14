#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid =  "...";   // name of your WiFi network
const char *password =  "..."; // password of the WiFi network

//cosntants for the pins where sensors are plugged into.
const int sensorPin = 39;
const int ledPin = 23;

//Set up some global variables for the light level an initial value.
int lightInit;  // initial value
int lightVal;   // light reading

//MQTT Config
const char *ID = "client_tripwireone";  // Name of our device, must be unique
const char *TOPIC = "trip/3";  // Topic to subcribe to

IPAddress broker(9,9,9,9); // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;

PubSubClient client(wclient); // Setup MQTT client
bool state=0;

// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


// Reconnect to client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');

    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // We'll set up the LED pin to be an output.
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  lightInit = analogRead(sensorPin);
  setup_wifi(); // Connect to network
  client.setServer(broker, 1883);
}


void loop()
{
  if (!client.connected())  // Reconnect if connection is lost
  {
    reconnect();
  }
  client.loop();

  lightVal = analogRead(sensorPin); // read the current light levels
  float voltage= lightVal * 1.1;
  String value = String(voltage);
  Serial.println(value);
  //if lightVal is less than our initial reading withing a threshold then it is dark.
  if(voltage == 0.00)
  {
    client.publish(TOPIC, "BROKE");
    digitalWrite (ledPin, HIGH); // turn on light
  }

  //otherwise, it is bright
  else
  {
    digitalWrite (ledPin, LOW); // turn off light
  }
}
