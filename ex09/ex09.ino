// ex09.ino - 实时传感器Web仪表盘（WiFi模式）
#include <WiFi.h>
#include <WebServer.h>

// ===== 请修改以下内容 =====
const char* ssid = "gdhs";          // 你的WiFi名称
const char* password = "asdf1234";  // 你的WiFi密码
// =========================

const int TOUCH_PIN = 4;  // 触摸引脚 GPIO4

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
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
  server.on("/data", handleData);
  server.begin();
  Serial.println("✅ Web服务器已启动");
  Serial.println("📊 实时传感器仪表盘");
}

void loop() {
  server.handleClient();
}

// ===== 主页：显示仪表盘 =====
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实时传感器仪表盘</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      text-align: center;
      min-height: 100vh;
      background: linear-gradient(135deg, #0f0c29, #302b63, #24243e);
      display: flex;
      justify-content: center;
      align-items: center;
    }
    .container {
      background: rgba(255,255,255,0.08);
      backdrop-filter: blur(20px);
      padding: 50px 60px;
      border-radius: 30px;
      border: 1px solid rgba(255,255,255,0.15);
      box-shadow: 0 25px 50px rgba(0,0,0,0.5);
      max-width: 500px;
      width: 90%;
    }
    h1 {
      color: #fff;
      font-size: 28px;
      font-weight: 300;
      letter-spacing: 2px;
      margin-bottom: 10px;
    }
    .subtitle {
      color: rgba(255,255,255,0.5);
      font-size: 14px;
      margin-bottom: 30px;
    }
    .value-display {
      font-size: 100px;
      font-weight: 700;
      color: #00d4ff;
      text-shadow: 0 0 40px rgba(0,212,255,0.3);
      transition: all 0.3s ease;
      font-family: 'Courier New', monospace;
    }
    .value-label {
      color: rgba(255,255,255,0.6);
      font-size: 16px;
      margin-top: 5px;
    }
    .status-bar {
      margin-top: 30px;
      padding: 15px 20px;
      background: rgba(255,255,255,0.05);
      border-radius: 12px;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .status-indicator {
      display: flex;
      align-items: center;
      gap: 10px;
      color: rgba(255,255,255,0.7);
      font-size: 14px;
    }
    .dot {
      width: 12px;
      height: 12px;
      border-radius: 50%;
      background: #00d4ff;
      animation: pulse 1.5s ease-in-out infinite;
    }
    .dot.touched {
      background: #ff6b6b;
      animation: pulse 0.5s ease-in-out infinite;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.4; transform: scale(0.8); }
    }
    .progress-bar {
      width: 100%;
      height: 6px;
      background: rgba(255,255,255,0.1);
      border-radius: 3px;
      margin-top: 20px;
      overflow: hidden;
    }
    .progress-fill {
      height: 100%;
      width: 0%;
      background: linear-gradient(90deg, #00d4ff, #7b2ffc);
      border-radius: 3px;
      transition: width 0.3s ease;
    }
    .info-text {
      color: rgba(255,255,255,0.3);
      font-size: 12px;
      margin-top: 20px;
    }
    .touch-status {
      color: rgba(255,255,255,0.8);
      font-size: 18px;
      margin-top: 10px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>📊 传感器仪表盘</h1>
    <div class="subtitle">实时触摸传感器数据监控</div>
    
    <div class="value-display" id="value">---</div>
    <div class="value-label">触摸数值 (越小越近)</div>
    
    <div class="touch-status" id="touchStatus">✋ 未触摸</div>
    
    <div class="progress-bar">
      <div class="progress-fill" id="progressFill"></div>
    </div>
    
    <div class="status-bar">
      <div class="status-indicator">
        <span class="dot" id="statusDot"></span>
        <span id="statusText">数据更新中...</span>
      </div>
      <span style="color:rgba(255,255,255,0.3); font-size:12px;" id="updateTime">--:--:--</span>
    </div>
    <div class="info-text">💡 手指靠近或触摸GPIO4引脚观察数值变化</div>
  </div>

  <script>
    // 获取传感器数据
    function fetchData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          // 更新数值显示
          document.getElementById('value').textContent = data.value;
          
          // 更新进度条 (数值范围 0-4095，映射到0-100%)
          let percent = Math.min(100, (data.value / 4095) * 100);
          document.getElementById('progressFill').style.width = percent + '%';
          
          // 更新触摸状态
          let touchStatus = document.getElementById('touchStatus');
          let dot = document.getElementById('statusDot');
          let statusText = document.getElementById('statusText');
          
          if (data.touched) {
            touchStatus.innerHTML = '🖐️ 触摸中!';
            touchStatus.style.color = '#ff6b6b';
            dot.className = 'dot touched';
            statusText.textContent = '🔴 已触摸';
          } else {
            touchStatus.innerHTML = '✋ 未触摸';
            touchStatus.style.color = 'rgba(255,255,255,0.8)';
            dot.className = 'dot';
            statusText.textContent = '🟢 等待触摸';
          }
          
          // 更新数值颜色 (根据触摸状态)
          let valueDisplay = document.getElementById('value');
          if (data.touched) {
            valueDisplay.style.color = '#ff6b6b';
            valueDisplay.style.textShadow = '0 0 40px rgba(255,107,107,0.3)';
          } else {
            valueDisplay.style.color = '#00d4ff';
            valueDisplay.style.textShadow = '0 0 40px rgba(0,212,255,0.3)';
          }
          
          // 更新时间
          let now = new Date();
          document.getElementById('updateTime').textContent = 
            now.getHours().toString().padStart(2,'0') + ':' +
            now.getMinutes().toString().padStart(2,'0') + ':' +
            now.getSeconds().toString().padStart(2,'0');
        })
        .catch(error => {
          console.log('获取数据失败:', error);
        });
    }
    
    // 每200ms更新一次
    setInterval(fetchData, 200);
    
    // 页面加载时立即获取一次
    fetchData();
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== 提供JSON数据 =====
void handleData() {
  int touchValue = touchRead(TOUCH_PIN);
  
  // 判断是否触摸（阈值500）
  bool touched = (touchValue < 500);
  
  // 构建JSON响应
  String json = "{\"value\":" + String(touchValue) + 
                ",\"touched\":" + String(touched ? "true" : "false") + "}";
  
  server.send(200, "application/json", json);
}