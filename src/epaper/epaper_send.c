#include <epaper_send.h>

void spiSendCmdChar(uint8_t c) {
    P8OUT &= ~BIT2; // Cmd: Clear P8.2 output
    P8OUT &= ~BIT3; // CS: Clear P8.3 output
    send_data_spi_value(c);
//    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupts
    P8OUT |= BIT3; // CS: Set P8.3 output
}

void spiSendDataStr(uint8_t* buff, size_t len) {
    P8OUT |= BIT2; // Data: Set P8.2 output
    P8OUT &= ~BIT3;  // CS: Clear P8.3 output
    send_data_spi(buff, sizeof(uint8_t) * len);
//    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupts
    P8OUT |= BIT3; // CS: Set P8.3 output
}

void spiSendDataChar(uint8_t c) {
    P8OUT |= BIT2; // Data: Set P8.2 output
    P8OUT &= ~BIT3;  // CS: Clear P8.3 output
    send_data_spi_value(c);
//    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupts
    P8OUT |= BIT3; // CS: Set P8.3 output
}
