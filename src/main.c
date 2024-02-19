#include <util.h>
#include <epaper_tasks.h>
#include <spi_hal.h>
#include <adc_hal.h>
#include <msp430.h>

void gpio_init(void) {
    // Setup P1.0 and P1.1 for debug
    P1DIR |= BIT0 | BIT1;
    P1OUT &= ~(BIT0 | BIT1);

    // Setup P1.2 for analog input
    P1SEL1 |= BIT2;
    P1SEL0 |= BIT2;

    // Setup USCIA3 SPI pins
    P6SEL0 |= BIT0 | BIT2;

    // Setup CS (P8.3) and D/C (P8.2) pins
    P8DIR |= BIT2 | BIT3;
    P8OUT |= BIT2 | BIT3;

    // Setup Busy pin
    P8DIR &= ~BIT1;
    P8REN |= BIT1;
    P8OUT |= BIT1;
    P8IES |= BIT1; // high-to-low interrupt transition
    P8IE &= ~BIT1; // disable interrupt
    P8IFG &= ~BIT1; // clear pending interrupts

    // Setup Reset pin
    P6DIR |= BIT3;
    P6OUT |= BIT3;

    CLEAR_LOCKLPM5();
}

void wake_up(void) {
    __bic_SR_register(LPM0_bits);
}

void spi_init(void) {
    config_cs_spi();
    init_spi(true, SMCLK, true, false);
    set_tx_complete_callback_spi(wake_up);
}

int main(void) {
    STOP_WDT();

    configure_adc(RES12BIT, P12);

    gpio_init();
    spi_init();
    tasks_init();

    set_vref(VREF12);

    ENABLE_GLOBAL_INTERRUPTS();

    DisplayData display_data[2];
    display_data[0].x = 24;
    display_data[0].y = 24;
    strcpy(display_data[0].text, "00,00c");
    display_data[0].scale = 2;
    display_data[0].scale_line = 0;

    display_data[1].x = 8;
    display_data[1].y = 160;
    strcpy(display_data[1].text, "00:00:00");
    display_data[1].scale = 2;
    display_data[1].scale_line = 0;

    set_display_data_arr(display_data, 2);

    postTask(app, ep_resetL, UNUSED);
    startVTimer(update_time, 16000, UNUSED);

    start_sampling_adc();

    while (1) {
        procTasks();
    }

    return 0;
}

