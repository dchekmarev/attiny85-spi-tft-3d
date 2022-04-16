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

    if (abs(dy) <= abs(dx)) {
      // it's more horizontal, draw with hLines
      step_x = (float) dx / abs(dy);
      step_y = (float) dy / abs(dy);
    } else {
      // it's more vertical, draw with vLines
      step_x = (float) dx / abs(dx);
      step_y = (float) dy / abs(dx);
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

void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  fillRect(x, y, w, 1, color);
  fillRect(x, y, 1, h, color);
  fillRect(x + w - 1, y, 1, h, color);
  fillRect(x, y + h - 1, w, 1, color);
}

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  if (x1 < x2) {
    _drawLine(x1, y1, x2, y2);
  } else {
    _drawLine(x2, y2, x1, y1);
  }
}
