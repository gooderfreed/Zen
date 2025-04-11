#ifndef FPS_STATS_H
#define FPS_STATS_H

#include "../components.h"

/*
 * FPS statistics structure
 * Tracks frame rate metrics including min, max, average, and current FPS.
 */
typedef struct FpsStats {
    double min_fps;        // Minimum recorded FPS
    double max_fps;        // Maximum recorded FPS
    double avg_fps;        // Average FPS over time
    double cur_fps;        // Current frame rate
    int frame_count;       // Number of frames recorded
    bool draw_to_screen;   // Flag to indicate if FPS should be drawn on screen
} FpsStats;

FpsStats *create_fps_stats(Arena *arena);
void update_fps_stats(FpsStats *stats, double current_fps);
void draw_fps_stats(const FpsStats *stats, Screen *screen);
double get_min_fps(const FpsStats *stats);
double get_max_fps(const FpsStats *stats);
double get_avg_fps(const FpsStats *stats);

#endif
