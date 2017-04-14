/*
External Temperature Sensor

For the Espressif ESP8266

Uses MQTT and a DS18B20 Sensor to read and transmit Temprature to an MQTT
Broker.

This code was written and tested using the PlatformIO IDE and the ESP8266-12F
module.

Ideas and code from various soruces:
https://github.com/chaeplin/esp8266_and_arduino
https://twitter.com/jpwsutton,

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the iplmplied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

This code is licensed under the GNU LGPL and is open for ditrbution
and copying in accordance with the license.
This header must be included in any derived code or copies of the code.

(C) Alan Lord 2017
*/
#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

// Set up ESP8266 ADC for voltage read
ADC_MODE(ADC_VCC);

// DS18B20
float temp(NAN);

// ESP
float vcc;
const uint32_t esp_id = ESP.getChipId();

// Timer placeholders
unsigned long startMills = 0;
unsigned long wifiMills = 0;
unsigned long subMills = 0;

// Function Prototypes
void readDS();
void setup_wifi();
void reconnect();
void publishJSON();
void goingToSleep();
// End Prototypes

// Begin code
WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature DS18B20(&oneWire);
char temperatureCString[6];

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  startMills = millis();
  //Power the DS18B20
  pinMode(ONE_WIRE_PWR, OUTPUT);
  digitalWrite(ONE_WIRE_PWR, HIGH);
  vcc = ESP.getVcc();
  Serial.begin(115200);
  DS18B20.begin();

  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
}

void setup_wifi() {
  //delay(10);
  // We start by connecting to a WiFi network
  if (DEBUG_PRINT) {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
  }
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (DEBUG_PRINT) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  readDS();

  if(temp) {
    if (DEBUG_PRINT) {
      Serial.print("Read DS18B20. Publish message: ");
    }
    publishJSON();
  }
  goingToSleep();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG_PRINT) {
      Serial.print("Attempting MQTT connection...");
    }
    // Attempt to connect
    if (client.connect("EXTMonitor")) {
      if (DEBUG_PRINT) {
        Serial.println("Connected");
      }
    } else {
      if (DEBUG_PRINT) {
        Serial.print("Failed to connect. State = ");
        Serial.print(client.state());
        Serial.println(" Retry in 2 seconds");
      }
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void publishJSON() {
  if (client.connected()) {
    StaticJsonBuffer<175> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& payload = root.createNestedArray("payload");
    JsonObject& data = payload.createNestedObject();
    data["sensorid"] = SENSORNAME;
    data["temp"] = temp;
    data["vcc"] = vcc/1000;
    data["cycletime"] = millis() - startMills;
    if (DEBUG_PRINT) {
      Serial.print("Sending payload: ");
      root.prettyPrintTo(Serial);
    }

    char buffer[150];
    root.printTo(buffer, sizeof(buffer));
    if (client.publish(TOPIC, buffer));
      if (DEBUG_PRINT) {
        Serial.print("Published OK  --> ");
        Serial.println(millis() - startMills);
      }
    } else {
      if (DEBUG_PRINT) {
        Serial.print("Publish failed --> ");
        Serial.println(millis() - startMills);
      }
    }
}

void readDS() {
  do {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    dtostrf(temp, 2, 2, temperatureCString);
    delay(100);
    if (DEBUG_PRINT) {
      Serial.print("Temp: ");
      Serial.print(temp);
    }
    if((millis() - startMills) > 10000) {
      if (DEBUG_PRINT) {
        Serial.print("Failed to read from DS18B20 within time limit ");
        Serial.print(millis() - startMills);
        Serial.print("Going to sleep");
      }
      goingToSleep();
    }
  } while (temp == 85.0 || temp == (-127.0));
}

void goingToSleep() {
  // All Good
  if (DEBUG_PRINT) {
    Serial.print("About to go to sleep: Time taken to complete cycle: ");
    Serial.println(millis() - startMills);
  }
  client.disconnect();
  yield();
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  ESP.deepSleep(deep_sleep, DS_MODE);
  //ESP.deepSleep(0);
  delay(250);
}
