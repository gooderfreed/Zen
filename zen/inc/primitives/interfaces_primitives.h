#ifndef INTERFACES_PRIMITIVES_H
#define INTERFACES_PRIMITIVES_H

/*
 * InterfacesPrimitives - Primitives for interfaces
 * Provides primitives for interfaces
 * Do not include this file in your object
 */
typedef struct ObjectInterfaces ObjectInterfaces;

typedef struct Drawable           Drawable;
typedef struct Updateable         Updateable;
typedef struct CursorInteractable CursorInteractable;
typedef struct ButtonHandler      ButtonHandler;
typedef struct PositionHandler    PositionHandler;
typedef struct CardHandler        CardHandler;
typedef struct InputHandler       InputHandler;
typedef struct TickDependent      TickDependent;
typedef struct CoreDependent      CoreDependent;
typedef struct Dynamic            Dynamic;
typedef struct Observer           Observer;
typedef struct Emitter            Emitter;

typedef struct SignalListeners    SignalListeners;
typedef struct SignalSubscription SignalSubscription;
typedef struct SignalSubscriptionList SignalSubscriptionList;

#endif
