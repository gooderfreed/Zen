#ifndef OBJECT_INTERFACES_H
#define OBJECT_INTERFACES_H

#include "../primitives/primitives.h"
#include "../components/components.h"

/*
 * ObjectInterfaces - Interface for objects that can be interacted with
 * Provides access to all interfaces and capabilities
 */
typedef struct ObjectInterfaces {
    char *name;
    struct {
        bool can_hold_cards  : 1;
        bool have_buttons    : 1;
        bool is_drawable     : 1;
        bool is_interactable : 1;
        bool is_dynamic      : 1;
        bool is_positionable : 1;
        bool requires_update : 1;
    } capabilities;

    const CardHandler     *card_handler;
    const Interactable    *interactable;
    const Dynamic         *dynamic;
    Drawable              *drawable;
    ButtonHandler         *button_handler;
    PositionHandler       *position_handler;
    Updateable            *updateable;
} ObjectInterfaces;

// Get interfaces
static inline ObjectInterfaces *GET_INTERFACES(const void *object) {
    return (ObjectInterfaces*)(object);
}

#endif
