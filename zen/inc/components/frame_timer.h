#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include "components.h"

/*
 * Frame timer structure
 * Measures frame start and end times to regulate FPS.
 */
typedef struct FrameTimer {
    struct timespec frame_start;   // Timestamp of frame start
    struct timespec frame_end;     // Timestamp of frame end
    double target_frame_time;      // Desired time per frame (for FPS control)
    FpsStats *stats;               // Pointer to FPS statistics
} FrameTimer;

FrameTimer init_frame_timer(void);
void set_target_fps(FrameTimer *timer, int fps);
bool should_render_frame(FrameTimer *timer, TimeManager *tm);

#endif
