#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Vishnu";
const char* password = "sainath2001";

// MQTT broker details
const char* mqtt_broker = "192.168.0.141"; // Replace with your server's IP
const int mqtt_port = 1883;
const char* dataTopic = "esp32/data";
const char* commandTopic = "esp32/commands";

// IR sensor pin
const int irPin = 15;
int sensorState = LOW;

// LED pin
const int ledPin = 2;

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Connect to MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Bidirectional")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Subscribe to command topic
  client.subscribe(commandTopic);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  // Handle commands (1 to turn LED on, 0 to turn LED off)
  if (String(topic) == commandTopic) {
    if (message == "1") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON");
    } else if (message == "0") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    } else {
      Serial.println("Unknown command");
    }
  }
}

void loop() {
  client.loop();

  // Publish IR sensor state
  int currentState = digitalRead(irPin);
  if (currentState != sensorState) {
    sensorState = currentState;
    String data = (sensorState == HIGH) ? "Object Detected" : "No Object";
    client.publish(dataTopic, data.c_str());
    Serial.println("Published data: " + data);
  }

  delay(1000);
}
