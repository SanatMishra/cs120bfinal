#ifndef _GAMESTATEH_
#define _GAMESTATEH_

#include "common.h"
#include "renderable.h"
#include "highscores.h"
#include "input.h"

inline constexpr char entryChars[4][16] PROGMEM = {{'0','1','2','3','4','5','6','7','8','9',':','A','B','C','D','<'},
                                                   {'E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T'},
                                                   {'U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j'},
                                                   {'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'}};

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
void initEnterScreen();
void updateEnterHS();
void clearEnterScreen();

void draw();

#endif