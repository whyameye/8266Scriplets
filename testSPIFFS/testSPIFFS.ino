#include <EEPROM.h>
#include <NeoPixelBus.h>
#include "FS.h"

#define PIXEL_PIN 2
#define PIXEL_COUNT 16

NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PIXEL_COUNT, PIXEL_PIN);

void colorWipe(RgbColor c) {

  // return; // doesn't crash when this is uncommented
  for(uint16_t i=0; i<PIXEL_COUNT; i++) {
    strip.SetPixelColor(i, c);
  }
  //return; // doesn't crash when this is uncommented
  strip.Show();
}

RgbColor wheelColor(byte WheelPos, byte iBrightness) {
  float R, G, B;
  float brightness = iBrightness / 255.0;

  if (WheelPos < 85) {
    R = WheelPos * 3;
    G = 255 - WheelPos * 3;
    B = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    R = 255 - WheelPos * 3;
    G = 0;
    B = WheelPos * 3;
  } else {
    WheelPos -= 170;
    R = 0;
    G = WheelPos * 3;
    B = 255 - WheelPos * 3;
  }
  R = R * brightness + .5;
  G = G * brightness + .5;
  B = B * brightness + .5;
  return RgbColor((byte) R,(byte) G,(byte) B);
}

void setup() {
  strip.Begin();
  Serial.begin(115200);
  SPIFFS.begin();
  SPIFFS.format(); // this takes some seconds
  File f = SPIFFS.open("/f.txt", "w");
  f.println(String(0));
  f.close();
}

void loop() {
  static uint32_t writes = 0;
  static uint8_t numToCheck = 0;
  
  File f = SPIFFS.open("/f.txt", "r");
  String line = f.readStringUntil('\n');
  f.close();
  uint8_t wheel = line.toInt();
  Serial.println("File write: "+String(writes++)+"\tvalue: "+String(wheel));
  colorWipe(wheelColor(wheel,255));
  if (numToCheck++ != wheel) {
    Serial.println("FAIL");
    while (1) yield();
  }
  File g = SPIFFS.open("/f.txt", "w");
  g.println(++wheel);
  g.close();
}
