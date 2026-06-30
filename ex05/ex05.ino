// ex05.ino - 多档位触摸调速呼吸灯
const int touchPin = 4;
const int ledPin = 2;
const int touchThreshold = 300;

int brightness = 0;
int fadeAmount = 5;
unsigned long lastUpdateTime = 0;

bool ledState = false;
bool lastTouchState = false;
bool currentTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

int speedLevel = 1;
const int speedDelays[3] = {30, 10, 3};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  ledcAttach(ledPin, 5000, 8);
}

void loop() {
  unsigned long currentMillis = millis();

  // --- 触摸检测与档位切换 ---
  int touchValue = touchRead(touchPin);
  bool rawTouchState = (touchValue < touchThreshold);

  if (rawTouchState != lastTouchState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    if (rawTouchState != currentTouchState) {
      currentTouchState = rawTouchState;
      if (currentTouchState == true) {
        speedLevel++;
        if (speedLevel > 3) speedLevel = 1;
        Serial.print("Speed Level: ");
        Serial.println(speedLevel);
      }
    }
  }
  lastTouchState = rawTouchState;

  // --- 呼吸灯逻辑 ---
  int updateInterval = speedDelays[speedLevel - 1];

  if (currentMillis - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentMillis;
    brightness += fadeAmount;

    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
      brightness = constrain(brightness, 0, 255);
    }

    ledcWrite(ledPin, brightness);
  }
}