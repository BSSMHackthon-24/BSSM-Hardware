#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int ledPin = 16;  // GPIO 16 for LED (dummy, not used)
const int lcdColumns = 16;  // LCD columns
const int lcdRows = 2;      // LCD rows

// Replace with your network credentials
const char* ssid = "ANYS";
const char* password = "87654321";

// MQTT Broker
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // Set LCD address to 0x27

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
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
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  // Skip displaying the message if it's "1" or "2"
  if (message == "1" || message == "2") {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("loading");
  
  delay(2000);  // Display loading for 2 seconds
  
  lcd.clear();
  lcd.setCursor(0, 0);

  if (message.length() <= lcdColumns) {
    // If message fits within LCD width, print it normally
    lcd.print(message);
  } else {
    // Print first 16 characters on the first line
    lcd.print(message.substring(0, lcdColumns));
    
    // Print remaining characters on the second line
    lcd.setCursor(0, 1);
    lcd.print(message.substring(lcdColumns));
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("bssm");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
