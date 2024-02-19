#ifndef EPAPER_TASKS_H
#define EPAPER_TASKS_H

#include <cg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SchedulerVTimer.h>
#include <epaper.h>
#include <adc_hal.h>

#define UNUSED 0
#define MAX_BUFF_LEN 100

extern char buff[MAX_BUFF_LEN];
extern volatile uint8_t buff_len;

typedef struct display_data {
    uint16_t x;
    uint16_t y;
    uint8_t scale;
    uint8_t scale_line;
    char text[30];
} DisplayData;

// Initializes tasks, using SchedulerVTimer library.
void tasks_init(void);
// Posts next task to be executed, from a global variable.
void do_next_task(uint8_t unused);
void update_time(uint8_t unused);
// Pulls reset pin low.
void ep_resetL(uint8_t unused);
// Pulls reset pin high.
void ep_resetH(uint8_t unused);
void ep_initFull(uint8_t unused);
void ep_initPartial(uint8_t unused);
void ep_writeText(uint8_t unused);
// unknown purpose, maybe only update partially (untested hypothesis)
void ep_applyPartial(uint8_t unused);
void ep_sleep(uint8_t unused);
// Clears the memory buffer.
void ep_clearMem(uint8_t unused);
// Initializes the global variable that holds the DisplayData structures
// to be written in the display.
void set_display_data_arr(DisplayData* data_arr, size_t num_elements);

#endif
