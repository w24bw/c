// ex06.ino - 警车双闪灯效（双通道PWM）
const int LED_A = 2;   // LED1 (GPIO2 板载LED)
const int LED_B = 5;   // LED2 (GPIO5 外接LED)

// PWM配置
const int PWM_FREQ = 5000;
const int PWM_RES = 8;   // 8位分辨率 0-255

// 亮度值
int brightnessA = 0;    // LED_A 亮度 0-255
int brightnessB = 255;  // LED_B 亮度 255-0（反相）

// 变化步长
int fadeAmount = 5;     // 每次变化步长

// 更新间隔
unsigned long lastUpdateTime = 0;
const int updateInterval = 15;  // 15ms更新一次（平滑过渡）

void setup() {
  Serial.begin(115200);

  // 初始化PWM
  ledcAttach(LED_A, PWM_FREQ, PWM_RES);
  ledcAttach(LED_B, PWM_FREQ, PWM_RES);

  // 初始值：A亮，B灭
  ledcWrite(LED_A, 255);
  ledcWrite(LED_B, 0);

  Serial.println("🚨 警车双闪灯效启动！");
  Serial.println("LED_A (GPIO2) 和 LED_B (GPIO5) 交替渐变闪烁");
}

void loop() {
  unsigned long currentMillis = millis();

  // 定时更新亮度
  if (currentMillis - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentMillis;

    // 更新亮度值
    brightnessA += fadeAmount;
    brightnessB = 255 - brightnessA;  // 反相

    // 到达边界时反转方向
    if (brightnessA >= 255 || brightnessA <= 0) {
      fadeAmount = -fadeAmount;
    }

    // 限制范围 0-255
    brightnessA = constrain(brightnessA, 0, 255);
    brightnessB = constrain(brightnessB, 0, 255);

    // 输出到LED
    ledcWrite(LED_A, brightnessA);
    ledcWrite(LED_B, brightnessB);

    // 串口调试（可选）
    // Serial.print("A: ");
    // Serial.print(brightnessA);
    // Serial.print("  B: ");
    // Serial.println(brightnessB);
  }
}