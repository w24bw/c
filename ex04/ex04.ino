// ex04.ino - 触摸“自锁”开关
const int touchPin = 4;
const int ledPin = 2;

// 触摸阈值（已根据你的测试校准）
const int touchThreshold = 500;

// 状态变量
bool ledState = false;
bool lastTouchState = false;
bool currentTouchState = false;

// 防抖
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.println("Touch Self-Locking Switch Started");
  Serial.println("Threshold: 500");
}

void loop() {
  int touchValue = touchRead(touchPin);
  bool rawTouchState = (touchValue < touchThreshold);

  // 防抖
  if (rawTouchState != lastTouchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (rawTouchState != currentTouchState) {
      currentTouchState = rawTouchState;

      // 边缘检测：未触摸→触摸的瞬间
      if (currentTouchState == true) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        Serial.print("Touch! Value: ");
        Serial.print(touchValue);
        Serial.print(" LED: ");
        Serial.println(ledState ? "ON" : "OFF");
      }
    }
  }

  lastTouchState = rawTouchState;
}