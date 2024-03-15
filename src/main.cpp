
#include <Arduino.h>



#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 19, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C

const uint8_t block[] PROGMEM = {
  0xf0, // B1111000
  0xb0, // B1011000
  0xd0, // B1101000
  0xf0, // B1111000
};


void setup(void) {
  u8g2.begin();
}


void loop(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(1,54,"github.com/olikraus/u8g2");
  u8g2.drawBitmap(0, 0, 1, 4, block);
  u8g2.sendBuffer();
  delay(1000);
}
