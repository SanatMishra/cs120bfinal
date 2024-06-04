#ifndef _RENDERABLEH_
#define _RENDERABLEH_

#include "common.h"
#include <avr/pgmspace.h>

// Opacity Red Green Blue
// Opacity: 0 = opaque >0 = clear
// CHECK YELLOW LINES
#define color(a,b,c) (((uint)(a) << 16) + ((uint)(b) << 8) + (uint)(c))
#define ccolor(x,a,b,c) (((uint)(x) << 24) + ((uint)(a) << 16) + ((uint)(b) << 8) + (uint)(c))
#define ccx(x) (((x) >> 24) & 0xFF)
#define ccr(x) (((x) >> 16) & 0xFF)
#define ccg(x) (((x) >> 8) & 0xFF)
#define ccb(x) ((x) & 0xFF)
inline constexpr uint cclear = ccolor(1, 0, 0, 0);
inline constexpr uint cblack = color(0, 0, 0);
//inline constexpr uint cblack = color(0, 0, 0);

#define NUM_TEXTCHARS (66)
#define LEN_TEXTDATA (8*(NUM_TEXTCHARS))
// https://github.com/susam/pcface/blob/main/out/oldschool-cga-8x8
inline constexpr uchar textData[LEN_TEXTDATA] PROGMEM = {0, 124, 198, 206, 222, 246, 230, 124, 0, 48, 112, 48, 48, 48, 48, 252, 0, 120, 204, 12, 56, 96, 204, 252, 0, 120, 204, 12, 56, 12, 204, 120, 0, 28, 60, 108, 204, 254, 12, 30, 0, 252, 192, 248, 12, 12, 204, 120, 0, 56, 96, 192, 248, 204, 204, 120, 0, 252, 204, 12, 24, 48, 48, 48, 0, 120, 204, 204, 120, 204, 204, 120, 0, 120, 204, 204, 124, 12, 24, 112, 0, 48, 120, 204, 204, 252, 204, 204, 0, 252, 102, 102, 124, 102, 102, 252, 0, 60, 102, 192, 192, 192, 102, 60, 0, 248, 108, 102, 102, 102, 108, 248, 0, 254, 98, 104, 120, 104, 98, 254, 0, 254, 98, 104, 120, 104, 96, 240, 0, 60, 102, 192, 192, 206, 102, 62, 0, 204, 204, 204, 252, 204, 204, 204, 0, 120, 48, 48, 48, 48, 48, 120, 0, 30, 12, 12, 12, 204, 204, 120, 0, 230, 102, 108, 120, 108, 102, 230, 0, 240, 96, 96, 96, 98, 102, 254, 0, 198, 238, 254, 254, 214, 198, 198, 0, 198, 230, 246, 222, 206, 198, 198, 0, 56, 108, 198, 198, 198, 108, 56, 0, 252, 102, 102, 124, 96, 96, 240, 0, 120, 204, 204, 204, 220, 120, 28, 0, 252, 102, 102, 124, 108, 102, 230, 0, 120, 204, 96, 48, 24, 204, 120, 0, 252, 180, 48, 48, 48, 48, 120, 0, 204, 204, 204, 204, 204, 204, 252, 0, 204, 204, 204, 204, 204, 120, 48, 0, 198, 198, 198, 214, 254, 238, 198, 0, 198, 198, 
                                                         108, 56, 56, 108, 198, 0, 204, 204, 204, 120, 48, 48, 120, 0, 254, 198, 140, 24, 50, 102, 254, 0, 0, 120, 12, 124, 204, 118, 0, 224, 96, 96, 124, 102, 102, 220, 0, 0, 0, 120, 204, 192, 204, 120, 0, 28, 12, 12, 124, 204, 204, 118, 0, 0, 0, 120, 204, 252, 192, 120, 0, 56, 108, 96, 240, 96, 96, 240, 0, 0, 0, 118, 204, 204, 124, 12, 248, 224, 96, 108, 118, 102, 102, 230, 0, 48, 0, 112, 48, 48, 48, 120, 0, 12, 0, 12, 12, 12, 204, 204, 120, 224, 96, 102, 108, 120, 108, 230, 0, 112, 48, 48, 48, 48, 48, 120, 0, 0, 0, 204, 254, 254, 214, 198, 0, 0, 0, 248, 204, 204, 204, 204, 0, 0, 0, 120, 204, 204, 204, 120, 0, 0, 0, 220, 102, 102, 124, 96, 240, 0, 0, 118, 204, 204, 124, 12, 30, 0, 0, 220, 118, 102, 96, 240, 0, 0, 0, 124, 192, 120, 12, 248, 0, 16, 48, 124, 48, 48, 52, 24, 0, 0, 0, 204, 204, 204, 204, 118, 0, 0, 0, 204, 204, 204, 120, 48, 0, 0, 0, 198, 214, 254, 254, 108, 0, 0, 0, 198, 108, 56, 108, 198, 0, 0, 0, 204, 204, 204, 124, 12, 248, 0, 0, 252, 152, 48, 100, 252, 0, 0, 48, 48, 0, 0, 48, 48, 0, 96, 48, 24, 12, 24, 48, 96, 0, 255, 255, 255, 255, 255, 255, 255, 255, 24, 48, 96, 192, 96, 48, 24, 0};

typedef struct Sprite {
  uint palette[4];
  ushort off;
  uchar w;
  uchar h;
} Sprite;

#define LEN_SPRITEDATA (41)
inline constexpr uchar spriteDataPGM[LEN_SPRITEDATA] PROGMEM = {1, 64, 1, 64, 193, 67, 193, 67, 133, 82, 134, 146, 150, 150, 42, 168, 1, 64, 144, 20, 7, 193, 67, 236, 20, 57, 133, 82, 120, 105, 46, 150, 150, 146, 170, 135, 158, 121, 224, 85, 80};
#define NUM_SPRITES (3)
inline constexpr Sprite spritesPGM[NUM_SPRITES] PROGMEM = {{{16777216, 986943, 789294, 4129539}, 0, 8, 8}, {{16777216, 986943, 789294, 197439}, 16, 10, 9}, {{16777216, 2047807, 16777216, 16777216}, 39, 2, 3}};

#define spriteData(i) (pgm_read_byte(&spriteDataPGM[(i)]))
#define spritesPalette(i, j) (pgm_read_dword(&spritesPGM[(i)].palette[(j)]))
#define spritesOff(i) (pgm_read_byte(&spritesPGM[(i)].off))
#define spritesW(i) (pgm_read_byte(&spritesPGM[(i)].w))
#define spritesH(i) (pgm_read_byte(&spritesPGM[(i)].h))

extern char text[16][16];
extern uchar tdata[16][16];
extern uint palette[4];
//extern uchar textc[32];
bool textch(uchar x, uchar y);
void textcu(uchar x, uchar y);
void textcd(uchar x, uchar y);
uchar palID(uchar x, uchar y);
uint getPaletteData(uchar i);
void setPaletteData(uchar i, uint v);
void initScreen();
ushort offset(char c);
uint getpx(uchar tx, uchar ty, uchar x, uchar y);
void scChar(char c, uchar x, uchar y, uchar pid = 4);
void scString(const char* c, uchar n, uchar x, uchar y, uchar pid = 4);
void scErase(uchar n, uchar x, uchar y);
void scClear();
void scColor(uchar x, uchar y, uchar i);
void scColorString(uchar n, uchar x, uchar y, uchar pid = 4);

uint getspx(uchar i, uchar x, uchar y);

#endif