#include <Arduino.h>
#include <pas-co2-ino.hpp>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define I2C_FREQ_HZ 400000
#define PERIODIC_MEAS_INTERVAL_IN_SECONDS 10
#define PRESSURE_REFERENCE 900

PASCO2Ino co2Sensor;
int16_t co2Level;
Error_t err;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(I2C_FREQ_HZ);
    
    // Initialize OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Initializing...");
    display.display();
    delay(2000);
    
    // Initialize CO2 sensor
    err = co2Sensor.begin();
    if (XENSIV_PASCO2_OK != err) {
        Serial.print("Initialization error: ");
        Serial.println(err);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("CO2 Sensor Err");
        display.display();
        while (1);
    }
    
    err = co2Sensor.setPressRef(PRESSURE_REFERENCE);
    if (XENSIV_PASCO2_OK != err) {
        Serial.print("Pressure reference error: ");
        Serial.println(err);
    }
    
    err = co2Sensor.startMeasure(PERIODIC_MEAS_INTERVAL_IN_SECONDS);
    if (XENSIV_PASCO2_OK != err) {
        Serial.print("Start measure error: ");
        Serial.println(err);
    }
    
    delay(500);
}

void loop() {
    delay(PERIODIC_MEAS_INTERVAL_IN_SECONDS * 1000);
    
    err = co2Sensor.getCO2(co2Level);
    if (XENSIV_PASCO2_OK != err) {
        if (XENSIV_PASCO2_ERR_COMM == err) {
            delay(600);
            err = co2Sensor.getCO2(co2Level);
            if (XENSIV_PASCO2_OK != err) {
                Serial.print("Get CO2 error: ");
                Serial.println(err);
            }
        }
    }
    
    Serial.print("CO2 ppm value: ");
    Serial.println(co2Level);
    
    // Display CO2 level on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("CO2: ");
    display.setTextSize(4);
    display.setCursor(30,30);
    display.println(co2Level);
    display.display();
    
    err = co2Sensor.setPressRef(PRESSURE_REFERENCE);
    if (XENSIV_PASCO2_OK != err) {
        Serial.print("Pressure reference error: ");
        Serial.println(err);
    }
}

