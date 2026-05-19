#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "ssid";
const char* password = "pwd";

WebServer server(80);

// RGB灯带配置
#define LED_PIN 13
#define NUM_LEDS 10
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// 灯珠状态数组，默认全灭
bool ledStates[NUM_LEDS] = {false};
// 每个灯珠的颜色数组，默认白色
String ledColors[NUM_LEDS] = {"#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF"};

// 生成网页内容的独立函数
String generateHtmlContent() {
  String html = "";
  html += "<!DOCTYPE HTML>\r\n";
  html += "<html>\r\n";
  html += "<head>\r\n";
  html += "<meta charset='UTF-8'>\r\n";
  html += "<title>ESP32 灯带控制器</title>\r\n";
  html += "<style>\r\n";
  html += "  .led-checkbox { margin: 10px; display: flex; align-items: center; }\r\n";
  html += "  .led-label { display: inline-block; width: 50px; }\r\n";
  html += "  .led-preview { display: inline-block; width: 30px; height: 30px; border-radius: 50%; background-color: #666; margin: 0 10px; cursor: pointer; border: 2px solid #999; }\r\n";
  html += "  .led-preview:hover { border-color: #333; box-shadow: 0 0 10px rgba(0,0,0,0.3); }\r\n";
  html += "  input[type='checkbox'] { transform: scale(1.5); margin-right: 10px; }\r\n";
  html += "  .color-picker-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background-color: rgba(0,0,0,0.5); display: none; justify-content: center; align-items: center; z-index: 1000; }\r\n";
  html += "  .color-picker-container { background-color: white; padding: 20px; border-radius: 10px; text-align: center; }\r\n";
  html += "  #colorPicker { width: 100px; height: 100px; border: none; cursor: pointer; }\r\n";
  html += "  .picker-buttons { margin-top: 15px; }\r\n";
  html += "  .picker-buttons button { margin: 0 10px; padding: 8px 16px; font-size: 14px; }\r\n";
  html += "</style>\r\n";
  html += "<script>\r\n";
  html += "  var currentLedIndex = -1;\r\n";
  html += "  var ledColors = ['#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF', '#FFFFFF'];\r\n";
  html += "\r\n";
  html += "  // 实时更新单个灯珠状态\r\n";
  html += "  function updateLed(index, enabled, color) {\r\n";
  html += "    var xhr = new XMLHttpRequest();\r\n";
  html += "    xhr.open('GET', '/led?index=' + index + '&enabled=' + enabled + '&color=' + encodeURIComponent(color), true);\r\n";
  html += "    xhr.send();\r\n";
  html += "  }\r\n";
  html += "\r\n";
  html += "  // 复选框状态改变时触发\r\n";
  html += "  function onLedToggle(index) {\r\n";
  html += "    var checkbox = document.getElementById('led' + index);\r\n";
  html += "    var preview = document.getElementById('preview' + index);\r\n";
  html += "    if (checkbox.checked) {\r\n";
  html += "      preview.style.backgroundColor = ledColors[index];\r\n";
  html += "      updateLed(index, 1, ledColors[index]);\r\n";
  html += "    } else {\r\n";
  html += "      preview.style.backgroundColor = '#666';\r\n";
  html += "      updateLed(index, 0, ledColors[index]);\r\n";
  html += "    }\r\n";
  html += "  }\r\n";
  html += "\r\n";
  html += "  function openColorPicker(index) {\r\n";
  html += "    currentLedIndex = index;\r\n";
  html += "    document.getElementById('colorPicker').value = ledColors[index];\r\n";
  html += "    document.getElementById('colorPreview').style.backgroundColor = ledColors[index];\r\n";
  html += "    document.getElementById('colorPickerOverlay').style.display = 'flex';\r\n";
  html += "  }\r\n";
  html += "\r\n";
  html += "  function closeColorPicker() {\r\n";
  html += "    document.getElementById('colorPickerOverlay').style.display = 'none';\r\n";
  html += "    currentLedIndex = -1;\r\n";
  html += "  }\r\n";
  html += "\r\n";
  html += "  function updateColorPreview() {\r\n";
  html += "    var color = document.getElementById('colorPicker').value;\r\n";
  html += "    document.getElementById('colorPreview').style.backgroundColor = color;\r\n";
  html += "  }\r\n";
  html += "\r\n";
  html += "  function applyColor() {\r\n";
  html += "    if (currentLedIndex >= 0) {\r\n";
  html += "      var color = document.getElementById('colorPicker').value;\r\n";
  html += "      ledColors[currentLedIndex] = color;\r\n";
  html += "      document.getElementById('preview' + currentLedIndex).style.backgroundColor = color;\r\n";
  html += "      // 如果灯珠已选中，立即更新灯条\r\n";
  html += "      if (document.getElementById('led' + currentLedIndex).checked) {\r\n";
  html += "        updateLed(currentLedIndex, 1, color);\r\n";
  html += "      }\r\n";
  html += "    }\r\n";
  html += "    closeColorPicker();\r\n";
  html += "  }\r\n";
  html += "</script>\r\n";
  html += "</head>\r\n";
  html += "<body>\r\n";
  html += "<h1>ESP32 RGB灯带控制器</h1>\r\n";
  html += "<p>\u70B9\u51FB\u590D\u9009\u6846\u5F00\u542F/\u5173\u95ED\u706F\u73E0，\u70B9\u51FB\u5706\u5F62\u56FE\u6807\u9009\u62E9\u989C\u8272（\u5B9E\u65F6\u66F4\u65B0）</p>\r\n";
  html += "<div style='margin: 20px;'>\r\n";
  for (int i = 0; i < NUM_LEDS; i++) {
    html += "  <div class='led-checkbox'>\r\n";
    html += "    <input type='checkbox' name='led' id='led" + String(i) + "' onchange='onLedToggle(" + String(i) + ")'>\r\n";
    html += "    <label for='led" + String(i) + "' class='led-label'>\u706F\u73E0" + String(i + 1) + "</label>\r\n";
    html += "    <div class='led-preview' id='preview" + String(i) + "' style='background-color: #666;' onclick='openColorPicker(" + String(i) + ")'></div>\r\n";
    html += "  </div>\r\n";
  }
  html += "</div>\r\n";
  html += "\r\n";
  html += "<div class='color-picker-overlay' id='colorPickerOverlay' onclick='closeColorPicker()'>\r\n";
  html += "  <div class='color-picker-container' onclick='event.stopPropagation()'>\r\n";
  html += "    <h3>\u9009\u62E9\u989C\u8272</h3>\r\n";
  html += "    <input type='color' id='colorPicker' oninput='updateColorPreview()'>\r\n";
  html += "    <div id='colorPreview' style='width: 50px; height: 50px; margin: 10px auto; border-radius: 50%; border: 2px solid #333;'></div>\r\n";
  html += "    <div class='picker-buttons'>\r\n";
  html += "      <button onclick='applyColor()'>\u786E\u8BA4</button>\r\n";
  html += "      <button onclick='closeColorPicker()'>\u53D6\u6D88</button>\r\n";
  html += "    </div>\r\n";
  html += "  </div>\r\n";
  html += "</div>\r\n";
  html += "</body>\r\n";
  html += "</html>\r\n";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", generateHtmlContent());
}

// 十六进制颜色转RGB
uint32_t hexToRgb(String hexColor) {
  if (hexColor.length() != 7 || hexColor[0] != '#') {
    return strip.Color(255, 255, 255); // 默认白色
  }
  
  int r = strtol(hexColor.substring(1, 3).c_str(), NULL, 16);
  int g = strtol(hexColor.substring(3, 5).c_str(), NULL, 16);
  int b = strtol(hexColor.substring(5, 7).c_str(), NULL, 16);
  
  return strip.Color(r, g, b);
}

// 处理单个灯珠的实时更新
void handleLed() {
  if (server.hasArg("index") && server.hasArg("enabled") && server.hasArg("color")) {
    int ledNum = server.arg("index").toInt();
    bool enabled = server.arg("enabled").toInt() == 1;
    String color = server.arg("color");
    
    if (ledNum >= 0 && ledNum < NUM_LEDS) {
      ledStates[ledNum] = enabled;
      ledColors[ledNum] = color;
      
      if (enabled) {
        uint32_t rgbColor = hexToRgb(color);
        strip.setPixelColor(ledNum, rgbColor);
        Serial.print("灯珠");
        Serial.print(ledNum + 1);
        Serial.print("已点亮，颜色: ");
        Serial.println(color);
      } else {
        strip.setPixelColor(ledNum, 0);
        Serial.print("灯珠");
        Serial.print(ledNum + 1);
        Serial.println("已关闭");
      }
      
      strip.show();
    }
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  
  // 初始化灯带
  strip.begin();
  strip.show(); // 初始时所有灯珠熄灭
  Serial.println("RGB灯带初始化完成");
  
  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Start web server
  server.on("/", handleRoot);
  server.on("/led", handleLed);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
