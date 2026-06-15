/*
  Node 3: LoRa Mesh Relay / Repeater Node
  Processor: ESP32 DevKit v1 (or ESP32 WROOM-32)
  Description: Acts as an intelligent mesh relay between Node 1 (SOS Transmitter)
               and Node 2 (Base Station Receiver). Listens for SOS and ACK packets
               on 433 MHz, filters duplicates, enforces a hop limit, and re-broadcasts
               with a small random jitter delay to avoid radio collisions.
               
               Simultaneously runs a local WiFi AP "RELAY_NODE_3" with a real-time
               status dashboard on 192.168.6.1 showing relay activity and logs.

  Packet Formats (v2 with hop field):
    SOS: "SOS:[ID]:[LAT]:[LON]:[HOPS]"
    ACK: "ACK:[ID]:[HOPS]"

  LoRa Pin Wiring (same as Nodes 1 & 2):
    SCK  -> GPIO 18
    MISO -> GPIO 19
    MOSI -> GPIO 23
    NSS  -> GPIO 5
    RST  -> GPIO 14
    DIO0 -> GPIO 4
*/

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// ============================================================
//  PIN DEFINITIONS
// ============================================================
#define SCK  18
#define MISO 19
#define MOSI 23
#define SS   5
#define RST  14
#define DIO0 4
#define BAND 433E6

// Built-in LED — blinks briefly on every packet relayed
#define LED_PIN 2

// ============================================================
//  MESH CONFIGURATION
// ============================================================
#define MAX_HOPS       2   // Drop packets that have already been relayed MAX_HOPS times
#define SEEN_CACHE_SZ  30  // How many packet IDs to remember (dedup ring buffer)
#define JITTER_MIN_MS  50  // Minimum relay delay (avoids immediate collision with source)
#define JITTER_MAX_MS 180  // Maximum relay delay

// ============================================================
//  WIFI CONFIGURATION
// ============================================================
const char* AP_SSID = "RELAY_NODE_3";
IPAddress apIP(192, 168, 6, 1);
IPAddress netMsk(255, 255, 255, 0);
WebServer server(80);

// ============================================================
//  RELAY STATE
// ============================================================

// Seen-packet ring buffer — tracks IDs we have already forwarded
struct SeenEntry {
  unsigned long id;   // packet ID
  char          type; // 'S' = SOS, 'A' = ACK
};
SeenEntry seenCache[SEEN_CACHE_SZ];
int seenHead = 0;   // Next write index (ring)
int seenCount = 0;  // How many entries are valid

// Relay log — last 12 entries for the dashboard
#define LOG_SIZE 12
struct RelayLog {
  String   summary;
  int      rssi;
  unsigned long timestamp; // millis()
};
RelayLog relayLog[LOG_SIZE];
int logHead  = 0;
int logCount = 0;

// Counters
unsigned long sosFwdCount = 0;
unsigned long ackFwdCount = 0;
unsigned long dropCount   = 0;

// Pending relay: we store a packet here and forward it after the jitter delay
bool      pendingRelay    = false;
String    pendingPayload  = "";
unsigned long pendingAt   = 0;   // millis() when we schedule the forward

// ============================================================
//  FORWARD DECLARATIONS
// ============================================================
bool isAlreadySeen(unsigned long id, char type);
void markSeen(unsigned long id, char type);
void addLog(const String& summary, int rssi);
void blinkLed();
bool safeSendLoRa(const String& payload);

// ============================================================
//  DASHBOARD HTML
// ============================================================
#include "dashboard_html.h"


// ============================================================
//  UTILITY: Seen-packet cache
// ============================================================

bool isAlreadySeen(unsigned long id, char type) {
  for (int i = 0; i < seenCount; i++) {
    if (seenCache[i].id == id && seenCache[i].type == type) return true;
  }
  return false;
}

void markSeen(unsigned long id, char type) {
  seenCache[seenHead] = {id, type};
  seenHead = (seenHead + 1) % SEEN_CACHE_SZ;
  if (seenCount < SEEN_CACHE_SZ) seenCount++;
}

// ============================================================
//  UTILITY: Relay event log
// ============================================================

void addLog(const String& summary, int rssi) {
  relayLog[logHead] = {summary, rssi, millis()};
  logHead = (logHead + 1) % LOG_SIZE;
  if (logCount < LOG_SIZE) logCount++;
}

// ============================================================
//  UTILITY: Blink built-in LED briefly
// ============================================================

void blinkLed() {
  digitalWrite(LED_PIN, HIGH);
  delay(30); // Very brief — doesn't block meaningfully
  digitalWrite(LED_PIN, LOW);
}

// ============================================================
//  UTILITY: Safe LoRa send
// ============================================================

bool safeSendLoRa(const String& payload) {
  LoRa.beginPacket();
  LoRa.print(payload);
  bool ok = LoRa.endPacket();
  return ok;
}

// ============================================================
//  WEB SERVER HANDLERS
// ============================================================

void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.send(200, "text/html", dashboard_html);
}

void handleStatusJson() {
  // Build log array (newest first)
  String json = "{";
  json += "\"uptime\":" + String(millis()) + ",";
  json += "\"sosFwd\":" + String(sosFwdCount) + ",";
  json += "\"ackFwd\":" + String(ackFwdCount) + ",";
  json += "\"drops\":"  + String(dropCount)   + ",";
  json += "\"log\":[";

  // Walk log backward from most recent entry
  bool first = true;
  for (int i = 0; i < logCount; i++) {
    // Index into ring: most recent first
    int idx = ((logHead - 1 - i) % LOG_SIZE + LOG_SIZE) % LOG_SIZE;
    if (!first) json += ",";
    json += "{\"summary\":\"" + relayLog[idx].summary + "\",";
    json += "\"rssi\":"       + String(relayLog[idx].rssi) + ",";
    json += "\"ts\":"         + String(relayLog[idx].timestamp) + "}";
    first = false;
  }

  json += "]}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

// ============================================================
//  SETUP
// ============================================================

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("==========================================");
  Serial.println("   Node 3: LoRa Mesh Relay / Repeater   ");
  Serial.println("==========================================");

  // --- WiFi Access Point ---
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(AP_SSID);
  Serial.print("Relay AP SSID: ");
  Serial.println(AP_SSID);
  Serial.print("Dashboard URL: http://");
  Serial.println(WiFi.softAPIP());

  // --- Web Server Routes ---
  server.on("/",            handleRoot);
  server.on("/status_json", handleStatusJson);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Web server started on port 80.");

  // --- LoRa Radio ---
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  Serial.print("Starting LoRa at ");
  Serial.print(BAND / 1E6, 2);
  Serial.println(" MHz ...");

  if (!LoRa.begin(BAND)) {
    Serial.println("[ERROR] LoRa initialisation failed! Halting.");
    while (true) {
      // Blink rapidly to signal hardware error
      digitalWrite(LED_PIN, HIGH); delay(100);
      digitalWrite(LED_PIN, LOW);  delay(100);
    }
  }

  // Match LoRa parameters to Nodes 1 & 2 (defaults: BW=125kHz, SF=7, CR=4/5)
  // If you have changed these in Node 1/2, mirror them here:
  // LoRa.setSignalBandwidth(125E3);
  // LoRa.setSpreadingFactor(7);
  // LoRa.setCodingRate4(5);

  Serial.println("LoRa ready. Entering relay loop.");
  Serial.println("------------------------------------------");

  // Solid LED for 500ms = boot OK
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
}

// ============================================================
//  MAIN LOOP
// ============================================================

void loop() {
  server.handleClient();

  unsigned long now = millis();

  // --- Dispatch pending relay after jitter delay ---
  if (pendingRelay && now >= pendingAt) {
    if (safeSendLoRa(pendingPayload)) {
      Serial.print("[RELAY TX] -> ");
      Serial.println(pendingPayload);
      blinkLed();
    } else {
      Serial.println("[RELAY TX] LoRa send failed.");
    }
    pendingRelay   = false;
    pendingPayload = "";
  }

  // --- Check for incoming LoRa packet ---
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;

  // Read the packet
  String received = "";
  while (LoRa.available()) {
    received += (char)LoRa.read();
  }
  int rxRssi = LoRa.packetRssi();

  Serial.print("[RX] '");
  Serial.print(received);
  Serial.print("' RSSI=");
  Serial.println(rxRssi);

  // --------------------------------------------------------
  //  CASE 1: SOS packet  "SOS:[ID]:[LAT]:[LON]:[HOPS]"
  //          or legacy   "SOS:[ID]:[LAT]:[LON]"
  // --------------------------------------------------------
  if (received.startsWith("SOS:")) {
    // Parse fields
    int c1 = received.indexOf(':');              // after "SOS"
    int c2 = received.indexOf(':', c1 + 1);      // after ID
    int c3 = received.indexOf(':', c2 + 1);      // after LAT
    int c4 = received.indexOf(':', c3 + 1);      // after LON (may be -1 for legacy)

    if (c1 < 0 || c2 < 0 || c3 < 0) {
      Serial.println("[DROP] Malformed SOS packet.");
      dropCount++;
      return;
    }

    String idStr  = received.substring(c1 + 1, c2);
    String latStr = received.substring(c2 + 1, c3);
    String lonStr = (c4 > 0) ? received.substring(c3 + 1, c4)
                              : received.substring(c3 + 1);
    int hops      = (c4 > 0) ? received.substring(c4 + 1).toInt() : 0;

    unsigned long id = idStr.toInt();

    // --- Hop-limit check ---
    if (hops >= MAX_HOPS) {
      Serial.print("[DROP] SOS #"); Serial.print(id);
      Serial.print(" reached hop limit ("); Serial.print(hops); Serial.println("). Discarding.");
      dropCount++;
      return;
    }

    // --- Duplicate check ---
    if (isAlreadySeen(id, 'S')) {
      Serial.print("[DROP] SOS #"); Serial.print(id);
      Serial.println(" already relayed. Discarding duplicate.");
      dropCount++;
      return;
    }

    // --- Schedule relay ---
    markSeen(id, 'S');
    sosFwdCount++;

    // Rebuild packet with incremented hop count
    String fwdPacket = "SOS:" + idStr + ":" + latStr + ":" + lonStr + ":" + String(hops + 1);

    unsigned long jitter = JITTER_MIN_MS + (esp_random() % (JITTER_MAX_MS - JITTER_MIN_MS));
    pendingPayload = fwdPacket;
    pendingAt      = millis() + jitter;
    pendingRelay   = true;

    String logEntry = "SOS #" + idStr + " (" + latStr + "," + lonStr + ") hops=" + String(hops + 1);
    addLog(logEntry, rxRssi);

    Serial.print("[RELAY QUEUED] SOS #"); Serial.print(id);
    Serial.print(" in "); Serial.print(jitter); Serial.println("ms");
  }

  // --------------------------------------------------------
  //  CASE 2: ACK packet  "ACK:[ID]:[HOPS]"
  //          or legacy   "ACK:[ID]"
  // --------------------------------------------------------
  else if (received.startsWith("ACK:")) {
    int c1 = received.indexOf(':');          // after "ACK"
    int c2 = received.indexOf(':', c1 + 1); // after ID (may be -1 for legacy)

    if (c1 < 0) {
      Serial.println("[DROP] Malformed ACK packet.");
      dropCount++;
      return;
    }

    String idStr = (c2 > 0) ? received.substring(c1 + 1, c2)
                             : received.substring(c1 + 1);
    int hops     = (c2 > 0) ? received.substring(c2 + 1).toInt() : 0;

    unsigned long id = idStr.toInt();

    // --- Hop-limit check ---
    if (hops >= MAX_HOPS) {
      Serial.print("[DROP] ACK #"); Serial.print(id);
      Serial.print(" reached hop limit ("); Serial.print(hops); Serial.println("). Discarding.");
      dropCount++;
      return;
    }

    // --- Duplicate check ---
    if (isAlreadySeen(id, 'A')) {
      Serial.print("[DROP] ACK #"); Serial.print(id);
      Serial.println(" already relayed. Discarding duplicate.");
      dropCount++;
      return;
    }

    // --- Schedule relay ---
    markSeen(id, 'A');
    ackFwdCount++;

    String fwdPacket = "ACK:" + idStr + ":" + String(hops + 1);

    unsigned long jitter = JITTER_MIN_MS + (esp_random() % (JITTER_MAX_MS - JITTER_MIN_MS));
    pendingPayload = fwdPacket;
    pendingAt      = millis() + jitter;
    pendingRelay   = true;

    String logEntry = "ACK #" + idStr + " hops=" + String(hops + 1);
    addLog(logEntry, rxRssi);

    Serial.print("[RELAY QUEUED] ACK #"); Serial.print(id);
    Serial.print(" in "); Serial.print(jitter); Serial.println("ms");
  }

  // --------------------------------------------------------
  //  Unknown packet type — ignore
  // --------------------------------------------------------
  else {
    Serial.print("[IGNORE] Unknown packet: ");
    Serial.println(received);
  }
}
