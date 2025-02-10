
#include "../inc/core.h"

/*
 * Initialize frame timer
 * Initializes the frame timer structure
 */
FrameTimer init_frame_timer(void) {
    FrameTimer frame_counter = {0};

    clock_gettime(CLOCK_MONOTONIC, &frame_counter.frame_start);
    frame_counter.target_frame_time = 0;

    return frame_counter;
}

/*
 * Set target FPS
 * Sets the target frames per second for the frame timer
 */
void set_target_fps(FrameTimer *timer, int fps) {
    timer->target_frame_time = 1.0 / fps;
}

bool should_render_frame(FrameTimer *timer, TimeManager *tm) {
    double elapsed = (double)(tm->current_time.tv_sec - timer->frame_start.tv_sec) +
                     (double)(tm->current_time.tv_nsec - timer->frame_start.tv_nsec) / 1e9;

    if (elapsed < timer->target_frame_time) return false;

    timer->frame_start = tm->current_time;

    if (timer->stats) {
        double current_fps = 1.0 / elapsed;
        update_fps_stats(timer->stats, current_fps);
    }

    return true;
}
