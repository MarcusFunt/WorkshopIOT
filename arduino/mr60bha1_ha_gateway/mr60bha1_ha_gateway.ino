/*
  MR60BHA1 MQTT Bridge for Home Assistant
  ---------------------------------------
  Board: Seeed Studio XIAO ESP32-C3
  Sensor: Seeed Studio 60GHz mmWave Sensor MR60BHA1

  Description:
  - Reads presence, movement, distance, heart rate, and breathing data from the MR60BHA1.
  - Publishes sensor state over MQTT for Home Assistant via separate topics and a combined JSON payload.

  Wiring (XIAO ESP32-C3 -> MR60BHA1):
  - 3V3  -> 3V3
  - GND  -> GND
  - D6   -> TX (sensor TX -> board RX)
  - D7   -> RX (sensor RX -> board TX)

  MQTT Topics (base: homeassistant/xiao/mr60bha1):
  - base/presence        -> "occupied" | "clear"
  - base/activity        -> "moving" | "stationary" | "none"
  - base/distance_m      -> numeric meters (retained)
  - base/heart_rate_bpm  -> numeric bpm (retained)
  - base/breath_rate_bpm -> numeric bpm (retained)
  - base/breath_state    -> "normal" | "rapid" | "slow" | "none"
  - base/state           -> JSON snapshot of all fields (retained)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <60ghzbreathheart.h>

// ---- Wi-Fi and MQTT configuration ----
const char *WIFI_SSID = "YOUR_WIFI_SSID";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char *MQTT_HOST = "home-assistant.local"; // Change to your broker IP/hostname
const uint16_t MQTT_PORT = 1883;
const char *MQTT_USERNAME = "mqtt_username";     // Set to nullptr if not required
const char *MQTT_PASSWORD = "mqtt_password";     // Set to nullptr if not required
const char *MQTT_CLIENT_ID = "xiao-mr60bha1";
const char *MQTT_BASE_TOPIC = "homeassistant/xiao/mr60bha1";

// ---- Sensor serial pins ----
const int RADAR_RX_PIN = D6; // XIAO ESP32-C3 RX receiving from MR60BHA1 TX
const int RADAR_TX_PIN = D7; // XIAO ESP32-C3 TX sending to MR60BHA1 RX
const uint32_t RADAR_BAUD = 115200;

BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

struct RadarState {
  bool presence = false;
  String activity = "none";
  float distance = 0.0f;
  uint16_t heartRate = 0;
  uint16_t breathRate = 0;
  String breathState = "none";
};

RadarState state;
RadarState lastPublished;

void connectWiFi();
void ensureMqtt();
void publishField(const char *subtopic, const String &payload, bool retain = true);
void publishSnapshot();
void handleRadarReport();
void pollRadar();

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("MR60BHA1 MQTT bridge starting...");

  Serial1.begin(RADAR_BAUD, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);

  WiFi.mode(WIFI_STA);
  connectWiFi();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setBufferSize(512);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  ensureMqtt();

  pollRadar();
  mqttClient.loop();
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed, retrying...");
  }
}

void ensureMqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      publishSnapshot();
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retry in 5s");
      delay(5000);
    }
  }
}

void pollRadar() {
  radar.HumanExis_Func();
  handleRadarReport();

  radar.Breath_Heart();
  handleRadarReport();
}

void handleRadarReport() {
  bool updated = false;

  switch (radar.sensor_report) {
    case NOONE:
      state.presence = false;
      state.activity = "none";
      updated = true;
      break;
    case SOMEONE:
      state.presence = true;
      updated = true;
      break;
    case NONEPSE:
      state.activity = "none";
      updated = true;
      break;
    case STATION:
      state.presence = true;
      state.activity = "stationary";
      updated = true;
      break;
    case MOVE:
      state.presence = true;
      state.activity = "moving";
      updated = true;
      break;
    case BODYVAL:
      state.activity = String("activity_level_") + radar.bodysign_val;
      updated = true;
      break;
    case DISVAL:
      state.distance = radar.distance;
      updated = true;
      break;
    case DIREVAL:
      // Directional data available in radar.Dir_x/y/z if you want to extend the payload
      updated = true;
      break;
    case HEARTRATEVAL:
      state.heartRate = radar.heart_rate;
      updated = true;
      break;
    case BREATHVAL:
      state.breathRate = radar.breath_rate;
      updated = true;
      break;
    case BREATHNOR:
      state.breathState = "normal";
      updated = true;
      break;
    case BREATHRAPID:
      state.breathState = "rapid";
      updated = true;
      break;
    case BREATHSLOW:
      state.breathState = "slow";
      updated = true;
      break;
    case BREATHNONE:
      state.breathState = "none";
      updated = true;
      break;
    default:
      break;
  }

  radar.sensor_report = 0; // prevent duplicate handling

  if (!updated) {
    return;
  }

  if (state.presence != lastPublished.presence) {
    publishField("presence", state.presence ? "occupied" : "clear");
    lastPublished.presence = state.presence;
  }

  if (state.activity != lastPublished.activity) {
    publishField("activity", state.activity);
    lastPublished.activity = state.activity;
  }

  if (state.distance != lastPublished.distance) {
    publishField("distance_m", String(state.distance, 2));
    lastPublished.distance = state.distance;
  }

  if (state.heartRate != lastPublished.heartRate) {
    publishField("heart_rate_bpm", String(state.heartRate));
    lastPublished.heartRate = state.heartRate;
  }

  if (state.breathRate != lastPublished.breathRate) {
    publishField("breath_rate_bpm", String(state.breathRate));
    lastPublished.breathRate = state.breathRate;
  }

  if (state.breathState != lastPublished.breathState) {
    publishField("breath_state", state.breathState);
    lastPublished.breathState = state.breathState;
  }

  publishSnapshot();
}

void publishField(const char *subtopic, const String &payload, bool retain) {
  char topic[128];
  snprintf(topic, sizeof(topic), "%s/%s", MQTT_BASE_TOPIC, subtopic);
  mqttClient.publish(topic, payload.c_str(), retain);
}

void publishSnapshot() {
  String json = "{";
  json += "\"presence\":\"" + String(state.presence ? "occupied" : "clear") + "\",";
  json += "\"activity\":\"" + state.activity + "\",";
  json += "\"distance_m\":" + String(state.distance, 2) + ",";
  json += "\"heart_rate_bpm\":" + String(state.heartRate) + ",";
  json += "\"breath_rate_bpm\":" + String(state.breathRate) + ",";
  json += "\"breath_state\":\"" + state.breathState + "\"";
  json += "}";

  publishField("state", json, true);
}
