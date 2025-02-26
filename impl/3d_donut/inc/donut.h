#ifndef DONUT_H
#define DONUT_H

#include "../../../core/inc/core.h"

#define DONUT_HEIGHT 30
#define DONUT_WIDTH  30

#define BORDER_OFFSET_X 1
#define BORDER_OFFSET_Y 1

#ifndef PI
    #define PI 3.14159265359
#endif

#define SCREEN_HEIGHT (DONUT_HEIGHT + ((BORDER_OFFSET_Y) * 2))
#define SCREEN_WIDTH ((DONUT_WIDTH  + ((BORDER_OFFSET_X) * 2)) * 2)

typedef struct Donut {
    ObjectInterfaces interfaces;
    float A;
    float B;
} Donut;


#endif
