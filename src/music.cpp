#include "common.h"
#include "music.h"
#include "serialATmega.h"

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

uint curTrackI;
uchar curToneI;
ushort curTone;
uchar beepNote;
uchar beepDur;
bool notebreak;

void newTrack(uint arg) {
  curTrackI = arg;
  curToneI = tracksToneI(arg);
  curTone = pgm_read_word(&tones[curToneI]);
  notebreak = 0;
}

void queueBeep(uchar note, uchar len) {
  beepNote = note;
  beepDur = len;
}

void initializeTracks() {
  
}

void advanceMusic() {
  bool notebreaki = notebreak;
  notebreak = 0;
  if (beepDur > 0) {
    stone(getFreq(beepNote));
    beepDur--;
  } else if (!notebreaki && curToneI != tracksToneI(curTrackI) + tracksN(curTrackI)) {
    stone(getFreq(t2n(curTone)));
  } else {
    stone(getFreq(0));
    return;
  }
  if (!notebreaki && curToneI != tracksToneI(curTrackI) + tracksN(curTrackI)) {
    if (t2d(curTone) != 0x1FF) {
      curTone = d2t(curTone, t2d(curTone) - 1);
    }
    if (t2d(curTone) == 0) {
      curToneI++;
      if (curToneI == tracksToneI(curTrackI) + tracksN(curTrackI) && tracksLoop(curTrackI))
        curToneI = tracksToneI(curTrackI);
      if (curToneI != tracksToneI(curTrackI) + tracksN(curTrackI))
        curTone = pgm_read_word(&tones[curToneI]);
      notebreak = 1;
    }
  }
}
