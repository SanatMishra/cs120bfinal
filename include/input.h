#ifndef _INPUTH_
#define _INPUTH_

#include "common.h"

// neutral x, y coordinates (will be set to first coordinate read in)
extern ushort nx, ny;

// bounds from neutral within which the input will be read as zero
inline constexpr ushort deadzoneX = 100;
inline constexpr ushort deadzoneY = 100;

// limits of the joystick in each direction - intentionally shallow to accommodate 
// poor joysticks, but range widens immediately after stick proceeds past 
// these bounds, so that we quickly get an accurate measure of stick coordinates 
extern ushort jfl, fjr, jfd, jfu;
//extern ushort jfu95, jfd95, jfr95, jfl95;

// input variables shared across all files
// sw_up only set to 1 when going up (i.e. previous value 1, new value 0), otherwise 0
// jxd: x direction as 0:neutral 1:right 2:left
// jyd: y direction as 0:neutral 3:up 4:down
// jcd: cardinal direction as 0:neutral 1:right 2:left 3:up 4:down
// j*d_up only set to 1 when going up (i.e. previous value not in range, current value in range), otherwise 0
extern ushort jx, jy;
extern uchar jcd, jcd_up,
       jxd, jxd_up, jyd, jyd_up, 
       sw, sw_up, bt, bt_up;

inline constexpr float maxSpeed = 0.6*WIDTH/(1000.0/UNIV_PERIOD); // 60% WIDTH
inline constexpr float minSpeed = 0.10*maxSpeed; // 10% maxSpeed
extern float jxi, jyi;

#endif