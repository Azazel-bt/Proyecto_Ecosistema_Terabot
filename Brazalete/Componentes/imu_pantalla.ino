#include <Wire.h>

#define SDA_PIN 8
#define SCL_PIN 9

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println(F("Iniciando escaneo I2C..."));

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(1000);

  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("Dispositivo encontrado en 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0) {
    Serial.println(F("No se encontro ningun dispositivo"));
  } else {
    Serial.print(F("Total encontrados: "));
    Serial.println(count);
  }
}

void loop() {
  delay(5000);
  Serial.println(F("Reintentando..."));
  setup(); // vuelve a escanear cada 5s
}