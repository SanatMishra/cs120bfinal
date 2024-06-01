#include "common.h"
#include "highscores.h"

// per ATmega328P Datasheet
// DO NOT USE, no turning off interrupts here
void EEPROM_readByte(uchar* dst, ushort src) {
  while(EECR & (1<<EEPE));
  EEAR = src;
  EECR |= (1<<EERE);
  *dst = EEDR;
}

void EEPROM_read(uchar* dst, ushort src, ushort n) {
    SREG &= 0x7F; // Global interrupt disable
    for (ushort i = 0; i < n; i++)
      EEPROM_readByte(dst + i, src + i);
    SREG |= 0x80; // Global interrupt enable
}

// per ATmega328P Datasheet
// DO NOT USE, no turning off interrupts here
void EEPROM_writeByte(ushort dst, uchar* src) {
  while(EECR & (1<<EEPE));
  EEAR = dst;
  EEDR = *src;
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
}

void EEPROM_write(ushort dst, uchar* src, ushort n) {
    SREG &= 0x7F; // Global interrupt disable
    for (ushort i = 0; i < n; i++)
      EEPROM_writeByte(dst + i, src + i);
    SREG |= 0x80; // Global interrupt enable
}

HSEntry HSEntries[MAX_HS_ENTRIES];

uchar numHSEntries;
ushort curHighScore;

void loadHSEntries() {
  ushort ind = 0;
  EEPROM_read(&numHSEntries, ind, 1);
  ind++;
  for (ushort i = 0; i < numHSEntries; i++) {
    EEPROM_read((uchar*)(HSEntries[i].name), ind, 11);
    ind += 11;
    EEPROM_read((uchar*)(&(HSEntries[i].score)), ind, 2);
    ind += 2;
  }
}

void getHSLine(char* line, uchar i) {
  for (uchar j = 0; j < 11; j++) {
      line[j] = HSEntries[i].name[j];
  }
  sprintf(&line[11], " %04hu", HSEntries[i].score % 10000);
}

void printHSEntries() {
  char HSline[17];
  serial_println("High scores:");
  for (uchar i = 0; i < numHSEntries; i++) {
    getHSLine(HSline, i);
    serial_println(HSline);
  }
}

// uname should be 12 chars long, 11 chars of sensible data and a null byte
void addHSEntry(char* uname, ushort score) {
  uchar i = 0;
  while (i < numHSEntries && HSEntries[i].score >= score)
    i++;
  uchar j = numHSEntries;
  if (j > MAX_HS_ENTRIES - 1)
    j--;
  else
    numHSEntries++;
  while (j > i) {
    HSEntries[j] = HSEntries[j - 1];
    j--;
  }
  for (uchar k = 0; k < 11; k++)
    HSEntries[i].name[k] = uname[k];
  HSEntries[i].score = score;
}

void writeHSEntries() {
  ushort ind = 0;
  EEPROM_write(ind, &numHSEntries, 1);
  uchar testt;
  EEPROM_read(&testt, ind, 1);
  ind++;
  for (ushort i = 0; i < numHSEntries; i++) {
    EEPROM_write(ind, (uchar*)(HSEntries[i].name), 11);
    ind += 11;
    EEPROM_write(ind, (uchar*)(&(HSEntries[i].score)), 2);
    ind += 2;
  }
}

void clearHSEntries() {
    numHSEntries = 0;
    writeHSEntries();
}
