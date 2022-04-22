/**
 * a floating 3d shape on ili9341 over spi running on attiny85
 *
 * check out oled/i2c version at https://wokwi.com/projects/328750728540586580
 */

#include <Arduino.h>

#include <util/delay.h>

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

static const uint8_t init_commands[] PROGMEM =
{
  // ref https://github.com/adafruit/Adafruit_ILI9341/blob/master/Adafruit_ILI9341.cpp
    1, ILI9341_SWRESET,
    4, 0xEF, 0x03, 0x80, 0x02,
    4, 0xCF, 0x00, 0XC1, 0X30, // 8.4.2 Power control B (CFh)
    5, 0xED, 0x64, 0x03, 0X12, 0X81, // 8.4.6 Power on sequence control (EDh)
    4, 0xE8, 0x85, 0x00, 0x78, // 8.4.3 Driver timing control A (E8h)
    6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02, // 8.4.1 Power control A (CBh)
    2, 0xF7, 0x20, // 8.4.8 Pump ratio control (F7h)
    3, 0xEA, 0x00, 0x00, // 8.4.5 Driver timing control B (EAh)
    2, ILI9341_PWCTR1, 0x23, // Power control, default 0x21 (4.5V), 0x09h is 3.ff
    2, ILI9341_PWCTR2, 0x10, // Power control, default is 0x10
    3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
    2, ILI9341_VMCTR2, 0x86, // VCM control2
    2, ILI9341_MADCTL, 0x48, // Memory Access Control
    2, ILI9341_VSCRSADD, 0x00,             // Vertical scroll zero
    2, ILI9341_PIXFMT, 0x55,
    3, ILI9341_FRMCTR1, 0x00, 0x18, // 8.3.2. Frame Rate Control (In Normal Mode/Full Colors) (B1h)
    4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control, default is 0A,82,27
    2, 0xF2, 0x00, // Gamma Function Disable
    2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
    16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    // // Backlight Control
    // 2, 0xB8, 0x0C, // default 0x0C, 00-0F
    // 2, 0xB9, 0xCC, // default 0xCC, 00-FF
    // 2, 0xBA, 0x04, // default 0x04
    // 2, 0xBB, 0x65, // default 0x65, DTH_ST [3:0]/DTH_MV [3:0]: This parameter is used set the minimum limitation of grayscale threshold value.
    // 2, 0xBC, 0x44, // default 0x44, DIM1 [2:0]: This parameter is used to set the transition time of brightness level
    //                //               to avoid the sharp brightness transition on vision. 
    // 2, 0xBE, 0x0F, // default 0x0F, PWM_DIV [7:0]: PWM_OUT output frequency control. This command is used to adjust
    //                //               the PWM waveform frequency of PWM_OUT.
    //                //               The PWM frequency can be calculated by using the following equation. 
    // 2, 0xBF, 0x00, // default 0x00, LEDPWMPOL: The bit is used to define polarity of LEDPWM signal.

// 8.2.42. Write Content Adaptive Brightness Control (55h) 

    0 // 0 indicates last command
};

#include "gfx.h"

#include "shape.h"

void tft_init() {
  DDRB |= _BV(TFT_DC);  // enable tft dc (pb3) as output
  PORTB |= _BV(TFT_DC); // set to high

  const uint8_t *init_commands_ptr = init_commands;
  for (;;) {
    uint8_t cmd_size = pgm_read_byte(init_commands_ptr++);
    if (!cmd_size) {
      break;
    }
    command(pgm_read_byte(init_commands_ptr++));
#if DEBUG_ENABLED == 1
    Debug.print(F("i = "));
    Debug.print(i);
    Debug.print(F(", cmd_size = "));
    Debug.print(cmd_size);
    Debug.print(F(", cmd = "));
    Debug.println(init_commands[i]);
#endif
    for (--cmd_size; cmd_size > 0; --cmd_size) {
      data(pgm_read_byte(init_commands_ptr++));
    }
  }

  command(ILI9341_SLPOUT);    //Exit Sleep
  _delay_ms(120); // btw, 12 bytes each call
  command(ILI9341_DISPON);
  _delay_ms(120);
}

void setup() {
  DDRB |= _BV(PB4);  // enable serial port (pb4) as output
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

  shape_init();

  fillRect(0, 0, 240, 320, 0);

#if (DEBUG_ENABLED == 1) || (SERIAL_ENABLED == 1)
  Serial.println(F("started"));
#endif
}

#define BOX_WIDTH 64
#define BOX_HEIGHT 64

uint16_t x = 0, y = 0;
int8_t dx = 3, dy = 3;
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

uint16_t framesTillColorChange = 0;

void floatingBoxLoop() {

  if ( (x + dx < 1) || (x + dx > SCREEN_WIDTH - BOX_WIDTH) ) {
    dx = -dx;
  }
  if ( (y + dy < 1) || (y + dy > SCREEN_HEIGHT- BOX_HEIGHT) ) {
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

void connectPoints(uint8_t i, uint8_t j, uint16_t points[][2]) {
  drawLine(points[i][0] + x, points[i][1] + y, points[j][0] + x, points[j][1] + y);
}

void boxAroundScreen(uint16_t color) {
    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
    // boxes at corners
    #define CORNER_BOX_SIZE 10
    fillRect(0, 0, CORNER_BOX_SIZE, CORNER_BOX_SIZE, color);
    fillRect(SCREEN_WIDTH - CORNER_BOX_SIZE, 0, CORNER_BOX_SIZE, CORNER_BOX_SIZE, color);
    fillRect(0, SCREEN_HEIGHT - CORNER_BOX_SIZE, CORNER_BOX_SIZE, CORNER_BOX_SIZE, color);
    fillRect(SCREEN_WIDTH - CORNER_BOX_SIZE, SCREEN_HEIGHT - CORNER_BOX_SIZE, CORNER_BOX_SIZE, CORNER_BOX_SIZE, color);
}

uint16_t old_points[NPOINTS][2];
void shapeLoop() {

#if DEBUG_ENABLED == 1
  c = 1;
#endif

  if ( (x + dx < 0) || ((x + dx) > (SCREEN_WIDTH - CUBE_SIZE)) ) {
    dx = -dx;
  }
  if ( (y + dy < 0) || ((y + dy) > (SCREEN_HEIGHT - CUBE_SIZE)) ) {
    dy = -dy;
  }

  memmove(old_points, points, sizeof(uint16_t) * NPOINTS * 2);

  shape_calculate();

  color = 0;
  shape_render(old_points);

  x += dx;
  y += dy;
#if DEBUG_ENABLED == 1
  x = y = 0;
#endif
  // x = (SCREEN_WIDTH - CUBE_SIZE) / 2; y = (SCREEN_HEIGHT - CUBE_SIZE) / 2;

  color = colors[c];
  shape_render(points);
}

void loop() {

#if SERIAL_ENABLED == 1
  uint32_t loopStart = millis();
#endif

  if (0 == framesTillColorChange--) {
    c = (c + 1) % (sizeof(colors) / sizeof(uint16_t));
    framesTillColorChange = 20 * 5; // 20 fps * 5 seconds
    boxAroundScreen(colors[c]);
  }

  refresh_rate_slow();
  _delay_ms(1); // wait till scanline runs out of screen

  shapeLoop();
  // fillScreenLoop();
  // floatingBoxLoop();

  refresh_rate_normal();

#if SERIAL_ENABLED == 1
  totalTimeSum += millis() - loopStart;
  count++;

  if (totalTimeSum > 5000 || count > 300) { // print stats every ~5000ms or 300 frames
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
  _delay_ms(1);

}