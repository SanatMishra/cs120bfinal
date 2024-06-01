#ifndef _GAMESTATEH_
#define _GAMESTATEH_

#include "common.h"
#include "renderable.h"
#include "highscores.h"
#include "input.h"

extern uchar menuOption;

extern bool gameActive;
extern ushort score;
extern uchar superMeter;

void initMenuScreen();
void clearMenuScreen();
void initHSScreen();
void clearHSScreen();
void initGameScreen();
void printGSMeters();
void clearGameScreen();
void updateGame();
void draw();

#endif