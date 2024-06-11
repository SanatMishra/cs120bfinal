#ifndef _MUSICH_
#define _MUSICH_

#include "common.h"
#include <math.h>

#define t2d(x) ((x) >> 7) 
#define t2n(x) ((x) & 0x7F) 
#define n2t(x,y) ( ((x) & 0xFF80) | ((y) & 0x7F) )
#define d2t(x,y) ( ((x) & 0x7F) | (((y) << 7) & 0xFF80) )
inline constexpr float c1freq = 32.703203125;
inline constexpr uint MAXTONES = 80;
inline constexpr uint NUM_TRACKS = 4;

extern ushort tones[MAXTONES];

// tone length * speed MUST be longer than notebreak
typedef struct track {
  uint numTones;
  ushort speed;
  uchar key;
  uchar loop;
  ushort tonei;
} track;

extern track tracks[NUM_TRACKS];

void st_init();
void newTrack(uint arg);
void queueBeep(uchar note, uchar len);
void initializeTracks();
void advanceMusic();

#endif //_MUSICH_