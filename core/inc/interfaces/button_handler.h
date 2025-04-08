#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "object_interfaces.h"


/*
 * Button - Button object
 * Contains a name, coordinates, context, and on_click function
 */
typedef struct {
    char *name;
    Coords coords;
    void *context;
    void (*on_click)(void *self, void *context);
} Button;

typedef struct ButtonList ButtonList;

/*
 * ButtonList - List of buttons
 * Contains a list of buttons
 */
struct ButtonList {
    Button *button;
    ButtonList *next;
};

/*
 * Direction - Enum for the direction of the buttons for auto positioning
 * HORIZONTAL - Horizontal direction flow
 * VERTICAL - Vertical direction flow
 */
typedef enum Direction {
    HORIZONTAL,
    VERTICAL
} Direction;

/*
 * ButtonGroup - Group of buttons
 * Contains a list of buttons and their position
 */
typedef struct ButtonGroup {
    Coords start_coords;
    Button **buttons;
    int length;
    Direction direction;
} ButtonGroup;

typedef struct ButtonGroupList ButtonGroupList;

/*
 * ButtonGroupList - List of button groups
 * Contains a list of button groups
 */
struct ButtonGroupList {
    ButtonGroup *button_group;
    ButtonGroupList *next;
};

/*
 * ButtonHandler - Interface for objects that can handle buttons
 * Provides button position and handling capabilities
 */
typedef struct ButtonHandler {
    ButtonGroup **button_groups;
    int button_groups_count;
} ButtonHandler;


void set_button_context(ButtonHandler *button_handler, char *name, void *context);
bool is_button(void *object, Coords coords);
Button *get_button_by_coords(void *object, Coords coords);
void handle_button(void *object, Coords coords);


// Button Handler macros
static inline ButtonHandler *GET_BUTTON_HANDLER(const void *object) {
    return GET_INTERFACES(object)->button_handler;
}

static inline bool IS_BUTTON_HANDLER(const void *object) {
    return GET_INTERFACES(object)->capabilities.have_buttons;
}

static inline bool HAVE_BUTTONS(const void *object) {
    return GET_INTERFACES(object)->capabilities.have_buttons;
}

// Button related macros
static inline void SET_BUTTON_CONTEXT(void *object, char *name, void *context) {
    set_button_context(GET_BUTTON_HANDLER(object), name, context);
}


#define BUTTON_FULL(arena, buttons_list, _params, _on_click)                                     \
    do {                                                                                         \
        ButtonList *new_button_list = (ButtonList *)arena_alloc(arena, sizeof(ButtonList));      \
        Button *new_button = (Button *)arena_alloc(arena, sizeof(Button));                       \
        new_button->on_click = _on_click;                                                        \
        new_button->name = #_on_click;                                                           \
        Coords coords = AUTO();                                                                  \
        void *context = NULL;                                                                    \
        _params;                                                                                 \
        if (coords.z < 0) {                                                                      \
            if (direction == HORIZONTAL) coords.x = base_coords.x++;                             \
            else coords.y = base_coords.y++;                                                     \
        } else {                                                                                 \
            base_coords = coords;                                                                \
        }                                                                                        \
        new_button->coords = coords;                                                             \
        new_button->context = context;                                                           \
        new_button_list->button = new_button;                                                    \
        new_button_list->next = buttons_list;                                                    \
        buttons_list = new_button_list;                                                          \
        button_count++;                                                                          \
    } while (0)

#define BUTTON_1(_on_click) \
    BUTTON_FULL(cur_arena, buttons_list, {}, _on_click)

#define BUTTON_2(_params, _on_click) \
    BUTTON_FULL(cur_arena, buttons_list, _params, _on_click)

#define BUTTON(...) GET_MACRO_2(__VA_ARGS__, BUTTON_2, BUTTON_1,)(__VA_ARGS__)





#define BUTTONS_GROUP_FULL(arena, button_group_list, _config, _buttons)                                          \
    do {                                                                                                         \
        ButtonList *buttons_list = NULL;                                                                         \
        Coords base_coords = COORDS(0, 0);                                                                       \
        Direction direction = VERTICAL;                                                                          \
        _config;                                                                                                 \
        Coords start_coords = base_coords;                                                                       \
        int button_count = 0;                                                                                    \
        _buttons;                                                                                                \
        if (button_count > 0) {                                                                                  \
            ButtonGroupList *new_group_list = (ButtonGroupList *)arena_alloc(arena, sizeof(ButtonGroupList));    \
            ButtonGroup *new_button_group = (ButtonGroup *)arena_alloc(arena, sizeof(ButtonGroup));              \
            new_button_group->start_coords = start_coords;                                                       \
            new_button_group->direction = direction;                                                             \
            new_button_group->length = button_count;                                                             \
            Button **buttons = (Button **)arena_alloc(arena, sizeof(Button *) * (size_t)button_count);           \
            new_button_group->buttons = buttons;                                                                 \
            for (int _i = button_count - 1; _i >= 0; _i--) {                                                     \
                ButtonList *button_list_elem = buttons_list;                                                     \
                new_button_group->buttons[_i] = button_list_elem->button;                                        \
                buttons_list = button_list_elem->next;                                                           \
                arena_free_block(button_list_elem);                                                              \
            }                                                                                                    \
            new_group_list->button_group = new_button_group;                                                     \
            new_group_list->next = button_group_list;                                                            \
            button_group_list = new_group_list;                                                                  \
            button_group_count++;                                                                                \
        }                                                                                                        \
    } while (0)

#define BUTTONS_GROUP_1(_buttons) \
    BUTTONS_GROUP_FULL(cur_arena, button_group_list, {}, _buttons)

#define BUTTONS_GROUP_2(_config, _buttons) \
    BUTTONS_GROUP_FULL(cur_arena, button_group_list, _config, _buttons)

#define BUTTONS_GROUP(...) GET_MACRO_2(__VA_ARGS__, BUTTONS_GROUP_2, BUTTONS_GROUP_1,)(__VA_ARGS__)





#define BUTTONS_HANDLER_FULL(arena, object, groups)                                                                                 \
    do {                                                                                                                            \
        ButtonGroupList *button_group_list = NULL;                                                                                  \
        int button_group_count = 0;                                                                                                 \
        groups;                                                                                                                     \
        if (button_group_count > 0) {                                                                                               \
            if (!IS_BUTTON_HANDLER(object))                                                                                         \
                GET_INTERFACES(object)->capabilities.have_buttons = true;                                                           \
            ButtonHandler *button_handler = GET_BUTTON_HANDLER(object);                                                             \
            if (button_handler == NULL) {                                                                                           \
                GET_INTERFACES(object)->button_handler = (ButtonHandler *)arena_alloc(arena, sizeof(ButtonHandler));                \
                button_handler = GET_BUTTON_HANDLER(object);                                                                        \
            }                                                                                                                       \
            button_handler->button_groups_count = button_group_count;                                                               \
            ButtonGroup **button_groups = (ButtonGroup **)arena_alloc(arena, sizeof(ButtonGroup *) * (size_t)button_group_count);   \
            button_handler->button_groups = button_groups;                                                                          \
            for (int _i = button_group_count - 1; _i >= 0; _i--) {                                                                  \
                ButtonGroupList *button_group_list_elem = button_group_list;                                                        \
                button_handler->button_groups[_i] = button_group_list_elem->button_group;                                           \
                button_group_list = button_group_list_elem->next;                                                                   \
                arena_free_block(button_group_list_elem);                                                                           \
            }                                                                                                                       \
        }                                                                                                                           \
    } while (0)

#define BUTTONS_HANDLER(groups) \
    BUTTONS_HANDLER_FULL(cur_arena, cur_object, groups)




#endif
