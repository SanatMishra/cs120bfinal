#ifndef _GAMESTATEH_
#define _GAMESTATEH_
  
#include "common.h"
#include "renderable.h"
#include "highscores.h"
#include "input.h"
#include "ActorList.h"

#define GXMIN (0)
#define GXMAX ((WIDTH) - 1)
#define GYMIN (8)
#define GYMAX ((HEIGHT) - 2*8 - 1)

inline constexpr char entryChars[4][16] PROGMEM = {{'0','1','2','3','4','5','6','7','8','9',':','A','B','C','D','<'},
                                                   {'E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T'},
                                                   {'U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j'},
                                                   {'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'}};

extern uchar menuOption;

extern bool gameActive;
extern bool gameNeedsClearing;
extern ushort score;
extern uchar superMeter;

typedef struct Actor {
  float xt, yt;
  uchar x, y, xp, yp, w, h;
  bool alive;
  bool needsRedraw;
  uchar s;
  uchar na, pa;
} Actor;

typedef struct Bullet : public Actor {
  uchar v;
} Bullet;

#define MAX_BULLETS 10

extern Actor player;
extern ActorList<Bullet, MAX_BULLETS> bullets;

void initMenuScreen();
void updateMenuScreen();
void clearMenuScreen();
void initHSScreen();
void clearHSScreen();
void initGameScreen();
void clearGameScreen();
void initGame();
void updateGame();
void endGame();
void initEnterScreen();
void updateEnterHS();
void clearEnterScreen();
void initGameOverScreen();
void clearGameOverScreen();

void draw();
void emptyGraveyard();

#endif