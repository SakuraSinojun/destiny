

#include "dice.h"
#include <stdlib.h>
#include <time.h>

void dice_init(void)
{
    srand((unsigned int)time(NULL));
}

int dice(int faces, int times)
{
    int i;
    int d = 0;
    for (i=0; i<times; i++) {
        int t = rand() % faces + 1; 
        d += t;
    }
    return d;
}

