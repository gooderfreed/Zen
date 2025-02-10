
#include "../inc/core.h"

/*
 * Create FPS statistics
 * Allocates and initializes an FPS stats structure.
 */
FpsStats *create_fps_stats(Arena *arena) {
    FpsStats *stats = (FpsStats *)arena_alloc(arena, sizeof(FpsStats));
    
    *stats = (FpsStats) {
        .cur_fps = 0,
        .avg_fps = 0,
        .max_fps = 0,
        .min_fps = 0,
        .frame_count = 0
    };

    return stats;
}

/*
 * Update FPS statistics
 * Updates current, min, max, and average FPS values.
 */
void update_fps_stats(FpsStats *stats, double current_fps) {
    if (stats == NULL) return;

    if (stats->frame_count == 0) {
        stats->min_fps = stats->max_fps = current_fps;
    } else {
        stats->cur_fps = current_fps;
        stats->min_fps = (current_fps < stats->min_fps) ? current_fps : stats->min_fps;
        stats->max_fps = (current_fps > stats->max_fps) ? current_fps : stats->max_fps;
        stats->avg_fps = (stats->avg_fps * stats->frame_count + current_fps) / (stats->frame_count + 1);
    }
    stats->frame_count++;
}

/*
 * Draw FPS statistics
 * Displays FPS information on the screen if enabled.
 */
void draw_fps_stats(const FpsStats *stats, Screen *screen) {
    if (stats == NULL || !stats->draw_to_screen) return;

    char fps_buffer[100];

    // int result_len = snprintf(fps_buffer, sizeof(fps_buffer), "FPS: %.2f (min: %.2f, max: %.2f, avg: %.2f)",
    //                           stats->cur_fps, stats->min_fps, stats->max_fps, stats->avg_fps);
    // if (screen->width >= result_len) {
    //     insert_text(screen, 0, 0, fps_buffer, COLOR_RED, COLOR_BLACK);
    // }

    // int result_len = snprintf(fps_buffer, sizeof(fps_buffer), "FPS:%.2f mn:%.2f mx:%.2f avg:%.2f",
    //                           stats->cur_fps, stats->min_fps, stats->max_fps, stats->avg_fps);
    // if (screen->width >= result_len) {
    //     insert_text(screen, 0, 0, fps_buffer, COLOR_RED, COLOR_BLACK);
    // }

    // int result_len = snprintf(fps_buffer, sizeof(fps_buffer), "FPS:%.2f",
    //                           stats->cur_fps);
    // if (screen->width >= result_len) {
    //     insert_text(screen, 0, 0, fps_buffer, COLOR_RED, COLOR_BLACK);
    // }



    int result_len = snprintf(fps_buffer, sizeof(fps_buffer), "FPS:%.2f (min: %.2f, avg:%.2f)",
                              stats->cur_fps, stats->min_fps, stats->avg_fps);
    if (screen->width >= result_len) {
        insert_text(screen, 0, 0, fps_buffer, COLOR_BOLD, COLOR_BLACK);
    }
}

/*
 * Get minimum FPS
 * Returns the minimum recorded FPS.
 */
double get_min_fps(const FpsStats *stats) {
    return (stats != NULL) ? stats->min_fps : -1;
}

/*
 * Get maximum FPS
 * Returns the maximum recorded FPS.
 */
double get_max_fps(const FpsStats *stats) {
    return (stats != NULL) ? stats->max_fps : -1;
}

/*
 * Get average FPS
 * Returns the average FPS value.
 */
double get_avg_fps(const FpsStats *stats) {
    return (stats != NULL) ? stats->avg_fps : -1;
}
