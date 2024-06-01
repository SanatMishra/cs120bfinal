#include "common.h"
#include "renderable.h"
#include "serialATmega.h"

char text[16][16];
uchar textc[32];

bool textch(uchar x, uchar y) {
  return GetBit(textc[(16*x + y)/8], (16*x + y)%8);
}

void textcu(uchar x, uchar y) {
  textc[(16*x + y)/8] = SetBit(textc[(16*x + y)/8], (16*x + y)%8, 1);
}

void textcd(uchar x, uchar y) {
  textc[(16*x + y)/8] = SetBit(textc[(16*x + y)/8], (16*x + y)%8, 0);
}

void initScreen() {
  for (uchar x = 0; x < 16; x++) {
    textc[2*x] = textc[2*x + 1] = 0;
    for (uchar y = 0; y < 16; y++) {
      text[x][y] = 0;
    }
  }
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
  }
  return 8*idx;
}

uint getpx(ushort offset, uchar x, uchar y) {
  if (x < 0 || x >= 8 || y < 0 || y >= 8)
    return cclear;
  if (offset == 8*NUM_TEXTCHARS)
    return cclear;
  return (( pgm_read_byte(&textData[offset + 7 - y]) >> (7 - x)) & 0x01) ? color(31, 31, 31) : cclear;
}

void scChar(char c, uchar x, uchar y) {
  if (text[x][y] != c) {
    text[x][y] = c;
    textcu(x, y);
  }
}

void scString(const char* c, uchar n, uchar x, uchar y) {
  for (uchar i = x; i <= clamp(0, 15, x + n - 1); i++) {
    scChar(c[i - x], i, y);
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
      scChar(0, x, y);
}
