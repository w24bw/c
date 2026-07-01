// ex07.ino - Web网页端无极调光器（WiFi模式）
#include <WiFi.h>
#include <WebServer.h>

// ===== 请修改以下内容 =====
const char* ssid = "gdhs";          // 你的WiFi名称
const char* password = "asdf1234";  // 你的WiFi密码
// =========================

const int LED_PIN = 2;  // GPIO2 (板载LED)
WebServer server(80);

// PWM配置
const int PWM_FREQ = 5000;
const int PWM_RES = 8;   // 8位分辨率 0-255

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // 初始化PWM
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);

  // 连接WiFi
  Serial.print("正在连接WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("✅ WiFi连接成功！");
  Serial.print("🌐 访问地址: http://");
  Serial.println(WiFi.localIP());

  // 设置路由
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("✅ Web服务器已启动");
}

void loop() {
  server.handleClient();
}

// ===== 主页：显示带滑动条的HTML页面 =====
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>无极调光器</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin-top: 60px;
      background: #f0f2f5;
    }
    .container {
      background: white;
      padding: 40px;
      border-radius: 16px;
      max-width: 400px;
      margin: 0 auto;
      box-shadow: 0 4px 20px rgba(0,0,0,0.1);
    }
    h1 { color: #1a73e8; }
    .slider-container {
      margin: 30px 0;
    }
    input[type="range"] {
      width: 100%;
      height: 10px;
      -webkit-appearance: none;
      background: linear-gradient(to right, #ddd, #1a73e8);
      border-radius: 5px;
      outline: none;
    }
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 28px;
      height: 28px;
      border-radius: 50%;
      background: #1a73e8;
      cursor: pointer;
      box-shadow: 0 2px 8px rgba(26,115,232,0.4);
    }
    .brightness-display {
      font-size: 56px;
      font-weight: bold;
      color: #1a73e8;
      margin: 10px 0;
    }
    .brightness-text {
      font-size: 16px;
      color: #888;
    }
    .light-icon {
      font-size: 64px;
      margin: 10px 0;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>💡 无极调光器</h1>
    <div class="light-icon" id="icon">💡</div>
    <div class="slider-container">
      <input type="range" id="slider" min="0" max="255" value="0" 
             oninput="sendValue(this.value)">
      <div class="brightness-display" id="display">0%</div>
      <div class="brightness-text">拖动滑块调节LED亮度</div>
    </div>
  </div>

  <script>
    function sendValue(value) {
      // 更新显示
      let percent = Math.round((value / 255) * 100);
      document.getElementById('display').textContent = percent + '%';
      
      // 更新图标亮度
      let icon = document.getElementById('icon');
      if (percent > 70) icon.textContent = '💡';
      else if (percent > 30) icon.textContent = '🔆';
      else if (percent > 5) icon.textContent = '🔅';
      else icon.textContent = '⚪';
      
      // 发送GET请求到ESP32
      fetch('/set?value=' + value)
        .then(response => {
          if (!response.ok) {
            console.log('请求失败');
          }
        })
        .catch(error => {
          console.log('错误:', error);
        });
    }
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== 接收滑动条数值 =====
void handleSet() {
  if (server.hasArg("value")) {
    int value = server.arg("value").toInt();
    // 限制范围 0-255
    if (value < 0) value = 0;
    if (value > 255) value = 255;
    
    // 设置PWM占空比
    ledcWrite(LED_PIN, value);
    
    Serial.print("亮度: ");
    Serial.println(value);
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}