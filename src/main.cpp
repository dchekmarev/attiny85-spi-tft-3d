/**
 * a floating 3d cube on ili9341 over spi running on attiny85
 *
 * check out oled/i2c version at https://wokwi.com/projects/328750728540586580
 */

#include <Arduino.h>

// #define DEBUG_ENABLED 1
// #define SERIAL_ENABLED 1

#if SERIAL_ENABLED == 1
#include <SoftwareSerial.h>
#define rxPin 5    // We use a non-existant pin as we are not interested in receiving data
#define txPin PB4
SoftwareSerial serial(rxPin, txPin);
#define Debug serial
#define Serial serial // redefine serial to be software implementation
#endif

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#if DEBUG_ENABLED == 1
#if SERIAL_ENABLED != 1
#include <TinyDebug.h>
#define Serial Debug
#endif
#endif

#define TFT_DC   PB3
#define TFT_DO   PB1
#define TFT_SC   PB2

// #include "spi_softduino.h";
// #include "spi_bitbang.h"
// #include "spi_tiny.h"
#include "spi_usi.h"

#include "ILI9341.h"

static const uint8_t init_commands[] =
{
    1, ILI9341_SWRESET,
    4, 0xEF, 0x03, 0x80, 0x02,
    4, 0xCF, 0x00, 0XC1, 0X30,
    5, 0xED, 0x64, 0x03, 0X12, 0X81,
    4, 0xE8, 0x85, 0x00, 0x78,
    6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
    2, 0xF7, 0x20,
    3, 0xEA, 0x00, 0x00,
    2, ILI9341_PWCTR1, 0x23, // Power control
    2, ILI9341_PWCTR2, 0x10, // Power control
    3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
    2, ILI9341_VMCTR2, 0x86, // VCM control2
    2, ILI9341_MADCTL, 0x48, // Memory Access Control
    2, ILI9341_PIXFMT, 0x55,
    3, ILI9341_FRMCTR1, 0x00, 0x18,
    4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
    2, 0xF2, 0x00, // Gamma Function Disable
    2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
    16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    0
};

#include "gfx.h"

void tft_init() {
  pinMode(TFT_DC, OUTPUT);

  for (uint8_t i = 0; i < sizeof(init_commands); ++i) {
    uint8_t cmd_size = init_commands[i];
    command(init_commands[++i]);
#if DEBUG_ENABLED == 1
    Debug.print(F("i = "));
    Debug.print(i);
    Debug.print(F(", cmd_size = "));
    Debug.print(cmd_size);
    Debug.print(F(", cmd = "));
    Debug.println(init_commands[i]);
#endif
    for (--cmd_size; cmd_size > 0; --cmd_size) {
      data(init_commands[++i]);
    }
  }

  command(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  command(ILI9341_DISPON);
  delay(120);
}

void setup() {
  pinMode(PB4, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  digitalWrite(TFT_DC, HIGH);
#if SERIAL_ENABLED == 1
  Serial.begin(9600);
#endif
#if (DEBUG_ENABLED == 1) && (SERIAL_ENABLED != 1)
  Serial.begin();
#endif
#if (DEBUG_ENABLED == 1) || (SERIAL_ENABLED == 1)
  Serial.println(F("starting"));
#endif
  spi_init();

  tft_init();

  fillRect(0, 0, 240, 320, 0);

#if (DEBUG_ENABLED == 1) || (SERIAL_ENABLED == 1)
  Serial.println(F("started"));
#endif
}

#define BOX_WIDTH 128
#define BOX_HEIGHT 128

uint16_t x = 0, y = 0;
int8_t dx = 5, dy = 3;
uint8_t c = 0;

static const uint16_t colors[] = {
  rgb_565(0xff, 0x07, 0x07),
  rgb_565(0x00, 0xff, 0x00),
  rgb_565(0x0f, 0x0f, 0xff),
  rgb_565(0xff, 0xff, 0x00),
  rgb_565(0xff, 0x00, 0xff),
  rgb_565(0x00, 0xff, 0xff)
};

uint32_t totalTimeSum = 0;
uint16_t count = 0;

void fillScreenLoop() {

  c = (c + 1) % (sizeof(colors) / sizeof(uint16_t));
  fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, colors[c]);
}

unsigned long lastColorChange;

void floatingBoxLoop() {

  if ( (x + dx < 0) || (x + dx > SCREEN_WIDTH - BOX_WIDTH) ) {
    dx = -dx;
  }
  if ( (y + dy < 0) || (y + dy > SCREEN_HEIGHT- BOX_HEIGHT) ) {
    dy = -dy;
  }
  clearRect(x, y, dx, BOX_HEIGHT);
  clearRect(x, y, BOX_WIDTH, dy);
  clearRect(x + BOX_WIDTH + dx, y, -dx, BOX_HEIGHT);
  clearRect(x, y + BOX_HEIGHT + dy, BOX_WIDTH, -dy);
  x += dx;
  y += dy;

  fillRect(x, y, BOX_WIDTH, BOX_HEIGHT, colors[c]);

}

#include "cube.h"

void connectPoints(uint8_t i, uint8_t j, uint16_t points[][2]) {
  drawLine(points[i][0] + x, points[i][1] + y, points[j][0] + x, points[j][1] + y);
}

void cubeLoop() {
  if (millis() > lastColorChange) {
    c = (c + 1) % (sizeof(colors) / sizeof(uint16_t));
    lastColorChange = millis() + 5000;

    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, colors[c]);
    // boxes at corners
    #define CORNER_BOX_SIZE 10
    fillRect(0, 0, CORNER_BOX_SIZE, CORNER_BOX_SIZE, colors[c]);
    fillRect(SCREEN_WIDTH - CORNER_BOX_SIZE, 0, CORNER_BOX_SIZE, CORNER_BOX_SIZE, colors[c]);
    fillRect(0, SCREEN_HEIGHT - CORNER_BOX_SIZE, CORNER_BOX_SIZE, CORNER_BOX_SIZE, colors[c]);
    fillRect(SCREEN_WIDTH - CORNER_BOX_SIZE, SCREEN_HEIGHT - CORNER_BOX_SIZE, CORNER_BOX_SIZE, CORNER_BOX_SIZE, colors[c]);
  }

#if DEBUG_ENABLED == 1
  c = 1;
#endif

  if ( (x + dx < 0) || ((x + dx) > (SCREEN_WIDTH - CUBE_SIZE)) ) {
    dx = -dx;
  }
  if ( (y + dy < 0) || ((y + dy) > (SCREEN_HEIGHT - CUBE_SIZE)) ) {
    dy = -dy;
  }

  uint16_t old_points[NPOINTS][2];
  memmove(old_points, points, sizeof(uint16_t) * NPOINTS * 2);

  cube_calculate();

  color = 0;
  cube_render(old_points);

  x += dx;
  y += dy;
#if DEBUG_ENABLED == 1
  x = y = 0;
#endif

  color = colors[c];
  cube_render(points);

  // delay(10);

}

void loop() {

  uint32_t loopStart = millis();

  cubeLoop();
  // fillScreenLoop();
  // floatingBoxLoop();

#if SERIAL_ENABLED == 1
  totalTimeSum += millis() - loopStart;
  count++;

  if (totalTimeSum > 5000 || count > 500) { // print stats every ~5000ms
    uint32_t millisPerCycle = totalTimeSum / count;
    Serial.print(millis());
    Serial.print(F("] cycle millis: "));
    Serial.print(millisPerCycle);
    Serial.print(F(", fps: "));
    Serial.println((uint32_t) 1000 * count / totalTimeSum);
    Serial.flush();
    count = totalTimeSum = 0;
  }
#endif

}