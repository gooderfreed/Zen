#ifndef EMITTER_H
#define EMITTER_H

#include "object_interfaces.h"

/*
 * Signal emission structure
 * Represents a declaration of intent to emit a specific signal to a target.
 * Used to define which signals an object can emit and to whom.
 */
typedef struct SignalEmission {
    char *signal;  // signal name
    char *target;  // target name
} SignalEmission;


typedef struct SignalEmissionList SignalEmissionList;

/*
 * Signal emission list structure
 * Represents a list of signal emissions.
 */
struct SignalEmissionList {
    SignalEmission emission;  // signal emission
    SignalEmissionList *next;  // next signal emission
};

/*
 * Emitter interface
 * Represents an object that can emit signals.
 * Contains declarations of which signals it can emit and to whom,
 * as well as the runtime connections to actual listeners.
 */
typedef struct Emitter {
    SignalEmissionList *signals;  // array of signal names
    SignalListenersList *observers;  // array of observers
} Emitter;


typedef struct EmitterList EmitterList;

/*
 * Emitter list structure
 * Represents a list of emitters.
 */
struct EmitterList {
    Emitter *emitter;
    EmitterList *next;
};


static inline Emitter *EMITTER_HANDLER(const void *object) {
    return GET_INTERFACES(object)->emitter;
}

static inline bool IS_EMITTER(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_emitter;
}


#define NEW_EMITTER_DIRECT(signal_name, target_name)                                          \
    do {                                                                                      \
        SignalEmissionList *new_emission = arena_alloc(arena, sizeof(SignalEmissionList));    \
        new_emission->emission.signal = signal_name;                                          \
        new_emission->emission.target = target_name;                                          \
        new_emission->next = emitter->signals;                                                \
        emitter->signals = new_emission;                                                      \
    } while (0)


#define NEW_EMITTER(signal_name)                                                              \
    do {                                                                                      \
        SignalEmissionList *new_emission = arena_alloc(arena, sizeof(SignalEmissionList));    \
        new_emission->emission.signal = signal_name;                                          \
        new_emission->next = emitter->signals;                                                \
        emitter->signals = new_emission;                                                      \
    } while (0)


#define EMITTER_FULL(arena, object, emitters)                                                                                                \
    do {                                                                                                                                     \
        if (!IS_EMITTER(object))                                                                                                             \
            GET_INTERFACES(object)->capabilities.is_emitter = true;                                                                          \
        Emitter *emitter = EMITTER_HANDLER(object);                                                                                          \
        if (emitter == NULL) {                                                                                                               \
            GET_INTERFACES(object)->emitter = (Emitter *)arena_alloc(arena, sizeof(Emitter));                                                \
            emitter = EMITTER_HANDLER(object);                                                                                               \
        }                                                                                                                                    \
        emitters;                                                                                                                            \
        if (!emitter->signals)                                                                                                               \
            wprintf(L"Error in '%s': Emitter declaration is missing while emitter interface is used\n", GET_INTERFACES(object)->name);       \
    } while (0)

#define EMITTER(emitters) \
    EMITTER_FULL(cur_arena, cur_object, emitters)


static inline void link_observers_to_emitters(Arena *arena, SignalListenersList *listeners, EmitterList *emitters) {
    while (emitters) {
        Emitter *emitter = emitters->emitter;
        EmitterList *next = emitters->next;

        while (emitter->signals) {
            SignalEmissionList *signal_emission = emitter->signals;

            while (signal_emission) {
                SignalListenersList *_listeners = listeners;

                while (_listeners) {
                    SignalListeners *listeners_elem = _listeners->listeners;

                    if ((listeners_elem->signal[0] == signal_emission->emission.signal[0])
                        && strcmp(listeners_elem->signal, signal_emission->emission.signal) == 0) {
                        
                        emitter->observers = add_signal_listeners_to_list(arena, emitter->observers, listeners_elem);
                        break;
                    }

                    _listeners = _listeners->next;
                }

                signal_emission = signal_emission->next;
            }

            emitter->signals = emitter->signals->next;
        }

        arena_free_block(emitters);
        emitters = next;
    }
}

static inline void emit_signal(void *object, char *signal, void *data) {
    Emitter *emitter = EMITTER_HANDLER(object);
    if (!emitter) return;
    SignalListenersList *listeners = emitter->observers;

    while (listeners) {
        SignalListeners *listeners_elem = listeners->listeners;

        if ((listeners_elem->signal[0] == signal[0])   // if you think that stupid -- read about cpu prediction, silly
            && strcmp(listeners_elem->signal, signal) == 0) {
            SignalListenerList *listener_list = listeners_elem->listeners;

            while (listener_list) {
                SignalListener *listener = listener_list->listener;

                listener->callback(listener->observer, data);
                listener_list = listener_list->next;
            }
            break;
        }

        listeners = listeners->next;
    }
}

#endif
