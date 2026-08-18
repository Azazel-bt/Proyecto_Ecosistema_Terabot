# 🤖 Proyecto Terabot — Monitoreo Integrado: IMU, Batería y Identificación NFC

Plataforma de prueba y simulación para el proyecto **Terabot**. Este módulo permite realizar la lectura, procesamiento y visualización en tiempo real de una unidad de medición inercial (**IMU MPU6050**), la identificación de tags mediante **NFC/RFID (RC522)** y el monitoreo del estado de carga de la batería utilizando un microcontrolador **ESP32-C3-MINI-1** y una pantalla **OLED SSD1306 (I2C)**.

---

## 📸 Demostración en Simulador

A continuación se muestra el esquema del circuito y el despliegue gráfico en pantalla, procesando datos inerciales, lecturas de tarjetas NFC (1, 2 o 3) y el porcentaje de batería en simultáneo:

<div align="center">
  <table>
    <tr>
      <td align="center">
        <img width="500" alt="Circuito Completo Wokwi" src="https://github.com/user-attachments/assets/6c11eea2-9232-41f4-a3e9-894d6b73c1ad" />
        <br>
        <i>Figura 1: Conexión de componentes en el simulador Wokwi.</i>
      </td>
      <td align="center">
        <img width="220" alt="Pantalla OLED Terabot" src="https://github.com/user-attachments/assets/b865f46b-d412-4bf2-aadd-880b11c47b1d" />
        <br>
        <i>Figura 2: Renderizado en pantalla OLED SSD1306.</i>
      </td>
    </tr>
  </table>
</div>

---

## 🛠️ Entorno de Simulación y Hardware

El proyecto fue diseñado y validado sobre la arquitectura de la placa **ESP32-C3-MINI-1**:

* **Plataforma de Simulación:** [Wokwi ESP32-C3 Simulator](https://wokwi.com/projects/new/esp32-c3)
* **Microcontrolador Base:** ESP32-C3-MINI-1
* **Buses de Comunicación:**
  * **I2C:** Pantalla OLED + IMU MPU6050
  * **SPI:** Lector RFID/NFC RC522

---

## 🚀 Características del Sistema

* **Identificación NFC / RFID:**
  * Lectura de tags y tarjetas MFRC522 vía SPI.
  * Mapeo de UIDs para identificar tarjetas asociadas (`ID: 1`, `ID: 2`, `ID: 3`).
  * Persistencia temporal de la lectura en pantalla al retirar el tag.
* **Lectura Inercial (IMU):** Captura de datos triaxiales de aceleración ($m/s^2$) y velocidad angular ($rad/s$) mediante el sensor MPU6050.
* **Monitor de Batería:**
  * Lectura analógica filtrada mediante promediado por el ADC (GPIO 1).
  * Conversión de voltaje a porcentaje de carga ($0\% - 100\%$).
  * Despliegue permanente del estado en la esquina superior derecha de la pantalla.
* **Interfaz Gráfica (OLED):** Renderizado optimizado a 20 Hz para un seguimiento fluido de periféricos.

---

## 📌 Configuración de Pines (Pinout ESP32-C3-MINI-1)

| Periférico | Pin Módulo | Pin ESP32-C3 | Protocolo / Función |
| :--- | :--- | :--- | :--- |
| **OLED / IMU** | SDA | **GPIO 8** | Datos I2C |
| **OLED / IMU** | SCL | **GPIO 9** | Reloj I2C |
| **RFID RC522** | SDA / SS | **GPIO 10** | SPI Chip Select |
| **RFID RC522** | SCK | **GPIO 4** | SPI Reloj |
| **RFID RC522** | MISO | **GPIO 5** | SPI Master In Slave Out |
| **RFID RC522** | MOSI | **GPIO 6** | SPI Master Out Slave In |
| **RFID RC522** | RST | **GPIO 7** | Reset Hardware |
| **Divisor / Batería** | VOUT | **GPIO 1** | Lectura ADC Batería |
| **Alimentación** | VCC / GND | **3.3V / GND** | Alimentación General |

> ⚠️ **Nota:** El módulo RC522 debe alimentarse estrictamente a **3.3V** para no dañar sus líneas lógicas.

---

## 📚 Librerías Requeridas

Para compilar el proyecto en **Arduino IDE**, asegúrate de instalar las siguientes bibliotecas desde el Administrador de Librerías (`Ctrl + Shift + I`):

* `Adafruit SSD1306`
* `Adafruit GFX Library`
* `Adafruit MPU6050`
* `Adafruit Unified Sensor`
* `MFRC522` (por GitHub/Community)
