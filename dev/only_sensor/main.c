#include <util.h>
#include <SchedulerVTimer.h>
#include <adc_hal.h>
#include <msp430.h>

#define UNUSED 0

void gpio_init(void) {
    // Setup P1.0 and P1.1 for debug
    P1DIR |= BIT0 | BIT1;
    P1OUT &= ~(BIT0 | BIT1);

    // Setup P1.2 for analog input
    P1SEL1 |= BIT2;
    P1SEL0 |= BIT2;

    CLEAR_LOCKLPM5();
}

void tasks_init(void) {
    setupTasks(vtimer, read_temperature);

    initSchedulerVTTimer();
}

void read_temperature(uint8_t unused) {
    start_sampling_adc();

    volatile uint8_t local_temp_c_int = temp_c_int; // inspect value more easily, but is unnecessary stack usage
    __no_operation(); // for debugging purposes

    startVTimer(read_temperature, 10000, UNUSED);
}

int main(void) {
    STOP_WDT();

    configure_adc(RES12BIT, P12);

    gpio_init();
    tasks_init();

    set_vref(VREF12);

    ENABLE_GLOBAL_INTERRUPTS();

    start_sampling_adc();

    startVTimer(read_temperature, 10000, UNUSED);

    while (1) {
        procTasks();
        // __bis_SR_register(LPM3_bits + GIE); // LPM4 may be possible, as ACLK is not stopped
    }

    return 0;
}

