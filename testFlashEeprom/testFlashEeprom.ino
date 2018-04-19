#include <EEPROM.h>
#include <NeoPixelBus.h>
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
  EEPROM.begin(4);
  Serial.begin(115200);
  EEPROM.write(0,0);
  EEPROM.commit();
}

void loop() {
  static uint8_t numToCheck = 0;
  static uint32_t writes = 0;
  uint8_t wheel = 0;
  wheel = EEPROM.read(0);
  Serial.println("EEPROM write: "+String(writes++)+"\tvalue: "+String(wheel));
  colorWipe(wheelColor(wheel,255));
  if (numToCheck++ != wheel) {
    Serial.println("FAIL");
    while (1) yield();
  }
  EEPROM.write(0,++wheel);
  EEPROM.commit();
  //delay(10);
}
