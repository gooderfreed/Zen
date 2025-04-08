#ifndef COORDS_H
#define COORDS_H

#include "components.h"

/*
 * Coords - Basic 3D coordinate structure
 * Used for positioning objects in the game|framework space
 */
typedef struct Coords {
    short y;
    short x;
    short z;
} Coords;

#define COORDS_1(_x)         ((Coords){.y = 0,  .x = _x, .z = 0})
#define COORDS_2(_x, _y)     ((Coords){.y = _y, .x = _x, .z = 0})
#define COORDS_3(_x, _y, _z) ((Coords){.y = _y, .x = _x, .z = _z})
#define COORDS(...) GET_MACRO_3(__VA_ARGS__, COORDS_3, COORDS_2, COORDS_1)(__VA_ARGS__)

#define AUTO_0(_)      ((Coords){.y = 0,  .x = 0,  .z = -1})
#define AUTO_1(_x)     ((Coords){.y = 0,  .x = _x, .z = -1})
#define AUTO_2(_x, _y) ((Coords){.y = _y, .x = _x, .z = -1})
#define AUTO(...) GET_MACRO_3(__VA_ARGS__, AUTO_2, AUTO_1, AUTO_0, )(__VA_ARGS__)

#endif
