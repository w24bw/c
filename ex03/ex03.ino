// ex03.ino - SOS 信号灯 (三短、三长、三短)
const int ledPin = 2;

// SOS 模式定义: 短=200ms, 长=600ms
// 9个步骤依次执行: S(短) S(短) S(短) O(长) O(长) O(长) S(短) S(短) S(短)
const int durations[] = {200, 200, 200, 600, 600, 600, 200, 200, 200};
const int gaps[] = {200, 200, 500, 200, 200, 500, 200, 200, 2000};

int step = 0;
unsigned long previousMillis = 0;
bool ledState = false;
unsigned long stepStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  stepStartTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - stepStartTime;
  
  // 总时长 = 亮灯时间 + 熄灭间隔
  int totalDuration = durations[step] + gaps[step];
  
  if (elapsed < durations[step]) {
    // 亮灯阶段
    digitalWrite(ledPin, HIGH);
    if (!ledState) {
      ledState = true;
      String type = (step < 3) ? "S短" : ((step < 6) ? "O长" : "S短");
      Serial.println(type + " 亮 " + String(durations[step]) + "ms");
    }
  } 
  else if (elapsed < totalDuration) {
    // 熄灭间隔阶段
    digitalWrite(ledPin, LOW);
    ledState = false;
  } 
  else {
    // 进入下一步
    step++;
    stepStartTime = currentMillis;
    
    // 完成一轮 SOS
    if (step >= 9) {
      step = 0;
      Serial.println("=== SOS 完成 ===");
    }
  }
}