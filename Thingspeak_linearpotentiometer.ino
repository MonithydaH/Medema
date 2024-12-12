#include <WiFiNINA.h>           // WiFi library for ESP32
#include <ThingSpeak.h>     // Include ThingSpeak library

// ThingSpeak credentials

const char* ssid = "uri_bme_2.4G";  // constant character pointer for wifi
const char* password = "URIb3m6e1";
unsigned long myChannelNumber =  2711389;
const char* myWriteAPIKey = "5U24SF1KELOEWI3D";

int p_val=0;
int pot_pin = A0;
float distance=0;
WiFiClient client;

void setup() {
  pinMode(pot_pin,INPUT);
  Serial.begin(9600);

  // Set up sensor pins
  //pinMode(trigPin, OUTPUT);
  //pinMode(echoPin, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  p_val= analogRead(pot_pin);
  // analog voltage of esp 32 is 0-4095, which is voltage level 0-3.3v 
  distance= map(p_val,0,1023,0,15); // output voltage to total length of liner pot. 15mm
  Serial.print("Distance: ");
  Serial.println(distance);

  // Send distance to ThingSpeak
  ThingSpeak.setField(1, distance);
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  // response code is status code that tell the result of the request sent from Arduino to thingspeak
  // follow HTTP status code, 200 is success
  if (responseCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.println("Failed to send data to ThingSpeak.");
  }

  delay(15000);  // // allow enough time for arduino to sent data to thingspeak 15seconds
}