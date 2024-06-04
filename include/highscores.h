#ifndef _HIGHSCORESH_
#define _HIGHSCORESH_

#include "common.h"
#include "serialATmega.h"
#include <stdio.h>

void EEPROM_read(uchar* dst, ushort src, ushort n);
void EEPROM_write(ushort dst, uchar* src, ushort n);

inline constexpr uint MAX_HS_ENTRIES = 10;

typedef struct HSEntry {
  char name[11];
  ushort score;
} HSEntry;
extern HSEntry HSEntries[MAX_HS_ENTRIES];

extern uchar numHSEntries;
extern ushort curHighScore;

void loadHSEntries();
void getHSLine(char* line, uchar i);
void printHSEntriesl();
uchar addHSEntry(char* uname, ushort score);
void writeHSEntries();
void clearHSEntries();


#endif