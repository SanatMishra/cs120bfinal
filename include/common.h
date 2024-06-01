#ifndef _COMMONH_
#define _COMMONH_

#include <avr/io.h>

typedef uint32_t uint;
typedef uint16_t ushort;
typedef uint8_t uchar;

inline constexpr int WIDTH = 128;
inline constexpr int HEIGHT = 128;
inline constexpr unsigned long UNIV_PERIOD = 80;

#define abs(x) ((x) < 0 ? (-(x)) : (x))
#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) > (y) ? (y) : (x))
#define uround(x) ((uint)((x) > 0 ? (x) + 0.5 : (x) - 0.5))
#define clamp(l, h, x) ((x) > (h) ? (h) : ((x) < (l) ? (l) : (x)))
#define calibrate(ly, hy, lx, hx, x) ( (ly) + 1.0*((x) - (lx))/((hx) - (lx))*((hy) - (ly)) )
#define g2sx(x) (129 - x)
#define g2sy(y) (1 + y)
#define SetBit(x, k, b) (uchar)((uchar)(b) ?  ((uchar)(x) | (0x01 << (uchar)(k)))  :  ((uchar)(x) & ~(0x01 << (uchar)(k))) )
#define GetBit(x, k) (uchar)(((uchar)(x) & (0x01 << (uchar)(k))) != 0)
//#define xxclear(i,j) ((i) >= 40 && (i) <= 70 && (j) >= 30 && (j) <= 45)

extern uint sdff;

#endif
