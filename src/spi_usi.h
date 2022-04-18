#define SPI_DDR_PORT DDRB

#define USCK_DD_PIN DDB2
#define DO_DD_PIN DDB1
#define DI_DD_PIN DDB0

void spi_init() {
  SPI_DDR_PORT |= _BV(USCK_DD_PIN);  // set the USCK pin as output
  SPI_DDR_PORT |= _BV(DO_DD_PIN);    // set the DO pin as output
  SPI_DDR_PORT &= ~_BV(DI_DD_PIN);   // set the DI pin as input

  USICR = (1 << USIWM0) | (1 << USICS1);
}

// 320x240 full screen fill: cycle millis: 280, fps: 3.57

#define CLK_TOGGLE ((uint8_t) ((1 << USIWM0) | (1 << USICS1) | (1 << USITC)))

void transfer(uint8_t dat) {
  USIDR = dat;

  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
  USICR = CLK_TOGGLE;
}
