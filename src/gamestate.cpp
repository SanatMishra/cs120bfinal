#include "common.h"
#include "renderable.h"
#include "input.h"
#include "spiAVR.h"
#include "highscores.h"
#include "gamestate.h"

uchar menuOption;

bool gameActive;
ushort score;
uchar superMeter;

void initMenuScreen() {
  menuOption = 0;
  scString("Start", 5, 6, 10);
  //scChar('t', 10, 10);
  //scString("High Scores", 11, 3, 8);
  //scChar('H', 3, 9);
  scString("High scores", 11, 3, 9);
  scChar('#', 5, 10);
  draw();
}

/*void updateMenuscreen() {

}*/

void clearMenuScreen() {
  scClear();
}

void initHSScreen() {
  char HSline[17];
  for (uchar i = 0; i < numHSEntries; i++) {
    getHSLine(HSline, i);
    scString(HSline, 16, 0, MAX_HS_ENTRIES - i - 1);
  }
}

void clearHSScreen() {
  scClear();
}

void initGame() {
  superMeter = 0;
  score = 0;
}

void printGSMeters() {
  char scorebuf[5];
  sprintf(scorebuf, "%04hu", score);
  scString(scorebuf, 4, 6, 15);

  ushort hs = numHSEntries > 0 ? HSEntries[0].score : 0;
  sprintf(scorebuf, "%04hu", score > hs ? score : hs);
  scString(scorebuf, 4, 6, 14);

  for (uchar i = 0; i < uround(superMeter/10.0); i++)
    scChar('#', 6 + i, 0);
}

void initGameScreen() {
  scString("Score:", 6, 0, 15);
  scString("High: ", 6, 0, 14);
  scString("Super ", 6, 0, 0);

  printGSMeters();
}

void clearGameScreen() {
  scClear();
}

void updateGame() {
  if (score < 9999) score++;
  if (superMeter < 100) superMeter++;
  printGSMeters();
}

void draw() {
  uchar xm, xn, ym, yn;
  for (uchar ty = 0; ty < 16; ty++) {
    for (uchar tx = 15; tx < 255; tx--) {
      if (textch(tx, ty)) {
        xm = 8*tx;
        xn = xm + 7;
        ym = 8*ty;
        yn = yn + 7;
        //serial_println(xm);
        //serial_println(xn);
        //serial_println(ym);
        //serial_println(yn);
        //serial_println(tx);
        //serial_println(ty);
        //serial_char(text[tx][ty]);
        //serial_println("");
        textcd(tx, ty);
        SREG &= 0x7F;
        spic4(CASET, 0, g2sx(xn), 0, g2sx(xm));
        spic4(RASET, 0, g2sy(ym), 0, g2sy(yn));
        spic(RAMWR);
        for (uchar py = 0; py < 8; py++) {
          for (uchar px = 7; px < 255; px--) {
            uint rpx = getpx(offset(text[tx][ty]), px, py);
            //serial_char(text[tx][ty]);
            //serial_print(": ");
            //serial_char(48 + px);
            //serial_char(48 + py);
            if (!ccx(rpx)) {
              spid(ccb(rpx) << 2);
              spid(ccg(rpx) << 2);
              spid(ccr(rpx) << 2);
              //serial_print("a ");
            } else {
              spid(0 << 2);
              spid(0 << 2);
              spid(0 << 2);
              //serial_print("b ");
            }
            //serial_println(rpx);
          }
        }
        SREG |= 0x80;
        //_delay_ms(1); // for good luck
      }
    }
  }
  
}
