/*
 * Copyright 2024 Cheban Sergiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include "../inc/core.h"

static bool validate_drawable(const void *interface) {
    const Drawable *drawable = interface;
    if (!drawable) {
        wprintf(L"Error: Drawable interface is NULL\n");
        return false;
    }

    if (!drawable->print) {
        wprintf(L"Error: Drawable interface is missing print function\n");
        return false;
    }
    return true;
}

static bool validate_interactable(const void *interface) {
    const Interactable *interactable = interface;
    if (!interactable) {
        wprintf(L"Error: Interactable interface is NULL\n");
        return false;
    }

    if (!interactable->move) {
        wprintf(L"Error: Interactable interface is missing 'move' function\n");
        return false;
    }

    if (!interactable->place_cursor) {
        wprintf(L"Error: Interactable interface is missing 'place_cursor' function\n");
        return false;
    }

    return true;
}

static bool validate_card_handler(const void *interface) {
    const CardHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error: CardHandler interface is NULL\n");
        return false;
    }

    if (handler->can_give_cards) {
        if (!handler->select_cards) {
            wprintf(L"Error: Object declares can_give_cards but 'select_cards' function are missing\n");
            return false;
        }

        if (!handler->get_cards) {
            wprintf(L"Error: Object declares can_give_cards but 'get_cards' function are missing\n");
            return false;
        }

        if (!handler->is_same_card) {
            wprintf(L"Error: Object declares can_give_cards but 'is_same_card' function are missing\n");
            return false;
        }
    }

    if (handler->can_take_cards) {
        if (!handler->can_place) {
            wprintf(L"Error: Object declares can_take_cards but 'can_place' function are missing\n");
            return false;
        }

        if (!handler->place_cards) {
            wprintf(L"Error: Object declares can_take_cards but 'place_cards' function are missing\n");
            return false;
        }
    }
    return true;
}

static bool validate_button_handler(const void *interface) {
    const ButtonHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error: ButtonHandler interface is NULL\n");
        return false;
    }

    if (!handler->is_button_position) {
        wprintf(L"Error: ButtonHandler interface is missing 'is_button_position' function\n");
        return false;
    }

    if (!handler->handle_button) {
        wprintf(L"Error: ButtonHandler interface is missing 'handle_button' function\n");
        return false;
    }

    return true;
}

static bool validate_dynamic(const void *interface) {
    const Dynamic *dynamic = interface;
    if (!dynamic) {
        wprintf(L"Error: Dynamic interface is NULL\n");
        return false;
    }

    if (!dynamic->free) {
        wprintf(L"Error: Dynamic interface is missing 'free' function\n");
        return false;
    }

    return true;
}

bool validate_object_interfaces(const ObjectInterfaces *interfaces) {
    InterfaceValidator validators[] = {
        VALIDATOR(is_drawable,     drawable,       validate_drawable),
        VALIDATOR(is_interactable, interactable,   validate_interactable),
        VALIDATOR(can_hold_cards,  card_handler,   validate_card_handler),
        VALIDATOR(have_buttons,    button_handler, validate_button_handler),
        VALIDATOR(is_dynamic,      dynamic,        validate_dynamic)
    };

    for (size_t i = 0; i < sizeof(validators) / sizeof(validators[0]); i++) {
        const InterfaceValidator *v = &validators[i];
        if (v->capability_flag && !v->validate(v->interface)) {
            return false;
        }
    }

    return true;
} 
