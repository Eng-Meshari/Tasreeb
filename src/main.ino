#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ————————— 1. بيانات الاتصال والتليجرام —————————
const char* ssid = "";      // اكتب اسم شبكة الواي فاي الخاصة بك
const char* pass = "";     // اكتب كلمة مرور الشبكة
const char* BOT_TOKEN = "";       // ضع توكن البوت المسحوب من BotFather
const char* CHAT_ID = "";       // ضع رقم CHAT_ID للمجموعة (مع علامة السالب -)

// ————————— 2. تعريف المنافذ (Pins) —————————
const int LEAK_SENSOR = D5; // منفذ إشارة حساس التسرب
const int BUZZER = D6;      // منفذ الطنان الصوتي

bool alertActive = false;
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);
  
  pinMode(LEAK_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // الاتصال بشبكة الواي فاي
  WiFi.begin(ssid, pass);
  client.setInsecure(); // لتجاوز فحص شهادات الأمان SSL لتليجرام

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // إرسال رسالة تأكيد عند تشغيل النظام
  String startupMsg = "نظام كشف تسرب المياه من السقف جاهز للعمل.";
  bot.sendMessage(CHAT_ID, startupMsg, "");
}

void loop() {
  int sensorRead = digitalRead(LEAK_SENSOR);

  // عند اكتشاف الماء (الحالة HIGH)
  if (sensorRead == HIGH) {
    if (!alertActive) {
      String alertMsg = "⚠️ تنبيه خطر: تم اكتشاف تسرب مياه من السقف!";
      bot.sendMessage(CHAT_ID, alertMsg, "");
      alertActive = true;
    }
    
    // إطلاق صوت إنذار متقطع
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  } else {
    // عند جفاف الحساس وعودة الوضع للطبيعي
    if (alertActive) {
      String safeMsg = "تحديث: توقف اكتشاف المياه، الوضع عاد للطبيعي.";
      bot.sendMessage(CHAT_ID, safeMsg, "");
      digitalWrite(BUZZER, LOW);
      alertActive = false;
    }
  }
  
  delay(500);
}
