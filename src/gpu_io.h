#define OUT_REG PORTB

// #define DC_HIGH digitalWrite(TFT_DC, HIGH)
// #define DC_LOW digitalWrite(TFT_DC, LOW)

#define DC_HIGH (OUT_REG |= (1 << TFT_DC))
#define DC_LOW (OUT_REG &= ~(1 << TFT_DC))

void command(uint8_t c) {
  DC_LOW;
  transfer(c);
}

void data(uint8_t c) {
  DC_HIGH;
  transfer(c);
}

#define REFRESH_PAUSE_METHOD 1

void refresh_rate_slow() {
#if REFRESH_PAUSE_METHOD == 1
  // set big porch which causes refresh pause when sum (VFP + VBP) = (0x60 + 0x62) for tjctm24024-spi
  // max possible values from spec: command(0xb5); data(0x7f); data(0x7f); data(0x1f); data(0x1f);
  command(0xb5); data(0x62); data(0x62); data(0x02); data(0x02);
#else
  // reduce screen height to minimum of 16px, it works fine if there's static header
  command(ILI9341_DFUNCTR); data(0x08); data(0x82); data(0x01); // Display Function Control, default is 0A,82,27
#endif
}

void refresh_rate_normal() {
#if REFRESH_PAUSE_METHOD == 1
  // command(0xb5); data(0x02); data(0x02); data(0x0a); data(0x14); // default
  command(0xb5); data(0x02); data(0x02); data(0x02); data(0x02);
#else
  command(ILI9341_DFUNCTR); data(0x08); data(0x82); data(0x27); // Display Function Control, default is 0A,82,27
#endif
}

void setAddrAndRW_cont(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  command(ILI9341_CASET); // Column addr set
  data(x1 >> 8);
  transfer(x1 & 0xFF);     // XSTART
  transfer(x2 >> 8);
  transfer(x2 & 0xFF);     // XEND

  command(ILI9341_PASET); // Row addr set
  data(y1 >> 8);
  transfer(y1 & 0xFF);     // YSTART
  transfer(y2 >> 8);
  transfer(y2 & 0xFF);     // YEND

  command(ILI9341_RAMWR); // write to RAM
}

void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c) {
  w = min(w, SCREEN_WIDTH - x);
  h = min(h, SCREEN_HEIGHT - y);
  setAddrAndRW_cont(x, y, x + w - 1, y + h - 1);
  uint8_t hi = c >> 8, lo = c & 0xFF;
  DC_HIGH;
  for (uint16_t x = w; x; --x) {
    for (uint16_t y = h; y; --y) {
      transfer(hi);
      transfer(lo);
    }
  }
  DC_LOW;
}
