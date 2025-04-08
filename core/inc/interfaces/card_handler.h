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
    bool (*is_same_card)(const void *self, const Coords, const void *);
} CardHandler;

// Card Handler macros
static inline CardHandler *GET_CARD_HANDLER(const void *object) {
    return GET_INTERFACES(object)->card_handler;
}

static inline bool IS_CARD_HANDLER(const void *object) {
    return GET_INTERFACES(object)->capabilities.can_hold_cards;
}

static inline bool GET_CAN_GIVE_CARDS(const void *object) {
    return IS_CARD_HANDLER(object) && GET_CARD_HANDLER(object)->can_give_cards;
}

static inline bool GET_CAN_TAKE_CARDS(const void *object) {
    return IS_CARD_HANDLER(object) && GET_CARD_HANDLER(object)->can_take_cards;
}

static inline bool GET_CAN_PLACE_CARDS(const void *object, const Coords coords, const Container *container) {
    return IS_CARD_HANDLER(object) && GET_CARD_HANDLER(object)->can_place(object, coords, container);
}

static inline bool GET_IS_SAME_CARD(const void *object, const Coords coords, const void *card) {
    return IS_CARD_HANDLER(object) && GET_CARD_HANDLER(object)->is_same_card(object, coords, card);
}

static inline void SELECT_CARDS(void *object, const Coords coords, Container *container) {
    GET_CARD_HANDLER(object)->select_cards(object, coords, container);
}

static inline void GET_CARDS(void *object, Container *container) {
    GET_CARD_HANDLER(object)->get_cards(object, container);
}

static inline void PLACE_CARDS(void *object, const Coords coords, Container *container) {
    GET_CARD_HANDLER(object)->place_cards(object, coords, container);
}



#define CAN_GIVE_CARDS_FULL(object, _select_cards, _is_same_card, _get_cards)                                                          \
    do {                                                                                                                               \
        if (!GET_CAN_GIVE_CARDS(object))                                                                                               \
            GET_CARD_HANDLER(object)->can_give_cards = true;                                                                           \
        GET_CARD_HANDLER(object)->select_cards = _select_cards;                                                                        \
        GET_CARD_HANDLER(object)->is_same_card = _is_same_card;                                                                        \
        GET_CARD_HANDLER(object)->get_cards = _get_cards;                                                                              \
        if (!_select_cards)                                                                                                            \
            wprintf(L"Error in '%s': CardHandler interface is missing 'select_cards' function\n", GET_INTERFACES(object)->name);       \
        if (!_is_same_card)                                                                                                            \
            wprintf(L"Error in '%s': CardHandler interface is missing 'is_same_card' function\n", GET_INTERFACES(object)->name);       \
        if (!_get_cards)                                                                                                               \
            wprintf(L"Error in '%s': CardHandler interface is missing 'get_cards' function\n", GET_INTERFACES(object)->name);          \
    } while (0)

#define CAN_GIVE_CARDS(_select_cards, _is_same_card, _get_cards) \
    CAN_GIVE_CARDS_FULL(cur_object, _select_cards, _is_same_card, _get_cards)


#define CAN_TAKE_CARDS_FULL(object, _can_place, _place_cards)                                                                     \
    do {                                                                                                                          \
        if (!GET_CAN_TAKE_CARDS(object))                                                                                          \
            GET_CARD_HANDLER(object)->can_take_cards = true;                                                                      \
        GET_CARD_HANDLER(object)->can_place = _can_place;                                                                         \
        GET_CARD_HANDLER(object)->place_cards = _place_cards;                                                                     \
        if (!_can_place)                                                                                                          \
            wprintf(L"Error in '%s': CardHandler interface is missing 'can_place' function\n", GET_INTERFACES(object)->name);     \
        if (!_place_cards)                                                                                                        \
            wprintf(L"Error in '%s': CardHandler interface is missing 'place_cards' function\n", GET_INTERFACES(object)->name);   \
    } while (0)

#define CAN_TAKE_CARDS(_can_place, _place_cards) \
    CAN_TAKE_CARDS_FULL(cur_object, _can_place, _place_cards)


#define CARD_HANDLER_FULL(arena, object, properties)                                                                                 \
    do {                                                                                                                             \
        if (!IS_CARD_HANDLER(object))                                                                                                \
            GET_INTERFACES(object)->capabilities.can_hold_cards = true;                                                              \
        CardHandler *card_handler = GET_CARD_HANDLER(object);                                                                        \
        if (card_handler == NULL) {                                                                                                  \
            GET_INTERFACES(object)->card_handler = (CardHandler *)arena_alloc(arena, sizeof(CardHandler));                           \
            card_handler = GET_CARD_HANDLER(object);                                                                                 \
            card_handler->can_give_cards = false;                                                                                    \
            card_handler->can_take_cards = false;                                                                                    \
        }                                                                                                                            \
        properties;                                                                                                                  \
        if (card_handler->can_give_cards == false && card_handler->can_take_cards == false)                                          \
            wprintf(L"Error in '%s': CardHandler interface is declared but not implemented\n", GET_INTERFACES(object)->name);        \
    } while (0)

#define CARD_HANDLER(properties) \
    CARD_HANDLER_FULL(cur_arena, cur_object, properties)

#endif
