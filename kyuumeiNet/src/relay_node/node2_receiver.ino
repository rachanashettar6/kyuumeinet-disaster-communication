/*
  Node 2: LoRa Base Station Receiver with LCD and Rescue Dashboard
  Processor: ESP32 DevKit v1 (or ESP32 WROOM-32)
  Description: Receives LoRa SOS packets directly. Displays coordinates on the 
               16x2 LCD screen and flashes the backlight. Simultaneously runs a 
               local WiFi AP "RESCUE_BASE_AP". Rescue operators can connect and view 
               a beautiful real-time dashboard listing all active alerts, RSSI, 
               and coordinate map links, with the ability to resolve alerts.
*/

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// --- PIN DEFINITIONS FOR LORA (ESP32) ---
#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5       // NSS (Chip Select)
#define RST 14     // Reset pin
#define DIO0 4     // Interrupt pin (Moved from 2 to 4 to avoid boot strapping conflicts)
#define BAND 433E6 // Change to 868E6 or 915E6 if needed

// --- LCD CONFIGURATION ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- WIFI CONFIGURATION ---
const char* ssid = "RESCUE_BASE_AP"; // Open WiFi network for rescue operators
IPAddress apIP(192, 168, 5, 1);       // Separate IP to prevent conflict with Node 1
IPAddress netMsk(255, 255, 255, 0);
WebServer server(80);
bool isConnectingWifi = false;
unsigned long wifiConnectStartMillis = 0;

// --- SOS ALERT STATE DATA ---
struct SOSAlert {
  unsigned long id;
  String lat;
  String lon;
  int rssi;
  unsigned long time; // Timestamp in millis
  bool active;
};

#define MAX_ALERTS 10
SOSAlert alerts[MAX_ALERTS];
int alertCount = 0;

// LCD Alarm variables
bool alertActive = false;
unsigned long lastFlashMillis = 0;
unsigned long alarmStartTime = 0; // Tracks when current alarm started
bool backlightState = true;
const unsigned long flashInterval = 200; // Toggle backlight every 200ms

bool loraActive = false; // Tracks if LoRa module is physically responsive

// --- FORWARD DECLARATIONS ---
bool safeSendLoRa(const String& payload);
bool addAlert(unsigned long id, String lat, String lon, int rssi);

// --- RTOS: LoRa SPI mutex (protects against concurrent TX/RX across cores) ---
SemaphoreHandle_t loraMutex = NULL;

// ACK job descriptor passed to the background task
struct AckJob {
  char   packet[64]; // Fixed buffer — avoids String heap issues across cores
  int    totalSends; // How many times to transmit
};

// ============================================================
//  RTOS TASK: Send ACK on Core 0 (non-blocking to main loop)
// ============================================================
void ackSendTask(void* param) {
  AckJob* job = (AckJob*)param;

  for (int i = 0; i < job->totalSends; i++) {
    if (i > 0) {
      vTaskDelay(pdMS_TO_TICKS(400)); // Non-blocking wait between retransmits
    }
    if (!loraActive) continue;
    // Acquire LoRa mutex before touching SPI bus
    if (xSemaphoreTake(loraMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
      LoRa.beginPacket();
      LoRa.print(job->packet);
      LoRa.endPacket();
      xSemaphoreGive(loraMutex);
      Serial.printf("[ACK TASK] Sent %d/%d: %s\n", i + 1, job->totalSends, job->packet);
    } else {
      Serial.println("[ACK TASK] WARNING: Could not acquire LoRa mutex — skipped.");
    }
  }

  free(param);     // Release heap memory
  vTaskDelete(NULL); // Self-delete task
}

// --- HTML EMERGENCY DASHBOARD (HTML/CSS/JS) ---
#include "dashboard_html.h"


void handleRoot() {
  Serial.println("[HTTP] Root dashboard page requested by operator.");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  server.send_P(200, "text/html", dashboard_html);
}

void handleNotFoundReceiver() {
  server.send(404, "text/plain", "404: Not Found");
}

void handleManifestReceiver() {
  String json = R"rawm({
  "name": "Rescue Command Center",
  "short_name": "Rescue HQ",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#07070a",
  "theme_color": "#07070a",
  "description": "LoRa Base Station Rescue Dashboard",
  "orientation": "any",
  "icons": [
    {
      "src": "/logo.svg",
      "sizes": "512x512",
      "type": "image/svg+xml",
      "purpose": "any maskable"
    }
  ]
})rawm";
  server.send(200, "application/json", json);
}

void handleLogoReceiver() {
  String svg = R"rawl(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <rect width="512" height="512" rx="100" fill="#07070a"/>
  <circle cx="256" cy="200" r="70" fill="none" stroke="#00ff88" stroke-width="8" opacity="0.3"/>
  <circle cx="256" cy="200" r="45" fill="none" stroke="#00ff88" stroke-width="6" opacity="0.5"/>
  <circle cx="256" cy="200" r="12" fill="#00ff88"/>
  <text x="256" y="370" font-family="sans-serif" font-size="60" font-weight="900" fill="#ffffff" text-anchor="middle">RESCUE</text>
  <text x="256" y="420" font-family="sans-serif" font-size="28" font-weight="700" fill="#00ff88" text-anchor="middle">COMMAND</text>
</svg>)rawl";
  server.send(200, "image/svg+xml", svg);
}

void handleSwReceiver() {
  String sw = R"raws(
self.addEventListener('install', (e) => { self.skipWaiting(); });
self.addEventListener('activate', (e) => { return self.clients.claim(); });
self.addEventListener('fetch', (e) => { e.respondWith(fetch(e.request)); });
)raws";
  server.send(200, "application/javascript", sw);
}

bool checkOptionsAndSendCors() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.sendHeader("Access-Control-Allow-Private-Network", "true"); // Fix Chrome Private Network Access blocking
    server.send(204);
    return true;
  }
  return false;
}

void handleGetAlerts() {
  if (checkOptionsAndSendCors()) return;
  
  // Rate-limit Serial prints: print once every 10 fetches (20 seconds) to avoid spamming the console
  static int getAlertsCount = 0;
  if (++getAlertsCount >= 10) {
    getAlertsCount = 0;
    Serial.println("[HTTP] Client polling active SOS alerts.");
  }
  String json = "{\"serverMillis\":" + String(millis()) + ",";
  json += "\"alerts\":[";
  bool first = true;
  for (int i = alertCount - 1; i >= 0; i--) { // Newest first
    if (!first) json += ",";
    json += "{\"id\":" + String(alerts[i].id) + ",";
    json += "\"lat\":\"" + alerts[i].lat + "\",";
    json += "\"lon\":\"" + alerts[i].lon + "\",";
    json += "\"rssi\":" + String(alerts[i].rssi) + ",";
    json += "\"timestamp\":" + String(alerts[i].time) + ",";
    json += "\"time\":" + String(alerts[i].time) + ",";
    json += "\"active\":" + String(alerts[i].active ? "true" : "false") + "}";
    first = false;
  }
  json += "]}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleResolve() {
  if (checkOptionsAndSendCors()) return;
  unsigned long id = server.arg("id").toInt();
  String resolvedLat = "";
  String resolvedLon = "";

  for (int i = 0; i < alertCount; i++) {
    if (alerts[i].id == id) {
      alerts[i].active = false;
      resolvedLat = alerts[i].lat;
      resolvedLon = alerts[i].lon;
      Serial.print("[RESOLVE] SOS alert #");
      Serial.print(id);
      Serial.print(" resolved. Coords: ");
      Serial.print(resolvedLat);
      Serial.print(", ");
      Serial.println(resolvedLon);
    }
  }

  // Spawn RTOS task on Core 0 to send 3 ACKs with non-blocking delays.
  // Main loop (Core 1) continues receiving new SOS packets immediately.
  AckJob* job = (AckJob*)malloc(sizeof(AckJob));
  if (job != NULL) {
    snprintf(job->packet, sizeof(job->packet), "ACK:%lu:0", id);
    job->totalSends = 3;
    BaseType_t created = xTaskCreatePinnedToCore(
      ackSendTask,  // Task function
      "AckTask",    // Task name
      4096,         // Stack size in bytes
      (void*)job,   // Parameter passed to task
      1,            // Priority (1 = low, above idle)
      NULL,         // Task handle (not needed)
      0             // Pin to Core 0
    );
    if (created == pdPASS) {
      Serial.println("[RESOLVE] ACK task spawned on Core 0: " + String(job->packet));
    } else {
      Serial.println("[RESOLVE] WARNING: Could not spawn ACK task — sending synchronously.");
      // Fallback: send once synchronously if task creation fails
      safeSendLoRa(String(job->packet));
      free(job);
    }
  } else {
    Serial.println("[RESOLVE] ERROR: malloc failed for AckJob.");
  }

  // Update LCD to show resolved status with coordinates
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RESCUED! ACK Sent");
  lcd.setCursor(0, 1);
  if (resolvedLat.length() > 0) {
    lcd.print(resolvedLat.substring(0, 7) + "," + resolvedLon.substring(0, 7));
  } else {
    lcd.print("Alert Cleared");
  }

  // Check if any active alerts remain
  bool anyActive = false;
  for (int i = 0; i < alertCount; i++) {
    if (alerts[i].active) {
      anyActive = true;
      break;
    }
  }

  if (!anyActive) {
    alertActive = false;
  }

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK");
}

void handleScanWifi() {
  if (checkOptionsAndSendCors()) return;
  int n = WiFi.scanNetworks();
  String json = "{\"networks\":[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"secure\":" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true") + "}";
  }
  json += "]}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleConnectWifi() {
  if (checkOptionsAndSendCors()) return;
  String ssidParam = server.arg("ssid");
  String passParam = server.arg("pass");
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (ssidParam.length() > 0) {
    WiFi.disconnect(); // Clear any existing connection/stuck attempts
    delay(100);
    WiFi.begin(ssidParam.c_str(), passParam.c_str());
    isConnectingWifi = true;
    wifiConnectStartMillis = millis();
    server.send(200, "application/json", "{\"status\":\"CONNECTING\"}");
    Serial.println("[WIFI] Connecting to " + ssidParam + "...");
  } else {
    server.send(400, "application/json", "{\"status\":\"ERROR\",\"message\":\"SSID required\"}");
  }
}

void handleWifiStatus() {
  if (checkOptionsAndSendCors()) return;
  
  wl_status_t status = WiFi.status();
  
  // If we were connecting, check if we connected or failed, or timed out
  if (isConnectingWifi) {
    if (status == WL_CONNECTED) {
      isConnectingWifi = false;
    } else if (status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL) {
      isConnectingWifi = false;
    } else if (millis() - wifiConnectStartMillis > 20000) { // 20s timeout
      WiFi.disconnect();
      isConnectingWifi = false;
      status = WL_CONNECT_FAILED; // Report failed due to timeout
    }
  }

  String json = "{";
  json += "\"status\":\"";
  if (status == WL_CONNECTED) {
    json += "CONNECTED\",";
    json += "\"ssid\":\"" + WiFi.SSID() + "\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI());
  } else if (isConnectingWifi) {
    json += "CONNECTING\"";
  } else if (status == WL_CONNECT_FAILED) {
    json += "FAILED\"";
  } else if (status == WL_NO_SSID_AVAIL) {
    json += "NO_SSID\"";
  } else {
    json += "DISCONNECTED\"";
  }
  json += "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

bool addAlert(unsigned long id, String lat, String lon, int rssi) {
  // Check if this ID is already registered in memory
  for (int i = 0; i < alertCount; i++) {
    if (alerts[i].id == id) {
      bool wasActive = alerts[i].active;
      alerts[i].rssi = rssi;
      alerts[i].time = millis();
      alerts[i].active = true;
      return !wasActive; // Returns true if it was inactive (reactivated)
    }
  }

  // Cache new alert
  if (alertCount < MAX_ALERTS) {
    alerts[alertCount] = {id, lat, lon, rssi, millis(), true};
    alertCount++;
  } else {
    // Shift left to overwrite oldest alert
    for (int i = 0; i < MAX_ALERTS - 1; i++) {
      alerts[i] = alerts[i + 1];
    }
    alerts[MAX_ALERTS - 1] = {id, lat, lon, rssi, millis(), true};
  }
  return true; // Brand new alert
}

void setup() {
  Serial.begin(115200);

  Serial.println("==========================================");
  Serial.println("   ESP32 LoRa Base Station + Dashboard    ");
  Serial.println("==========================================");

  // Initialize LCD Screen
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("LoRa Base Station");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Setup WiFi Access Point + Station Mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid);

  // Start mDNS so dashboard is accessible at http://rescue.local
  if (MDNS.begin("rescue")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS started: http://rescue.local");
  }

  Serial.print("Base Station AP SSID: ");
  Serial.println(ssid);
  Serial.print("Dashboard URL: http://rescue.local or http://");
  Serial.println(WiFi.softAPIP());

  // Initialize LoRa SPI
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  Serial.print("Starting LoRa at ");
  Serial.print(BAND / 1000000.0);
  Serial.println(" MHz...");

  if (!LoRa.begin(BAND)) {
    Serial.println("Error: Starting LoRa failed! Running in WiFi Dashboard mode only.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LoRa Init Failed");
    lcd.setCursor(0, 1);
    lcd.print("Check Wiring!");
    loraActive = false;
    delay(3000); // Show warning for 3 seconds before continuing
  } else {
    Serial.println("LoRa Initialization successful.");
    loraActive = true;
  }

  // Create LoRa SPI mutex for RTOS multi-core safety
  loraMutex = xSemaphoreCreateMutex();
  if (loraMutex == NULL) {
    Serial.println("[ERROR] Failed to create loraMutex! System may be unstable.");
  } else {
    Serial.println("RTOS mutex created. ACK task will run on Core 0.");
  }
  
  // Set WebServer routes
  server.on("/", handleRoot);
  server.on("/manifest.json", handleManifestReceiver);
  server.on("/logo.svg", handleLogoReceiver);
  server.on("/sw.js", handleSwReceiver);
  server.on("/get_alerts", handleGetAlerts);
  server.on("/resolve", handleResolve);
  server.on("/scan_wifi", handleScanWifi);
  server.on("/connect_wifi", handleConnectWifi);
  server.on("/wifi_status", handleWifiStatus);
  server.onNotFound(handleNotFoundReceiver);
  server.begin();
  
  Serial.println("Rescue Server started on port 80.");
  Serial.println("------------------------------------------");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LoRa Ready");
  lcd.setCursor(0, 1);
  lcd.print("Waiting for data");
}

bool safeSendLoRa(const String& payload) {
  if (!loraActive) return false;
  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket();
  return true;
}

void loop() {
  server.handleClient();

  // --- Track WiFi connection status for display ---
  static wl_status_t lastWifiStatus = WL_IDLE_STATUS;
  wl_status_t currentWifiStatus = WiFi.status();
  if (currentWifiStatus != lastWifiStatus) {
    lastWifiStatus = currentWifiStatus;
    if (currentWifiStatus == WL_CONNECTED) {
      isConnectingWifi = false;
      Serial.print("[WIFI] Connected! IP address: ");
      Serial.println(WiFi.localIP());
      // Update LCD to show new IP
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi: Connected");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP().toString());
      delay(3000);
      
      // Restore LoRa Ready view
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LoRa Ready");
      lcd.setCursor(0, 1);
      lcd.print("IP: " + WiFi.localIP().toString());
    } else if (currentWifiStatus == WL_CONNECT_FAILED || currentWifiStatus == WL_NO_SSID_AVAIL) {
      isConnectingWifi = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Connection");
      lcd.setCursor(0, 1);
      lcd.print("Failed/Lost");
      delay(2000);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LoRa Ready");
      lcd.setCursor(0, 1);
      lcd.print("Waiting for data");
    }
  }
  
  // --- Check Serial for incoming commands from PC dashboard ---
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.startsWith("RESOLVE:")) {
      unsigned long resolveID = input.substring(8).toInt();
      String resolvedLat = "";
      String resolvedLon = "";
      
      for (int i = 0; i < alertCount; i++) {
        if (alerts[i].id == resolveID) {
          alerts[i].active = false;
          resolvedLat = alerts[i].lat;
          resolvedLon = alerts[i].lon;
          Serial.print("[SERIAL CMD] SOS alert #");
          Serial.print(resolveID);
          Serial.println(" resolved via serial.");
        }
      }
      
      // Trigger ACK broadcast on Core 0 (non-blocking RTOS task)
      AckJob* job = (AckJob*)malloc(sizeof(AckJob));
      if (job != NULL) {
        snprintf(job->packet, sizeof(job->packet), "ACK:%lu:0", resolveID);
        job->totalSends = 3;
        BaseType_t created = xTaskCreatePinnedToCore(
          ackSendTask,  // Task function
          "AckTask",    // Task name
          4096,         // Stack size
          (void*)job,   // Parameter
          1,            // Priority
          NULL,         // Handle
          0             // Core 0
        );
        if (created != pdPASS) {
          // Fallback if task creation fails
          safeSendLoRa(String(job->packet));
          free(job);
        }
      }
      
      // Update LCD display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RESCUED! ACK Sent");
      lcd.setCursor(0, 1);
      if (resolvedLat.length() > 0) {
        lcd.print(resolvedLat.substring(0, 7) + "," + resolvedLon.substring(0, 7));
      } else {
        lcd.print("Alert Cleared");
      }
      
      // Re-evaluate if there are any remaining active alerts
      bool anyActive = false;
      for (int i = 0; i < alertCount; i++) {
        if (alerts[i].active) {
          anyActive = true;
          break;
        }
      }
      if (!anyActive) {
        alertActive = false;
      }
    }
  }

  unsigned long currentMillis = millis();

  // --- Toggle LCD alarm flashing if alert is active ---
  if (alertActive) {
    if (currentMillis - alarmStartTime < 1000) { // Flash for first 1 second to attract attention
      if (currentMillis - lastFlashMillis >= flashInterval) {
        lastFlashMillis = currentMillis;
        backlightState = !backlightState;
        if (backlightState) {
          lcd.backlight();
        } else {
          lcd.noBacklight();
        }
      }
    } else {
      // After 1 second, keep backlight solid ON so it is easy to read
      lcd.backlight();
    }
  }

  // --- Parse incoming packets (mutex guards SPI from concurrent ACK task) ---
  int packetSize = 0;
  String receivedText = "";
  int rxRssi = 0;
  
  bool lockAcquired = false;
  static unsigned long lastMutexFailMillis = 0;
  static unsigned long lastHeartbeatMillis = 0;
  
  if (loraMutex) {
    if (xSemaphoreTake(loraMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      lockAcquired = true;
    } else {
      if (millis() - lastMutexFailMillis >= 3000) {
        lastMutexFailMillis = millis();
        Serial.println("[ERROR] Failed to acquire loraMutex in loop!");
      }
    }
  } else {
    // Fallback: if mutex failed to create, allow non-mutex parsing rather than failing completely
    lockAcquired = true;
  }

  if (lockAcquired && loraActive) {
    if (millis() - lastHeartbeatMillis >= 5000) {
      lastHeartbeatMillis = millis();
      Serial.println("[Heartbeat] loraMutex acquired, parsing packets...");
    }
    packetSize = LoRa.parsePacket();
    if (packetSize) {
      while (LoRa.available()) {
        receivedText += (char)LoRa.read();
      }
      rxRssi = LoRa.packetRssi();
    }
    if (loraMutex) {
      xSemaphoreGive(loraMutex);
    }
  }

  if (packetSize) {
    
    Serial.print("Received: '");
    Serial.print(receivedText);
    Serial.print("' RSSI: ");
    Serial.println(rxRssi);

    // Parse SOS packet — format from Node 1: "SOS:[ID]:[LAT]:[LON]"
    // Format from relay Node 3:             "SOS:[ID]:[LAT]:[LON]:[HOPS]"
    if (receivedText.startsWith("SOS:")) {
      int colon1 = receivedText.indexOf(':');              // after "SOS"
      int colon2 = receivedText.indexOf(':', colon1 + 1); // after ID
      int colon3 = receivedText.indexOf(':', colon2 + 1); // after LAT
      int colon4 = receivedText.indexOf(':', colon3 + 1); // after LON (hop field, may be -1)
      
      if (colon1 != -1 && colon2 != -1 && colon3 != -1) {
        String idStr  = receivedText.substring(colon1 + 1, colon2);
        String latStr = receivedText.substring(colon2 + 1, colon3);
        // Strip hop count if present: take lon only up to colon4, or to end if no colon4
        String lonStr = (colon4 > colon3) ? receivedText.substring(colon3 + 1, colon4)
                                          : receivedText.substring(colon3 + 1);
        unsigned long packetID = idStr.toInt();

        // Register the alert in memory and check if it is new or reactivated
        bool isNew = addAlert(packetID, latStr, lonStr, rxRssi);
        
        if (isNew) {
          // Trigger alarm
          alertActive = true;
          alarmStartTime = currentMillis;
          lastFlashMillis = currentMillis;
          backlightState = true;
          lcd.backlight();

          // Display on LCD: row 0 shows alert and RSSI, row 1 shows coordinates
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("SOS! RSSI:" + String(rxRssi) + "dB");
          lcd.setCursor(0, 1);
          lcd.print(latStr.substring(0, 7) + "," + lonStr.substring(0, 7));

          Serial.print(">>> SOS DETECTED from Packet #");
          Serial.print(packetID);
          Serial.print(" | Coordinates: ");
          Serial.print(latStr);
          Serial.print(", ");
          Serial.println(lonStr);
          Serial.println("[INFO] Waiting for rescue operator to click Resolve to send ACK.");
        } else {
          // Silent update for duplicate active alert packets
          Serial.print("[LORA] Duplicate SOS packet #");
          Serial.print(packetID);
          Serial.print(" received (RSSI: ");
          Serial.print(rxRssi);
          Serial.println("dB)");
        }
      }
    }
  }
}
