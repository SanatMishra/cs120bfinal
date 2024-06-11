#include "common.h"
#include "timerISR.h"
#include "periph.h"
#include "spiAVR.h"
#include "serialATmega.h"
#include "music.h"
#include "highscores.h"
#include "renderable.h"
#include "input.h"
#include "gamestate.h"
#include <string.h>
#include <stdio.h>

uint RSEED;

//#define NUM_TASKS 4
constexpr int NUM_TASKS = 4;

typedef struct _task{
  signed char state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

// common.h const unsigned long UNIV_PERIOD = 30;
const unsigned long BUZZ_PERIOD = 15;
const unsigned long GCD_PERIOD = 15;

task tasks[NUM_TASKS];

void TimerISR() {
	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {
		if ( tasks[i].elapsedTime == tasks[i].period ) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += GCD_PERIOD;
	}
}

ushort nx, ny, jfl, jfr, jfd, jfu, jx, jy;
uchar jcd, jcd_up,
      jxd, jxd_up, jyd, jyd_up, 
      sw, sw_up, bt, bt_up;
float jxi, jyi;
// ushort jfu95, jfd95, jfr95, jfl95;

enum RIstates{RI_INIT, RI_READ};
int TickReadInput(int state) {
  ushort x = ADC_read(0);
  ushort y = ADC_read(1);
  uchar new_jxd, new_jyd, new_jcd;
  uchar new_sw = 1 - GetBit(PIND, 4);
  uchar new_bt = GetBit(PINC, 2);

  switch (state) {
    case RI_INIT:
      jfl = jfd = 310;
      jfr = jfu = 790;
      nx = jx = x;
      ny = jy = y;
      jxi = 0;
      jyi = 0;
      jcd = jcd_up = jxd = jxd_up = jyd = jyd_up = 0;
      sw = sw_up = new_sw;
      bt = bt_up = new_bt;
      state = RI_READ;
      break;
    case RI_READ:
      state = RI_READ;
      break;
    default: break;
  }

  switch (state) {
    case RI_INIT: break;
    case RI_READ:
      jx = x;
      jy = y;
      if (jy < jfd) jfd = jy;
      if (jy > jfu) jfu = jy;
      if (jx < jfl) jfl = jx;
      if (jx > jfr) jfr = jx;
      if (jx > nx + deadzoneX) {
        new_jxd = 1;
        ushort jfr95 = nx + (ushort)(0.95*(jfr - nx) + 0.5);
        jxi = calibrate(minSpeed, maxSpeed, nx + deadzoneX + 1, jfr95, min(jx, jfr95));
      } else if (jx < nx - deadzoneX) {
        new_jxd = 2;
        ushort jfl95 = nx - (ushort)(0.95*(nx - jfl) + 0.5);
        jxi = calibrate(-minSpeed, -maxSpeed, nx - deadzoneX - 1, jfl95, max(jx, jfl95));
      } else {
        new_jxd = 0;
        jxi = 0;
      }
      if (jy > ny + deadzoneY) {
        new_jyd = 3;
        ushort jfu95 = ny + (ushort)(0.95*(jfu - ny) + 0.5);
        jyi = calibrate(minSpeed, maxSpeed, ny + deadzoneY + 1, jfu95, min(jy, jfu95));
      } else if (jy < ny - deadzoneY) {
        new_jyd = 4;
        ushort jfd95 = ny - (ushort)(0.95*(ny - jfd) + 0.5);
        jyi = calibrate(-minSpeed, -maxSpeed, ny - deadzoneY - 1, jfd95, max(jy, jfd95));
      } else {
        new_jyd = 0;
        jyi = 0;
      }
      new_jcd = abs(jx - nx) > abs(jy - ny) ? new_jxd : new_jyd;
      jcd_up = (jcd == new_jcd) ? 0 : new_jcd;
      jcd = new_jcd;
      jxd_up = (jxd == new_jxd) ? 0 : new_jxd;
      jxd = new_jxd;
      jyd_up = (jyd == new_jyd) ? 0 : new_jyd;
      jyd = new_jyd;
      sw_up = !sw && new_sw;
      sw = new_sw;
      bt_up = !bt && new_bt;
      bt = new_bt;
      break;
    default: break;
  }
  return state;
}

bool toPrint;

// ONLY Audio code in Game Engine is to set a new track or queue a beep.
enum GEstates{GE_INIT, GE_MENU, GE_GAMEPLAY, GE_SUPER, GE_GAMEOVER, GE_ENTER_HS, GE_HS, GE_PURGATORY};
int TickGameEngine(int state) {
  switch (state) {
    case GE_INIT:
      newTrack(0);
      toPrint = 1;
      initMenuScreen();
      state = GE_MENU;
      break;
    case GE_MENU:
      if (toPrint) {
        serial_println("Main Menu");
        toPrint = 0;
      }

      if (bt_up) {
        queueBeep(68, 7);
        if (menuOption == 0) {
          newTrack(1);
          initGame();
          clearMenuScreen();
          initGameScreen();
          toPrint = 1;
          state = GE_GAMEPLAY;
        } else {
          toPrint = 1;
          clearMenuScreen();
          initHSScreen();
          state = GE_HS;
        }
      } else {
        state = GE_MENU;
      }
      break;
    case GE_GAMEPLAY:
      if (toPrint) {
        serial_println("Playing Game");
        toPrint = 0;
      }
      if (!gameActive) {
        serial_println("Ending Game");
        serial_print("Score: ");
        serial_println(score);
        newTrack(0);
        endGame();
        if (gameForcedReset) {
          clearGameScreen();
          initMenuScreen();
          gameForcedReset = 0;
          state = GE_MENU;
        } else {
          if (numHSEntries < MAX_HS_ENTRIES || score > HSEntries[numHSEntries - 1].score) {
            clearGameScreen();
            initEnterScreen();
            state = GE_ENTER_HS;
          } else {
            clearGameScreen();
            initGameOverScreen();
            state = GE_GAMEOVER;
          }
        }
      } else {
        state = GE_GAMEPLAY;
      }
      break;
    case GE_SUPER:
      state = GE_GAMEPLAY;
      break;
    case GE_GAMEOVER:
      if (toPrint) {
        serial_println("Game Over");
        toPrint = 0;
      }
      if (bt_up) {
        menuOption = 0;
        toPrint = 1;
        newTrack(0);

        clearGameOverScreen();
        initMenuScreen();
        state = GE_MENU;
      } else {
        state = GE_GAMEOVER;
      }
      break;
    case GE_ENTER_HS:
      //serial_println("High score! Enter name.");
      //serial_print("score = "); serial_println(score);
      //sprintf(enterHSuname, "score%04dXX", score);
      toPrint = 1;

      if (bt_up) {
        writeHSEntries();
        clearEnterScreen();
        initHSScreen();
        state = GE_HS;
      } else {
        state = GE_ENTER_HS;
      }
      break;
    case GE_HS:
      if (toPrint) {
        printHSEntriesl();
        toPrint = 0;
      }
      
      if (bt_up) {
        newTrack(0);
        menuOption = 0;
        toPrint = 1;

        clearHSScreen();
        initMenuScreen();
        state = GE_MENU;
      } else {
        state = GE_HS;
      }
      break;
    default: break;
  }
  
  switch (state) {
    case GE_INIT: break;
    case GE_MENU:
      updateMenuScreen();
      break;
    case GE_GAMEPLAY:
      updateGame();
      break;
    case GE_SUPER:
      break;
    case GE_GAMEOVER:
      break;
    case GE_ENTER_HS:
      updateEnterHS();
      break;
    case GE_HS:
      break;
    default: break;
  }
  return state;
}

enum BWstates{BW_INIT, BW_WRITE};
int TickBuzzWrite(int state) {
  switch (state) {
    case BW_INIT:
      state = BW_WRITE;
      break;
    case BW_WRITE:
      state = BW_WRITE;
      break;
    default:
      break;
  }
  
  uchar outNote;

  switch (state) {
    case BW_INIT: break;
    case BW_WRITE:
      if (curToneI == tracks[curTrackI].numTones && beepDur == 0) {
        stone(getFreq(0));
        break;
      }
      if (beepDur > 0) {
        outNote = beepNote;
        beepDur--;
      } else {
        outNote = t2n(curTone);
      }
      if (curToneI != tracks[curTrackI].numTones) {
        if (t2d(curTone) != 0x1FF) {
          curTone = d2t(curTone, t2d(curTone) - 1);
        }
        if (t2d(curTone) == 0) {
          curToneI++;
          if (curToneI == tracks[curTrackI].numTones && tracks[curTrackI].loop)
            curToneI = 0;
          if (curToneI != tracks[curTrackI].numTones)
            curTone = tracks[curTrackI].track[curToneI];
        }
      }
      stone(getFreq(outNote));
      break;
    default:
      break;
  }
  return state;
}

enum LCDstates{LCD_INIT, LCD_WRITE, LCD_PURGATORY};
int TickLCDWrite(int state) {
  switch (state) {
    case LCD_INIT:
      state = LCD_WRITE;
      break;
    case LCD_WRITE:
      state = LCD_WRITE;
      break;
    default: break;
  }
  
  switch (state) {
    case LCD_INIT: break;
    case LCD_WRITE:
      //for (int i = 0; i < 32; i++) {
      //  serial_println(textc[i]);
      //}
      draw();
      if (gameActive || gameNeedsClearing)
        emptyGraveyard();
      break;
    default: break;
  }
  return state;
}

void initSMAS() {
  gameActive = 0;
  gameNeedsClearing = 0;
  gameForcedReset = 0;
  player.pa = player.na = 2;
  // bullets = ActorList<Bullet, MAX_BULLETS>();
  // bullets.actb = bullets.acte = MAX_BULLETS;
  // bullets.actfb = 0;
  // for (uchar i = 0; i < MAX_BULLETS; i++) {
  //   bullets[i].na = i + 1;
  //   bullets[i].pa = MAX_BULLETS + 1;
  // }
  // bullets.n = 0;
}

void predraw() {
  uchar xm = 2, xn = 129, ym = 1, yn = 128;
  SREG &= 0x7F;
  spic4(CASET, 0, xm, 0, xn);
  spic4(RASET, 0, ym, 0, yn);
  spic(RAMWR);
  for (int i = 0; i < yn - ym + 1; i++) {
    for (int j = 0; j < xn - xm + 1; j++) {
      spid(0 << 2);
      spid(0 << 2);
      spid(0 << 2);
    }
  }
  SREG |= 0x80;
}

int main(void) {
  serial_init(9600);
  if (0) {
    clearHSEntries();
    return 0;
  }
  DDRB  =   0b111111;
  PORTB =   0b000000;
  DDRC  =   0b110000;
  PORTC =   0b001111;
  DDRD  = 0b11101111;
  PORTD = 0b00010000;

  ADC_init();         // Initializes ADC
  RSEED = ADC_read(5) + ADC_read(4) + ADC_read(3) + ADC_read(2) + ADC_read(1) + ADC_read(0);
  rr(); rr(); rr();   // Initialize RSEED
  loadHSEntries();    // Load high scores from EEPROM
  initializeTracks(); // Initialize music tracks
  initScreen();       // Init text array
  initSMAS();         // Init variables persistent across games
  st_init();          // Initialize music PWM
  SPI_INIT();         // Initializes SPI
  predraw();
  //scString("Start", 5, 6, 8);
  //scChar('>', 5, 10);
  //draw();
  //while(1);

  TCCR1A |= (1 << WGM11) | (1 << COM1A1);
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
  ICR1 = 39999;
  OCR1A = 2999;
  
  tasks[0] = {RI_INIT, UNIV_PERIOD, UNIV_PERIOD, &TickReadInput};
  tasks[1] = {GE_INIT, UNIV_PERIOD, UNIV_PERIOD, &TickGameEngine};
  tasks[2] = {BW_INIT, BUZZ_PERIOD, BUZZ_PERIOD, &TickBuzzWrite};
  tasks[3] = {LCD_INIT, UNIV_PERIOD, UNIV_PERIOD, &TickLCDWrite};

  //scString("Start", 5, 6, 8);
  //draw();
  //newTrack(1);
  //serial_println(curTone);
  //queueBeep(49 + 12*2, 100);

  TimerSet(GCD_PERIOD);
  TimerOn();

  while (1) {}

  return 0;
}