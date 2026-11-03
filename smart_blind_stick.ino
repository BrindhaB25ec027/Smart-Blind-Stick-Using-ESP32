#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <TinyGPS++.h>
#include <ESP32Servo.h>

const char* ssid = "MyESP";
const char* password = "12345678";

#define BOTtoken "PASTE_NEW_BOT_TOKEN"
#define CHAT_ID "8787462172"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

#define BUTTON 14
#define SENSOR 2
#define SERVO_PIN 13

Servo myServo;

bool pressed = false;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(SENSOR, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(90);

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  client.setInsecure();
}

void loop() {

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  int obstacle = digitalRead(SENSOR);

  if (obstacle == LOW) {
    Serial.println("Obstacle detected");
    myServo.write(150);
  } else {
    Serial.println("No obstacle");
    myServo.write(90);
  }

  if (digitalRead(BUTTON) == LOW && !pressed) {
    pressed = true;

    Serial.println("Button Pressed");

    String message;

    if (gps.location.isValid()) {
      String link = "https://maps.google.com/?q=";
      link += String(gps.location.lat(), 6);
      link += ",";
      link += String(gps.location.lng(), 6);

      message = "📍 Location:\n" + link;
    } else {
      message = "⚠ GPS not fixed yet";
    }

    bot.sendMessage(CHAT_ID, message, "");
    delay(3000);
  }

  if (digitalRead(BUTTON) == HIGH) {
    pressed = false;
  }

  delay(200);
}
