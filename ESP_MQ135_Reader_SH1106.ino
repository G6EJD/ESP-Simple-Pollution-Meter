#ifdef ESP8266    // The pins for an ESP8266 Wemos D1 Mini
#define SDA D3   // Choose your pins according to your boards SDA and SCL lines
 #define SCL D4   // Pins for the ESP8266 for direct plug-in OLED, use e.g. dupont cables for other connections
#else
  #define SDA 5  // Choose your pins according to your boards SDA and SCL lines
  #define SCL 4 
#endif

#include "SH1106.h"               // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
#include <Wire.h>

SH1106 display(0x3c, SDA, SCL);   // OLED display object definition (address, SDA, SCL)

String message;

void setup() {
  Serial.begin(115200);
  Serial.println("Wait 2-mins or more for sensor to warm-up!");
  #if ESP8266
  Wire.begin(SDA, SCL);               // (sda,scl,bus_speed) Start the Wire service for the OLED display using pin=D4 for SCL and Pin-D3 for SDA
  #else
  Wire.begin(SDA, SCL, 100000);       // (sda,scl,bus_speed) Start the Wire service for the OLED display using pin=D4 for SCL and Pin-D3 for SDA
  #endif  
  display.init();                     // Initialise the display
  display.flipScreenVertically();     // In my case flip the screen around by 180°
  display.setContrast(255);           // If you want turn the display contrast down, 255 is maxium and 0 in minimum, in practice about 128 is OK
}

void loop() {
  int   Reading       = analogRead(A0);          // Raw ADC reading ESP8266
  //int   Reading       = analogRead(36);        // Raw ADC reading ESP32 on VP or analogRead(39); on VN or analogRead(35); on GPIO 35, you choose!
  float sensorVoltage = Reading/1024.0 * 5.00;   // Calibrated to measured sensor voltage using resitive divider
  float ppm           = sensorVoltage * 2500.0;  // Adjusted to ppm of Co2, clean air has 400ppm of Co2, sensor output = 0.16v in clean air (0.16x2500=400)
  Serial.print(String(Reading) + " " + String(sensorVoltage) + "v " + String(ppm) + "ppm Co2  ");
  if      (ppm > 6000) message = "Extreme";   // Arbitrary thresholds! or 'qualitative' or cannot be measured absolutely
  else if (ppm > 3000) message = "High";
  else if (ppm > 1500) message = "Moderate";
  else if (ppm > 500)  message = "Low";
  else                 message = "None";
  Serial.println(message);
  display.clear();
  display.setFont(ArialMT_Plain_16);  // Set the Font size LARGE
  display.drawString(20, 10, String(ppm,0));
  display.setFont(ArialMT_Plain_10);  // Set the Font size SMALL
  display.drawString(65,10,"ppm Co2");
  display.setFont(ArialMT_Plain_16);  // Set the Font size LARGE
  display.drawString(30, 30, message);
  display.display();
  delay(500);
}
