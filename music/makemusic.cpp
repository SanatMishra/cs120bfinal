
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#include <stdio.h>

#define tn(x,y) tones[tracks[i].tonei + tracks[i].numTones++] = (ushort)((tracks[i].key + (x)) + (((y)*tracks[i].speed - 1) << 7));
#define t0(x) tones[tracks[i].tonei + tracks[i].numTones++] = (ushort)(((x)*tracks[i].speed) << 7);

#define MAXTONES (80)
#define NUM_TRACKS (4)

ushort tones[MAXTONES];

// tone length * speed MUST be longer than notebreak
typedef struct track {
  uint numTones;
  ushort speed;
  uchar key;
  uchar loop;
  ushort tonei;
} track;

track tracks[NUM_TRACKS];

int main() {
  uchar i;

  i = 0;
  tracks[0].key = 35;
  tracks[0].speed = 7;
  tracks[0].numTones = 0;
  tracks[0].loop = 0;
  tracks[0].tonei = 0;
  tn(7, 3); tn(7, 1); tn(7, 1); tn(7, 1); tn(12, 3);

  i = 1;
  tracks[1].key = 30;
  tracks[1].speed = 10;
  tracks[1].numTones = 0;
  tracks[1].loop = 1;
  tracks[1].tonei = tracks[0].tonei + tracks[0].numTones;
  t0(1); tn(-5,1); tn(0,1); tn(3,1);
  tn(7,1); tn(0,1); tn(3,1); tn(7,1);
  tn(12,1); tn(3,1); tn(7,1); tn(12,1);
  tn(12,1); tn(15,1); tn(12,1); tn(7,1);

  t0(1); tn(0,1); tn(3,1); tn(7,1);
  tn(0,1); tn(3,1); tn(7,1); tn(12,1);
  tn(12,1); tn(15,1); tn(12,1); tn(7,1);
  tn(7,1); tn(3,1); tn(7,1); tn(6,1);

  tn(5,1); tn(2,1); tn(5,1); tn(8,1);
  tn(12,1); tn(5,1); tn(8,1); tn(12,1);
  tn(19,1); tn(17,1); tn(16,1); tn(19,1);
  tn(12,1); tn(10,1); tn(8,1); tn(12,1);

  tn(10,1); tn(7,1); tn(10,1); tn(8,1);
  tn(5,1); tn(8,1); tn(7,1); tn(2,1);
  tn(-1,1); tn(0,1); tn(2,1); tn(3,1);
  tn(5,2);           tn(2,2);

  i = 2;
  tracks[2].key = 30;
  tracks[2].speed = 5;
  tracks[2].numTones = 0;
  tracks[2].loop = 0;
  tracks[2].tonei = tracks[1].tonei + tracks[1].numTones;
  tn(12, 6); tn(11, 1); tn(9, 1); tn(8, 5);

  i = 3;
  tracks[3].key = 42;
  tracks[3].speed = 7;
  tracks[3].numTones = 0;
  tracks[3].loop = 0;
  tracks[3].tonei = tracks[2].tonei + tracks[2].numTones;
  tn(-2, 1); tn(-1, 1); tn(0, 1); tn(-2, 1); tn(-1, 1); tn(0, 1); tn(-2, 1); tn(-1, 1); tn(0, 1);

  uint nmt = tracks[3].tonei + tracks[3].numTones;
  printf("%d\n{", nmt);
  for (int i = 0; i < nmt - 1; i++) {
    printf("%hu, ", tones[i]);
  }
  printf("%hu};\n{", tones[nmt - 1]);
  for (int i = 0; i < NUM_TRACKS - 1; i++) {
    printf("{%u, %hu, %hhu, %hhu, %hu}, ", tracks[i].numTones, tracks[i].speed, tracks[i].key, tracks[i].loop, tracks[i].tonei);
  }
  printf("{%u, %hu, %hhu, %hhu, %hu}};\n", tracks[i].numTones, tracks[i].speed, tracks[i].key, tracks[i].loop, tracks[i].tonei);
}
/*
// tone length * speed MUST be longer than notebreak
typedef struct track {
  uint numTones;
  ushort speed;
  uchar key;
  uchar loop;
  ushort tonei;
} track;
*/
