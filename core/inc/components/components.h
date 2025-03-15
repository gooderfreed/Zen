#ifndef COMPONENTS_H
#define COMPONENTS_H

/*
 * Components - Components for the core
 * Provides components for the core
 * Include this file in your object to use components
 */
#include "../primitives/primitives.h"

#define GET_MACRO_3(_1, _2, _3, NAME, ...) NAME
#define GET_MACRO_2(_1, _2, NAME, ...) NAME

#include "arena_alloc.h"
#include "coords.h"
#include "color.h"
#include "container.h"
#include "screen.h"
#include "cursor.h"
#include "map.h"
#include "card.h"
#include "time_manager.h"
#include "fps_stats.h"
#include "tick_counter.h"
#include "frame_timer.h"

#endif
