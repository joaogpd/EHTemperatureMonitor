#include <epaper.h>

#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// LUT // 30 bytes
const char lut_len = 30;
uint8_t lut_full_update[] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

uint8_t lut_partial_update[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT8_VECTOR
__interrupt void PORT8_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT8_VECTOR))) PORT8_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (P8IFG | BIT1) {
        P8IE &= ~BIT1;  // Disable interrupts
        P8IFG &= ~BIT1; // Clear interrupt flag (maybe not necessary)
        __low_power_mode_off_on_exit();
    }
}

void WaitUntilIdle(void) {
    P8IE |= BIT1;  // Enable interrupt on busy pin
    __bis_SR_register(LPM3_bits + GIE);
}

void cfgDisp(void) {
    spiSendCmdChar(DRIVER_OUTPUT_CONTROL);
    spiSendDataChar((EPD_HEIGHT - 1));
    spiSendDataChar((EPD_HEIGHT - 1) >> 8);
    spiSendDataChar(0x00);
    spiSendCmdChar(BOOSTER_SOFT_START_CONTROL);
    spiSendDataChar(0xD7);
    spiSendDataChar(0xD6);
    spiSendDataChar(0x9D);
    spiSendCmdChar(WRITE_VCOM_REGISTER);
    spiSendDataChar(0xA8);
    spiSendCmdChar(SET_DUMMY_LINE_PERIOD);
    spiSendDataChar(0x1A);
    spiSendCmdChar(SET_GATE_LINE_WIDTH);
    spiSendDataChar(0x08);
    spiSendCmdChar(DATA_ENTRY_MODE_SETTING);
    spiSendDataChar(0x03);
}

void lutFull(void) {
    spiSendCmdChar(WRITE_LUT_REGISTER);
    spiSendDataStr(lut_full_update, lut_len);
}
void lutPartial(void) {
    spiSendCmdChar(WRITE_LUT_REGISTER);
    spiSendDataStr(lut_partial_update, lut_len);
}

void SetMemoryArea(int x_start, int y_start, int x_end, int y_end) {
    spiSendCmdChar(SET_RAM_X_ADDRESS_START_END_POSITION);
    spiSendDataChar((x_start >> 3) & 0xFF);
    spiSendDataChar((x_end >> 3) & 0xFF);
    spiSendCmdChar(SET_RAM_Y_ADDRESS_START_END_POSITION);
    spiSendDataChar(y_start & 0xFF);
    spiSendDataChar((y_start >> 8) & 0xFF);
    spiSendDataChar(y_end & 0xFF);
    spiSendDataChar((y_end >> 8) & 0xFF);
}

void SetMemoryPointer(int x, int y) {
    spiSendCmdChar(SET_RAM_X_ADDRESS_COUNTER);
    spiSendDataChar((x >> 3) & 0xFF);
    spiSendCmdChar(SET_RAM_Y_ADDRESS_COUNTER);
    spiSendDataChar(y & 0xFF);
    spiSendDataChar((y >> 8) & 0xFF);
}

void ClearPatternMemory(unsigned char color) {
    uint8_t x = 0;
    SetMemoryArea(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
    SetMemoryPointer(0, 0);
    spiSendCmdChar(WRITE_RAM);
    // send the color data
    for (uint16_t i = (EPD_WIDTH / 8 * EPD_HEIGHT); i ; i--) {
        spiSendDataChar(color);
        x++;
        if (x == 100) {
            x = 0;
        }
    }
    spiSendCmdChar(NOP);
}

void DisplayPattern(void) {
    spiSendDataStr((uint8_t*)buff, buff_len);
    spiSendCmdChar(DISPLAY_UPDATE_CONTROL_2);
    spiSendDataChar(0xC4);
    spiSendCmdChar(MASTER_ACTIVATION);
    spiSendCmdChar(NOP);
    WaitUntilIdle();
}

void dispSleep(void) {
   spiSendCmdChar(DEEP_SLEEP_MODE);
   spiSendDataChar(0x01);
}

