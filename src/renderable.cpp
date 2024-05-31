#include "common.h"
#include "renderable.h"

TextRenderable::TextRenderable(char c, uint p = color(31, 31, 31)) {
  if (c >= '0' && c <= '9') {
    idx = c - 48;
  } else if (c >= 'A' && c <= 'Z') {
    idx = c - 55;
  } else if (c >= 'a' && c <= 'z') {
    idx = c - 61;
  } else if (c == ':') {
    idx = 62;
  } else {
    idx = 63;
  }
  offset = 8*idx;
  palette = p;
}

TextRenderable::~TextRenderable() {}

void TextRenderable::changePalette(uint a) { palette = a; }
uchar TextRenderable::width() { return 8; }
uchar TextRenderable::height() { return 8; }

uint TextRenderable::getpx(uchar x, uchar y) {
  if (x < 0 || x >= 8 || y < 0 || y >= 8)
    return cclear;
  return ((textData[offset + 7 - y] >> (7 - x)) & 0x01) ? palette : cclear;
}

SpriteRenderable::SpriteRenderable(ushort i) {
  idx = i;
  offset = 0;
  ushort j = 0;
  while (j < i) {
    offset += 12;
    offset += (spriteData[offset]*spriteData[offset + 1] - 1)/8 + 1; // ceil(w*h/8)
  }
  for (int k = 0; k < 4; k++) {
    uchar x = (spriteData[offset] & 0xfc) >> 2;
    uchar r = ((spriteData[offset] & 0x02) << 4) | ((spriteData[offset + 1] & 0xf0) >> 4);
    uchar g = ((spriteData[offset + 1] & 0x0f) << 2) | ((spriteData[offset + 2] & 0xc0) >> 6);
    uchar b = (spriteData[offset + 2] & 0x3f);
    palette[k] = ccolor(x, r, g, b);
    offset += 3;
  }
  w = spriteData[offset];
  h = spriteData[offset + 1];
  offset += 2;
}
SpriteRenderable::~SpriteRenderable() {}

void SpriteRenderable::changePalette(uint a, uint b, uint c, uint d) {
  palette[0] = a; palette[1] = b; palette[2] = c; palette[3] = d;
}

uchar SpriteRenderable::width() { return w; }

uchar SpriteRenderable::height() { return h; }

uint SpriteRenderable::getpx(uchar x, uchar y) {
  if (x < 0 || x >= width() || y < 0 || y >= height())
    return cclear;
  ushort ti = x + w*(h - y - 1);
  return palette[(spriteData[ti / 4] >> 2*(3 - ti % 4)) & 0x03];
}
