#include <Wire.h>
#include <time.h>
#include <WiFi.h>
#include <rdm6300.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <WiFiManager.h>
#include <Adafruit_GFX.h>
#include <SoftwareSerial.h>
#include <Adafruit_SH110X.h>
#include <Firebase_ESP_Client.h>

const char* NTP_SERVER = "pool.ntp.org";
const long gmtOffsetSec = 3600 * 6;
const int daylightOffsetSec = 0;

// OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Reset pin for OLED (not used with ESP32)
#define OLED_RESET -1
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Serial for RDM6300
Rdm6300 rdm6300;

#define BLACK 0
#define WHITE 1

// Firebase and Firestore includes
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define API_KEY "MY_API_KEY_HERE"
#define FIREBASE_PROJECT_ID "MY_PROJECT_ID"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long previousMillis = 0;
unsigned long previousCardMillis = 0;
const long secondInterval = 1000;
const long gpsPushinterval = 5000;

// Miscellaneous
uint32_t chipId;
String latitude = "", longitude = "";

// Pin mapping
#define BUZZER_PIN 19
#define LED_PIN 2

// GPS settings
TinyGPSPlus gps;

SoftwareSerial gpsSerial(13, 12);
char timeString[64];
char dateString[64];
char dayString[64];

WiFiManager wm;
bool internetConnected = false;
unsigned long gpsStartTime;
bool ntpTimeValid = false;

void setupWiFi() {
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(120);
  if (wm.autoConnect()) {
    internetConnected = true;
    Serial.println(F("WiFi connected"));
    configTime(gmtOffsetSec, daylightOffsetSec, NTP_SERVER);
    oledDisplay(0, "WiFi Connected", WiFi.localIP().toString());
  } else {
    display.clearDisplay();
    internetConnected = false;
    display.display();
    oledDisplay(0, "WiFi Failed", "Restarting...");
    oledDisplay(1, wm.getConfigPortalSSID(), WiFi.softAPIP().toString());
  }
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  gpsSerial.begin(9600);
  rdm6300.begin(4);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  if (!display.begin(0x3C)) {
    Serial.println(F("SH1106 allocation failed"));
    for (;;)
      ;
  } else {
    Serial.println(F("Initialized OLED display"));
  }

  setupWiFi();
  setupFirebase();

  printChipId();
  if (internetConnected)
    oledDisplay(2, "Place your card...", "");
  printLocalTime();
  digitalWrite(LED_PIN, HIGH);
  Serial.printf("\n%s\t | %s\t | %s\n", __FILE__, __DATE__, __TIME__);
  gpsStartTime = millis();
}

void loop() {
  wm.process();
  printLocalTime();
  handleGPS();
  checkCard();
  updtateDisplay();
}

void updtateDisplay() {
  unsigned long currentMillis = millis();             // Get the current time
  if (currentMillis - previousCardMillis >= 10000) {  // Check if 10 seconds have passed
    previousCardMillis = currentMillis;               // Save the current time
    oledDisplay(2, "Place your card...", "");
  }
}

void checkCard() {
  if (rdm6300.get_new_tag_id()) {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, rdm6300.get_tag_id());
    String cardID = String(rdm6300.get_tag_id(), DEC);
    oledDisplay(2, "Card No: " + cardID, "Data uploading!");
    digitalWrite(BUZZER_PIN, LOW);
    if (ntpTimeValid) {
      pushCardData(cardID);
      if (!latitude.isEmpty() && !longitude.isEmpty()) {
        pushLocationData();
      }
      oledDisplay(2, "Card No: " + cardID, "Data uploaded!");
    } else {
      oledDisplay(2, "Card No: " + cardID, "No NTP/Location!");
    }
    digitalWrite(LED_PIN, HIGH);
  }
}

void setupFirebase() {
  config.api_key = xxxxxxxxx;
  config.token_status_callback = tokenStatusCallback;
  config.database_url = "https://xxxxxxxxx.firebaseio.com";
  config.signer.tokens.legacy_token = "xxxxxxxxxxxxxxxxxxx";
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (!Firebase.ready()) {
    Serial.println(F("Failed to initialize Firebase"));
    oledDisplay(2, "Firebase Error", "Check Config");
  } else {
    Serial.println(F("Firebase Initialized"));
    oledDisplay(2, "Firebase Ready", "");
  }
}

void printChipId() {
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  Serial.println("Device ID: " + String(chipId));
}

void handleGPS() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  if (gps.location.isValid()) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    oledDisplay(1, "Lat: " + latitude, "Lon: " + longitude);
    if (millis() - gpsStartTime > gpsPushinterval) {  // Check if 5 seconds have passed
      pushLocationData();
      gpsStartTime = millis();
    }
  } else {
    oledDisplay(1, "GPS Data Invalid", "Check Signal");
  }
}

void pushLocationData() {
  if (!latitude.isEmpty() && !longitude.isEmpty()) {
    String path = "/current_location_table";
    FirebaseJson content;  // Create a FirebaseJson object
    content.set("lat", latitude.toDouble());
    content.set("lon", longitude.toDouble());
    if (Firebase.RTDB.pushJSON(&fbdo, path.c_str(), &content)) {
      // Serial.println(F("Data pushed successfully."));
    } else {
      Serial.printf("Error pushing data: %s\n", fbdo.errorReason().c_str());
    }
  }
}

void pushCardData(String data) {
  String path = "/scanned_card";                                // Push string data
  FirebaseJson content;                                         // Create a FirebaseJson object
  content.set("card_no", data);                                 // Populate the JSON object with data
  if (Firebase.RTDB.pushJSON(&fbdo, path.c_str(), &content)) {  // Push the JSON object to Firebase
    // Serial.println(F("Data pushed successfully."));
  } else {
    Serial.printf("Error pushing data: %s\n", fbdo.errorReason().c_str());
  }
}

void oledDisplay(const int line, const String& line1, const String& line2) {
  if (line < 0 || line > 2) return;        // Ensure valid line index
  const int yPositions[] = { 0, 20, 40 };  // Line y-coordinates
  const int lineHeight = 20;               // Line height
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.fillRect(0, line * lineHeight, SCREEN_WIDTH, lineHeight, 0);  // Clear the line
  display.setCursor(0, yPositions[line]);
  display.println(line1);
  display.println(line2);
  display.display();
}

void printLocalTime() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= secondInterval) {
    previousMillis = currentMillis;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      oledDisplay(0, "NTP Time Invalid", "Check Internet");
      ntpTimeValid = false;
      return;
    }
    ntpTimeValid = true;
    strftime(dayString, sizeof(dayString), "Day: %A", &timeinfo);
    strftime(dateString, sizeof(dateString), "Date: %d %b %Y", &timeinfo);
    strftime(timeString, sizeof(timeString), "Time: %H:%M:%S", &timeinfo);
    oledDisplay(0, dateString, timeString);
  }
}