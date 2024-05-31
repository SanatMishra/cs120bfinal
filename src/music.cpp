#include "common.h"
#include "music.h"

#define tf(x,y) tracks[i].track[tracks[i].numTones++] = (ushort)((tracks[i].key + (x)) + (((y)*tracks[i].speed) << 7));
#define tn(x,y) tracks[i].track[tracks[i].numTones++] = (ushort)((tracks[i].key + (x)) + (((y)*tracks[i].speed - tracks[i].notebreak) << 7)); \
                tracks[i].track[tracks[i].numTones++] = (ushort)((tracks[i].notebreak << 7));
#define t0(x) tracks[i].track[tracks[i].numTones++] = (ushort)(((x)*tracks[i].speed) << 7);

float getFreq(uchar note) {
  if (note == 0) return 0;
  return c1freq * pow(2.0, (note - 1.0)/12);
}

void st_init() {
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
}

void stoff() {
  // check if can be handled with OCR1A = 0;
  TCCR1A &= ~(1 << COM1A1);
}

void stone(float freq) {
  if (freq == 0) {
    stoff();
    return;
  }
  if (freq < 31 || freq > 2000000)
    return;
  ushort j = (ushort)(1000000.0/freq);
  OCR1A = (1000000.0/j - freq > freq - 1000000.0/(j + 1)) ? (j + 1) : j;
  ICR1 = 2*OCR1A - 1;
  //ICR1 = min(65535, 4*(2*OCR1A - 1)); // artificially cut volume
  TCCR1A |= (1 << COM1A1);
}

track tracks[NUM_TRACKS];

uint curTrackI;
uchar curToneI;
ushort curTone;
uchar beepNote;
uchar beepDur;

void newTrack(uint arg) {
  curTrackI = arg;
  curToneI = 0;
  curTone = tracks[curTrackI].track[curToneI];
}

void queueBeep(uchar note, uchar len) {
  beepNote = note;
  beepDur = len;
}

void initializeTracks() {
  uchar i;

  i = 0;
  tracks[0].key = 0;
  tracks[0].speed = 1;
  tracks[0].numTones = 0;
  tracks[0].notebreak = 0;
  tracks[0].loop = 0;
  t0(1);

  i = 1;
  tracks[1].key = 30;
  tracks[1].speed = 16;
  tracks[1].numTones = 0;
  tracks[1].notebreak = 1;
  tracks[1].loop = 1;
   t0(1); tf(-5,1); tf(0,1); tf(3,1);
   tf(7,1); tf(0,1); tf(3,1); tf(7,1);
   tf(12,1); tf(3,1); tf(7,1); tf(12,1);
   tf(12,1); tf(15,1); tf(12,1); tf(7,1);

   t0(1); tf(0,1); tf(3,1); tf(7,1);
   tf(0,1); tf(3,1); tf(7,1); tf(12,1);
   tf(12,1); tf(15,1); tf(12,1); tf(7,1);
   tf(7,1); tf(3,1); tf(7,1); tf(6,1);

   tf(5,1); tf(2,1); tf(5,1); tf(8,1);
   tf(12,1); tf(5,1); tf(8,1); tf(12,1);
   tf(19,1); tf(17,1); tf(16,1); tf(19,1);
   tf(12,1); tf(10,1); tf(8,1); tf(12,1);

   tf(10,1); tf(7,1); tf(10,1); tf(8,1);
   tf(5,1); tf(8,1); tf(7,1); tf(2,1);
   tf(-1,1); tf(0,1); tf(2,1); tf(3,1);
   tf(5,2);           tf(2,2);
}
