#ifndef EPAPER_SEND_H
#define EPAPER_SEND_H

#include <stdint.h>
#include <gpio_hal.h>
#include <spi_hal.h>
#include <msp430.h>

void spiSendCmdChar(uint8_t c);
void spiSendDataStr(uint8_t* buff, size_t len);
void spiSendDataChar(uint8_t c);

#endif
