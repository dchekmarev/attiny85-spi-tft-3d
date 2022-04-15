#include <tinySPI.h>            // https://github.com/JChristensen/tinySPI
// #include <SPI.h>

void spi_init() {
  pinMode(TFT_DC, OUTPUT);
  SPI.begin();                   // start hardware SPI (the library sets up the clock and data pins)
}

void transfer(uint8_t dat) {
  SPI.transfer(dat);
}

void command(uint8_t c) {
  digitalWrite(TFT_DC, LOW);
  transfer(c);
}

void data(uint8_t c) {
  digitalWrite(TFT_DC, HIGH);
  transfer(c);
}