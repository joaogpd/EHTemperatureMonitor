#ifndef ADC_H
#define ADC_H

#include <msp430.h>
#include <stdint.h>

#define RES12BIT ADC12RES_2
#define RES10BIT ADC12RES_1
#define RES8BIT  ADC12RES_0

#define DIVIDER12BIT 40.95F
#define DIVIDER10BIT 10.23F
#define DIVIDER8BIT  2.55F

#define P12 ADC12INCH_2
#define P13 ADC12INCH_3

#define VREF12 REFVSEL_0 // 1.2V
#define VREF20 REFVSEL_1 // 2.0V

extern volatile int16_t temp_c_int;
extern volatile int16_t temp_c_float;

void configure_adc(uint16_t resolution, uint16_t input_select);
void set_vref(uint16_t voltage);
void start_sampling_adc(void);

#endif
