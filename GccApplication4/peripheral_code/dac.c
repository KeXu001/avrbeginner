/*
 * dac.c
 *
 * Created: 1/31/2020 8:38:08 PM
 *  Author: Kevin
 */ 

#include "./dac.h"

/*
 *  Without scaling, the converted 8-bit value could go from 0 to 255
 *  If we are using a 4.3V reference and only want the output to go to 3.3v,
 *   we would set DAC_MAXIMUM to 3.3/4.3*255 = 196
 *  However, since the chip is only being powered by 3.3V, the 4.3V reference
 *   is not valid so we don't need to scale
 */
#define DAC_MINIMUM     0
#define DAC_MAXIMUM     255

void setup_dac(void)
{
    VREF.CTRLA = (0x3 << VREF_DAC0REFSEL_gp);           // 4.3V reference
    VREF.CTRLB = (1 << VREF_DAC0REFEN_bp);              // enable reference

    DAC0.DATA = 0;                                      // reset to known value

    DAC0.CTRLA = (1 << DAC_RUNSTDBY_bp) |               // run standby
                 (1 << DAC_OUTEN_bp) |                  // enable output (to pin)
                 (1 << DAC_ENABLE_bp);                  // enable

    return;
}

void dac_convert(uint8_t val)
{
    /* 
     *  scale (0, 65536) to fit in (DAC_MINIMUM, DAC_MAXIMUM)
     */
    uint64_t scaled_val = val;
    scaled_val *= (DAC_MAXIMUM - DAC_MINIMUM);
    scaled_val /= 0xFF;
    scaled_val += DAC_MINIMUM;

    DAC0.DATA = (uint8_t)scaled_val;

    return;
}