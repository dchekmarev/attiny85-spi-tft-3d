#define rgb_565(r, g, b) ( \
  ((r & ((1 << 6) - 1)) << 11) | \
    ((g & ((1 << 7) - 1)) << 6) | \
    (b & ((1 << 6) - 1)) )

void setAddrAndRW_cont(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  command(ILI9341_CASET); // Column addr set
  data(x >> 8);
  data(x & 0xFF);     // XSTART
  data(w >> 8);
  data(w & 0xFF);     // XEND

  command(ILI9341_PASET); // Row addr set
  data(y >> 8);
  data(y & 0xFF);     // YSTART
  data(h >> 8);
  data(h & 0xFF);     // YEND

  command(ILI9341_RAMWR); // write to RAM
}

void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c) {
  // Debug.print(F("x = "));
  // Debug.print(x);
  // Debug.print(F(", y = "));
  // Debug.print(y);
  // Debug.print(F(", w = "));
  // Debug.print(w);
  // Debug.print(F(", h = "));
  // Debug.print(h);
  // Debug.print(F(", c = "));
  // Debug.println(c);
  setAddrAndRW_cont(x, y, x + w - 1, y + h - 1);
  uint8_t hi = c >> 8, lo = c & 0xFF;
  digitalWrite(TFT_DC, HIGH);
  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      transfer(hi);
      transfer(lo);
    }
  }
  digitalWrite(TFT_DC, LOW);
}

void clearRect(uint16_t x, uint16_t y, int16_t w, int16_t h) {
  if (w > 0 && h > 0) {
    fillRect(x, y, w, h, 0);
  }
}
