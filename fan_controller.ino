#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <string.h>
#include "RFanController.h"
#include "secrets.h"

#define PIN_GDD0_OUTPUT (1)
#define RECONNECT_INTERVAL  30000

// IPAddress localIP(x, x, x, x);
// IPAddress gateway(x, x, x, x);
// IPAddress subnet(x, x, x, x);

unsigned long lastReconnectAttempt = 0;

struct CommandMQTT {
    const char* mqttCommand;
    Command rfCommand;
};

CommandMQTT commandDict[] = {
    {"SWITCH_FAN", CMD_ON_OFF},
    {"SWITCH_LIGHT", CMD_LIGHT_ON_OFF},
    {"SWITCH_FAN_1", CMD_1},
    {"SWITCH_FAN_2", CMD_2},
    {"SWITCH_FAN_3", CMD_3},
    {"SWITCH_FAN_4", CMD_4},
    {"SWITCH_FAN_5", CMD_5},
    {"SWITCH_FAN_FR", CMD_FR},
    {"HOUR_ONE", CMD_1H},
    {"HOUR_FOUR", CMD_4H},
    {"HOUR_EIGHT", CMD_8H},
};

const int numCommands = sizeof(commandDict) / sizeof(commandDict[0]);

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
RFanController fanController = RFanController(PIN_GDD0_OUTPUT);

/////////////////////////////////// SUPPORT FUNCTIONS ///////////////////////////////////
void blinkLED(int blinkCount = 5, int delayTime = 50) {
  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayTime);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayTime);
  }
}

void setup_wifi() {
  // Set up external antenna (XIAO ESP32 https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/#hardware-overview)
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  delay(100);
  pinMode(14, OUTPUT); 
  digitalWrite(14, HIGH);

  if (!WiFi.config(localIP, gateway, subnet)) {
    Serial.println("Static IP ERROR");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin (WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    blinkLED();
    delay (10);
  }
  Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
}

bool setup_mqtt() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(MQTT_SERVER);
  mqttClient.setId(MQTT_DEVID);
  mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);
  if (!mqttClient.connect(MQTT_SERVER, MQTT_PORT)) {
    Serial.printf("MQTT connection failed! Error code = %d\n", mqttClient.connectError());
    if(mqttClient.connectError() < 0) {
      return false;
    } else {
      while (true);
    }
  }

  Serial.println("Connected... ");
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(MQTT_TOPIC);
  Serial.printf("Subscribed to topic: %s\n", MQTT_TOPIC);
  return mqttClient.connected();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  blinkLED(5, 500);
  fanController.setRepeatTransmit(5);
  setup_wifi();
  setup_mqtt();
}

/////////////////////////////////// MQTT FUNCTIONS ///////////////////////////////////
const Command getRFCommand(const char* mqttCommand) {
  for (int i = 0; i < numCommands; i++) {
      if (strcmp(commandDict[i].mqttCommand, mqttCommand) == 0) {
          return commandDict[i].rfCommand;
      }
  }
  return CMD_NONE;
}

void onMqttMessage(int messageSize) {
  Serial.printf("Message with topic '%s', length %d bytes:\n", mqttClient.messageTopic().c_str(), messageSize);
  while (mqttClient.available()) {
        char buffer[100];
        int index = 0;
        while (mqttClient.available() && index < sizeof(buffer) - 1) {
            buffer[index++] = (char)mqttClient.read();
        }
        buffer[index] = '\0';
        Command rfCommand = getRFCommand(buffer);
        if(rfCommand != CMD_NONE) {
          Serial.printf("'%s'", buffer);
          fanController.sendCommand(rfCommand);
        } else {
          Serial.printf("-- INVALID COMMAND! '%s'", buffer);
        }
  }
  Serial.println();
  blinkLED();
}

/////////////////////////////////// MAIN ///////////////////////////////////
void loop() {
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      if (setup_mqtt()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.poll();
  }
  delay(500);
}
