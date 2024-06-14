#ifndef _MUSICH_
#define _MUSICH_

#include "common.h"
#include <math.h>
#include <avr/pgmspace.h>

#define t2d(x) ((x) >> 7) 
#define t2n(x) ((x) & 0x7F) 
#define n2t(x,y) ( ((x) & 0xFF80) | ((y) & 0x7F) )
#define d2t(x,y) ( ((x) & 0x7F) | (((y) << 7) & 0xFF80) )
#define c1freq (32.703203125)
#define MAXTONES (80)
#define NUM_TRACKS (4)

constexpr inline ushort tones[MAXTONES] PROGMEM = {2602, 810, 810, 810, 2607, 1280, 1177, 1182, 1185, 1189, 1182, 1185, 1189, 1194, 1185, 1189, 1194, 1194, 1197, 1194, 1189, 1280, 1182, 1185, 1189, 1182, 1185, 1189, 1194, 1194, 1197, 1194, 1189, 1189, 1185, 1189, 1188, 1187, 1184, 1187, 1190, 1194, 1187, 1190, 1194, 1201, 1199, 1198, 1201, 1194, 1192, 1190, 1194, 1192, 1189, 1192, 1190, 1187, 1190, 1189, 1184, 1181, 1182, 1184, 1185, 2467, 2464, 3754, 553, 551, 3110, 808, 809, 810, 808, 809, 810, 808, 809, 810};

// tone length * speed MUST be longer than notebreak
typedef struct track {
  uint numTones;
  ushort speed;
  uchar key;
  uchar loop;
  ushort tonei;
} track;

constexpr inline track tracks[NUM_TRACKS] PROGMEM = {{5, 7, 35, 0, 0}, {62, 10, 30, 1, 5}, {4, 5, 30, 0, 67}, {9, 7, 42, 0, 71}};

#define tracksN(i) (pgm_read_dword(&tracks[(i)].numTones))
#define tracksSpeed(i) (pgm_read_word(&tracks[(i)].speed))
#define tracksKey(i) (pgm_read_byte(&tracks[(i)].key))
#define tracksLoop(i) (pgm_read_byte(&tracks[(i)].loop))
#define tracksToneI(i) (pgm_read_word(&tracks[(i)].tonei))

void st_init();
void newTrack(uint arg);
void queueBeep(uchar note, uchar len);
void initializeTracks();
void advanceMusic();

#endif //_MUSICH_