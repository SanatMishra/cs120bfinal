#ifndef _MUSICH_
#define _MUSICH_

#include "common.h"
#include <math.h>

#define t2d(x) ((x) >> 7) 
#define t2n(x) ((x) & 0x7F) 
#define n2t(x,y) ( ((x) & 0xFF80) | ((y) &  0x7F) )
#define d2t(x,y) ( ((x) & 0x7F) | (((y) << 7) &  0xFF80) )
inline constexpr float c1freq = 32.703203125;
inline constexpr uint MAXTONES = 64;
inline constexpr uint NUM_TRACKS = 2;

// tone length * speed MUST be longer than notebreak
typedef struct track {
  uchar key;
  ushort speed;
  uint numTones;
  ushort notebreak;
  uchar loop;
  ushort track[MAXTONES];
} track;
extern track tracks[NUM_TRACKS];

extern uint curTrackI;
extern uchar curToneI;
extern ushort curTone;
extern uchar beepNote;
extern uchar beepDur;

float getFreq(uchar note);
void st_init();
void stone(float freq);
void newTrack(uint arg);
void queueBeep(uchar note, uchar len);
void initializeTracks();

#endif _MUSICH_