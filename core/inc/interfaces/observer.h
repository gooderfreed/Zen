#ifndef OBSERVER_H
#define OBSERVER_H

#include "object_interfaces.h"

/*
 * Observer callback function type
 * Represents a observer function that is called when a signal is emitted.
 */
typedef void (*Observer_callback)(void *observer, void *data);



/*
 * Signal subscription structure
 * Represents a subscription to a specific signal with its callback handler.
 * Used to declare which signals an object wants to receive.
 */
typedef struct SignalSubscription {
    char *signal;                // signal name
    Observer_callback callback;  // callback function
} SignalSubscription;

/*
 * Signal subscription list structure
 * Represents a list of signal subscriptions.
 */
typedef struct SignalSubscriptionList {
    SignalSubscription subscription;  // signal subscription
    SignalSubscriptionList *next;     // next signal subscription
} SignalSubscriptionList;

/*
 * Observer interface
 * Represents an object that can listen to signals.
 * Contains a list of signal subscriptions that define which signals
 * the object is interested in and how to handle them.
 */
typedef struct Observer {
    void *observer;                         // observer object
    SignalSubscriptionList *subscriptions;  // array of signal listeners
} Observer;

/*
 * Signal listener structure
 * Runtime structure created by the core system that links
 * an observer object with its callback for a specific signal.
 * This is the actual connection point used during signal dispatch.
 */
typedef struct SignalListener {
    void *observer;              // observer object
    Observer_callback callback;  // callback function
} SignalListener;


typedef struct SignalListenerList SignalListenerList;

/*
 * Signal listener list structure
 * Represents a list of signal listeners.
 */
struct SignalListenerList {
    SignalListener *listener;     // listener object
    SignalListenerList *next;     // next listener
};

/*
 * Signal listeners collection
 * Groups all listeners for a specific signal.
 * Used by the core system to efficiently dispatch signals
 * to all interested observers.
 */
typedef struct SignalListeners {
    char *signal;                   // signal name
    SignalListenerList *listeners;  // array of listeners
} SignalListeners;


typedef struct SignalListenersList SignalListenersList;

/*
 * Signal listeners collection list
 * Represents a list of signal listeners.
 */
struct SignalListenersList {
    SignalListeners *listeners;    // listeners object
    SignalListenersList *next;     // next listeners
};


static inline Observer *OBSERVER_HANDLER(const void *object) {
    return GET_INTERFACES(object)->observer;
}

static inline bool IS_OBSERVER(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_observer;
}


#define NEW_OBSERVER(signal_name, callback_function)                                             \
    do {                                                                                         \
        SignalSubscriptionList *new_sub = arena_alloc(arena, sizeof(SignalSubscriptionList));    \
        new_sub->subscription.signal = signal_name;                                              \
        new_sub->subscription.callback = callback_function;                                      \
        new_sub->next = observer->subscriptions;                                                 \
        observer->subscriptions = new_sub;                                                       \
    } while (0)


#define OBSERVER_FULL(arena, object, observers)                                                    \
    do {                                                                                           \
        if (!IS_OBSERVER(object))                                                                  \
            GET_INTERFACES(object)->capabilities.is_observer = true;                               \
        Observer *observer = OBSERVER_HANDLER(object);                                             \
        if (observer == NULL) {                                                                    \
            GET_INTERFACES(object)->observer = (Observer *)arena_alloc(arena, sizeof(Observer));   \
            observer = OBSERVER_HANDLER(object);                                                   \
            observer->observer = object;                                                           \
        }                                                                                          \
        observers;                                                                                 \
    } while (0)

#define OBSERVER(observers) \
    OBSERVER_FULL(cur_arena, cur_object, observers)


static inline SignalListener *create_signal_listener(Arena *arena, void *observer, Observer_callback callback) {
    SignalListener *listener = arena_alloc(arena, sizeof(SignalListener));
                                    
    listener->observer = observer;
    listener->callback = callback;

    return listener;
}

static inline SignalListenerList *create_signal_listener_list(Arena *arena, SignalListener *listener) {
    SignalListenerList *listener_list = arena_alloc(arena, sizeof(SignalListenerList));

    listener_list->listener = listener;
    listener_list->next = NULL;

    return listener_list;
}

static inline SignalListenerList *add_signal_listener_to_list(Arena *arena, SignalListenerList *list, SignalListener *listener) {
    SignalListenerList *new_listener_list = create_signal_listener_list(arena, listener);

    new_listener_list->listener = listener;
    new_listener_list->next = list;

    return new_listener_list;
}


static inline SignalListeners *create_signal_listeners(Arena *arena, char *signal) {
    SignalListeners *listeners = arena_alloc(arena, sizeof(SignalListeners));

    listeners->signal = signal;
    listeners->listeners = NULL;

    return listeners;
}

static inline SignalListenersList *create_signal_listeners_list(Arena *arena, SignalListeners *listeners) {
    SignalListenersList *listeners_list = (SignalListenersList *)arena_alloc(arena, sizeof(SignalListenersList));

    listeners_list->listeners = listeners;
    listeners_list->next = NULL;

    return listeners_list;
}

static inline SignalListenersList *add_signal_listeners_to_list(Arena *arena, SignalListenersList *list, SignalListeners *listeners) {
    SignalListenersList *new_listeners_list = create_signal_listeners_list(arena, listeners);

    new_listeners_list->next = list;

    return new_listeners_list;
}

static inline SignalListenersList *add_subscription_to_listeners(Arena *arena, SignalListenersList *listeners, SignalSubscription subscription, void *object) {
    SignalListenersList *_listeners = listeners;

    while (_listeners) {
        SignalListeners *listeners_elem = _listeners->listeners;
        if (strcmp(listeners_elem->signal, subscription.signal) == 0) {
            SignalListener *listener = create_signal_listener(arena, object, subscription.callback);
            listeners_elem->listeners = add_signal_listener_to_list(arena, listeners_elem->listeners, listener);
            return listeners;
        }
        _listeners = _listeners->next;
    }


    SignalListeners *new_listeners = create_signal_listeners(arena, subscription.signal);
    SignalListener  *listener      = create_signal_listener(arena, object, subscription.callback);
    new_listeners->listeners = add_signal_listener_to_list(arena, new_listeners->listeners, listener);
    return add_signal_listeners_to_list(arena, listeners, new_listeners);
}

#endif
