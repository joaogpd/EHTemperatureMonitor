#ifndef EPAPER_H
#define EPAPER_H

#include <epaper_send.h>
#include <epaper_tasks.h>
#include <SchedulerVTimer.h>
#include <stdbool.h>
#include <msp430.h>

// List of EPaper commands.
enum epdCtl {
    DRIVER_OUTPUT_CONTROL                       = 0x01,
    BOOSTER_SOFT_START_CONTROL                  = 0x0C,
    GATE_SCAN_START_POSITION                    = 0x0F,
    DEEP_SLEEP_MODE                             = 0x10,
    DATA_ENTRY_MODE_SETTING                     = 0x11,
    SW_RESET                                    = 0x12,
    TEMPERATURE_SENSOR_CONTROL                  = 0x1A,
    MASTER_ACTIVATION                           = 0x20,
    DISPLAY_UPDATE_CONTROL_1                    = 0x21,
    DISPLAY_UPDATE_CONTROL_2                    = 0x22,
    WRITE_RAM                                   = 0x24,
    WRITE_VCOM_REGISTER                         = 0x2C,
    WRITE_LUT_REGISTER                          = 0x32,
    SET_DUMMY_LINE_PERIOD                       = 0x3A,
    SET_GATE_LINE_WIDTH                         = 0x3B,
    BORDER_WAVEFORM_CONTROL                     = 0x3C,
    SET_RAM_X_ADDRESS_START_END_POSITION        = 0x44,
    SET_RAM_Y_ADDRESS_START_END_POSITION        = 0x45,
    SET_RAM_X_ADDRESS_COUNTER                   = 0x4E,
    SET_RAM_Y_ADDRESS_COUNTER                   = 0x4F,
    NOP                                         = 0xFF
};

// Spinning on Busy pin, until interrupt flags it has been pulled high by EPaper.
void WaitUntilIdle(void);
// Initial EPaper display configuration.
void cfgDisp(void);
// Loads the full lookup table.
void lutFull(void);
// Loads partial lookup table.
void lutPartial(void);
// Sets the area in RAM for data to be sent.
void SetMemoryArea(int x_start, int y_start, int x_end, int y_end);
// Sets the RAM pointer to write data.
void SetMemoryPointer(int x, int y);
// Clears a memory buffer by filling it with blank characters. There are two
// buffers, so this needs to be called twice followed by DisplayPattern.
void ClearPatternMemory(unsigned char color);
// Updates the display so that contents in RAM are shown.
void DisplayPattern(void);
// Puts display in deep sleep mode.
void dispSleep(void);

#endif
