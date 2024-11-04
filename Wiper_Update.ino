const int relayLowPin = 3;     // Output pin for Low Speed relay control
const int relayHighPin = 2;    // Output pin for High Speed relay control
const int relayWashPin = 4;    // Output pin for Wash relay control
const int limitswitchPin = 5;  // Input pin for position sensor or park switch

// Timing variables
unsigned long lastWipeTime = 0;           // Last time the wiper motor was activated
const unsigned long wipeInterval = 3000;  // Interval between wipes in AUTO mode (3 seconds)
const unsigned long washDuration = 2000;  // Duration for wash mode (2 seconds)
bool isWashing = false;                   // Flag for wash mode

void setup() {
  Serial.begin(9600);
  pinMode(A7, INPUT_PULLUP);       // Analog input for mode selection
  pinMode(limitswitchPin, INPUT);  // Position sensor or park switch
  pinMode(relayLowPin, OUTPUT);    // Relay for low speed control
  pinMode(relayHighPin, OUTPUT);   // Relay for high speed control
  pinMode(relayWashPin, OUTPUT);   // Relay for wash control

  // Initialize all relays to off
  digitalWrite(relayLowPin, LOW);
  digitalWrite(relayHighPin, LOW);
  digitalWrite(relayWashPin, LOW);
}

void loop() {
  // Read inputs and compute voltage
  int rawReading = analogRead(A7);
  float B = rawReading * 0.0048;        // Convert reading to voltage
  int C = digitalRead(limitswitchPin);  // Read park/position sensor

  // Auto Mode with periodic wipes
  if (B >= 1.35 && B <= 1.5) {
    if (millis() - lastWipeTime >= wipeInterval) {  // Check if it's time for another wipe
      startWipe();
      lastWipeTime = millis();  // Reset the wipe timer
    }
  }

  // Manual High Speed Mode
  else if (B >= 1.90 && B <= 2.7) {
    digitalWrite(relayLowPin, HIGH);   // Enable low speed relay
    digitalWrite(relayHighPin, HIGH);  // Enable high speed relay
    digitalWrite(relayWashPin, LOW);   // Disable wash relay
  }

  // Manual Low Speed Mode
  else if (B >= 0.65 && B <= 0.9) {
    digitalWrite(relayLowPin, HIGH);  // Enable low speed relay
    digitalWrite(relayHighPin, LOW);  // Disable high speed relay
    digitalWrite(relayWashPin, LOW);  // Disable wash relay
  }

  // Wash Mode with timing
  else if (B >= 0.45 && B <= 0.6 && !isWashing) {
    isWashing = true;                  // Set washing flag
    lastWipeTime = millis();           // Set wash start time
    digitalWrite(relayWashPin, HIGH);  // Enable wash relay
    digitalWrite(relayLowPin, HIGH);   // Enable low speed relay
  }
  if (isWashing && (millis() - lastWipeTime >= washDuration)) {  // Check wash duration
    isWashing = false;                                           // Clear washing flag
    digitalWrite(relayWashPin, LOW);                             // Disable wash relay
  }

  // OFF Mode or Park Position
  else if (B >= 4.80 || (B < 0.65 && C == LOW)) {  // Check if off voltage or park position
    digitalWrite(relayLowPin, LOW);
    digitalWrite(relayHighPin, LOW);
    digitalWrite(relayWashPin, LOW);
  }
}

// Function to start a wipe cycle
void startWipe() {
  digitalWrite(relayLowPin, HIGH);  // Enable low speed relay
  digitalWrite(relayHighPin, LOW);  // Ensure high speed relay is off
  delay(1000);                      // Allow time for one full wipe
  digitalWrite(relayLowPin, LOW);   // Disable low speed relay after wipe
}
