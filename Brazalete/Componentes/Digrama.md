# 🤖 Proyecto Terabot — Monitoreo de IMU y Estado de Batería

Plataforma de prueba y simulación para el proyecto **Terabot**. Este módulo permite realizar la lectura y visualización en tiempo real de la unidad de medición inercial (**IMU MPU6050**) y el estado de carga de la batería utilizando un microcontrolador **ESP32-C3** y una pantalla **OLED SSD1306 (I2C)**.

---

## 📸 Demostración en Simulador

A continuación se muestra la ejecución de la prueba donde se procesan los datos del acelerómetro/giroscopio en simultáneo con el porcentaje y voltaje de la batería:

<div align="center">
  <img width="650" alt="Simulación Terabot en Wokwi" src="https://github.com/user-attachments/assets/7ac73af6-726d-4729-8ab4-1afc0c2910df" />
  <p><i>Figura 1: Entorno de simulación en tiempo real desplegando datos inerciales y nivel de batería.</i></p>
</div>

---

## 🛠️ Entorno de Simulación

El desarrollo y validación funcional se realizó utilizando el simulador web de sistemas embebidos **Wokwi**:

* **Plataforma de Simulación:** [Wokwi ESP32-C3 Simulator](https://wokwi.com/projects/new/esp32-c3)
* **Microcontrolador Base:** ESP32-C3
* **Protocolo de Comunicación:** I2C (SDA: GPIO 8, SCL: GPIO 9)

---

## 🚀 Características del Módulo

* **Lectura Inercial (IMU):** Captura de datos triaxiales de aceleración ($m/s^2$) y velocidad angular ($rad/s$) mediante el MPU6050.
* **Monitor de Batería:**
  * Lectura analógica filtrada a través de pin ADC (GPIO 1).
  * Conversión de voltaje en el pin a porcentaje de carga ($0\% - 100\%$).
  * Despliegue permanente del porcentaje en la esquina superior derecha de la pantalla.
* **Interfaz Gráfica (OLED):** Renderizado compacto a 10 Hz para un seguimiento fluido de los sensores.
* **Verificación I2C:** Escaneo y reporte de periféricos conectados durante la secuencia de arranque (*setup*).

---

## 📌 Configuración de Pines (Pinout)

| Componente | Pin del Componente | Pin ESP32-C3 | Función |
| :--- | :--- | :--- | :--- |
| **OLED / IMU** | SDA | **GPIO 8** | Datos I2C |
| **OLED / IMU** | SCL | **GPIO 9** | Reloj I2C |
| **Divisor/Batería** | VOUT | **GPIO 1** | Lectura ADC Batería |
| **Alimentación** | VCC / GND | 3.3V / GND | Alimentación General |

---

## 📚 Librerías Requeridas

Si deseas compilar este proyecto localmente en **Arduino IDE**, instala las siguientes bibliotecas desde el Administrador de Librerías:

* `Adafruit SSD1306`
* `Adafruit GFX Library`
* `Adafruit MPU6050`
* `Adafruit Unified Sensor`
