

//--------------------------------------------------------------
//   Duino coin Monitor V1.0 2021 By Chetsarhit (FB:idea04)
//   Facebook fan page: https://www.facebook.com/pro45s
//
//   Wemos NodeMCU ESP8266 ESP-12F 1.3/0.96 inch OLED Display
//
//--------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306Wire.h"
ESP8266WiFiMulti WiFiMulti;
SSD1306Wire display(0x3c, D1, D2);
#define StaticJsonDocument<4000> doc;

//-----------------------------------------------

const char *ssid         = "MyWifi";  // wifi name
const char *password     = "WifiPassword";       // wifi password
const String duco_user   = "YourDucoUsername";            // duino coin user

//-----------------------------------------------
float last = 0.45;
void setup() {
  WiFi.begin ( ssid, password );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 10 );
  }
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_16);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Dev by Idea04:\n" + WiFi.localIP().toString());
  display.display();
  delay(2500);
}

void loop() {
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    String serverPath = "https://server.duinocoin.com/balances/" + String(duco_user);
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient https;
    if (https.begin(*client, serverPath))
    {
      int httpCode = https.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = https.getString();
        DynamicJsonBuffer jsonBuffer(1500);
        const char* json = payload.c_str();
        JsonObject& root = jsonBuffer.parseObject(json);
        JsonObject& data = root["result"];
        float balance = data["balance"].as<float>();
        if (balance != last) {
          display.clear();
          display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
          display.setFont(ArialMT_Plain_16);
          display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Your Coins:\n" + String(balance) + " DU\n" + String((balance - last) * 1440) + "/Day.");
          display.display();
          last = balance;
          delay(45000);
        }
      }
      https.end();
    }
  }
  delay(15000);
}
