/**
 * ================================================================
 * FINAL FIRMWARE: ICM-20948 + WebSocket Server (SPI Mode)
 * ================================================================
 * - Wi-Fi Access Point: ESP32-Dashboard (Password: 12345678)
 * - WebSocket server pushes data every 50ms (20Hz).
 * - If sensor is missing/wired wrong, automatically falls back to sine waves.
 * ================================================================
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <ICM_20948.h>
#include <LittleFS.h>

// --------------------- SPI PINS ---------------------
#define ICM_SCK   12
#define ICM_MOSI  11
#define ICM_MISO  13
#define ICM_CS    10

// --------------------- GLOBAL OBJECTS ---------------------
ICM_20948_SPI IMU;          // SPI subclass for the sensor
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool sensorOnline = false;  // Global flag to track sensor status

// --------------------- SENSOR INITIALIZATION ---------------------
bool initSensor() {
  Serial.print("[SENSOR] Initializing ICM-20948 over SPI...");
  pinMode(ICM_CS, OUTPUT);
  digitalWrite(ICM_CS, HIGH);
  
  // Initialize SPI bus with your custom pins
  SPI.begin(ICM_SCK, ICM_MISO, ICM_MOSI, ICM_CS);

  for (int i = 0; i < 3; i++) {
    // begin(CS, SPI) is the correct signature for SPI mode
    if (IMU.begin(ICM_CS, SPI) == ICM_20948_Stat_Ok) {
      Serial.println(" SUCCESS!");
      return true;
    }
    Serial.print(".");
    delay(500);
  }
  Serial.println(" FAILED.");
  return false;
}

// --------------------- WEBSOCKET EVENT HANDLER ---------------------
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[WS] ✅ Client #%u CONNECTED!\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WS] ❌ Client #%u DISCONNECTED.\n", client->id());
  }
}

// --------------------- SEND SENSOR DATA (Real or Fallback) ---------------------
void sendSensorData() {
  static float fake_angle = 0;
  JsonDocument doc;

  if (sensorOnline) {
    // --- REAL SENSOR DATA ---
    if (IMU.dataReady()) {
      IMU.getAGMT();  // Reads Accel, Gyro, Mag, Temp

      doc["ax"] = IMU.accX();
      doc["ay"] = IMU.accY();
      doc["az"] = IMU.accZ();
      doc["gx"] = IMU.gyrX();
      doc["gy"] = IMU.gyrY();
      doc["gz"] = IMU.gyrZ();
      doc["mx"] = IMU.magX();
      doc["my"] = IMU.magY();
      doc["mz"] = IMU.magZ();
      doc["temp"] = IMU.temp();
    } else {
      // Sensor is online but data isn't ready yet (skip this frame)
      return;
    }
  } else {
    // --- FALLBACK: SINE WAVES (if sensor failed or not connected) ---
    fake_angle += 0.1;
    doc["ax"] = sin(fake_angle);
    doc["ay"] = cos(fake_angle * 0.8);
    doc["az"] = 0.5;
    doc["gx"] = 0; 
    doc["gy"] = 0; 
    doc["gz"] = 0;
    doc["mx"] = 0; 
    doc["my"] = 0; 
    doc["mz"] = 0;
    doc["temp"] = 25.0;
  }

  String jsonString;
  serializeJson(doc, jsonString);

  if (ws.count() > 0) {
    ws.textAll(jsonString);
  }
}

// --------------------- SETUP ---------------------
void setup() {
  Serial.begin(115200);
  Serial.println("\n\n===== FINAL FIRMWARE (SENSOR + WS) =====");

  // 1. Mount LittleFS (contains index.html & chart.min.js)
  if (!LittleFS.begin()) {
    Serial.println("[FS] ERROR: LittleFS mount failed! Did you upload the filesystem?");
    return;
  }
  Serial.println("[FS] LittleFS mounted OK.");

  // 2. Initialize Sensor
  sensorOnline = initSensor();
  if (!sensorOnline) {
    Serial.println("[SENSOR] WARNING: Running in FALLBACK mode (sine waves). Check wiring!");
  }

  // 3. Start Wi-Fi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-Dashboard", "12345678");
  Serial.printf("[WiFi] AP IP: %s\n", WiFi.softAPIP().toString().c_str());

  // 4. Setup WebSocket
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // 5. Serve Static Files (HTML/JS)
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // 6. Start HTTP Server
  server.begin();
  Serial.println("[HTTP] Server started.");
  Serial.println("========================================");
  Serial.println("Open browser to: http://192.168.4.1");
  Serial.println("========================================");
}

// --------------------- MAIN LOOP ---------------------
void loop() {
  static unsigned long lastSend = 0;

  if (millis() - lastSend > 50) {  // 20Hz
    sendSensorData();
    lastSend = millis();
  }

  ws.cleanupClients();
}