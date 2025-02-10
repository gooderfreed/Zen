#ifndef INTERFACES_PRIMITIVES_H
#define INTERFACES_PRIMITIVES_H

/*
 * InterfacesPrimitives - Primitives for interfaces
 * Provides primitives for interfaces
 * Do not include this file in your object
 */
typedef struct ObjectInterfaces ObjectInterfaces;

typedef struct Drawable           Drawable;
typedef struct CursorInteractable CursorInteractable;
typedef struct TickDependent      TickDependent;
typedef struct Dynamic            Dynamic;
typedef struct ButtonHandler      ButtonHandler;
typedef struct PositionHandler    PositionHandler;
typedef struct Updateable         Updateable;
typedef struct CardHandler        CardHandler;
typedef struct CoreDependent      CoreDependent;
typedef struct InputHandler       InputHandler;

#endif
