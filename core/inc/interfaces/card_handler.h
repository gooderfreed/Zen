#ifndef CARD_HANDLER_H
#define CARD_HANDLER_H

#include "object_interfaces.h"

/*
 * CardHandler - Interface for objects that can work with cards
 * Provides full card manipulation capabilities
 */
typedef struct CardHandler {
    bool can_give_cards : 1;
    bool can_take_cards : 1;

    void (*select_cards)(void       *self, const Coords, Container *);
    void (*place_cards) (void       *self, const Coords, Container *);
    void (*get_cards)   (void       *self,         const Container *);

    bool (*can_place)   (const void *self, const Coords, const Container *);
    bool (*is_same_card)(const void *self, const Coords, const Card *);
} CardHandler;

// Card Handler macros
static inline const CardHandler *CARD_HANDLER(const void *object) {
    return GET_INTERFACES(object)->card_handler;
}

static inline bool IS_CARD_HANDLER(const void *object) {
    return GET_INTERFACES(object)->capabilities.can_hold_cards;
}

static inline bool CAN_GIVE_CARDS(const void *object) {
    return IS_CARD_HANDLER(object) && CARD_HANDLER(object)->can_give_cards;
}

static inline bool CAN_TAKE_CARDS(const void *object) {
    return IS_CARD_HANDLER(object) && CARD_HANDLER(object)->can_take_cards;
}

static inline bool CAN_PLACE_CARDS(const void *object, const Coords coords, const Container *container) {
    return IS_CARD_HANDLER(object) && CARD_HANDLER(object)->can_place(object, coords, container);
}

static inline bool IS_SAME_CARD(const void *object, const Coords coords, const Card *card) {
    return IS_CARD_HANDLER(object) && CARD_HANDLER(object)->is_same_card(object, coords, card);
}

static inline void SELECT_CARDS(void *object, const Coords coords, Container *container) {
    CARD_HANDLER(object)->select_cards(object, coords, container);
}

static inline void GET_CARDS(void *object, Container *container) {
    CARD_HANDLER(object)->get_cards(object, container);
}

static inline void PLACE_CARDS(void *object, const Coords coords, Container *container) {
    CARD_HANDLER(object)->place_cards(object, coords, container);
}

#endif
