#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

// ---------- PANTALLA OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- RFID RC522 ----------
#define RST_PIN  14
#define SS_PIN   10
#define SPI_SCK  12
#define SPI_MISO 13
#define SPI_MOSI 11
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Iniciando..."));

  // ---- OLED ----
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(1000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("ERROR: OLED no responde"));
    while (1) delay(1000);
  }
  Serial.println(F("OLED OK"));

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Acerca un tag NFC..."));
  display.display();

  // ---- RFID ----
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SS_PIN);
  rfid.PCD_Init();
  delay(50);

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print(F("MFRC522 version: 0x"));
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF) {
    Serial.println(F("ERROR: RC522 no detectado (revisar cableado SPI)"));
  } else {
    Serial.println(F("RC522 OK"));
  }
}

void loop() {
  // Espera a que haya un tag nuevo
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  // ---- UID ----
  Serial.print(F("UID: "));
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) { Serial.print("0"); uidStr += "0"; }
    Serial.print(rfid.uid.uidByte[i], HEX);
    uidStr += String(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
    if (i < rfid.uid.size - 1) uidStr += " ";
  }
  Serial.println();

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print(F("Tipo: "));
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // ---- Leer paginas de memoria (NTAG213/215, tipo MIFARE Ultralight) ----
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("UID:"));
  display.println(uidStr);
  display.println();
  display.println(F("Leyendo datos..."));
  display.display();

  Serial.println(F("--- Contenido (paginas 4 a 15) ---"));

  String textoLegible = "";
  byte buffer[18];
  byte size = sizeof(buffer);

  for (byte page = 4; page <= 15; page++) {
    MFRC522::StatusCode status = rfid.MIFARE_Read(page, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Error leyendo pagina "));
      Serial.println(page);
      break;
    }

    Serial.print(F("Pag "));
    Serial.print(page);
    Serial.print(F(": "));
    for (byte i = 0; i < 4; i++) {
      if (buffer[i] < 0x10) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");

      // Guarda caracteres imprimibles para mostrar como texto
      if (buffer[i] >= 32 && buffer[i] <= 126) {
        textoLegible += (char)buffer[i];
      } else {
        textoLegible += ".";
      }
    }
    Serial.println();
  }

  Serial.print(F("Texto ASCII detectado: "));
  Serial.println(textoLegible);
  Serial.println(F("-----------------------------"));

  // ---- Mostrar en pantalla ----
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("UID:"));
  display.println(uidStr);

  display.setCursor(0, 24);
  display.println(F("Texto:"));
  display.println(textoLegible.substring(0, 21));
  if (textoLegible.length() > 21) {
    display.println(textoLegible.substring(21, 42));
  }

  display.display();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(2000); // pausa antes de aceptar otro tag
}