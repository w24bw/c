// ex04.ino - 触摸“自锁”开关
const int touchPin = 4;      // T0 触摸通道对应 GPIO4
const int ledPin = 2;        // 板载 LED

// 触摸阈值，低于该值判定为触摸
const int touchThreshold = 300;

// 状态变量
bool ledState = LOW;          // LED 状态
bool lastTouchState = false;  // 上一次触摸状态
bool currentTouchState = false;

// 防抖
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
}

void loop() {
  // 1. 读取原始触摸值
  int touchValue = touchRead(touchPin);

  // 2. 判断当前是否处于触摸状态
  bool rawTouchState = (touchValue < touchThreshold);

  // 3. 防抖处理
  if (rawTouchState != lastTouchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (rawTouchState != currentTouchState) {
      currentTouchState = rawTouchState;

      // 4. 边缘检测: 检测到从"未触摸"到"触摸"的瞬间
      if (currentTouchState == true) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        Serial.println(ledState ? "LED ON" : "LED OFF");
      }
    }
  }

  lastTouchState = rawTouchState;
}