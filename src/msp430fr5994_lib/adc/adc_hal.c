#include <adc_hal.h>

volatile int16_t temp_c_int = 0;
volatile int16_t temp_c_float = 0;
volatile float temp = 0.0;

void configure_adc(uint16_t resolution, uint16_t input_select) {
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;         // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                      // Use sampling timer
    ADC12CTL2 |= resolution;                   // 12-bit conversion results
    ADC12MCTL0 |= input_select | ADC12VRSEL_1; // A2 input select, VR+=VREF buffered, VR-=AVSS
    ADC12IER0 |= ADC12IE0;                     // Enable ADC conv complete interrupt, conversion result at MEM0
}

void set_vref(uint16_t voltage) {
    // By default, REFMSTR=1 => REFCTL is used to configure the internal reference
    while (REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
    REFCTL0 |= voltage | REFON;                // Select internal ref, internal Reference ON
    while (!(REFCTL0 & REFGENRDY));            // Wait for reference generator to settle
}

void start_sampling_adc(void) {
    ADC12CTL0 |= ADC12ENC | ADC12SC;        // Start sampling/conversion
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
  {
    case ADC12IV_ADC12IFG0:                 // Vector 12:  ADC12MEM0 Interrupt
      temp =(ADC12MEM0 / DIVIDER12BIT) * 1.2;
      temp_c_int = (int16_t)temp;
      temp_c_float = (int16_t)(temp * 100) % 100;
      break;
    default: break;
  }
}

