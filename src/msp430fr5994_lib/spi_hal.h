#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <msp430.h>
#include <util.h>

#define MAX_TX_BUFFER_SIZE 50

typedef void (*spi_callback_t)(void);

typedef enum {
    UCLK = UCSSEL__UCLK,
    ACLK = UCSSEL__ACLK, 
    SMCLK = UCSSEL__SMCLK
} ClockSource;

void config_cs_spi(void);
void init_spi(bool master, ClockSource source, bool tx_interrupt, bool rx_interrupt);
void set_tx_complete_callback_spi(spi_callback_t callback);
void set_rx_complete_callback_spi(spi_callback_t callback);
void send_data_spi_value(uint8_t data);
void send_data_spi(uint8_t* data, int32_t nbytes);
void receive_data_spi(uint8_t* data, size_t nbytes);

#endif
