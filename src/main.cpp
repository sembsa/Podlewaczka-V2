#include <settings.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Ticker.h>

#define SENSOR_ON D5
#define FAN_ON D6
#define PUMP_ON D7
#define SEALEVELPRESSURE_HPA (1013.25)

Ticker timer;
Adafruit_BME280 bme;

bool statusSendMQTTDebug = false;
bool statusBME280;

void fanControl(bool on) {
  if (on) {
    digitalWrite(FAN_ON, HIGH);
    fan.publishStatus("true");
  } else {
    digitalWrite(FAN_ON, LOW);
    fan.publishStatus("false");
  }
}

void pumpControl(bool on) {
  if (on) {
    digitalWrite(PUMP_ON, LOW);
    pump.publishStatus("true");
  } else {
    digitalWrite(PUMP_ON, HIGH);
    pump.publishStatus("false");
  }
}

void sendBME280Data() {
  temperature.publishStatus(String(bme.readTemperature()));
  pressure.publishStatus(String(bme.readPressure()));
  humidity.publishStatus(String(bme.readHumidity()));
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String topicString = String(topic);
  String payloadString = "";
  for (unsigned int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }
  Serial.println(payloadString);
  
  if (topicString == fan._topicSet) {
    if (payloadString == "true") {
      fanControl(true);
    } else if (payloadString == "false") {
      fanControl(false);
    }
  }

  
  if (topicString == pump._topicSet) {
    if (payloadString == "true") {
      pumpControl(true);
    } else if (payloadString == "false") {
      pumpControl(false);
    }
  }
  
  
  // Serial.println();

  // // Switch on the LED if an 1 was received as first character
  // if ((char)payload[0] == '1') {
  //   digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //   // but actually the LED is on; this is because
  //   // it is acive low on the ESP-01)
  // } else {
  //   digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }

}

void setup() {
  //Setup Pinout
  pinMode(SENSOR_ON, OUTPUT);
  pinMode(FAN_ON, OUTPUT);
  pinMode(PUMP_ON, OUTPUT);
  digitalWrite(SENSOR_ON, HIGH);
  digitalWrite(FAN_ON, LOW);
  digitalWrite(PUMP_ON, HIGH);

  //Setup Serial
  Serial.begin(115200);
  delay(1000);

  setupWiFi();

  //Show Topics
  temperature.showTopic();
  pressure.showTopic();
  humidity.showTopic();
  fan.showTopic();

  //Setup BME280
  statusBME280 = bme.begin();

  //Setup MQTT Calback
  mqtt.setCallback(callback);

  //Setup timer
  timer.attach(10.0, sendBME280Data);
  
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