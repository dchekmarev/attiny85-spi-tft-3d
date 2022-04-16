#define OUT_REG PORTB
#define IN_REG PINB

#define OUT_HIGH(reg) (OUT_REG |= (1 << reg))
#define OUT_LOW(reg) (OUT_REG &= ~(1 << reg))

void spi_init() {
  DDRB |= (1 << TFT_DC) | (1 << TFT_DO) | (1 << TFT_SC);
}

inline void dly() { __asm__("nop\n\t"); };

void transfer(uint8_t dat) {

  for (uint8_t i = 8; i; --i) {
    (dat & 0x80) ? OUT_HIGH(TFT_DO) : OUT_LOW(TFT_DO);  // Mask for the eigth bit
    dat <<= 1;                        // Move
    dly();
    OUT_HIGH(TFT_SC);
    dly();
    OUT_LOW(TFT_SC);
    dly();
  }
  return true;

}
