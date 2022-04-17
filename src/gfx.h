#include "gpu_io.h"

#define rgb_565(r, g, b) ( \
  ((uint16_t) (r & ((1 << 6) - 1)) << 11) | \
    ((uint16_t) (g & ((1 << 7) - 1)) << 6) | \
    ((uint16_t) b & ((1 << 6) - 1)) )

void clearRect(uint16_t x, uint16_t y, int16_t w, int16_t h) {
  if (w > 0 && h > 0) {
    fillRect(x, y, w, h, 0);
  }
}

uint16_t color = 0;

#define swap_u16(a, b) { uint16_t t = a; a = b; b = t; }

__attribute__((noinline))
void fillRectMinMaxCoords(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  if (x1 > x2) {
    swap_u16(x1, x2);
  }
  if (y1 > y2) {
    swap_u16(y1, y2);
  }
  fillRect(x1, y1, max(x2 - x1, 1), max(y2 - y1, 1), color);
}

#define FLOAT_FACTOR 100

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
#if DEBUG_ENABLED == 1
  Debug.print(F("_drawLine x1 = "));
  Debug.print(x1);
  Debug.print(F(", y1 = "));
  Debug.print(y1);
  Debug.print(F(", x2 = "));
  Debug.print(x2);
  Debug.print(F(", y2 = "));
  Debug.print(y2);
  Debug.println();
#endif
  int16_t dx = x2 - x1;
  int16_t dy = y2 - y1;
  if (dx == 0 || dy == 0) {
    // vertical / horizontal
    fillRectMinMaxCoords(x1, y1, x2, y2, color);
  } else {
    // non-vertical
    uint16_t abs_dy = abs(dy);
    uint16_t abs_dx = abs(dx);
    // horizontal vs vertical
    int16_t step_size = min(abs_dy, abs_dx);
    int16_t step_x = FLOAT_FACTOR * dx / step_size;
    int16_t step_y = FLOAT_FACTOR * dy / step_size;

    x1 *= FLOAT_FACTOR; y1 *= FLOAT_FACTOR; x2 *= FLOAT_FACTOR; y2 *= FLOAT_FACTOR;

    #define between(a, b, inner) ((a <= inner && inner <= b) || (b <= inner && inner <= a))
    while ((x1 != x2) || (y1 != y2)) {
      uint16_t new_x = (int16_t) x1 + step_x;
      uint16_t new_y = (int16_t) y1 + step_y;
      if (between(x1, new_x, x2)) {
        new_x = x2;
      }
      if (between(y1, new_y, y2)) {
        new_y = y2;
      }
      fillRectMinMaxCoords(x1 / FLOAT_FACTOR, y1 / FLOAT_FACTOR, new_x / FLOAT_FACTOR, new_y / FLOAT_FACTOR, color);
      x1 = new_x;
      y1 = new_y;
    }
  }
}

void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  fillRect(x, y, w, 1, color);
  fillRect(x, y, 1, h, color);
  fillRect(x + w - 1, y, 1, h, color);
  fillRect(x, y + h - 1, w, 1, color);
}
