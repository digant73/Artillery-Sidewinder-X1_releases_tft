#include "ST7920_Simulator.h"
#include "includes.h"
#include "GUI.h"
#include "../../Configuration.h"

#ifdef ST7920_SPI
// CGRAM buffer
uint8_t ST7920_CGRAM[64][2]; // [64*2] = [4 * 16*2*8], means 4 * [16*16] bitmap font,

const uint8_t ascii16x8[] = {
  /******************************************************************************
* Font Width:8
* Font Height:16
* Font Count:95
*******************************************************************************/
//UNICODE:20
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:21
0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:22
0x00,0x00,0x08,0x00,0x30,0x00,0x40,0x00,0x08,0x00,0x30,0x00,0x40,0x00,0x00,0x00,

//UNICODE:23
0x00,0x00,0x02,0x20,0x03,0xFC,0x1E,0x20,0x02,0x20,0x03,0xFC,0x1E,0x20,0x00,0x00,

//UNICODE:24
0x00,0x00,0x0E,0x18,0x11,0x04,0x11,0x04,0x3F,0xFF,0x10,0x84,0x0C,0x78,0x00,0x00,

//UNICODE:25
0x0F,0x00,0x10,0x8C,0x0F,0x30,0x01,0xC0,0x06,0x78,0x18,0x84,0x00,0x78,0x00,0x00,

//UNICODE:26
0x00,0x78,0x0F,0x84,0x10,0xC4,0x11,0x34,0x0E,0x98,0x00,0xE4,0x00,0x84,0x00,0x08,

//UNICODE:27
0x00,0x00,0x48,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:28
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x18,0x18,0x20,0x04,0x40,0x02,0x00,0x00,

//UNICODE:29
0x00,0x00,0x40,0x02,0x20,0x04,0x18,0x18,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:2A
0x02,0x40,0x02,0x40,0x01,0x80,0x0F,0xF0,0x01,0x80,0x02,0x40,0x02,0x40,0x00,0x00,

//UNICODE:2B
0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x07,0xF0,0x00,0x80,0x00,0x80,0x00,0x80,

//UNICODE:2C
0x00,0x00,0x00,0x09,0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:2D
0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,

//UNICODE:2E
0x00,0x00,0x00,0x0C,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:2F
0x00,0x00,0x00,0x06,0x00,0x18,0x00,0xE0,0x03,0x00,0x1C,0x00,0x20,0x00,0x00,0x00,

//UNICODE:30
0x00,0x00,0x07,0xF0,0x08,0x08,0x10,0x04,0x10,0x04,0x08,0x08,0x07,0xF0,0x00,0x00,

//UNICODE:31
0x00,0x00,0x00,0x00,0x08,0x04,0x08,0x04,0x1F,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,

//UNICODE:32
0x00,0x00,0x0E,0x0C,0x10,0x14,0x10,0x24,0x10,0x44,0x10,0x84,0x0F,0x0C,0x00,0x00,

//UNICODE:33
0x00,0x00,0x0C,0x18,0x10,0x04,0x10,0x84,0x10,0x84,0x11,0x44,0x0E,0x38,0x00,0x00,

//UNICODE:34
0x00,0x00,0x00,0x60,0x01,0xA0,0x02,0x24,0x0C,0x24,0x1F,0xFC,0x00,0x24,0x00,0x24,

//UNICODE:35
0x00,0x00,0x1F,0x98,0x11,0x04,0x11,0x04,0x11,0x04,0x10,0x88,0x10,0x70,0x00,0x00,

//UNICODE:36
0x00,0x00,0x07,0xF0,0x08,0x88,0x11,0x04,0x11,0x04,0x09,0x04,0x00,0xF8,0x00,0x00,

//UNICODE:37
0x00,0x00,0x18,0x00,0x10,0x00,0x10,0x7C,0x11,0x80,0x16,0x00,0x18,0x00,0x00,0x00,

//UNICODE:38
0x00,0x00,0x0E,0x38,0x11,0x44,0x10,0x84,0x10,0x84,0x11,0x44,0x0E,0x38,0x00,0x00,

//UNICODE:39
0x00,0x00,0x0F,0x80,0x10,0x48,0x10,0x44,0x10,0x44,0x08,0x88,0x07,0xF0,0x00,0x00,

//UNICODE:3A
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0C,0x03,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:3B
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:3C
0x00,0x00,0x00,0x80,0x01,0x40,0x02,0x20,0x04,0x10,0x08,0x08,0x10,0x04,0x00,0x00,

//UNICODE:3D
0x00,0x00,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x00,0x00,

//UNICODE:3E
0x00,0x00,0x10,0x04,0x08,0x08,0x04,0x10,0x02,0x20,0x01,0x40,0x00,0x80,0x00,0x00,

//UNICODE:3F
0x00,0x00,0x0E,0x00,0x12,0x00,0x10,0x0C,0x10,0xEC,0x11,0x00,0x0E,0x00,0x00,0x00,

//UNICODE:40
0x03,0xE0,0x0C,0x18,0x13,0xE4,0x14,0x14,0x17,0xF4,0x08,0x14,0x07,0xE8,0x00,0x00,

//UNICODE:41
0x00,0x04,0x00,0x3C,0x03,0xC4,0x1C,0x40,0x07,0x40,0x00,0xE4,0x00,0x1C,0x00,0x04,

//UNICODE:42
0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x04,0x11,0x04,0x0E,0x88,0x00,0x70,0x00,0x00,

//UNICODE:43
0x03,0xE0,0x0C,0x18,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x08,0x1C,0x10,0x00,0x00,

//UNICODE:44
0x10,0x04,0x1F,0xFC,0x10,0x04,0x10,0x04,0x10,0x04,0x08,0x08,0x07,0xF0,0x00,0x00,

//UNICODE:45
0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x04,0x17,0xC4,0x10,0x04,0x08,0x18,0x00,0x00,

//UNICODE:46
0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x00,0x17,0xC0,0x10,0x00,0x08,0x00,0x00,0x00,

//UNICODE:47
0x03,0xE0,0x0C,0x18,0x10,0x04,0x10,0x04,0x10,0x44,0x1C,0x78,0x00,0x40,0x00,0x00,

//UNICODE:48
0x10,0x04,0x1F,0xFC,0x10,0x84,0x00,0x80,0x00,0x80,0x10,0x84,0x1F,0xFC,0x10,0x04,

//UNICODE:49
0x00,0x00,0x10,0x04,0x10,0x04,0x1F,0xFC,0x10,0x04,0x10,0x04,0x00,0x00,0x00,0x00,

//UNICODE:4A
0x00,0x03,0x00,0x01,0x10,0x01,0x10,0x01,0x1F,0xFE,0x10,0x00,0x10,0x00,0x00,0x00,

//UNICODE:4B
0x10,0x04,0x1F,0xFC,0x11,0x04,0x03,0x80,0x14,0x64,0x18,0x1C,0x10,0x04,0x00,0x00,

//UNICODE:4C
0x10,0x04,0x1F,0xFC,0x10,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x0C,0x00,0x00,

//UNICODE:4D
0x10,0x04,0x1F,0xFC,0x1F,0x80,0x00,0x7C,0x1F,0x80,0x1F,0xFC,0x10,0x04,0x00,0x00,

//UNICODE:4E
0x10,0x04,0x1F,0xFC,0x0C,0x04,0x03,0x00,0x00,0xE0,0x10,0x18,0x1F,0xFC,0x10,0x00,

//UNICODE:4F
0x07,0xF0,0x08,0x08,0x10,0x04,0x10,0x04,0x10,0x04,0x08,0x08,0x07,0xF0,0x00,0x00,

//UNICODE:50
0x10,0x04,0x1F,0xFC,0x10,0x84,0x10,0x80,0x10,0x80,0x10,0x80,0x0F,0x00,0x00,0x00,

//UNICODE:51
0x07,0xF0,0x08,0x08,0x10,0x14,0x10,0x14,0x10,0x0C,0x08,0x0A,0x07,0xF2,0x00,0x00,

//UNICODE:52
0x10,0x04,0x1F,0xFC,0x11,0x04,0x11,0x00,0x11,0xC0,0x11,0x30,0x0E,0x0C,0x00,0x04,

//UNICODE:53
0x00,0x00,0x0E,0x1C,0x11,0x04,0x10,0x84,0x10,0x84,0x10,0x44,0x1C,0x38,0x00,0x00,

//UNICODE:54
0x18,0x00,0x10,0x00,0x10,0x04,0x1F,0xFC,0x10,0x04,0x10,0x00,0x18,0x00,0x00,0x00,

//UNICODE:55
0x10,0x00,0x1F,0xF8,0x10,0x04,0x00,0x04,0x00,0x04,0x10,0x04,0x1F,0xF8,0x10,0x00,

//UNICODE:56
0x10,0x00,0x1E,0x00,0x11,0xE0,0x00,0x1C,0x00,0x70,0x13,0x80,0x1C,0x00,0x10,0x00,

//UNICODE:57
0x10,0x00,0x1F,0xC0,0x00,0x7C,0x1F,0x80,0x00,0x7C,0x1F,0xC0,0x10,0x00,0x00,0x00,

//UNICODE:58
0x10,0x04,0x18,0x0C,0x16,0x34,0x01,0xC0,0x01,0xC0,0x16,0x34,0x18,0x0C,0x10,0x04,

//UNICODE:59
0x10,0x00,0x1C,0x00,0x13,0x04,0x00,0xFC,0x13,0x04,0x1C,0x00,0x10,0x00,0x00,0x00,

//UNICODE:5A
0x08,0x04,0x10,0x1C,0x10,0x64,0x10,0x84,0x13,0x04,0x1C,0x04,0x10,0x18,0x00,0x00,

//UNICODE:5B
0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x40,0x02,0x40,0x02,0x40,0x02,0x00,0x00,

//UNICODE:5C
0x00,0x00,0x20,0x00,0x1C,0x00,0x03,0x80,0x00,0x60,0x00,0x1C,0x00,0x03,0x00,0x00,

//UNICODE:5D
0x00,0x00,0x40,0x02,0x40,0x02,0x40,0x02,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:5E
0x00,0x00,0x00,0x00,0x20,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x00,0x00,0x00,0x00,

//UNICODE:5F
0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,

//UNICODE:60
0x00,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:61
0x00,0x00,0x00,0x98,0x01,0x24,0x01,0x24,0x01,0x48,0x00,0xFC,0x00,0x04,0x00,0x00,

//UNICODE:62
0x08,0x00,0x0F,0xFC,0x00,0x88,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x70,0x00,0x00,

//UNICODE:63
0x00,0x00,0x00,0x70,0x00,0x88,0x01,0x04,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x00,

//UNICODE:64
0x00,0x00,0x00,0xF8,0x01,0x04,0x01,0x04,0x01,0x04,0x09,0x08,0x0F,0xFC,0x00,0x04,

//UNICODE:65
0x00,0x00,0x00,0xF8,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x24,0x00,0xE8,0x00,0x00,

//UNICODE:66
0x00,0x00,0x01,0x04,0x01,0x04,0x07,0xFC,0x09,0x04,0x09,0x04,0x04,0x00,0x00,0x00,

//UNICODE:67
0x00,0x00,0x00,0xD6,0x01,0x29,0x01,0x29,0x01,0x29,0x01,0xC9,0x01,0x06,0x00,0x00,

//UNICODE:68
0x08,0x04,0x0F,0xFC,0x00,0x84,0x01,0x00,0x01,0x00,0x01,0x04,0x00,0xFC,0x00,0x04,

//UNICODE:69
0x00,0x00,0x01,0x04,0x19,0x04,0x19,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00,

//UNICODE:6A
0x00,0x00,0x00,0x03,0x00,0x01,0x01,0x01,0x19,0x01,0x19,0xFE,0x00,0x00,0x00,0x00,

//UNICODE:6B
0x08,0x04,0x0F,0xFC,0x00,0x24,0x00,0x60,0x01,0x94,0x01,0x0C,0x01,0x04,0x00,0x00,

//UNICODE:6C
0x00,0x00,0x08,0x04,0x08,0x04,0x1F,0xFC,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00,

//UNICODE:6D
0x01,0x04,0x01,0xFC,0x01,0x04,0x01,0x00,0x01,0xFC,0x01,0x04,0x01,0x00,0x00,0xFC,

//UNICODE:6E
0x01,0x04,0x01,0xFC,0x00,0x84,0x01,0x00,0x01,0x00,0x01,0x04,0x00,0xFC,0x00,0x04,

//UNICODE:6F
0x00,0x00,0x00,0xF8,0x01,0x04,0x01,0x04,0x01,0x04,0x01,0x04,0x00,0xF8,0x00,0x00,

//UNICODE:70
0x01,0x01,0x01,0xFF,0x00,0x89,0x01,0x04,0x01,0x04,0x00,0x88,0x00,0x70,0x00,0x00,

//UNICODE:71
0x00,0x00,0x00,0x70,0x00,0x88,0x01,0x04,0x01,0x04,0x00,0x89,0x01,0xFF,0x00,0x01,

//UNICODE:72
0x01,0x04,0x01,0x04,0x01,0xFC,0x00,0x84,0x01,0x04,0x01,0x00,0x01,0x80,0x00,0x00,

//UNICODE:73
0x00,0x00,0x00,0xCC,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x24,0x01,0x98,0x00,0x00,

//UNICODE:74
0x00,0x00,0x01,0x00,0x01,0x00,0x07,0xF8,0x01,0x04,0x01,0x04,0x00,0x08,0x00,0x00,

//UNICODE:75
0x01,0x00,0x01,0xF8,0x00,0x04,0x00,0x04,0x00,0x04,0x01,0x08,0x01,0xFC,0x00,0x04,

//UNICODE:76
0x01,0x00,0x01,0xC0,0x01,0x30,0x00,0x0C,0x01,0x30,0x01,0xC0,0x01,0x00,0x00,0x00,

//UNICODE:77
0x01,0x80,0x01,0x70,0x00,0x0C,0x01,0x30,0x01,0xE0,0x00,0x1C,0x01,0x60,0x01,0x80,

//UNICODE:78
0x00,0x00,0x01,0x04,0x01,0x8C,0x01,0x70,0x00,0x74,0x01,0x8C,0x01,0x04,0x00,0x00,

//UNICODE:79
0x01,0x00,0x01,0x81,0x01,0x61,0x00,0x1E,0x00,0x18,0x01,0x60,0x01,0x80,0x01,0x00,

//UNICODE:7A
0x00,0x00,0x01,0x84,0x01,0x0C,0x01,0x34,0x01,0x44,0x01,0x84,0x01,0x0C,0x00,0x00,

//UNICODE:7B
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x7C,0x40,0x02,0x40,0x02,

//UNICODE:7C
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:7D
0x40,0x02,0x40,0x02,0x3F,0x7C,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//UNICODE:7E
0x00,0x00,0x40,0x00,0x80,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x40,0x00,0x00,0x00,
};


ST7920_PIXEL st7920 = {
  .x = 0, // current x pixel, range is 0 - 127
  .y = 0, // current y pixel, range is 0 - 63
  .address_is_y = 1, // Extended Instruction, The first address set Y, second address set X
};

ST7920_REG st7920_reg = {
  .fs = //0x00, // default basic instruction, 8-bit MPU interface, Graphic display OFF
        0x26, // default extended instruction, 4-bit MPU interface, Graphic display On
  .bi = {
    .ems  = 0x02, // Cursor move to right ,DDRAM address counter (AC) plus 1
    .dc   = 0x00, // Display, cursor and blink are ALL OFF
    .cdsc = 0x00, // No cursor or display shift operation
 },
  .ei = {
    .vsra = 0x00, // Allow vertical scroll or set CGRAM address
    .rev  = 0x00, // Begin with normal and toggle to reverse
  },
  .reverse = 0, // Begin with normal and toggle to reverse
  .cgram = 0,
  .data_type = ST7920_DATA_DDRAM,
  .ctrl_status = ST7920_IDLE,
};

void ST7920_DrawPixel(int16_t x, int16_t y, bool isForeGround)
{
//  GUI_DrawPixel(x, y, color);
  if (infoSettings.marlin_mode_fullscreen)
  {
    GUI_FillRectColor(SIMULATOR_XSTART_FULLSCREEN + PIXEL_XSIZE_FULLSCREEN*x,
                      SIMULATOR_YSTART_FULLSCREEN + PIXEL_YSIZE_FULLSCREEN*y,
                      SIMULATOR_XSTART_FULLSCREEN + PIXEL_XSIZE_FULLSCREEN*(x+1),
                      SIMULATOR_YSTART_FULLSCREEN + PIXEL_YSIZE_FULLSCREEN*(y+1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);

  } else {
    GUI_FillRectColor(SIMULATOR_XSTART + PIXEL_XSIZE*x,     SIMULATOR_YSTART + PIXEL_YSIZE*y ,
                      SIMULATOR_XSTART + PIXEL_XSIZE*(x+1), SIMULATOR_YSTART + PIXEL_YSIZE*(y+1),
                      isForeGround ? infoSettings.marlin_mode_font_color : infoSettings.marlin_mode_bg_color);
  }
}

// Display graphic
void ST7920_DrawGDRAM(uint8_t data)
{
  if (st7920_reg.fs.g != 1) return; // Graphic display off
  for (uint8_t i = 0; i < 8; i++) {
    if (data & 0x80)
      ST7920_DrawPixel(st7920.x, st7920.y, true);
    else
      ST7920_DrawPixel(st7920.x, st7920.y, false);
    data <<= 1;
    st7920.x++;
  }

  if (st7920.x >= 128) {
    st7920.x = 0;
    st7920.y = (st7920.y + 32);
    if (st7920.y >= 64) st7920.y = (st7920.y + 1) & (64 - 1);
  }
}

// Display CGRAM fonts, only 0x0000, 0x0002, 0x0004, 0x0006
void ST7920_DispCGRAM(uint8_t data)
{
  uint8_t ex = st7920.x + 16, // GRAM, 16*16 bitmap font
          ey = st7920.y + 16,
          yIndex = data * 8;

  for (uint8_t y = st7920.y; y < ey; y++) {
    uint16_t temp = ST7920_CGRAM[yIndex][0] << 8 | ST7920_CGRAM[yIndex][1];
    for (uint8_t x = st7920.x; x < ex; x++) {
      ST7920_DrawPixel(x, y, temp & (1<<15));
      temp <<= 1;
    }
    yIndex++;
  }

  st7920.x += 16;
  if (st7920.x >= 128) {
    st7920.x = 0;
    st7920.y = (st7920.y + 32);
    if (st7920.y == 64) st7920.y = 16;
    else if (st7920.y == 80) st7920.y = 0;
  }
}

// Display HCGROM fonts, 02H~7FH.
void ST7920_DispHCGROM(uint8_t p)
{
  const uint8_t *pStart = ascii16x8 + (p - ' ') * 16;
  uint8_t ex = st7920.x + 8,
          ey = st7920.y + 16;

  if (p < ' ' || p > '~') return;

  for (uint8_t x = st7920.x; x < ex; x++) {
    uint16_t temp = *(pStart++) << 8;
    temp |= *pStart++;

    for (uint8_t y = st7920.y; y < ey; y++) {
      ST7920_DrawPixel(x, y, temp & (1<<15));
      temp <<= 1;
    }
  }
  st7920.x += 8;
  if (st7920.x >= 128) {
    st7920.x = 0;
    st7920.y = (st7920.y + 32);
    if (st7920.y == 64) st7920.y = 16;
    else if (st7920.y == 80) st7920.y = 0;
  }
}


/*** Common instruction ***/
// cmd : 1 << 5
void ST7920_CI15_FunctionSet(uint8_t cmd)
{
  st7920_reg.fs.reg = cmd;
}

/*** Basic instruction ***/
// cmd : 1 << 0
void ST7920_BI10_DisplayClear(uint8_t cmd)
{
  // Fill DDRAM with "20H"(space code).
  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 128; x++) {
      ST7920_DrawPixel(x, y, false);
    }
  }
  // Set DDRAM address counter (AC) to"00H"
  st7920.x = st7920.y = 0;
  // Set Entry Mode I/D bit to be "1". Cursor moves right and AC adds 1 after write or read operation
  st7920_reg.bi.ems.id = 1;
}
// cmd : 1 << 1
void ST7920_BI11_ReturnHome(uint8_t cmd)
{
  // Set address counter (AC) to "00H".
  st7920.x = st7920.y = 0;
}
// cmd : 1 << 2
void ST7920_BI12_EntryModeSet(uint8_t cmd)
{
  st7920_reg.bi.ems.reg = cmd;
}
// cmd : 1 << 3
void ST7920_BI13_DisplayControl(uint8_t cmd)
{
  st7920_reg.bi.dc.reg = cmd;
}
// cmd : 1 << 4
void ST7920_BI14_CursorDisplayControl(uint8_t cmd)
{
  st7920_reg.bi.cdsc.reg = cmd;
}
// cmd : 1 << 6
void ST7920_BI16_SetCGRAMAddress(uint8_t cmd)
{
  st7920_reg.bi.cgrama.reg = cmd;
  uint8_t address = st7920_reg.bi.cgrama.ac;
  // Set CGRAM address
  // x is 0-1 = [2*8], y is 0-64 = [4*16], means 4 * [16*16] bitmap font
  st7920.x = 0;
  st7920.y = address;
  st7920_reg.data_type = ST7920_DATA_CGRAM;
}
// the data follow cmd : 1 << 6
void ST7920_BI_SetCGRAMData(uint8_t data)
{
  ST7920_CGRAM[st7920.y][st7920.x++] = data;
  if (st7920.x >= 2) {
    st7920.x = 0;
    st7920.y = (st7920.y + 1) & (64 - 1);
  }
}
// cmd : 1 << 7
void ST7920_BI17_SetDDRAMAddress(uint8_t cmd)
{
  st7920_reg.bi.ddrama.reg = cmd;
  uint8_t address = st7920_reg.bi.ddrama.ac;
  // Set DDRAM address
  // x is 0-127. y is 0-63
  st7920.x = (address & 0x07) * 16;
  st7920.y = ((address >> 4) & 0x01) * 16 + ((address >> 3) & 0x01) * 32;
  st7920_reg.data_type = ST7920_DATA_DDRAM;
}

/*** Extended Instruction ***/
// cmd : 1 << 0
void ST7920_EI10_StandBy(uint8_t cmd)
{
  st7920_reg.ei.sb.reg = cmd;
}
// cmd : 1 << 1
void ST7920_EI11_ScrollOrRAMAddress(uint8_t cmd)
{
  st7920_reg.ei.vsra.reg = cmd;
}
// cmd : 1 << 2
void ST7920_EI12_Reverse(uint8_t cmd)
{
  st7920_reg.ei.rev.reg = cmd;
  st7920_reg.reverse = !st7920_reg.reverse; // Reverse the display by toggling this instruction
}
// cmd : 1 << 6
void ST7920_EI16_SetScrollAddress(uint8_t cmd)
{
  st7920_reg.ei.sa.reg = cmd;
}
// cmd : 1 << 7
void ST7920_EI17_SetGRAMAddress(uint8_t cmd)
{
  st7920_reg.ei.grama.reg = cmd;
  uint8_t address = st7920_reg.ei.grama.ac;
  if (st7920.address_is_y) {
    st7920.y = address;
    st7920.address_is_y = 0;
  } else {
    if (address & 0x08) st7920.y += 32;
    st7920.x = (address & 0x07) * 16;
    st7920.address_is_y = 1;
  }
  st7920_reg.data_type = ST7920_DATA_GDRAM;
}

const FP_CMD cmdCallBack[8][2] = {
// Basic Instruction                 Extended Instruction
{ ST7920_BI10_DisplayClear,          ST7920_EI10_StandBy},            // cmd 1 << 0
{ ST7920_BI11_ReturnHome,            ST7920_EI11_ScrollOrRAMAddress}, // cmd 1 << 1
{ ST7920_BI12_EntryModeSet,          ST7920_EI12_Reverse},            // cmd 1 << 2
{ ST7920_BI13_DisplayControl,        NULL},                           // cmd 1 << 3
{ ST7920_BI14_CursorDisplayControl,  NULL},                           // cmd 1 << 4
{ ST7920_CI15_FunctionSet,           ST7920_CI15_FunctionSet},        // cmd 1 << 5
{ ST7920_BI16_SetCGRAMAddress,       ST7920_EI16_SetScrollAddress},   // cmd 1 << 6
{ ST7920_BI17_SetDDRAMAddress,       ST7920_EI17_SetGRAMAddress},     // cmd 1 << 7
};


u8 ST7920_IsCtrlByte(u8 data)
{
  if(data == ST7920_WCMD || data == ST7920_WDATA || data == ST7920_RCMD || data == ST7920_RDATA)
    return true;
  else
    return false;
}

void ST7920_ParseRecv(u8 val)
{
  static u8 rcvData = 0;
  static u8 rcvIndex = 0;
  if (ST7920_IsCtrlByte(val))
  {
    st7920_reg.ctrl_status = (ST7920_CTRL_STATUS)val;
    rcvIndex = 0;
    if (st7920_reg.ctrl_status == ST7920_WDATA) {
      st7920.address_is_y = 1;
      st7920_reg.cgram = 0;
    }
  }
  else
  {
    val &= 0xF0;           // Every 8 bits instruction/data will be separated into 2 groups, lower 4 bits always 0 in every groups
    if (rcvIndex == 0) {
      rcvData = val;       // Higher 4 bits in first byte
      rcvIndex++;
      return;
    } else {
      rcvData |= val >> 4; // Lower 4 bits in second byte
      rcvIndex = 0;
    }

    switch (st7920_reg.ctrl_status)
    {
      case ST7920_WCMD:
        ST7920_ST7920_ParseWCmd(rcvData);
        break;

      case ST7920_WDATA:
        ST7920_ST7920_ParseWData(rcvData);
        break;

      case ST7920_RCMD:
        break;

      case ST7920_RDATA:
        break;

      default:
        break;
    }
  }
}


void ST7920_ST7920_ParseWCmd(u8 cmd)
{
  for (int8_t i = 7; i >= 0; i--) {
    if (cmd & (1 << i)) {
      if (cmdCallBack[i][st7920_reg.fs.re] != NULL) {
        (*cmdCallBack[i][st7920_reg.fs.re])(cmd);
      }
      break;
    }
  }
}

void ST7920_ST7920_ParseWData(u8 data)
{
  switch (st7920_reg.data_type) {
    case ST7920_DATA_DDRAM:
      if (st7920_reg.cgram == 1) {
        st7920_reg.cgram = 0;
        ST7920_DispCGRAM(data);
      } else if (data == 0x00) {
        st7920_reg.cgram = 1;
        return;
      } else {
        ST7920_DispHCGROM(data);
      }
      break;
    case ST7920_DATA_CGRAM:
      ST7920_BI_SetCGRAMData(data);
      break;
    case ST7920_DATA_GDRAM:
      ST7920_DrawGDRAM(data);
      break;
  }
}

void menuST7920(void)
{
  GUI_Clear(infoSettings.marlin_mode_bg_color);
  GUI_SetColor(infoSettings.marlin_mode_font_color);
  GUI_SetBkColor(infoSettings.marlin_mode_bg_color);

  if(infoSettings.marlin_mode_showtitle == 1){
    STRINGS_STORE tempST;
    W25Qxx_ReadBuffer((uint8_t *)&tempST,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));
    GUI_DispStringInRect(0, 0, LCD_WIDTH, SIMULATOR_YSTART, (uint8_t *)tempST.lcd12864_title);
  }

  SPI_Slave();

  while(infoMenu.menu[infoMenu.cur] == menuST7920)
  {
    while(SPISlave.rIndex != SPISlave.wIndex)
    {
      ST7920_ParseRecv(SPISlave.data[SPISlave.rIndex]);
      SPISlave.rIndex = (SPISlave.rIndex + 1) % SPI_SLAVE_MAX;
    }

    #if LCD_ENCODER_SUPPORT
      sendEncoder(LCD_ReadTouch());

      if(LCD_BtnTouch(LCD_BUTTON_INTERVALS))
        sendEncoder(1);
      loopCheckEncoder();
    #endif

    loopCheckMode();

    if (infoSettings.serial_alwaysOn == 1)
    {
      loopBackEnd();
    }
  }
  SPI_SlaveDeInit();
}

#endif
