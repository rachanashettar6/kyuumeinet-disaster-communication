/*
  Node 1: User SOS Transmitter with Embedded Mobile App (Captive Portal)
  Processor: ESP32 WROOM-32 (30-Pin)
  Description: Establishes a local WiFi AP. Serves a beautiful, fully functional 
               offline Web App directly to connected devices (Captive Portal). 
               The Web App captures the phone's hardware GPS coordinates, sends 
               them to the ESP32, and polls for the rescue team's acknowledgment.
*/

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

// SPI LoRa Pins
#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5
#define RST 14
#define DIO0 4 // Interrupt pin (Moved from 2 to 4 to avoid boot strapping conflicts)
#define BAND 433E6

// WiFi configuration
const char* ssid = "SOS_EMERGENCY_AP";
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);

// SOS System States
String currentLat = "0.0000";
String currentLon = "0.0000";
volatile unsigned long currentPacketID = 0;
enum SystemStatus { IDLE, SENDING, ACKNOWLEDGED };
volatile SystemStatus systemStatus = IDLE; // volatile for multi-core access safety
volatile unsigned long lastSentPacketID = 0; // Tracks ID of the last successfully sent packet
bool loraActive = false; // Tracks if LoRa module is physically responsive

// --- RTOS: LoRa SPI mutex and background task ---
SemaphoreHandle_t loraMutex = NULL;
void loraTask(void* param);

// --- EMBEDDED READY WEB APP (HTML/CSS/JS) ---
#include "app_html.h"

void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  server.send(200, "text/html", app_html);
}

void handleManifest() {
  String json = R"rawliteral({
  "name": "SOS Emergency Beacon",
  "short_name": "SOS Portal",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#08080c",
  "theme_color": "#ff3344",
  "description": "Offline Earthquake SOS beacon portal",
  "orientation": "portrait",
  "icons": [
    {
      "src": "/logo.svg",
      "sizes": "512x512",
      "type": "image/svg+xml",
      "purpose": "any maskable"
    }
  ]
})rawliteral";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleLogo() {
  String svg = R"rawliteral(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <circle cx="256" cy="256" r="230" fill="#ff3344" />
  <circle cx="256" cy="256" r="180" fill="none" stroke="#ffffff" stroke-width="20" opacity="0.8" />
  <text x="256" y="280" font-family="sans-serif" font-size="70" font-weight="900" fill="#ffffff" text-anchor="middle">SOS</text>
</svg>)rawliteral";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "image/svg+xml", svg);
}

void handleServiceWorker() {
  String sw = R"rawliteral(
self.addEventListener('install', (e) => {
  self.skipWaiting();
});

self.addEventListener('activate', (e) => {
  return self.clients.claim();
});

self.addEventListener('fetch', (e) => {
  e.respondWith(fetch(e.request));
});
)rawliteral";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/javascript", sw);
}

bool safeSendLoRa(const String& payload) {
  if (!loraActive) return false;
  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket(); // Call synchronously to ensure packet completes transmission
  return true;
}

bool checkOptionsAndSendCors() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.sendHeader("Access-Control-Allow-Private-Network", "true");
    server.send(204);
    return true;
  }
  return false;
}

// Mobile app posts GPS coordinates here
void handleSendSOS() {
  if (checkOptionsAndSendCors()) return;
  String lat = server.arg("lat");
  String lon = server.arg("lon");
  
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");

  if (systemStatus == SENDING || systemStatus == ACKNOWLEDGED) {
    String response = "{\"status\":\"";
    if (systemStatus == SENDING) response += "SENDING";
    else response += "ACKNOWLEDGED";
    response += "\"}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", response);
    return;
  }

  if (lat.length() > 0 && lon.length() > 0) {
    // Acquire mutex to safely write to shared variables accessed by Core 0
    if (loraMutex && xSemaphoreTake(loraMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      currentLat = lat;
      currentLon = lon;
      currentPacketID = millis(); // Generate simple unique ID
      systemStatus = SENDING;
      xSemaphoreGive(loraMutex);
    } else {
      // Fallback
      currentLat = lat;
      currentLon = lon;
      currentPacketID = millis();
      systemStatus = SENDING;
    }
    
    Serial.println("\n--- SOS Triggered from Web App ---");
    Serial.print("Lat: "); Serial.println(currentLat);
    Serial.print("Lon: "); Serial.println(currentLon);
    Serial.print("ID:  "); Serial.println(currentPacketID);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"SENDING\"}");
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "application/json", "{\"status\":\"ERROR\", \"message\":\"Missing coordinates\"}");
  }
}

// Mobile app polls status here
void handleStatus() {
  if (checkOptionsAndSendCors()) return;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  String response = "{\"status\":\"";
  if (systemStatus == IDLE) response += "IDLE";
  else if (systemStatus == SENDING) response += "SENDING";
  else if (systemStatus == ACKNOWLEDGED) response += "ACKNOWLEDGED";
  response += "\"}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", response);
}

// Reset the system state back to IDLE
void handleResetStatus() {
  if (checkOptionsAndSendCors()) return;
  if (loraMutex && xSemaphoreTake(loraMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    systemStatus = IDLE;
    xSemaphoreGive(loraMutex);
  } else {
    systemStatus = IDLE;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK");
}

// Redirect all unknown requests to the Captive Portal root
void handleNotFound() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

// ============================================================
//  RTOS TASK: Monitor & Transmit LoRa packets on Core 0
// ============================================================
void loraTask(void* param) {
  unsigned long lastTxMillis = 0;
  const unsigned long txInterval = 4000; // Re-transmit every 4 seconds
  
  while (true) {
    if (loraActive) {
      unsigned long currentMillis = millis();

      // 1. Re-transmit SOS packet if in SENDING status
      if (systemStatus == SENDING) {
        // Auto-timeout after 60 seconds of no ACK
        if (currentMillis - currentPacketID >= 60000) {
          if (loraMutex && xSemaphoreTake(loraMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            Serial.println("\n[TIMEOUT] SOS Broadcast timed out. Resetting to IDLE.");
            systemStatus = IDLE;
            xSemaphoreGive(loraMutex);
          } else {
            systemStatus = IDLE;
          }
        } else if (currentMillis - lastTxMillis >= txInterval || currentPacketID != lastSentPacketID) {
          // Copy variables and send under a single mutex block for thread safety and SPI lock
          if (loraMutex && xSemaphoreTake(loraMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
            String latCopy = currentLat;
            String lonCopy = currentLon;
            unsigned long idCopy = currentPacketID;
            String payload = "SOS:" + String(idCopy) + ":" + latCopy + ":" + lonCopy;
            
            if (safeSendLoRa(payload)) {
              Serial.print("[LORA TASK] Broadcasting LoRa SOS... ID: ");
              Serial.println(idCopy);
              lastSentPacketID = idCopy; 
              lastTxMillis = currentMillis; // Update last transmit timestamp
            }
            xSemaphoreGive(loraMutex);
          }
        }
      }

      // 2. Listen for incoming LoRa ACK packets
      int packetSize = 0;
      String rxData = "";
      if (loraMutex && xSemaphoreTake(loraMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        packetSize = LoRa.parsePacket();
        if (packetSize) {
          while (LoRa.available()) {
            rxData += (char)LoRa.read();
          }
        }
        xSemaphoreGive(loraMutex);
      }

      if (packetSize && rxData.startsWith("ACK:")) {
        int colonPos = rxData.indexOf(':', 4);
        String idPart = (colonPos > 4) ? rxData.substring(4, colonPos) : rxData.substring(4);
        unsigned long ackID = idPart.toInt();
        
        // Mutex wrap to safely check and transition status
        if (loraMutex && xSemaphoreTake(loraMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          if (ackID == currentPacketID && systemStatus == SENDING) {
            systemStatus = ACKNOWLEDGED;
            Serial.print("[LORA TASK] ACK received! Rescue confirmed for packet ID: ");
            Serial.println(ackID);
          }
          xSemaphoreGive(loraMutex);
        } else {
          if (ackID == currentPacketID && systemStatus == SENDING) {
            systemStatus = ACKNOWLEDGED;
          }
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // Sleep briefly to yield CPU
  }
}

void setup() {
  Serial.begin(115200);
  
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("[WARNING] LoRa initialization failed! Check wiring. System starting in WiFi-only mode.");
    loraActive = false;
  } else {
    Serial.println("LoRa Active.");
    loraActive = true;
  }

  // Create LoRa SPI mutex for RTOS multi-core safety
  loraMutex = xSemaphoreCreateMutex();
  if (loraMutex == NULL) {
    Serial.println("[ERROR] Failed to create loraMutex! System may be unstable.");
  } else {
    Serial.println("RTOS mutex created. LoRa background task will run on Core 0.");
  }

  // Start background LoRa task on Core 0
  xTaskCreatePinnedToCore(
    loraTask,      // Task function
    "LoRaTask",    // Task name
    4096,          // Stack size
    NULL,          // Parameter
    1,             // Priority
    NULL,          // Task handle
    0              // Core 0
  );

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.on("/generate_204", handleRoot);  // Android captive portal support
  server.on("/fwlink", handleRoot);        // Windows captive portal support
  server.on("/send_sos", handleSendSOS);
  server.on("/status", handleStatus);
  server.on("/reset_status", handleResetStatus);
  server.onNotFound(handleNotFound);
  server.begin();
  
  Serial.println("Captive Portal with Embedded App ready.");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  delay(2); // yield to RTOS idle task
}
