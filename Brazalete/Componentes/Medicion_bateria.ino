#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BATTERY_PIN 1

// CALIBRACIÓN DE VOLTAJE EN EL PIN ADC (GPIO 1)
// Si al estar 100% cargada el pin mide 1.88V, establecemos VBAT_MAX = 1.88
#define VBAT_MAX 1.88   // Voltaje en pin a plena carga (100%)
#define VBAT_MIN 1.40   // Voltaje en pin cuando la batería está descargada (0%)

// OPCIONAL: Si usas un divisor de tensión (ej. 2x100k), pon el factor aquí.
// Si deseas mostrar el voltaje directo del pin, déjalo en 1.0
#define VOLTAGE_DIVIDER_FACTOR 1.0  

// Configuración del filtro de promedio (suavizado de lectura)
const int NUM_SAMPLES = 10;

float readBatteryVoltage() {
  uint32_t totalMv = 0;
  
  // Tomamos varias muestras para eliminar el ruido del ADC
  for (int i = 0; i < NUM_SAMPLES; i++) {
    totalMv += analogReadMilliVolts(BATTERY_PIN);
    delay(2);
  }
  
  float averageMv = (float)totalMv / NUM_SAMPLES;
  float pinVoltage = averageMv / 1000.0;
  
  return pinVoltage * VOLTAGE_DIVIDER_FACTOR;
}

int batteryPercent(float voltage) {
  if (voltage >= VBAT_MAX) return 100;
  if (voltage <= VBAT_MIN) return 0;
  
  float pct = ((voltage - VBAT_MIN) / (VBAT_MAX - VBAT_MIN)) * 100.0;
  return constrain((int)pct, 0, 100);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Iniciando Monitor de Bateria Calibrado..."));

  // Configuración de resolución ADC a 12 bits (0 - 4095)
  analogReadResolution(12);

  // Inicialización de I2C con pines personalizados
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(1000);

  // Inicialización de pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("ERROR: OLED no responde en dirección 0x3C"));
    while (1) delay(1000);
  }

  Serial.println(F("OLED Inicializada Correctamente"));
  display.clearDisplay();
  display.display();
}

void loop() {
  // Lecturas
  int rawADC = analogRead(BATTERY_PIN);
  float voltage = readBatteryVoltage();
  int pct = batteryPercent(voltage);

  // ---- Salida por Puerto Serie ----
  Serial.print(F("Raw ADC: "));
  Serial.print(rawADC);
  Serial.print(F(" | Voltaje Pin: "));
  Serial.print(voltage, 3);
  Serial.print(F(" V | Batería: "));
  Serial.print(pct);
  Serial.println(F(" %"));

  // ---- Renderizado en Pantalla OLED ----
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Encabezado
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("=== BATERIA (OK) ==="));

  // Lectura ADC cruda
  display.setCursor(0, 14);
  display.print(F("ADC Raw: "));
  display.println(rawADC);

  // Voltaje
  display.setCursor(0, 26);
  display.print(F("Voltaje: "));
  display.print(voltage, 3);
  display.println(F(" V"));

  // Porcentaje destacado
  display.setTextSize(2);
  display.setCursor(0, 44);
  display.print(pct);
  display.print(F(" %"));

  // Barra de progreso visual rápida
  int barWidth = map(pct, 0, 100, 0, 50);
  display.drawRect(70, 46, 54, 14, WHITE);
  display.fillRect(72, 48, barWidth > 0 ? barWidth - 4 : 0, 10, WHITE);

  display.display();

  delay(500);
}
