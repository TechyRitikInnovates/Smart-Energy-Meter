#include <FS.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>  // OLED SH1106 Library
#include <Firebase_ESP_Client.h>
#include <PZEM004Tv30.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SH1106 display(21, 22);

const int resetButtonPin = 0;      // GPIO 0 for the reset button
const int energyResetButtonPin = 12; // GPIO 2 for the energy reset button
bool resetButtonPressed = false;
bool energyResetButtonPressed = false;

unsigned long previousMillis = 0;  // Stores the last time the WiFi check was performed
const long interval = 5000;        // Interval to check the WiFi connection (5 seconds)

unsigned long previousSensorMillis = 0;  // Stores the last time the sensor was checked
const long sensorInterval = 3000;        // Interval to check sensor data (3 seconds)

// Replace with your Firebase project credentials
#define FIREBASE_HOST "energy-meter-2deb1-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "wR5Z4ywkNRcdO8SvhTMJsVAdbjrbjAoAeMVFaRJc"

// Google Sheets URL
const String sheet_url = "https://script.google.com/macros/s/AKfycbz1OH3YiWgKvrJcJJJFdQ2gcYe8fZuDQ18ytSBtbxQkUYlIJKvkUE7MTQDXt1s36j8/exec";

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 16, 17);  // Use GPIO 16 and 17 for Serial2
#else
PZEM004Tv30 pzem(Serial2);
#endif

// Variable declarations
float voltage = 0;
float current = 0;
float pf = 0;
float frequency = 0;
float energy = 0;
float power = 0;

// Declare WiFiManager globally
WiFiManager wifiManager;

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialize OLED display
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);      // Set text size
  display.setTextColor(WHITE); // Set text color

  // Display initial message
  display.setCursor(0, 0);
  display.print("Connecting WiFi...");
  display.display(); // Actually draw it on the screen

  // Initialize button pin
  pinMode(resetButtonPin, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(energyResetButtonPin, INPUT_PULLUP); 

  wifiManager.setBreakAfterConfig(true);

  // Try to connect to last known settings
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("Failed to connect, resetting...");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Resetting ESP...");
    display.display();
    delay(3000);
    ESP.restart();
  }

  Serial.println("Connected...yeey :)");
  Serial.println("Local IP: ");
  Serial.println(WiFi.localIP());

  // Display connection status on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connected!");
  display.setCursor(0, 16);
  display.print("IP: ");
  display.print(WiFi.localIP());
  display.display();

  // Configure Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  config.timeout.serverResponse = 10000;
}

void loop() {
  unsigned long currentMillis = millis();

  // WiFi Check Interval
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected! Attempting to reconnect...");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Reconnecting...");
      display.display();

      if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
        Serial.println("Reconnection failed, resetting...");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Reconnect Fail");
        display.display();
        delay(3000);
        ESP.restart();
      }
    } else {
      Serial.print("Connected. Local IP: ");
      Serial.println(WiFi.localIP());

      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Connected!");
      display.setCursor(0, 16);
      display.print("IP: ");
      display.print(WiFi.localIP());
      display.display();
    }
  }

  // Sensor Data Check Interval
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;

    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energy = pzem.energy();
    frequency = pzem.frequency();
    pf = pzem.pf();

    if (isnan(voltage) || isnan(current) || isnan(power) || isnan(energy) || isnan(frequency) || isnan(pf)) {
      Serial.println("Error reading sensor values");
      return;
    }

    Serial.printf("Voltage: %.2fV\n", voltage);
    Serial.printf("Current: %.2fA\n", current);
    Serial.printf("Power: %.2fW\n", power);
    Serial.printf("Energy: %.3fkWh\n", energy);
    Serial.printf("Frequency: %.1fHz\n", frequency);
    Serial.printf("PF: %.2f\n\n", pf);

    // Display the sensor values on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Voltage: " + String(voltage) + "V");
    display.println("Current: " + String(current) + "A");
    display.println("Power: " + String(power) + "W");
    display.println("Energy: " + String(energy, 3) + "kWh");
    display.println("Frequency: " + String(frequency, 1) + "Hz");
    display.println("PF: " + String(pf));
    display.display();
    delay(3000);

    // Path to store data in Firebase
    String path = "/sensorData";

    // Send data to Firebase with error handling
    if (Firebase.RTDB.setFloat(&fbdo, path + "/voltage", voltage)) {
      Serial.println("Voltage data sent successfully");
    } else {
      Serial.println("Failed to send voltage data: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, path + "/current", current)) {
      Serial.println("Current data sent successfully");
    } else {
      Serial.println("Failed to send current data: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, path + "/power", power)) {
      Serial.println("Power data sent successfully");
    } else {
      Serial.println("Failed to send power data: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, path + "/energy", energy)) {
      Serial.println("Energy data sent successfully");
    } else {
      Serial.println("Failed to send energy data: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, path + "/frequency", frequency)) {
      Serial.println("Frequency data sent successfully");
    } else {
      Serial.println("Failed to send frequency data: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, path + "/pf", pf)) {
      Serial.println("PF data sent successfully");
    } else {
      Serial.println("Failed to send PF data: " + fbdo.errorReason());
    }

    // Send data to Google Sheets
    sendData(energy, power, voltage, current, pf, frequency);
  }

  // Check if the reset button is pressed
  if (digitalRead(resetButtonPin) == LOW) {
    if (!resetButtonPressed) {
      Serial.println("Resetting WiFi settings...");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Resetting WiFi...");
      display.display();
      delay(2000);
      wifiManager.resetSettings();
      ESP.restart();
    }
    resetButtonPressed = true;
  } else {
    resetButtonPressed = false;
  }
  if (digitalRead(energyResetButtonPin) == LOW) {
        if (!energyResetButtonPressed) {
            Serial.println("Resetting Energy...");
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print("Resetting Energy...");
            display.display();

            // Reset energy reading
            pzem.resetEnergy(); // Resets the energy reading in PZEM004T
            delay(2000); // Wait for a short period to show the reset message

            // Optionally, you could display the new energy value if needed
            energy = 0; // Reset the energy variable for local reference
            display.clearDisplay();
            display.setCursor(0, 0);
            display.print("Energy Reset!");
            display.display();
        }
        energyResetButtonPressed = true;
    } else {
        energyResetButtonPressed = false;
    }
}

// Function to send data to Google Sheets
void sendData(float energy, float power, float voltage, float current, float pf, float frequency) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // Note: Insecure can expose you to man-in-the-middle attacks!

    String requestUrl = sheet_url + "?energy=" + String(energy) + "&power=" + String(power) + "&voltage=" + String(voltage) + "&current=" + String(current) + "&pf=" + String(pf) + "&frequency=" + String(frequency);

    HTTPClient https;
    https.begin(client, requestUrl);

    int httpCode = https.GET();
    if (httpCode > 0) {
      Serial.printf("HTTP request sent successfully: %d\n", httpCode);
    } else {
      Serial.printf("Error sending HTTP request: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  }
}
