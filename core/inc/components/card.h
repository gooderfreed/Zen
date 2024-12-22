#ifndef CARD_H
#define CARD_H

#include "components.h"

/*
 * Card - Base card structure that can be extended by implementations
 * Default implementation provides only coordinates
 */
#ifndef CARD_HEIGHT
    #define CARD_HEIGHT 1
#endif
#ifndef CARD_WIDTH
    #define CARD_WIDTH 1
#endif

#ifndef CARD_STRUCT
typedef struct Card {
    Coords coords;
} Card;
#endif

#endif