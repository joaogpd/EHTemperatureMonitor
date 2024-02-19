#ifndef UTIL_HAL_H
#define UTIL_HAL_H

#include <msp430.h>

#define STOP_WDT() WDTCTL = WDTPW | WDTHOLD
#define START_WDT() WDTCTL = WDTPW
#define CLEAR_LOCKLPM5() PM5CTL0 &= ~LOCKLPM5
#define ENABLE_GLOBAL_INTERRUPTS() __bis_SR_register(GIE)
#define DISABLE_GLOBAL_INTERRUPTS() __bic_SR_register(GIE)

#endif
