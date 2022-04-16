// void command(uint8_t c) {
//   // DC_LOW;
//   digitalWrite(TFT_DC, LOW);
//   transfer(c);
// }

// void data(uint8_t c) {
//   // DC_HIGH;
//   digitalWrite(TFT_DC, HIGH);
//   transfer(c);
// }

#define OUT_REG PORTB

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

void setAddrAndRW_cont(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  command(ILI9341_CASET); // Column addr set
  data(x1 >> 8);
  data(x1 & 0xFF);     // XSTART
  data(x2 >> 8);
  data(x2 & 0xFF);     // XEND

  command(ILI9341_PASET); // Row addr set
  data(y1 >> 8);
  data(y1 & 0xFF);     // YSTART
  data(y2 >> 8);
  data(y2 & 0xFF);     // YEND

  command(ILI9341_RAMWR); // write to RAM
}

void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c) {
#if DEBUG_ENABLED == 1
  Debug.print(F("fillRect x = "));
  Debug.print(x);
  Debug.print(F(", y = "));
  Debug.print(y);
  Debug.print(F(", w = "));
  Debug.print(w);
  Debug.print(F(", h = "));
  Debug.print(h);
  Debug.print(F(", c = "));
  Debug.println(c);
#endif
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
