// ex02.ino - 使用 millis() 实现 1Hz LED 闪烁
const int ledPin = 2;
unsigned long previousMillis = 0;
const long interval = 500;  // 500ms 亮 + 500ms 灭 = 1Hz

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));
    Serial.println(digitalRead(ledPin) ? "LED ON" : "LED OFF");
  }
}