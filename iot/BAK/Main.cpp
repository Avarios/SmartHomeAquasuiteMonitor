/*
 *  This sketch demonstrates how to scan WiFi networks. 
 *  The API is almost the same as with the WiFi Shield library, 
 *  the most obvious difference being the different file you need to include:
 */
#include "ESP8266WiFi.h"            // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"  

const String SSID = "Adfnet";
const String WIFIPW = "33394151923058759658";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
SSD1306Wire display(0x3C, D1, D2, GEOMETRY_128_32);


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  bool isConnected =  WiFi.begin(SSID,WIFIPW);
  while (!isConnected)
  {
    Serial.println("Connecting to" + SSID);
    delay(100);
  }
  Serial.println("Connected to " + SSID);
 
  delay(2000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Setup done");
   display.init();
}

void loop() {
  digitalWrite(LED_BUILTIN,HIGH);

  display.drawString(0,0,"Connected to " + WiFi.SSID());
  delay(1000);
  display.display();
  digitalWrite(LED_BUILTIN,LOW);
  delay(1000);
}
