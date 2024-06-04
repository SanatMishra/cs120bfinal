#include "common.h"
#include "renderable.h"
#include "serialATmega.h"

char text[16][16];
uchar tdata[16][16];
uint palette[4];

bool textch(uchar x, uchar y) {
  return GetBit(tdata[x][y], 2);
}

void textcu(uchar x, uchar y) {
  tdata[x][y] = SetBit(tdata[x][y], 2, 1);
}

void textcd(uchar x, uchar y) {
  tdata[x][y] = SetBit(tdata[x][y], 2, 0);
}

uchar palID(uchar x, uchar y) {
  return tdata[x][y] & 0x03;
}

uint getPaletteData(uchar i) {
  return palette[i];
}

void setPaletteData(uchar i, uint v) {
  palette[i] = v;
}

void initScreen() {
  for (uchar x = 0; x < 16; x++) {
    for (uchar y = 0; y < 16; y++) {
      scChar(0, x, y, 0);
      textcu(x, y);
    }
  }
  setPaletteData(0, color(47, 47, 47));
  setPaletteData(1, color(63, 0, 0));
  setPaletteData(2, color(0, 63, 0));
  setPaletteData(3, color(10, 10, 63));
}

ushort offset(char c) {
  uchar idx = 64;
  //serial_print("offset ");
  //serial_char(c);
  //serial_println("");
  if (c == 0 || c == ' ') {
    idx = NUM_TEXTCHARS;
  } else if (c >= '0' && c <= '9') {
    idx = c - 48;
  } else if (c >= 'A' && c <= 'Z') {
    idx = c - 55;
  } else if (c >= 'a' && c <= 'z') {
    idx = c - 61;
  } else if (c == ':') {
    idx = 62;
  } else if (c == '>') {
    idx = 63;
  } else if (c == '#') {
    idx = 64;
  } else if (c == '<') {
    idx = 65;
  }
  return 8*idx;
}

uint getpx(uchar tx, uchar ty, uchar px, uchar py) {
  ushort off = offset(text[tx][ty]);
  if (px < 0 || px >= 8 || py < 0 || py >= 8)
    return cclear;
  if (off == 8*NUM_TEXTCHARS)
    return cclear;
  return (( pgm_read_byte(&textData[off + 7 - py]) >> (7 - px)) & 0x01) ? getPaletteData(palID(tx, ty)) : cclear;
}

void scChar(char c, uchar x, uchar y, uchar pid = 4) {
  if (text[x][y] != c) {
    text[x][y] = c;
    textcu(x, y);
  }
  if (pid != 4)
    scColor(x, y, pid);
}

void scString(const char* c, uchar n, uchar x, uchar y, uchar pid = 4) {
  for (uchar i = x; i <= clamp(0, 15, x + n - 1); i++) {
    scChar(c[i - x], i, y, pid);
  }
}

void scErase(uchar n, uchar x, uchar y) {
  for (uchar i = x; i <= clamp(0, 15, x + n - 1); i++) {
    scChar(0, i, y);
  }
}

void scClear() {
  for (uchar x = 0; x < 16; x++)
    for (uchar y = 0; y < 16; y++)
      scChar(0, x, y, 0);
}

void scColor(uchar x, uchar y, uchar pid) {
  if (palID(x, y) != pid) {
    tdata[x][y] = (tdata[x][y] & 0xFC) | pid;
    textcu(x, y);
  }
}

void scColorString(uchar n, uchar x, uchar y, uchar pid) {
  for (uchar i = x; i <= clamp(0, 15, x + n - 1); i++) {
    scColor(i, y, pid);
  }
}

uint getspx(uchar i, uchar x, uchar y) {
  if (x < 0 || x >= spritesW(i) || y < 0 || y >= spritesH(i))
    return cclear;
  ushort ti = x + spritesW(i)*(spritesH(i) - y - 1);
  return spritesPalette(i, (spriteData(spritesOff(i) + ti / 4) >> 2*(3 - ti % 4)) & 0x03);
}
