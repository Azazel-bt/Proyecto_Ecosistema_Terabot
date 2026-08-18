#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Configuración de pantalla OLED (128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;

// Configuración Batería (ADC GPIO 1)
#define BATTERY_PIN 1
#define VBAT_MAX 1.88   // Voltaje en el pin al 100%
#define VBAT_MIN 1.40   // Voltaje en el pin al 0%
#define VOLTAGE_DIVIDER_FACTOR 1.0  
const int NUM_SAMPLES = 10;

// LECTURA Y CALCULO DE BATERÍA
float readBatteryVoltage() {
  uint32_t totalMv = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    totalMv += analogReadMilliVolts(BATTERY_PIN);
    delay(1);
  }
  float averageMv = (float)totalMv / NUM_SAMPLES;
  return (averageMv / 1000.0) * VOLTAGE_DIVIDER_FACTOR;
}

int batteryPercent(float voltage) {
  if (voltage >= VBAT_MAX) return 100;
  if (voltage <= VBAT_MIN) return 0;
  float pct = ((voltage - VBAT_MIN) / (VBAT_MAX - VBAT_MIN)) * 100.0;
  return constrain((int)pct, 0, 100);
}

// ESCANEO INICIAL DE DISPOSITIVOS I2C
void scanI2C() {
  Serial.println(F("Iniciando escaneo I2C..."));
  byte count = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("Dispositivo I2C en 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  Serial.print(F("Total I2C encontrados: "));
  Serial.println(count);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Iniciando MPU6050 + OLED + Monitor de Bateria..."));

  analogReadResolution(12);

  // Inicialización de bus I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(1000);

  // Escaneo I2C por puerto serie
  scanI2C();

  // Inicializar Pantalla OLED (Dirección por defecto 0x3C)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("ERROR: No se encontró la pantalla OLED en 0x3C"));
    while (1) delay(500);
  }

  // Inicializar IMU (MPU6050 en 0x68 o 0x69)
  if (!mpu.begin()) {
    Serial.println(F("ERROR: No se encontró la IMU MPU6050"));
    
    // Mensaje de advertencia en OLED si falla la IMU
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(F("ERROR: IMU no hallada"));
    display.display();
    while (1) delay(500);
  }

  // Configuración MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println(F("Sensores e interfaces iniciados correctamente."));
  display.clearDisplay();
  display.display();
}

void loop() {
  // 1. Obtener datos de la IMU
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // 2. Obtener datos de Batería
  float vbat = readBatteryVoltage();
  int pct = batteryPercent(vbat);

  // 3. Imprimir datos por Monitor Serie
  Serial.print(F("Accel X:")); Serial.print(a.acceleration.x, 1);
  Serial.print(F(" Y:")); Serial.print(a.acceleration.y, 1);
  Serial.print(F(" Z:")); Serial.print(a.acceleration.z, 1);
  Serial.print(F(" | Bat: ")); Serial.print(vbat, 2);
  Serial.print(F("V (")); Serial.print(pct); Serial.println(F("%)"));

  // 4. Renderizar en la pantalla OLED
  display.clearDisplay();
  display.setTextColor(WHITE);

  // --- ENCABEZADO Y BATERÍA EN LA ESQUINA SUPERIOR DERECHA ---
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("IMU MPU6050"));

  // Dibujar porcentaje y pequeño icono de batería a la derecha (x: 88 px)
  display.setCursor(88, 0);
  if (pct < 100) display.print(F(" ")); // Alineación
  display.print(pct);
  display.print(F("%"));

  // Línea divisora
  display.drawFastHLine(0, 11, 128, WHITE);

  // --- MUESTRA DE DATOS DE LA IMU ---
  // Acelerómetro (m/s²)
  display.setCursor(0, 16);
  display.print(F("AccX: ")); display.print(a.acceleration.x, 1);
  display.setCursor(0, 27);
  display.print(F("AccY: ")); display.print(a.acceleration.y, 1);
  display.setCursor(0, 38);
  display.print(F("AccZ: ")); display.print(a.acceleration.z, 1);

  // Giroscopio (rad/s)
  display.setCursor(68, 16);
  display.print(F("GyrX:")); display.print(g.gyro.x, 1);
  display.setCursor(68, 27);
  display.print(F("GyrY:")); display.print(g.gyro.y, 1);
  display.setCursor(68, 38);
  display.print(F("GyrZ:")); display.print(g.gyro.z, 1);

  // Voltaje directo en la parte inferior
  display.setCursor(0, 54);
  display.print(F("VBat: ")); display.print(vbat, 2); display.print(F("V"));

  display.display();
  delay(100); // Refresco fluido a 10 Hz
}
