#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <config.h>
#include <shk_mqtt.h>

const char *ssid     = "SembsaMi_2DAB";
const char *password = "Zakopane2015";
const char *adres_mqtt = "192.168.54.124";

WiFiClient client;
PubSubClient mqtt(client);

//INFO

shk_mqtt temperature(mqtt, "podlewaczka", "bme280", "temperature");
shk_mqtt pressure(mqtt, "podlewaczka", "bme280", "pressure");
shk_mqtt humidity(mqtt, "podlewaczka", "bme280", "humidity");
shk_mqtt fan(mqtt, "podlewaczka", "fan", "on");

void setupOTA() {

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("esp8266-podlewaczka");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"mQa83zseba");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    Serial.println("Start updating ");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void setupWiFi() {

  WiFi.mode(WIFI_STA);
  delay(10);
  Serial.println();
  Serial.print("[WIFI] Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  Serial.print("[WIFI] ");
  while (WiFi.status() != WL_CONNECTED) {
    //digitalWrite(13, LOW);
    delay(500);
    Serial.print(".");
    //digitalWrite(13, HIGH);
    delay(500);
  }
  
  if(WiFi.status()) {
    WiFi.hostname("esp8266-podlewaczka");

    if (!MDNS.begin("esp8266-podlewaczka")) {
      Serial.println("[MDNS] Nie udalo sie ustawic MDNS");
    } else {
      Serial.println("[MDNS] Nazwa mDNS ustawiona");
    }

    MDNS.addService("http", "tcp", 1883);
  }

  randomSeed(micros());

  Serial.println();
  Serial.println("[WIFI] WiFi connected");
  Serial.print("[WIFI] IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("[WIFI] Connecting finished");
  setupOTA();
  Serial.println("[OTA] Setup");
}

void connectMQTT() {

  while (!mqtt.connected()) {

    Serial.print("[MQTT] Connected to MQTT: ");
    Serial.println(adres_mqtt);

    String clientId = "ESP8266-";
    clientId += ESP.getChipId();
    Serial.print("[MQTT] Client ID: ");
    Serial.println(clientId);

    mqtt.setServer(adres_mqtt, 1883);
    
    if (mqtt.connect(clientId.c_str(), "/info", 0, false, "/error/podlewaczka")) {
      Serial.printf("[MQTT] Client %s connected to MQTT\n", clientId.c_str());

      //Wątek do subskrypcji
      temperature.subscribeTopic();
      pressure.subscribeTopic();
      humidity.subscribeTopic();
      fan.subscribeTopic();

    } else {
      //Serial.printf("MQTT: Failed, rc=%i\n", mqtt.state());
      Serial.println("[MQTT] Try again in 5 secounds");
      delay(5000);
    }

  }
}

void sendDebugMQTT(String name, bool b) {
  String out = b ? "true" : "false";
  String _name = "["+name+"] Status: ";
  String payload = _name + out;
  const char* debugTopic = "/debug/podlewaczka";
  mqtt.publish(debugTopic, payload.c_str());
}