#include "common.h"
#include "renderable.h"
#include "input.h"
#include "spiAVR.h"
#include "highscores.h"
#include "gamestate.h"

PlayerShip player;
Bullet bullets[MAX_BULLETS];
EnemyShip enemyShips[MAX_ENEMIES];
Drop drops[MAX_DROPS];
Actor loneRenderables[MAX_LRS];
ushort numBullets;
ushort numEnemies;
ushort numDrops;
ushort numLRs;

Renderable* Actor::getRenderable() {
  return renderable;
}

ActorIterator ActorIterator::begin() {
  uchar ll = 0;
  ushort ii = 0;
  ActorIterator::autoIncrement(&ll, &ii);
  return ActorIterator(ll, ii);
}
ActorIterator ActorIterator::end() { return ActorIterator(5, 0); }

Actor ActorIterator::operator*() {
  switch(l) {
    case 0: return gameActive ? player : *(Actor*)0;
    case 1: return bullets[i];
    case 2: return enemyShips[i];
    case 3: return drops[i];
    case 4: return loneRenderables[i];
    default: return *(Actor*)0;
  }
}
Actor* ActorIterator::operator->() {
  switch(l) {
    case 0: return gameActive ? &player : (Actor*)0;
    case 1: return &bullets[i];
    case 2: return &enemyShips[i];
    case 3: return &drops[i];
    case 5: return &loneRenderables[i];
    default: return (Actor*)0;
  }
}

ushort maxAIbound(int l) {
  switch(l) {
    case 0: return gameActive;
    case 1: return numBullets;
    case 2: return numEnemies;
    case 3: return numDrops;
    default: return numLRs;
  }
}

void ActorIterator::autoIncrement(uchar* ll, ushort* ii) {
  ushort m = maxAIbound(*ll);
  while (*ii >= m && *ll < 5) {
    *ii = 0;
    *ll += 1;
    m = maxAIbound(*ll);
  }
}

ActorIterator& ActorIterator::operator++() {
  i++;
  autoIncrement(&l, &i);
  return *this;
}

bool ActorIterator::operator==(const ActorIterator& other) const { return l == other.l && i == other.i; }
bool ActorIterator::operator!=(const ActorIterator& other) const { return !(l == other.l && i == other.i); }

bool gameActive;
uint gameTime;
ushort score;
uchar superMeter;

void initGame() {
  gameActive = 1;
  gameTime = 0;
  superMeter = 0;
  score = 0;
  numBullets = numEnemies = numDrops = numLRs = 0;
  player.w = player.h = 8;
  player.x = player.xp = (WIDTH - player.w)/2;
  player.y = player.yp = 0;
  player.xt = player.x;
  player.yt = player.y;
  player.alive = 1;
  serial_println("jdvjwe");
  player.renderable = SpriteRenderable(0);
  serial_println("after new SR");
}

void endGame() {
  gameActive = 0;
  numBullets = numEnemies = numDrops = numLRs = 0;
  delete player.renderable;
}

void boundedMove(uchar* x, uchar* y, uchar w, uchar h, uchar dx, uchar dy) {
  *x = clamp(0, WIDTH - w, *x + dx);
  *y = clamp(0, HEIGHT - h, *y + dy);
}
void playerMove(uchar dx, uchar dy) {
  boundedMove(&player.x, &player.y, player.w, player.h, dx, dy);
} 

void updateGame() {
//super? --> instant super, break/goto end

player.xp = player.x;
player.yp = player.y;

player.xt = clamp(0, WIDTH - player.w, player.xt + jxi);
player.yt = clamp(0, HEIGHT - player.h, player.yt + jyi);
player.x = uround(player.xt);
player.y = uround(player.yt);

/*actors act
 - new enemy?
 - update bullet location
 - update powerup location
 - playerMove
    $ update player location
    - if should fire, fire
 - enemyMove
    - update enemy behavior state
    - update enemy location
    - if should fire, fire
environment rules act
 - player bullets hit opponent
    - enemy deleted
    - drop power-up?
    - create explosion
    - super up
 - powerup hit player
 - opponent bullets hit player
    - minus hp
    - no hp --> instant game over, break/goto end
    - trigger invuln
    - super way up*/
  
  
}

void drawAllActorsInRange(uchar xm, uchar xn, uchar ym, uchar yn) {
  uint buf[xn - xm + 1][yn - ym + 1][3] = {{{0}}};
  for (auto it = ActorIterator::begin(); it != ActorIterator::end(); ++it) {
    if (xn < it->x ||  xm > it->x + it->getRenderable()->width() - 1
        || yn < it->y || ym > it->y + it->getRenderable()->height() - 1 || !it->alive)
      continue;
    for (ushort y = ym; y <= yn; y++) {
      for (ushort x = xn; x >= xm; x--) {
        uint rpx = it->getRenderable()->getpx(x - it->x, y - it->y);
        if (!ccx(rpx)) {
          buf[x - xm][y - yn][0] = ccr(rpx);
          buf[x - xm][y - yn][1] = ccg(rpx);
          buf[x - xm][y - yn][2] = ccb(rpx);
        }
      }
    }
  }
  SREG &= 0x7F;
  spic4(CASET, 0, g2sx(xn), 0, g2sx(xm));
  spic4(RASET, 0, g2sy(ym), 0, g2sy(yn));
  for (ushort y = ym; y <= yn; y++) {
    for (ushort x = xn; x >= xm; x--) {
      spid(buf[x - xm][y - yn][2]);
      spid(buf[x - xm][y - yn][1]);
      spid(buf[x - xm][y - yn][0]);
    }
  }
  SREG |= 0x80;
}

void draw() {
  uchar w = player.getRenderable()->width();
  uchar h = player.getRenderable()->height();
  uchar xm = player.x;
  uchar xn = player.x + w - 1;
  uchar ym = player.y;
  uchar yn = player.y + h - 1;
  uint buf[xn - xm + 1][yn - ym + 1][3] = {{{0}}};
  if (player.alive && (player.xp != player.x || player.yp != player.y)) {
    for (ushort y = ym; y <= yn; y++) {
      for (ushort x = xn; x >= xm; x--) {
        uint rpx = player.getRenderable()->getpx(x - player.x, y - player.y);
        if (!ccx(rpx)) {
          buf[x - xm][y - yn][0] = ccr(rpx);
          buf[x - xm][y - yn][1] = ccg(rpx);
          buf[x - xm][y - yn][2] = ccb(rpx);
        }
      }
    }
    SREG &= 0x7F;
    spic4(CASET, 0, g2sx(xn), 0, g2sx(xm));
    spic4(RASET, 0, g2sy(ym), 0, g2sy(yn));
    spic(RAMWR);
    for (ushort y = ym; y <= yn; y++) {
      for (ushort x = xn; x >= xm; x--) {
        spid(buf[x - xm][y - yn][2]);
        spid(buf[x - xm][y - yn][1]);
        spid(buf[x - xm][y - yn][0]);
      }
    }
    SREG |= 0x80;
  }
  if (player.xp != player.x) {
    ym = player.yp;
    yn = player.yp + h - 1;
    if (player.xp < player.x) {
      xm = player.xp;
      xn = player.x - 1;
    } else {
      xm = player.x + w;
      xn = player.xp + w - 1;
    }
    drawAllActorsInRange(xm, xn, ym, yn);
  }
  if (player.yp != player.y) {
    if (player.xp < player.x) {
      xm = player.x;
      xn = player.xp + w - 1;
    } else {
      xm = player.xp;
      xn = player.x + w - 1;
    }
    if (player.yp < player.y) {
      ym = player.yp;
      yn = player.y - 1;
    } else {
      ym = player.y + h;
      yn = player.yp + h - 1;
    }
    drawAllActorsInRange(xm, xn, ym, yn);
  }
}

void emptyGraveyard() {
  for (auto it = ActorIterator::begin(); it != ActorIterator::end(); ) {
    if (!it->alive) {
      switch(it.l) {
        case 0: return;
        case 1:
          numBullets--;
          for (ushort i = it.i; i < numBullets; i++) {
            bullets[i] = bullets[i + 1];
          }
          break;
        case 2:
          numEnemies--;
          for (ushort i = it.i; i < numEnemies; i++) {
            enemyShips[i] = enemyShips[i + 1];
          }
          break;
        case 3:
          numDrops--;
          for (ushort i = it.i; i < numDrops; i++) {
            drops[i] = drops[i + 1];
          } 
          break;
        default:
          numLRs--;
          for (ushort i = it.i; i < numLRs; i++) {
            loneRenderables[i] = loneRenderables[i + 1];
          }
          break;
      }
    } else {
      ++it;
    }
  }
}
