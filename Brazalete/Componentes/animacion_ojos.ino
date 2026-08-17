#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// ---------- PANTALLA OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- IMU ----------
Adafruit_MPU6050 mpu;

// ---------- TOUCH ----------
#define TOUCH_PIN 4
int touchThreshold = 30;
bool happy = false;
unsigned long happyStart = 0;

// ---------- BUZZER ----------
#define BUZZER_PIN 5
void beep(int freq, int duration) {
  ledcWriteTone(0, freq);
  delay(duration);
  ledcWriteTone(0, 0);
}
void soundHappy() { beep(1200,100); beep(1500,100); beep(1800,100); }
void soundAngry() { beep(400,200); beep(300,200); }
void soundDizzy() { for (int f=800; f>300; f-=50) beep(f,30); }

// ---------- BATERIA ----------
#define BATTERY_PIN 1
float readBatteryVoltage() {
  uint32_t mv = analogReadMilliVolts(BATTERY_PIN);
  return mv / 1000.0;
}
int batteryPercent(float v) {
  int pct = (int)((v - 1.5) / (2.0 - 1.5) * 100.0);
  return constrain(pct, 0, 100);
}

// ---------- OJOS ----------
int eyeW = 30, eyeH = 30;
int baseX1 = 30, baseX2 = 70, baseY = 20;
float offsetX = 0, offsetY = 0;
int eyeState = 0;
unsigned long lastEyeChange = 0;

bool dizzy = false; unsigned long dizzyStart = 0; int spiralAngle = 0; float lastShake = 0;
bool angry = false; unsigned long angryStart = 0;

void drawThickPixel(int x, int y, int t) {
  for (int i = -t; i <= t; i++)
    for (int j = -t; j <= t; j++)
      display.drawPixel(x + i, y + j, WHITE);
}

void drawSpiral(int cx, int cy, int angleOffset) {
  for (int i = 0; i < 18; i++) {
    float angle = 0.5 * i + angleOffset * 0.2;
    int x = cx + cos(angle) * i;
    int y = cy + sin(angle) * i;
    drawThickPixel(x, y, 2);
  }
}

void drawDizzy() {
  display.clearDisplay();
  drawSpiral(40, 35, spiralAngle);
  drawSpiral(95, 35, -spiralAngle);
  display.display();
  spiralAngle += 6;
}

void drawHappy() {
  display.clearDisplay();
  int x1 = baseX1 + offsetX, x2 = baseX2 + offsetX, y = baseY + offsetY;
  for (int i = 0; i < eyeW; i++) {
    int curve = sin(i * 0.2) * 4;
    display.drawPixel(x1 + i, y + 20 - curve, WHITE);
    display.drawPixel(x1 + i, y + 21 - curve, WHITE);
    display.drawPixel(x2 + i, y + 20 - curve, WHITE);
    display.drawPixel(x2 + i, y + 21 - curve, WHITE);
  }
  display.display();
}

void drawAngry() {
  display.clearDisplay();
  int shakeX = random(-3, 4), shakeY = random(-3, 4);
  int x1 = baseX1 + offsetX + shakeX, x2 = baseX2 + offsetX + shakeX, y = baseY + offsetY + shakeY;
  display.fillRoundRect(x1, y + 6, eyeW, 18, 6, WHITE);
  display.fillRoundRect(x2, y + 6, eyeW, 18, 6, WHITE);
  for (int i = 0; i < 6; i++) {
    display.drawLine(x1, y - i, x1 + eyeW, y + 14 - i, BLACK);
    display.drawLine(x2, y + 14 - i, x2 + eyeW, y - i, BLACK);
  }
  display.display();
}

void drawEyes() {
  display.clearDisplay();
  int x1 = baseX1 + offsetX, x2 = baseX2 + offsetX, y = baseY + offsetY;
  switch (eyeState) {
    case 0: display.fillRoundRect(x1, y, eyeW, eyeH, 8, WHITE); display.fillRoundRect(x2, y, eyeW, eyeH, 8, WHITE); break;
    case 1: display.fillRoundRect(x1, y+5, eyeW, 20, 8, WHITE); display.fillRoundRect(x2, y+5, eyeW, 20, 8, WHITE); break;
    case 2: display.fillRoundRect(x1, y, eyeW, eyeH, 8, WHITE); display.fillRoundRect(x2, y+8, eyeW, 20, 8, WHITE); break;
    case 3: display.fillRoundRect(x1, y+10, eyeW, 10, 6, WHITE); display.fillRoundRect(x2, y+10, eyeW, 10, 6, WHITE); break;
    case 4:
      for (int i = 0; i < 4; i++) {
        display.drawCircle(x1 + 15, y + 15, 12 + i, WHITE);
        display.drawCircle(x2 + 15, y + 15, 12 + i, WHITE);
      }
      break;
  }
  display.display();
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("ERROR: OLED no responde en 0x3C"));
    while (1) delay(1000);
  }
  Serial.println(F("OLED OK"));

  display.clearDisplay();
  display.display();

  if (!mpu.begin()) {
    Serial.println(F("ERROR: MPU6050 no encontrado"));
    while (1) delay(1000);
  }
  Serial.println(F("MPU6050 OK"));

  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER_PIN, 0);

  analogReadResolution(12);
}

// ---------- LOOP ----------
void loop() {

  // --- Voltaje de batería por Serial ---
  float vBat = readBatteryVoltage();
  int pctBat = batteryPercent(vBat);
  Serial.print(F("Bateria: "));
  Serial.print(vBat, 3);
  Serial.print(F(" V  ("));
  Serial.print(pctBat);
  Serial.println(F("%)"));

  // --- Lectura IMU ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  int targetX = -a.acceleration.y * 5.0;
  int targetY = -a.acceleration.x * 4.0;
  targetX = constrain(targetX, -25, 25);
  targetY = constrain(targetY, -20, 20);

  offsetX = 0.7 * offsetX + 0.3 * targetX;
  offsetY = 0.7 * offsetY + 0.3 * targetY;
  offsetX += random(-1, 2) * 0.3;
  offsetY += random(-1, 2) * 0.3;

  int touchValue = touchRead(TOUCH_PIN);
  if (touchValue < touchThreshold && !happy && !dizzy && !angry) {
    happy = true;
    happyStart = millis();
    soundHappy();
  }

  if (millis() - lastEyeChange > 2000) {
    eyeState = random(0, 5);
    lastEyeChange = millis();
  }

  float accelMag = sqrt(a.acceleration.x*a.acceleration.x + a.acceleration.y*a.acceleration.y + a.acceleration.z*a.acceleration.z);
  float shake = abs(accelMag - 9.8);
  float delta = abs(shake - lastShake);
  lastShake = shake;

  if (delta > 4.2 && !dizzy && !angry) {
    dizzy = true;
    dizzyStart = millis();
    soundDizzy();
  }

  if (dizzy) {
    drawDizzy();
    if (millis() - dizzyStart > 3000) {
      dizzy = false;
      angry = true;
      angryStart = millis();
      soundAngry();
    }
  } else if (angry) {
    drawAngry();
    if (millis() - angryStart > 3000) angry = false;
  } else if (happy) {
    drawHappy();
    if (millis() - happyStart > 2000) happy = false;
  } else {
    drawEyes();
  }

  delay(100);
}