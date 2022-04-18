#include <tinySPI.h>            // https://github.com/JChristensen/tinySPI
// #include <SPI.h>

void spi_init() {
  SPI.begin();                   // start hardware SPI (the library sets up the clock and data pins)
}

void transfer(uint8_t dat) {
  SPI.transfer(dat);
}
