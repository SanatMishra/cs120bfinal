#include "common.h"
#include "renderable.h"
#include "input.h"
#include "spiAVR.h"
#include "highscores.h"
#include "music.h"
#include "gamestate.h"

uchar menuOption;

uint gameTime;
bool gameActive;
bool gameNeedsClearing;
bool gameForcedReset;
uchar playerHP;
uchar playerInvuln;
ushort score;
uchar superMeter;
ushort newEnemyChance;

Actor player;
ActorList<Bullet, MAX_BULLETS> bullets;
ActorList<Enemy, MAX_ENEMIES> enemies;

char enterHSuname[12];
uchar ehscx, ehscy, ehscp;
uchar hspos;

void initMenuScreen() {
  newTrack(0);
  menuOption = 0;
  scString("Start", 5, 6, 10);
  //scChar('t', 10, 10);
  //scString("High Scores", 11, 3, 8);
  //scChar('H', 3, 9);
  scString("High scores", 11, 3, 9);
  scChar('>', 5, 10);
  draw();
}

void updateMenuScreen() {
  if (jyd_up == 4 && menuOption < 1) {
    menuOption++;
    scChar('>', 2, 9);
    scChar(0, 5, 10);
    serial_println("High scores");
    queueBeep(61, 4);
  } else if (jyd_up == 3 && menuOption > 0) {
    menuOption--;
    scChar('>', 5, 10);
    scChar(0, 2, 9);
    serial_println("Start");
    queueBeep(61, 4);
  }
}

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

void printGSMeters() {
  char scorebuf[5];
  sprintf(scorebuf, "%04hu", score);
  scString(scorebuf, 4, 6, 15);

  ushort hs = numHSEntries > 0 ? HSEntries[0].score : 0;
  sprintf(scorebuf, "%04hu", score > hs ? score : hs);
  scString(scorebuf, 4, 6, 14);

  for (uchar i = 0; i < uround(superMeter/10.0); i++)
    scChar('#', 6 + i, 0);

  for (uchar i = 0; i < 3; i++)
    scChar(i < playerHP ? '#' : ' ', 13 + i, 15);
  
  scColorString(5, 11, 15, playerHP > 1 ? 2 : 1);
}

void initGameScreen() {
  newTrack(1);
  scString("Score:", 6, 0, 15);
  scString("High: ", 6, 0, 14);
  scString("Super ", 6, 0, 0);
  scString("HP", 2, 11, 15);

  printGSMeters();
}

void clearGameScreen() {
  scClear();
}

void initGame() {
  gameTime = 0;
  gameActive = 1;
  superMeter = 0;
  score = 0;
  playerHP = 3;
  playerInvuln = 0;
  newEnemyChance = 0;

  player.w = player.h = 8;
  player.xt = 0.5*(GXMIN + GXMAX - player.w + 1);
  player.yt = GYMIN;
  player.xp = player.x = uround(player.xt);
  player.yp = player.y = uround(player.yt);
  player.alive = 1;
  player.s = PLAYER_SPRITENO;
  player.needsRedraw = 1;
}

uchar newBullet(float xt, float yt, float vx, float vy, uchar own) {
  if (xt < 0 || yt < 0 || xt >= GXMAX || yt >= GYMAX) return MAX_BULLETS;
  uchar bi = bullets.addActor();
  if (bi < MAX_BULLETS) {
    bullets[bi].w = 2; bullets[bi].h = 3;
    bullets[bi].xt = xt;
    bullets[bi].yt = yt;
    bullets[bi].xp = bullets[bi].x = uround(bullets[bi].xt);
    bullets[bi].yp = bullets[bi].y = uround(bullets[bi].yt);
    bullets[bi].alive = 1;
    bullets[bi].s = BULLET_SPRITENO;
    bullets[bi].needsRedraw = 1;
    bullets[bi].vx = vx;
    bullets[bi].vy = vy;
    bullets[bi].own = own;
  }
  return bi;
}

uchar moveWithinBounds(Actor* a, float vx, float vy) {
  uchar ret = 4;
  a->xt += vx;
  if (a->xt > GXMAX - a->w + 1) {
    a->xt = GXMAX - a->w + 1;
    ret += 1;
  }
  if (a->xt < GXMIN) {
    a->xt = GXMIN;
    ret -= 1;
  }
  a->yt += vy; 
  if (a->yt > GYMAX - a->h + 1) {
    a->yt = GYMAX - a->h + 1;
    ret += 3;
  }
  if (a->yt < GYMIN) {
    a->yt = GYMIN;
    ret -= 3;
  }
  a->x = uround(a->xt);
  a->y = uround(a->yt);
  return ret;
}

void setXPYP() {
  player.xp = player.x;
  player.yp = player.y;
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    bullets[i].xp = bullets[i].x;
    bullets[i].yp = bullets[i].y;
  }
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    enemies[i].xp = enemies[i].x;
    enemies[i].yp = enemies[i].y;
  }
}

void bulletsMove() {
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    if (moveWithinBounds(&bullets[i], bullets[i].vx, bullets[i].vy) != 4) {
      bullets[i].alive = 0;
      bullets[i].needsRedraw = 1;
    }
  }
}

bool singleCollision(Actor* a, Actor* b, float oax = 0, float oay = 0, float obx = 0, float oby = 0) {
  float bxm = b->x + obx;
  float bxn = b->x + obx + b->w;
  float bym = b->y + oby;
  float byn = b->y + oby + b->h;

  float axm = a->x + oax;
  float axn = a->x + oax + a->w;
  float aym = a->y + oay;
  float ayn = a->y + oay + a->h;

  return axn >= bxm && axm <= bxn && ayn >= bym && aym <= byn;
}

bool detectBullet(uchar x) {
  if (rr() % 1500 >= gameTime + 250) return false;
  //serial_print("detect "); serial_print(x); serial_print(" ");
  uchar dbtime = 10;
  float oax, oay;
  switch(enemies[x].state) {
    case 0:
      oax = dbtime; oay = 0;
      break;
    case 255:
      oax = -dbtime; oay = 0;
      break;
    case 1:
      oax = 0; oay = -dbtime;
      break;
    case 254:
      oax = 0; oay = dbtime;
    default: break;
  }
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    if (bullets[i].own == 0 && singleCollision(&enemies[x], &bullets[i], oax, oay, dbtime*bullets[i].vx, dbtime*bullets[i].vy))
      return true;
  }
  return false;
}

void enemiesMove() {
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    switch(enemies[i].state) {
      case 0: 
        if (detectBullet(i) || moveWithinBounds(&enemies[i], 1, 0) == 5) {
          enemies[i].state = 255;
        }
        break;
      case 255:
        if (detectBullet(i) || moveWithinBounds(&enemies[i], -1, 0) == 3) {
          enemies[i].state = 0;
        }
        break;
      case 1:
        if (detectBullet(i) || moveWithinBounds(&enemies[i], 0, -1) == 1) {
          enemies[i].state = 254;
        }
        break;
      case 254:
        if (detectBullet(i) || moveWithinBounds(&enemies[i], 0, 1) == 7) {
          enemies[i].state = 1;
        }
        break;
      default: break;
    }
    // if (moveWithinBounds(&enemies[i], enemies[i].vx, bullets[i].vy) != 4) {
    //   enemies[i].alive = 0;
    //   enemies[i].needsRedraw = 1;
    // }
  }
}

void playerMoves() {
  if (moveWithinBounds(&player, jxi, jyi) != 4) {
    gameActive = 0;
  }
}

uchar fireCooldown() {
  return 80 + rr() % 41;
}

uchar newEnemy() {
  uchar ei = enemies.addActor();
  if (ei < MAX_ENEMIES) {
    uchar theType = rr() % NUM_ENEMY_TYPES;
    enemies[ei].w = enemies[ei].h = 8;
    enemies[ei].xt = GXMIN + rr() % (GXMAX - GXMIN - enemies[ei].w + 2);
    enemies[ei].yt = (GYMIN + GYMAX)/2 + rr() % ((uint)(GYMAX - (GYMIN + GYMAX)/2 - enemies[ei].h + 2));
    enemies[ei].xp = enemies[ei].x = uround(enemies[ei].xt);
    enemies[ei].yp = enemies[ei].y = uround(enemies[ei].yt);
    enemies[ei].alive = 1;
    enemies[ei].s = ENEMY_SPRITENO + theType;
    enemies[ei].needsRedraw = 1;
    enemies[ei].state = theType;
    enemies[ei].tsf = rr() % 100;
  }
  return ei;
}

void genEnemyCheck() {
  newEnemyChance += (ushort)(calibrate(300, 0, 0, MAX_ENEMIES, enemies.n));
  if (rr() % 10000 < newEnemyChance) {
    newEnemyChance = 0;
    newEnemy();
  }
}

void playerFires() {
  if (bt_up && gameTime > 0) {
    queueBeep(61, 4);
    newBullet(player.x + 0.5*(player.w - 2), player.y + player.h, 0, 2, 0);
  }
}

void enemiesFire() {
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    if (enemies[i].tsf == 0) {
      float xt, yt, vx, vy;
      bool dir;
      switch(enemies[i].state) {
        case 0: case 255:
          xt = enemies[i].x + 0.5*(enemies[i].w - 2); yt = enemies[i].y  - 3; vx = 0; vy = -3;
          break;
        case 1: case 254:
          dir = enemies[i].xt < player.xt;
          xt = dir ? enemies[i].x + enemies[i].w : enemies[i].x - 2;
          yt = enemies[i].y + 0.5*(enemies[i].h - 3);
          vx = dir ? 3 : -3;
          vy = 0;
          break;
        default:
          xt = yt = vx = vy = -1; // fail bullet creation
      }
      if (newBullet(xt, yt, vx, vy, 1) < MAX_BULLETS)
        enemies[i].tsf = fireCooldown();
    } else {
      enemies[i].tsf--;
    }
  }
}

void checkNeedsRedraw() {
  if (player.x != player.xp || player.y != player.yp) {
    /*serial_print("nx="); serial_print(nx); serial_print(" ny="); serial_print(ny);
    serial_print(" jx="); serial_print(jx); serial_print(" jy="); serial_println(jy);
    serial_print("jfu="); serial_print(jfu); serial_print(" jfu95="); serial_print(jfu95);
    serial_print(" jfd="); serial_print(jfd); serial_print(" jfd95="); serial_println(jfd95);
    serial_print("jfr="); serial_print(jfr); serial_print(" jfr95="); serial_print(jfr95);
    serial_print(" jfl="); serial_print(jfl); serial_print(" jfl95="); serial_println(jfl95);
    serial_print(" jxi="); serial_print(jxi*10000); serial_print(" jyi="); serial_println(jyi*10000);
    serial_println("-----");*/
    player.needsRedraw = 1;
  }
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    if(bullets[i].x != bullets[i].xp || bullets[i].y != bullets[i].yp)
      bullets[i].needsRedraw = 1;
  }
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    if(enemies[i].x != enemies[i].xp || enemies[i].y != enemies[i].yp)
      enemies[i].needsRedraw = 1;
  }
}

void playerBulletCollision() {
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    for (uchar j = enemies.actb; j < MAX_ENEMIES; j = enemies[j].na) {
      if (bullets[i].alive && bullets[i].own == 0 && enemies[j].alive && singleCollision(&enemies[j], &bullets[i])) {
        enemies[j].alive = 0;
        enemies[j].needsRedraw = 1;
        bullets[i].alive = 0;
        bullets[i].needsRedraw = 1;
        if (score < 9999) score++;
      }
    }
  }
}

void damagePlayer() {
  playerHP--;
  if (playerHP == 0) {
    gameActive = 0;
    return;
  }
  playerInvuln = BLINKDUR;
  player.needsRedraw = 1;
}

void enemyBulletCollision() {
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    if (bullets[i].alive && bullets[i].own == 1 && !playerInvuln && singleCollision(&player, &bullets[i])) {
      damagePlayer();
      if (!gameActive) return;
      bullets[i].alive = 0;
      bullets[i].needsRedraw = 1;
    }
  }
}

void enemyPlayerCollision() {
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    if (enemies[i].alive && !playerInvuln && singleCollision(&player, &enemies[i])) {
      damagePlayer();
      if (!gameActive) return;
    }
  }
}

void updateGame() {
  setXPYP();
  if (bt_up && sw_up) {
    gameActive = 0;
    score = 0;
    gameForcedReset = 1;
    return;
  }
  genEnemyCheck();
  bulletsMove();
  playerMoves();
  if (!gameActive) return;
  playerFires();
  enemiesMove();
  enemiesFire();
  playerBulletCollision();
  enemyBulletCollision();
  if (!gameActive) return;
  enemyPlayerCollision();
  checkNeedsRedraw();

  //if (score < 9999) score++;
  if (superMeter < 100) superMeter++;
  gameTime++;
  if (playerInvuln == 1 || (BLINKDUR - playerInvuln) % BLINKSPEED == 0)
    player.needsRedraw = 1;
  if (playerInvuln > 0) playerInvuln--;
  printGSMeters();
}

void endGame() {
  gameActive = 0;
  player.alive = 0;
  player.needsRedraw = 1;
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    bullets[i].alive = 0;
    bullets[i].needsRedraw = 1;
  }
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    enemies[i].alive = 0;
    enemies[i].needsRedraw = 1;
  }
  gameNeedsClearing = 1;
  superMeter = 0;
}

void EHSScreenPrintEntries() {
  char HSline[17];
  for (uchar i = 0; i < numHSEntries; i++) {
    getHSLine(HSline, i);
    scString(HSline, 16, 0, MAX_HS_ENTRIES - i - 1);
  }
}

void moveCursor(uchar x, uchar y, uchar nx, uchar ny) {
  scChar(pgm_read_byte(&(entryChars[y][x])), x, 13 - y, (y == 0 && x == 15 ? 1 : 3));
  scChar('#', nx, 13 - ny);
  serial_print(x); serial_print(" "); serial_print(y); serial_print(" "); serial_print(13 - y); serial_print(" ");
  serial_print(nx); serial_print(" "); serial_print(ny); serial_print(" "); serial_println(13 - ny);
}

void initEnterScreen() {
  newTrack(3);
  hspos = addHSEntry("           ", score);

  scString("Enter High Score", 16, 0, 15);
  scString("Score: ", 10, 3, 14);
  char scorebuf[5];
  sprintf(scorebuf, "%04hu", score);
  scString(scorebuf, 4, 10, 14);

  for (uchar i = 0; i < 4; i++) {
    for (uchar j = 0; j < 16; j++) {
      scChar(pgm_read_byte(&(entryChars[i][j])), j, 13 - i, (i == 0 && j == 15 ? 1 : 3));
    }
  }
  //scString("0123456789:ABCD<", 16, 0, 13);
  //scString("EFGHIJKLMNOPQRST", 16, 0, 12);
  //scString("UVWXYZabcdefghij", 16, 0, 11);
  //scString("klmnopqrstuvwxyz", 16, 0, 10);
  EHSScreenPrintEntries();
  scString("           ", 11, 0, MAX_HS_ENTRIES - hspos - 1);
  scColorString(16, 0, MAX_HS_ENTRIES - hspos - 1, 2);
  serial_println(palID(0, MAX_HS_ENTRIES - hspos - 1 + 1));
  serial_println(palID(0, MAX_HS_ENTRIES - hspos - 1));
  serial_println(palID(15, MAX_HS_ENTRIES - hspos - 1));
  ehscx = 0;
  ehscy = 0;
  ehscp = 0;
  for (uchar i = 0; i < 11; i++) {
    enterHSuname[i] = ' ';
  }
  enterHSuname[11] = '\0';

  for (uchar y = 15; y < 255; y--) {
    for (uchar x = 0; x < 16; x++) {
      serial_char(text[x][y]);
    }
  }
  moveCursor(0, 0, 0, 0);
}

void updateEnterHS() {
  uchar htu = 5;
  if (jxd_up == 1 || jxd == 1 && holdTime % htu == 0) {
    uchar newEhscx = min(15, ehscx + 1);
    moveCursor(ehscx, ehscy, newEhscx, ehscy);
    ehscx = newEhscx;
  } else if (jxd_up == 2 || jxd == 2 && holdTime % htu == 0) {
    uchar newEhscx = max(1, ehscx) - 1;
    moveCursor(ehscx, ehscy, newEhscx, ehscy);
    ehscx = newEhscx;
  }
  if (jyd_up == 3 || jyd == 3 && holdTime % htu == 0) {
    uchar newEhscy = max(1, ehscy) - 1;
    moveCursor(ehscx, ehscy, ehscx, newEhscy);
    ehscy = newEhscy;
  } else if (jyd_up == 4 || jyd == 4 && holdTime % htu == 0) {
    uchar newEhscy = min(3, ehscy + 1);
    moveCursor(ehscx, ehscy, ehscx, newEhscy);
    ehscy = newEhscy;
  }
  if (sw_up) {
    if (ehscp > 0 && ehscx == 15 && ehscy == 0) {
      ehscp--;
      scChar(' ', ehscp, MAX_HS_ENTRIES - hspos - 1);
      HSEntries[hspos].name[ehscp] = enterHSuname[ehscp] = ' ';
    }
    else if (ehscp < 11) {
      char theChar = pgm_read_byte(&(entryChars[ehscy][ehscx]));
      HSEntries[hspos].name[ehscp] = enterHSuname[ehscp] = theChar;
      scChar(theChar, ehscp, MAX_HS_ENTRIES - hspos - 1);
      ehscp++;
    }
    
    /*char buff[12];
    for (uchar i = 0; i < 11; i++) {
      buff[i] = HSEntries[hspos].name[i];
    }
    buff[11] = '\0';
    serial_println(buff);
    serial_println(enterHSuname);*/
  }
}

void clearEnterScreen() {
  scClear();
}

void initGameOverScreen() {
  newTrack(2);
  scString("GAME OVER", 9, 4, 8);
}

void clearGameOverScreen() {
  scClear();
}

void drawText() {
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
            uint rpx = getpx(tx, ty, px, py);
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

void drawOneActorInRange(Actor* a, uchar* buf, uchar xm, uchar xn, uchar ym, uchar yn) {
  bool db = 0;
  if (a->alive
      && a->x <= xn && a->x + spritesW(a->s) - 1 >= xm
      && a->y <= yn && a->y + spritesH(a->s) - 1 >= ym) {
    if (db) {
      serial_println("--DOA--");
      for (uchar y = ym; y <= yn; y++) {
        serial_print(y); serial_print("| ");
        for (uchar x = xn; x >= xm && x < 255; x--) {
          uint rpx = getspx(a->s, x - a->x, y - a->y);
          serial_print(x); serial_print(": ");
          serial_print(ccr(rpx)); serial_print(" ");
          serial_print(ccg(rpx)); serial_print(" ");
          serial_print(ccb(rpx)); serial_print(" ");
        }
        serial_println("");
      }
    }
    for (uchar y = ym; y <= yn; y++) {
      for (uchar x = xn; x >= xm && x < 255; x--) {
        uint rpx = getspx(a->s, x - a->x, y - a->y);
        if (!ccx(rpx)) {
          buf[(x - xm)*(yn - ym + 1)*3 + (y - ym)*3 + 0] = ccr(rpx);
          buf[(x - xm)*(yn - ym + 1)*3 + (y - ym)*3 + 1] = ccg(rpx);
          buf[(x - xm)*(yn - ym + 1)*3 + (y - ym)*3 + 2] = ccb(rpx);
        }
      }
    }
  }
}

void memsett(void* arg, uchar x, uint n) {
  for (uchar* i = (uchar*)arg; i < (uchar*)arg + n; i++) {
    *i = x;
  }
}

// assumes called from drawSprites, with accompanying conditions
void drawAllActorsInRange(uchar xm, uchar xn, uchar ym, uchar yn) {
  bool db = 0;
  uchar buf[xn - xm + 1][yn - ym + 1][3];
  memsett(buf, 0, sizeof(buf));

  drawOneActorInRange(&player, (uchar*)buf, xm, xn, ym, yn);
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    drawOneActorInRange(&bullets[i], (uchar*)buf, xm, xn, ym, yn);
  }
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    drawOneActorInRange(&enemies[i], (uchar*)buf, xm, xn, ym, yn);
  }
  // if (db) {
  //   serial_println("--DAAIR--");
  //   for (uchar y = ym; y <= yn; y++) {
  //     serial_print(y); serial_print("| ");
  //     for (uchar x = xn; x >= xm && x < 255; x--) {
  //       serial_print(x); serial_print(": ");
  //       serial_print(buf[x - xm][y - ym][2]); serial_print(" ");
  //       serial_print(buf[x - xm][y - ym][1]); serial_print(" ");
  //       serial_print(buf[x - xm][y - ym][0]); serial_print(" ");
  //     }
  //     serial_println("");
  //   }
  // }
  SREG &= 0x7F;
  spic4(CASET, 0, g2sx(xn), 0, g2sx(xm));
  spic4(RASET, 0, g2sy(ym), 0, g2sy(yn));
  spic(RAMWR);
  for (uchar y = ym; y <= yn; y++) {
    for (uchar x = xn; x >= xm && x < 255; x--) {
      spid(buf[x - xm][y - ym][2] << 2);
      spid(buf[x - xm][y - ym][1] << 2);
      spid(buf[x - xm][y - ym][0] << 2);
    }
  }
  SREG |= 0x80;
}

void drawActor(Actor* a, bool db = 0) {
  //bool db = 0;
  uchar w, h, xm, xn, ym, yn;
  bool flash = (a == &player) && playerInvuln && (BLINKDUR - playerInvuln - 1) % (2*BLINKSPEED) < BLINKSPEED;
  if (a->needsRedraw) {
    w = spritesW(a->s);
    h = spritesH(a->s);
    if (a->alive) {
      xm = a->x;
      xn = a->x + w - 1;
      ym = a->y;
      yn = a->y + h - 1;
      if (db) {
        serial_print(w); serial_print(" ");
        serial_print(h); serial_print(" ");
        serial_print(xm); serial_print(" ");
        serial_print(xn); serial_print(" ");
        serial_print(ym); serial_print(" ");
        serial_println(yn);
      }
      SREG &= 0x7F;
      spic4(CASET, 0, g2sx(xn), 0, g2sx(xm));
      spic4(RASET, 0, g2sy(ym), 0, g2sy(yn));
      spic(RAMWR);
      for (uchar y = ym; y <= yn; y++) {
        for (uchar x = xn; x >= xm && x < 255; x--) {
          uint rpx = flash ? cclear : getspx(a->s, x - a->x, y - a->y);
          // serial_print(x); serial_print("#");
          // serial_print(y); serial_print("#");
          // serial_print(ccx(rpx)); serial_print("#");
          // serial_print(ccr(rpx)); serial_print("#");
          // serial_print(ccg(rpx)); serial_print("#");
          // serial_println(ccb(rpx));
          spid(ccb(rpx) << 2);
          spid(ccg(rpx) << 2);
          spid(ccr(rpx) << 2);
        }
      }
      SREG |= 0x80;

      if (a->xp != a->x) {
        ym = a->yp;
        yn = a->yp + h - 1;
        if (a->xp < a->x) {
          xm = a->xp;
          xn = a->x - 1;
        } else {
          xm = a->x + w;
          xn = a->xp + w - 1;
        }
        if (xm < a->xp) xm = a->xp;
        if (xn > a->xp + w - 1) xn = a->xp + w - 1;
        if (ym < a->yp) ym = a->yp;
        if (yn > a->yp + h - 1) yn = a->yp + h - 1;

        if (db) {
          serial_print(xm); serial_print(" $ ");
          serial_print(xn); serial_print(" $ ");
          serial_print(ym); serial_print(" $ ");
          serial_println(yn);
        }
        drawAllActorsInRange(xm, xn, ym, yn);
      }
      if (a->yp != a->y) {
        if (a->xp < a->x) {
          xm = a->x;
          xn = a->xp + w - 1;
        } else {
          xm = a->xp;
          xn = a->x + w - 1;
        }
        if (a->yp < a->y) {
          ym = a->yp;
          yn = a->y - 1;
        } else {
          ym = a->y + h;
          yn = a->yp + h - 1;
        }
        if (xm < a->xp) xm = a->xp;
        if (xn > a->xp + w - 1) xn = a->xp + w - 1;
        if (ym < a->yp) ym = a->yp;
        if (yn > a->yp + h - 1) yn = a->yp + h - 1;
        
        if (db) {
          serial_print(xm); serial_print(" $ ");
          serial_print(xn); serial_print(" $ ");
          serial_print(ym); serial_print(" $ ");
          serial_println(yn);
        }
        drawAllActorsInRange(xm, xn, ym, yn);
      }
    } else {
      if (db) {
        serial_print(a->xp); serial_print(" $ ");
        serial_print(a->xp + w - 1); serial_print(" $ ");
        serial_print(a->yp); serial_print(" $ ");
        serial_println(a->yp + h - 1);
      }
      drawAllActorsInRange(a->xp, a->xp + w - 1, a->yp, a->yp + h - 1);
    }

    a->needsRedraw = 0;
  }
}

void drawSprites() {
  drawActor(&player);
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = bullets[i].na) {
    drawActor(&bullets[i]);
  }
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = enemies[i].na) {
    drawActor(&enemies[i]);
  }
}

void draw() {
  if (!gameNeedsClearing)
    drawText();
  if (gameActive || gameNeedsClearing) {
    drawSprites();
  }
}

// pop dead actors from the list only after having drawn them
void emptyGraveyard() {
  uchar ni;
  for (uchar i = bullets.actb; i < MAX_BULLETS; i = ni) {
    ni = bullets[i].na;
    if (!bullets[i].alive) {
      bullets.deleteActor(i);
    }
  }
  for (uchar i = enemies.actb; i < MAX_ENEMIES; i = ni) {
    ni = enemies[i].na;
    if (!enemies[i].alive) {
      enemies.deleteActor(i);
    }
  }
  if (gameNeedsClearing)
    gameNeedsClearing = 0;
}
