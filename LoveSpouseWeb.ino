#include <Arduino.h>
#include <NimBLEDevice.h> //version 1.4.2 (h2zero)
#include <WiFi.h>
#include <WebServer.h>

// WiFi Credentials
const char *ssid = "SSID";
const char *password = "PASSWORD";

// Web Server Port
const int webPort = 80;

// Web Server Object
WebServer server(webPort);

static uint16_t companyId = 0xFFF0;

#define MANUFACTURER_DATA_PREFIX 0x6D, 0xB6, 0x43, 0xCE, 0x97, 0xFE, 0x42, 0x7C

// Updated list of manufacturer data options with both speed and vibration modes
uint8_t manufacturerDataList[][11] = {
    {MANUFACTURER_DATA_PREFIX, 0xE5, 0x00, 0x00}, // Off (E50000)
    {MANUFACTURER_DATA_PREFIX, 0xF4, 0x00, 0x00}, // Mode 1 (F40000)
    {MANUFACTURER_DATA_PREFIX, 0xF7, 0x00, 0x00}, // Mode 2 (F70000)
    {MANUFACTURER_DATA_PREFIX, 0xF6, 0x00, 0x00}, // Mode 3 (F60000)
    {MANUFACTURER_DATA_PREFIX, 0xF1, 0x00, 0x00}, // Mode 4 (F10000)
    {MANUFACTURER_DATA_PREFIX, 0xF3, 0x00, 0x00}, // Mode 5 (F30000)
    {MANUFACTURER_DATA_PREFIX, 0xE7, 0x00, 0x00}, // Mode 6 (E70000)
    {MANUFACTURER_DATA_PREFIX, 0xE6, 0x00, 0x00}, // Mode 7 (E60000)
    {MANUFACTURER_DATA_PREFIX, 0xE1, 0x00, 0x00}, // Vibration 1 (E10000)
    {MANUFACTURER_DATA_PREFIX, 0xD0, 0x00, 0x00}, // Vibration 2 (D00000)
    {MANUFACTURER_DATA_PREFIX, 0xD3, 0x00, 0x00}, // Vibration 3 (D30000)
    {MANUFACTURER_DATA_PREFIX, 0xE2, 0x00, 0x00}, // Vibration 4 (E20000)
    {MANUFACTURER_DATA_PREFIX, 0xED, 0x00, 0x00}, // Vibration 5 (ED0000)
    {MANUFACTURER_DATA_PREFIX, 0xDC, 0x00, 0x00}, // Vibration 6 (DC0000)
    {MANUFACTURER_DATA_PREFIX, 0xE9, 0x00, 0x00}  // Vibration 7 (E90000)
};

const char *deviceName = "MuSE_Advertiser";

volatile uint8_t currentManufacturerDataIndex = 0;
volatile bool modeChanged = false;

void advertiseManufacturerData(uint8_t index) {
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->stop();
  uint8_t *manufacturerData = manufacturerDataList[index];
  pAdvertising->setManufacturerData(std::string((char *)&companyId, 2) + std::string((char *)manufacturerData, 11));
  pAdvertising->setScanResponse(true);
  pAdvertising->setMaxPreferred(0x12);
  pAdvertising->setMinPreferred(0x02);
  pAdvertising->start();

  Serial.print("Advertising mode: ");
  Serial.println(index);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html lang=\"ru\">";
  html += "<head>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 0; background-color: #f4f4f4; }";
  html += ".container { display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; }";
  html += ".speed-slider { width: 80%; max-width: 300px; height: 20px; border-radius: 10px; appearance: none; outline: none; background: linear-gradient(to right, yellow, orange, red); }";
  html += ".speed-slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 30px; height: 30px; background: white; border-radius: 50%; border: 2px solid #333; cursor: pointer; }";
  html += ".speed-slider::-moz-range-thumb { width: 30px; height: 30px; background: white; border-radius: 50%; border: 2px solid #333; cursor: pointer; }";
  html += ".speed-label { margin-top: 10px; font-size: 18px; color: #555; }";
  html += ".button { margin: 10px; padding: 10px 20px; font-size: 16px; color: white; border: none; border-radius: 5px; cursor: pointer; }";
  html += ".button-off { background-color: #FF4444; }";
  html += ".button-vibration { background-color: #4CAF50; }";
  html += "</style>";
  html += "<title>MuSE</title>";
  html += "</head>";
  html += "<body>";
  html += "<div class=\"container\">";
  
  // Speed Slider Section
  html += "<h2>Speed Modes</h2>";
  html += "<input type=\"range\" min=\"0\" max=\"7\" value=\"" + String(currentManufacturerDataIndex) + "\" class=\"speed-slider\" id=\"speedSlider\" oninput=\"updateSpeedLabel(this.value); updateMode(this.value)\">";
  html += "<p class=\"speed-label\" id=\"speedLabel\">" + String(
      currentManufacturerDataIndex == 0 ? "off" :
      currentManufacturerDataIndex == 1 ? "speed 1" :
      currentManufacturerDataIndex == 2 ? "speed 2" :
      currentManufacturerDataIndex == 3 ? "speed 3" :
      currentManufacturerDataIndex == 4 ? "speed 4" :
      currentManufacturerDataIndex == 5 ? "speed 5" :
      currentManufacturerDataIndex == 6 ? "speed 6" :
      "speed 7"
  ) + "</p>";

  // Vibration Buttons Section
  html += "<h2>Vibration Modes</h2>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(8)\">Vibration 1</button>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(9)\">Vibration 2</button>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(10)\">Vibration 3</button>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(11)\">Vibration 4</button>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(12)\">Vibration 5</button>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(13)\">Vibration 6</button>";
  html += "<button class=\"button button-vibration\" onclick=\"setVibrationMode(14)\">Vibration 7</button>";

  // Off Button
  html += "<button class=\"button button-off\" onclick=\"setVibrationMode(0)\">Off</button>";

  html += "</div>";
  html += "<script>";
  html += "function updateSpeedLabel(value) {";
  html += "  const labels = ['off', 'speed 1', 'speed 2', 'speed 3', 'speed 4', 'speed 5', 'speed 6', 'speed 7'];";
  html += "  document.getElementById('speedLabel').textContent = labels[value];";
  html += "}";
  html += "function updateMode(value) {";
  html += "  fetch('/setMode', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'mode=' + value";
  html += "  });";
  html += "}";
  html += "function setVibrationMode(mode) {";
  html += "  fetch('/setMode', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'mode=' + mode";
  html += "  });";
  html += "}";
  html += "document.getElementById('speedSlider').addEventListener('wheel', function(event) {";
  html += "  event.preventDefault();";
  html += "  let newValue = this.value - (event.deltaY > 0 ? 1 : -1);";
  html += "  if (newValue < 0) newValue = 0;";
  html += "  if (newValue > 7) newValue = 7;";
  html += "  this.value = newValue;";
  html += "  updateSpeedLabel(newValue);";
  html += "  updateMode(newValue);";
  html += "});";
  html += "</script>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void IRAM_ATTR handleSetMode() {
  if (server.hasArg("mode")) {
    uint8_t newMode = server.arg("mode").toInt();
    if (newMode >= 0 && newMode <= 14) { // Check for valid range (0-14)
      currentManufacturerDataIndex = newMode;
      modeChanged = true;
      Serial.print("Mode changed to: ");
      Serial.println(newMode);
    }
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void initWebServer() {
  server.on("/", handleRoot);
  server.on("/setMode", HTTP_POST, handleSetMode);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Web server started.");
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  initWebServer();
  NimBLEDevice::init(deviceName);
  advertiseManufacturerData(currentManufacturerDataIndex);
  Serial.println("BLE advertising started.");
}

unsigned long previousMillis = 0;
const unsigned long interval = 50; // Interval in milliseconds

void loop() {
  server.handleClient();
  if (modeChanged) {
    modeChanged = false;
    advertiseManufacturerData(currentManufacturerDataIndex);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  }
}
