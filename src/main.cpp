#include <settings.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SENSOR_ON D5
#define FAN_ON D6
#define PUMP_ON D7
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

bool statusSendMQTTDebug = false;
bool statusBME280;

void setup() {
  Serial.begin(115200);
  delay(1000);

  setupWiFi();

  temperature.showTopic();

  //Setup BME280
  statusBME280 = bme.begin();
  
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
    if (mqtt.connected()) {
      mqtt.loop();
      
      if (!statusSendMQTTDebug) {
        sendDebugMQTT("BME280", statusBME280);
        statusSendMQTTDebug = true;
      }
      
    } else if (!mqtt.connected()) {
      connectMQTT();
    }
  } else if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
}