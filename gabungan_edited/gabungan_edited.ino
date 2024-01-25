#include "DHT.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#define DHTPIN 4
#define DHTTYPE DHT11
int m, sensor_analog;
const int sensor_pin = 34;
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "abcdefghijkl";
const char* password = "mnopqrstu";
#define CHAT_ID "6612161304"
#define BOTtoken "6843460535:AAHkz7pj2OTHkvPnP1XUpqKU3rEtJN1vT1E"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
unsigned long bot_lasttime;

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Loading....", "");

  delay(2000);
  Serial.println("Monitoring Tanaman, Humiditas gas udara dan Temperatur Linkungan:");
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  sensor_analog = analogRead(sensor_pin);
  m = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
  if(isnan(h) || isnan(t) || isnan(m)) {
    Serial.println("Sensor Tidak Terbaca!");
    return;
  }
  // Serial.print("Humiditas gas udara: ");
  // Serial.print(h);
  // Serial.print(" %\n");
  // Serial.print("Temperatur Linkungan: ");
  // Serial.print(t);
  // Serial.print(" C\n");
  // Serial.print("Kelembaban: ");
  // Serial.print(m);
  // Serial.print(" %\n");
  // delay(2000);
  // bot.sendMessage(CHAT_ID, "Monitoring Tanaman, Humiditas gas udara dan Temperatur Linkungan:");
  // kirimPesanTelegram(h, t, m);
  if (millis() - bot_lasttime > 100)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      for (int i = 0; i < numNewMessages; i++) {
        String command = bot.messages[i].text;
        if (command == "/moist") {
          bot.sendMessage(CHAT_ID, "Tingkat kelembaban tanah saat ini: " + String(m) + " %", "Markdown");
        }
        if (command == "/temp") {
          bot.sendMessage(CHAT_ID, "Suhu saat ini: " + String(t, 2) + " C", "Markdown");
        }
        if (command == "/humid") {
          bot.sendMessage(CHAT_ID, "Humiditas udara saat ini: " + String(h, 2) + " %", "Markdown");
        }
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}