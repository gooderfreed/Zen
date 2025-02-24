#ifndef TYPE_PRIMITIVES_H
#define TYPE_PRIMITIVES_H

/*
 * TypePrimitives - Primitives for types
 * Provides primitives for types
 * Do not include this file in your object
 */
// typedef unsigned char Color;
typedef unsigned char TextEffect;
typedef struct Color Color;
typedef struct Pixel Pixel;
typedef struct Screen Screen;

typedef struct Block Block;
typedef struct Arena Arena;

typedef struct Map Map;
typedef struct MapLayer MapLayer;
typedef struct MapObject MapObject;

typedef unsigned char CursorType;
typedef struct CursorConfig CursorConfig;
typedef struct Cursor Cursor;

typedef struct Coords Coords;

typedef struct Card Card;

typedef struct Container Container;

typedef struct TimeManager TimeManager;
typedef struct TickCounter TickCounter;
typedef struct FrameTimer FrameTimer;
typedef struct FpsStats FpsStats; 

typedef struct Core Core;

#endif
