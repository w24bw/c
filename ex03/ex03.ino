// ex03.ino - SOS 信号灯 (三短、三长、三短)
const int ledPin = 2;

const int durations[] = {200, 200, 200, 600, 600, 600, 200, 200, 200};
const int gaps[] = {200, 200, 500, 200, 200, 500, 200, 200, 2000};

int step = 0;
unsigned long stepStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  stepStartTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - stepStartTime;
  int totalDuration = durations[step] + gaps[step];
  
  if (elapsed < durations[step]) {
    digitalWrite(ledPin, HIGH);
  } else if (elapsed < totalDuration) {
    digitalWrite(ledPin, LOW);
  } else {
    step++;
    stepStartTime = currentMillis;
    if (step >= 9) {
      step = 0;
      Serial.println("=== SOS 完成 ===");
    }
  }
}
