/**
 * Math functions implementation where { x = (int16_t) x * FLOAT_FACTOR }
 */

#define PI_FACTOR (314 * FLOAT_FACTOR / 100)

#define radians_fast_int(x) ((int16_t) ((uint16_t) ((x + 360) % 360) * (PI_FACTOR / 10) / 18)) // (x * PI / 180 * FLOAT_FACTOR)

int16_t sin_fast_int(int16_t a) {
  while (a < 0) {
    a += PI_FACTOR * 2;
  }
  if (a > PI_FACTOR) {
    return -sin_fast_int(a - PI_FACTOR);
  }
  if (a > PI_FACTOR / 2) {
    return sin_fast_int(PI_FACTOR - a);
  }
  // Taylor series
  uint16_t ret = a;
  uint16_t ta = a;
  ta = ta * a / FLOAT_FACTOR / 2 / 3 * a / FLOAT_FACTOR;
  ret -= ta;
  ta = ta * a / FLOAT_FACTOR / 4 / 5 * a / FLOAT_FACTOR;
  ret += ta;
  ta = ta * a / FLOAT_FACTOR / 6 / 7 * a / FLOAT_FACTOR;
  ret -= ta;
  return ret;
}

int16_t cos_fast_int(int16_t a) {
  return sin_fast_int(PI_FACTOR / 2 - a);
}
