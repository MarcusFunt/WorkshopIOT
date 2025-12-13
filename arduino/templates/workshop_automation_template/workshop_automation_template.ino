/*
  Workshop Automation Template
  ----------------------------
  Purpose: Describe the automation goal (e.g., control shop lights, trigger dust collection, monitor temperature).

  Hardware:
  - Board: <Board name>
  - Pins: <Pin mappings>
  - Components: <Switches, relays, sensors>

  Notes:
  - Document wiring here.
  - Keep logic small and focused; use helper functions for repeated actions.
*/

// Pin assignments
const int RELAY_PIN = 8;   // Example output pin
const int SENSOR_PIN = A0; // Example input pin

// Timing and thresholds
const unsigned long DEBOUNCE_MS = 50;
const int SENSOR_THRESHOLD = 600;

unsigned long lastChange = 0;
bool relayState = false;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(9600);
  Serial.println("Workshop automation template starting...");
}

void loop() {
  const int sensorValue = analogRead(SENSOR_PIN);
  const unsigned long now = millis();

  if (shouldToggle(sensorValue, now)) {
    toggleRelay();
    logState(sensorValue);
  }
}

bool shouldToggle(int sensorValue, unsigned long now) {
  if (sensorValue < SENSOR_THRESHOLD) {
    return false;
  }

  const bool allowed = now - lastChange >= DEBOUNCE_MS;
  return allowed;
}

void toggleRelay() {
  relayState = !relayState;
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
  lastChange = millis();
}

void logState(int sensorValue) {
  Serial.print("Sensor: ");
  Serial.print(sensorValue);
  Serial.print(" | Relay: ");
  Serial.println(relayState ? "ON" : "OFF");
}
