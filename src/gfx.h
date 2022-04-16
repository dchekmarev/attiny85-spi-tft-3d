#include "gpu_io.h"

#define rgb_565(r, g, b) ( \
  ((r & ((1 << 6) - 1)) << 11) | \
    ((g & ((1 << 7) - 1)) << 6) | \
    (b & ((1 << 6) - 1)) )

void clearRect(uint16_t x, uint16_t y, int16_t w, int16_t h) {
  if (w > 0 && h > 0) {
    fillRect(x, y, w, h, 0);
  }
}

uint16_t color = 0;

#define diff(a, b) (max(a, b) - min(a, b))

void _drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
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
    fillRect(min(x1, x2), min(y1, y2), diff(x2, x1) + 1, diff(y2, y1) + 1, color);
  } else {
    // non-vertical
    float step_x, step_y, x = x1, y = y1;

    float a = 1.0 * dy / dx;
    if (abs(a) <= 1) {
      // it's more horizontal, draw with hLines
      step_x = 1.0 * dx / abs(dy);
      step_y = 1.0 * dy / abs(dy);
    } else {
      // it's more vertical, draw with vLines
      step_x = 1.0 * dx / abs(dx);
      step_y = 1.0 * dy / abs(dx);
    }

    while (round(x) != x2 || round(y) != y2) {
      fillRect(min(x, x + step_x), min(y, y + step_y), abs(step_x), abs(step_y), color);
      if (round(x) != x2) {
        x += step_x;
      }
      if (round(y) != y2) {
        y += step_y;
      }
    }

  }
}

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  if (x1 < x2) {
    _drawLine(x1, y1, x2, y2);
  } else {
    _drawLine(x2, y2, x1, y1);
  }
}
