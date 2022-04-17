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

#define diff(a, b) (max(a, b) - min(a, b))

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
    int16_t step_x, step_y;

    if (abs(dy) <= abs(dx)) {
      // it's more horizontal, draw with hLines
      step_x = 100 * dx / abs(dy);
      step_y = 100 * dy / abs(dy);
    } else {
      // it's more vertical, draw with vLines
      step_x = 100 * dx / abs(dx);
      step_y = 100 * dy / abs(dx);
    }

    x1 *= 100; y1 *= 100; x2 *= 100; y2 *= 100;

    while ((x1 != x2) || (y1 != y2)) {
      uint16_t new_x = (int16_t) x1 + step_x;
      uint16_t new_y = (int16_t) y1 + step_y;
      if (min(x1, new_x) <= x2 && max(x1, new_x) >= x2) {
        new_x = x2;
      }
      if (min(y1, new_y) <= y2 && max(y1, new_y) >= y2) {
        new_y = y2;
      }
      fillRectMinMaxCoords(x1 / 100, y1 / 100, new_x / 100, new_y / 100, color);
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
