#include <tinySPI.h>  // https://github.com/JChristensen/tinySPI

#define OUT_REG PORTB
#define IN_REG PINB

#define DC_HIGH (OUT_REG |= (1 << TFT_DC))
#define DC_LOW (OUT_REG &= ~(1 << TFT_DC))

void spi_init() {
  pinMode(TFT_DC, OUTPUT);
  USICR &= ~(_BV(USISIE) | _BV(USIOIE) | _BV(USIWM1));
  USICR |= _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
  SPI_DDR_PORT |= _BV(USCK_DD_PIN);  // set the USCK pin as output
  SPI_DDR_PORT |= _BV(DO_DD_PIN);    // set the DO pin as output
  SPI_DDR_PORT &= ~_BV(DI_DD_PIN);   // set the DI pin as input
}

void transfer(uint8_t dat) {
  USIDR = dat;

// cycle millis: 420, fps: 2.38 -> cycle millis: 285, fps: 3.51

  uint8_t CLK_LOW = (1 << USIWM0) | (1 << USICS1) | (1 << USITC);
  uint8_t CLK_HIGH = (1 << USIWM0) | (1 << USICS1) | (1 << USITC) | (1 << USICLK);

  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
  USICR = CLK_LOW;
  USICR = CLK_HIGH;
}

void command(uint8_t c) {
  DC_LOW;
  transfer(c);
}

void data(uint8_t c) {
  DC_HIGH;
  transfer(c);
}