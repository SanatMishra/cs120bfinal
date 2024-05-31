#ifndef _GAMESTATEH_
#define _GAMESTATEH_

#include "common.h"
#include "renderable.h"
#include "input.h"

class Actor {
public:
  // x measured l-r, y measured d-u 
  uchar x, y, xp, yp, w, h;
  float xt, yt;
  bool alive;
};

class TextActor : Actor {
private:
  uint palette;
  uchar idx;
  ushort offset;
public:
  TextActor();
  uint getpx(uchar x, uchar y);
  void changePalette(uint a, uint b, uint c, uint d);
};

class SpriteActor : Actor {
private:
  uint palette[4];
  uchar idx;
  ushort offset;
public:
  SpriteActor();
  uint getpx(uchar x, uchar y);
  void changePalette(uint a, uint b, uint c, uint d);
}

class PlayerShip : public Actor {
public:
};

class Bullet : public Actor {
public:
  uchar owner;
};

class EnemyShip : public Actor {
public:
};

class Drop : public Actor {
public:
};

class ActorIterator {
private:
public:
  uchar l;
  ushort i;
  static void autoIncrement(uchar* ll, ushort* ii);
  static ActorIterator begin();
  static ActorIterator end();
  ActorIterator(uchar ll = 0, ushort ii = 0): l(ll), i(ii) {}
  Actor operator*();
  Actor* operator->();
  ActorIterator& operator++();
  bool operator==(const ActorIterator& other) const;
  bool operator!=(const ActorIterator& other) const;
};

extern bool gameActive;
extern uint gameTime;
extern ushort score;
extern uchar superMeter;
extern PlayerShip player;

inline constexpr uint MAX_BULLETS = 10;
extern ushort numBullets;
extern Bullet bullets[MAX_BULLETS];

inline constexpr uint MAX_ENEMIES = 5;
extern ushort numEnemies;
extern EnemyShip enemyShips[MAX_ENEMIES];

inline constexpr uint MAX_DROPS = 2;
extern ushort numDrops;
extern Drop drops[MAX_DROPS];

inline constexpr uint MAX_ACTORS = 64;
inline constexpr uint MAX_LRS = MAX_ACTORS - MAX_BULLETS - MAX_ENEMIES - MAX_DROPS;
extern ushort numLRs;
extern Actor loneRenderables[MAX_LRS];

void initGame();
void endGame();
void draw();
void updateGame();
void emptyGraveyard();

#endif