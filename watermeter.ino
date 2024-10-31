#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "everblu_meters.h"
#include <ESP8266mDNS.h>

#ifndef LED_BUILTIN
// Change this pin if needed
#define LED_BUILTIN 2
#endif

ESP8266WiFiMulti WifiMulti;

const char*    ssid = "name_SSID";     // your network SSID (name)
const char*    password = "Key";       // your network password
IPAddress local_IP(192, 168, 1, 100);  // ip static pour se connecter depuis un navigateur
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

// au demarrage
int litre = 1;
int batterie = 2;
int compteur = 3;
String lbc;
bool flag = true;

AsyncWebServer server(80);

void onUpdateData() {
  struct tmeter_data meter_data;
  meter_data = get_meter_data();
  yield();
  
  if (meter_data.reads_counter == 0 || meter_data.liters == 0) {
    Serial.println("Unable to retrieve data from meter. Retry later...");
	flag = true;
  return;
  }
  
  digitalWrite(LED_BUILTIN, LOW); // turned on
  Serial.printf("Liters : %d\nBattery (in months) : %d\nCounter : %d\n\n", meter_data.liters, meter_data.battery_left, meter_data.reads_counter);
  litre = meter_data.liters;
  batterie = meter_data.battery_left;
  compteur = meter_data.reads_counter;
  flag = false;
}

/*
// This function calls onUpdateData() every days at 14:00pm UTC
void onScheduled() {
  time_t tnow = time(nullptr);
  struct tm *ptm = gmtime(&tnow);
  // At 14:00 UTC
  if (ptm->tm_hour == 14 && ptm->tm_min == 0 && ptm->tm_sec == 0) {
    // Call back in 23 hours
    // mqtt.executeDelayed(1000 * 60 * 60 * 23, onScheduled);
    Serial.println("It is time to update data from meter :)");
    // Update data
    onUpdateData();
    return;
  }
}
*/

String processor(const String& var) {
  if (var == "LITRE") {
    return String(litre);
    }
    else if (var == "BATTERIE") {
    return String(batterie);
    }
    else if (var == "COMPTEUR") {
    return String(compteur);
    }
  return String();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n");
  LittleFS.begin(); 
  MDNS.begin("esp8266");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turned off
/*
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);   // Get Zulu Time from Server
  Update_Time();          // Set Up Time,Hour,Minute
*/
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);                          // Wait a little bit
  }
  WiFi.mode(WIFI_STA);
  WifiMulti.addAP(ssid, password);
  delay(200);

  server.serveStatic("/", LittleFS, "/");
  /*
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "hello from esp8266!");
  });
  */
  server.on("/compteur", HTTP_GET, [](AsyncWebServerRequest * request) {
    // onUpdateData(); // problème wdt reset
    lbc = String(litre) + ":" + String(batterie) + ":" + String(compteur);
    request->send(200, "text/plain", lbc);
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    // onUpdateData(); // problème wdt reset
    // flag = true;    // si on veut un update à chaque connexion
    lbc = String(litre) + ":" + String(batterie) + ":" + String(compteur);
    request->send(LittleFS,  "/index.html", "text/html", false, processor);
  });
  server.begin();

  cc1101_init(FREQUENCY);
  
  /*
  // Use this piece of code to find the right frequency.
  for (float i = 433.78f; i < 433.890f; i += 0.001f) {
    Serial.printf("Test frequency : %f\n", i);
    cc1101_init(i);

    struct tmeter_data meter_data;
    meter_data = get_meter_data();

    if (meter_data.reads_counter != 0 || meter_data.liters != 0) {
      Serial.printf("\n------------------------------\nGot frequency : %f\n------------------------------\n", i);

      Serial.printf("Liters : %d\nBattery (in months) : %d\nCounter : %d\n\n", meter_data.liters, meter_data.battery_left, meter_data.reads_counter);

      digitalWrite(LED_BUILTIN, LOW); // turned on

      while (42);
    }
  }
  */

  /*
  // Use this piece of code to test
  struct tmeter_data meter_data;
  meter_data = get_meter_data();
  Serial.printf("\nLiters : %d\nBattery (in months) : %d\nCounter : %d\nTime start : %d\nTime end : %d\n\n", meter_data.liters, meter_data.battery_left, meter_data.reads_counter, meter_data.time_start, meter_data.time_end);
  yield();
  // while (42); 
  */
   
}

void loop() {
  if (flag) {
     onUpdateData();
  } 
  delay(5000); // 5 secondes
}
