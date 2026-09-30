#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ————————— 1. WiFi & Telegram Credentials —————————
const char* ssid = "";      // Enter your WiFi network SSID
const char* pass = "";     // Enter your WiFi network password
const char* BOT_TOKEN = "";       // Enter your Telegram bot token from BotFather
const char* CHAT_ID = "";       // Enter your group/user CHAT_ID (include minus sign '-' if applicable)

// ————————— 2. Pin Definitions —————————
const int LEAK_SENSOR = D5; // Leak sensor signal pin
const int BUZZER = D6;      // Active buzzer pin

bool alertActive = false;
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);
  
  pinMode(LEAK_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  client.setInsecure(); // Skip SSL certificate validation for Telegram

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Send startup confirmation message
  String startupMsg = "نظام كشف تسرب المياه من السقف جاهز للعمل.";
  bot.sendMessage(CHAT_ID, startupMsg, "");
}

void loop() {
  int sensorRead = digitalRead(LEAK_SENSOR);

  // When water is detected (HIGH state)
  if (sensorRead == HIGH) {
    if (!alertActive) {
      String alertMsg = "⚠️ تنبيه خطر: تم اكتشاف تسرب مياه من السقف!";
      bot.sendMessage(CHAT_ID, alertMsg, "");
      alertActive = true;
    }
    
    // Trigger intermittent buzzer alarm
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  } else {
    // When sensor dries up and system returns to normal
    if (alertActive) {
      String safeMsg = "تحديث: توقف اكتشاف المياه، الوضع عاد للطبيعي.";
      bot.sendMessage(CHAT_ID, safeMsg, "");
      digitalWrite(BUZZER, LOW);
      alertActive = false;
    }
  }
  
  delay(500);
}
