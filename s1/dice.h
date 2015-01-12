

#pragma once


void dice_init(void);

int dice(int faces, int times);

#define dice4(x)    dice(4, x)
#define dice6(x)    dice(6, x)
#define dice20(x)   dice(20, x)
#define d20()       dice(20, 1)

