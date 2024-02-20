#include <spi_hal.h>

volatile uint8_t tx_buffer[MAX_TX_BUFFER_SIZE] = {};
volatile uint8_t tx_buf_position = 0;
volatile int32_t nbytes_send = 0;
volatile uint8_t* rx_buffer = NULL;
volatile uint8_t nbytes_receive = 0;
volatile spi_callback_t tx_complete_cb = NULL;
volatile spi_callback_t rx_complete_cb = NULL;

void _clear_interrupt_flags(void) {
    UCA3IFG &= ~UCRXIFG;
    UCA3IFG &= ~UCTXIFG;
}

void config_cs_spi(void) {
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz
    CSCTL2 = SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVS__1 | DIVM__1;     // set all dividers
    CSCTL0_H = 0;
}

void init_spi(bool master, ClockSource source, bool tx_interrupt, bool rx_interrupt) {
    UCA3CTLW0 |= UCSWRST;

    UCA3CTLW0 = UCSYNC | UCCKPL | UCMSB | source;

    if (master) {
        UCA3CTLW0 |= UCMST;
    } else {
        UCA3CTLW0 &= ~UCMST;
    }

    UCA3BR0 = 0x01; // divider at 1
    UCA3BR1 = 0;
    UCA3MCTLW = 0; // no modulation

    UCA3CTLW0 &= ~UCSWRST;

    if (rx_interrupt) {
        UCA3IE |= UCRXIE;
    }

    if (tx_interrupt) {
        UCA3IE |= UCTXIE;
    }

    _clear_interrupt_flags();
}

void set_tx_complete_callback_spi(spi_callback_t callback) {
    if (callback != NULL) {
        tx_complete_cb = callback;
    }
}

void set_rx_complete_callback_spi(spi_callback_t callback) {
    if (callback != NULL) {
        rx_complete_cb = callback;
    }
}

void send_data_spi_value(uint8_t data) {
//    nbytes_send = 0;
    UCA3TXBUF = data;

//    __bis_SR_register(LPM0_bits + GIE);

    while (UCA3STATW & UCBUSY);
}

void send_data_spi(uint8_t* data, int32_t nbytes) {
    if (nbytes == 0) return;
    // use a global pointer to prevent lifetime/scope woes
    memset((uint8_t*)tx_buffer, 0x00, nbytes);
    memcpy((uint8_t*)tx_buffer, data, nbytes);

    nbytes_send = nbytes;
    tx_buf_position = 0;

    nbytes_send--;
    tx_buf_position++;
    UCA3TXBUF = tx_buffer[tx_buf_position - 1];

    __bis_SR_register(LPM0_bits + GIE);

    while (nbytes_send > 0 || (UCA3STATW & UCBUSY));
}

void receive_data_spi(uint8_t* data, size_t nbytes) {
    rx_buffer = data;
    nbytes_receive = nbytes;
}

// If the user wishes to use their own ISR, this one needs to be commented out or removed.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A3_VECTOR))) USCI_A3_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA3IV, USCI_SPI_UCTXIFG)) {
      case USCI_NONE: break;
      case USCI_SPI_UCRXIFG:
            UCA3IFG &= ~UCRXIFG;

            if (nbytes_receive > 0) {
                *rx_buffer = UCA3RXBUF;
                rx_buffer++;
                nbytes_receive--;
            } else if (rx_complete_cb != NULL) {
                rx_complete_cb();
            }

            break;
      case USCI_SPI_UCTXIFG:
            UCA3IFG &= ~UCTXIFG;

            if (nbytes_send > 0) {
                nbytes_send--;
                tx_buf_position++;
                UCA3TXBUF = tx_buffer[tx_buf_position - 1];
//                __bis_SR_register(LPM0_bits + GIE);
            } else if (tx_complete_cb != NULL) {
                __low_power_mode_off_on_exit();
            }

            break;
      default: break;
  }
}
