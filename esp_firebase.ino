#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <WiFi.h>
#include <Wire.h>

// Firebase initialization
#include "addons/TokenHelper.h"  // Provide the token generation process info.
#include "addons/RTDBHelper.h"   // Provide the RTDB payload printing info.

// Initialize Wi-Fi credentials
#define WIFI_SSID "Ritik"
#define WIFI_PASSWORD "8291189618"

// Firebase credentials
#define API_KEY "AIzaSyBVBlE_iton_QqHCuLqoKCBtHpv3R-SVKY"
#define DATABASE_URL "https://esp32-home-20456-default-rtdb.firebaseio.com"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Global Variables
bool signupOK = false;
const int espnum = 101;  // Dummy ESP ID
unsigned long sendDataPrevMillis = 0;

// Function to initialize WiFi connection
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

// Function to initialize Firebase
void initFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up to Firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Sign-up OK");
    signupOK = true;
  } else {
    Serial.printf("Sign-up Error: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;  // Firebase Token callback
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// Function to send dummy data to Firebase
void sendDataToFirebase() {
  if (Firebase.ready() && signupOK) {
    String path = "Sensors/ESP" + String(espnum);

    // Send dummy int data
    if (!Firebase.RTDB.setInt(&fbdo, path + "/dummy_int", 123)) {
      Serial.println("Failed to send dummy int data");
      Serial.println("Reason: " + fbdo.errorReason());
    } else {
      Serial.println("Dummy int data sent successfully.");
    }

    // Send dummy float data
    if (!Firebase.RTDB.setFloat(&fbdo, path + "/dummy_float", 45.67)) {
      Serial.println("Failed to send dummy float data");
      Serial.println("Reason: " + fbdo.errorReason());
    } else {
      Serial.println("Dummy float data sent successfully.");
    }

    // Send dummy string data
    if (!Firebase.RTDB.setString(&fbdo, path + "/dummy_string", "Hello Firebase!")) {
      Serial.println("Failed to send dummy string data");
      Serial.println("Reason: " + fbdo.errorReason());
    } else {
      Serial.println("Dummy string data sent successfully.");
    }
  }
}

// Function to read dummy data from Firebase
void readDataFromFirebase() {
  if (Firebase.ready() && signupOK) {
    String path = "Sensors/ESP" + String(espnum);

    // Read dummy integer
    if (Firebase.RTDB.getInt(&fbdo, path + "/dummy_int")) {
      Serial.print("Dummy Int: ");
      Serial.println(fbdo.intData());
    } else {
      Serial.println("Failed to get dummy int data");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Read dummy float
    if (Firebase.RTDB.getFloat(&fbdo, path + "/dummy_float")) {
      Serial.print("Dummy Float: ");
      Serial.println(fbdo.floatData());
    } else {
      Serial.println("Failed to get dummy float data");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Read dummy string
    if (Firebase.RTDB.getString(&fbdo, path + "/dummy_string")) {
      Serial.print("Dummy String: ");
      Serial.println(fbdo.stringData());
    } else {
      Serial.println("Failed to get dummy string data");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }
}

// Setup function to initialize everything
void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi
  initWiFi();

  // Initialize Firebase
  initFirebase();
}

// Main loop
void loop() {
  // Send dummy data to Firebase
  sendDataToFirebase();

  // Read dummy data from Firebase
  readDataFromFirebase();

  // Add a delay or condition for periodic checks
  delay(5000);  // Wait for 5 seconds before repeating
}
