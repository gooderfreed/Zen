#ifndef COMPONENTS_H
#define COMPONENTS_H

/*
 * Components - Components for the zen framework
 * Provides components for the zen framework
 */
#include "../primitives/primitives.h"

#define GET_MACRO_4(_1, _2, _3, _4, NAME, ...) NAME
#define GET_MACRO_3(_1, _2, _3, NAME, ...) NAME
#define GET_MACRO_2(_1, _2, NAME, ...) NAME

#include "coords.h"
#include "color.h"
#include "zen_arena/arena_alloc.h"
#include "container/container.h"
#include "screen/screen.h"
#include "cursor/cursor.h"
#include "map/map.h"
#include "time_manager/time_manager.h"

#endif
