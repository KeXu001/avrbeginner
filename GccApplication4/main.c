/*
 * GccApplication4.c
 *
 * Created: 1/14/2020 11:46:36 AM
 * Author : Kevin
 */

 /*
 cd C:\Users\Kevin\Documents\pyupdi repo\pyupdi
 python pyupdi.py -c COM4 -d tiny412 -fs
 python pyupdi.py -c COM4 -d tiny412 -r
 python pyupdi.py -c COM4 -d tiny412 -f "C:\Users\Kevin\Documents\Atmel Studio\7.0\GccApplication4\GccApplication4\Release\GccApplication4.hex"
  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "peripheral_code/i2c.h"
#include "peripheral_code/dac.h"

#define TCA_SINGLE_CTRLA_CLKSEL_Pos     1
#define TCA_SINGLE_CTRLA_ENABLE_Pos     0
#define TCA_SINGLE_INTCTRL_OVF_Pos      0
#define TCA_SINGLE_INTFLAGS_OVF_Pos     0

volatile int tca_trigger = 0;

int main(void)
{
    /* PORT */
    PORTA.DIRSET = (1u << PORT3);
    PORTA.OUTSET = (1u << PORT3);

    // for DAC output
    PORTA.DIRSET = (1u << PORT6);

    /*  
     *  CLK_PER runs at 3.33MHz by default
     *   since CLKCTRL.MCLKCTRLB.PDIV is 0x8 (div 6) of CLK_MAIN (20MHz) by default
     *  this can be changed but requires going through Configuration Change Protection (Key=IOREG)
     */

    /* TCA (Timer/Counter Type A) */
    TCA0.SINGLE.PER = 0x0411;                                       // 1041 <-- 50Hz
    TCA0.SINGLE.INTCTRL = (1 << TCA_SINGLE_INTCTRL_OVF_Pos);        // disable overflow interrupts
    TCA0.SINGLE.CTRLA = (0x5 << TCA_SINGLE_CTRLA_CLKSEL_Pos) |      // div clk_per by 64
                        (1 << TCA_SINGLE_CTRLA_ENABLE_Pos);         // enable

    /* TWI */
    setup_i2c();

    /* DAC */
    setup_dac();

    /* interrupts */
    sei();  // enable global interrupts
    
    /*
     *  Set up BMG sensor
     *  By default:
     *      FIFO is disabled, not interrupts are enabled
     *      Offset is disabled
     *      Data output rate of 100Hz
     *      Suspend mode (no sampling, no fifo readout)
     */
    uint8_t pmumode = 0x15;  // set PMU mode of gyroscope to Normal
    bmg_write(0x68, 0x7E, &pmumode, 0);
    while(i2c_routine_done==0);

    while (1) 
    {
        if (tca_trigger)
        {
            /*
             *  read from bmg250
             */
            uint8_t gyrodata[6] = {0};
            bmg_read(0x68, 0x12, gyrodata, 6, 0);
            while(i2c_routine_done==0);

            /*
             *  the reading is signed (-32768, 32767) but we want it unsigned (0, 65536)
             */
            uint16_t gyrodatax = (gyrodata[0] | (gyrodata[1] << 8));

            // TODO: figure out why right-shifting by more than 4 causing problems
            dac_convert((uint8_t)((gyrodatax >> 4) + 128));
                        
            //PORTA.OUTTGL = (1u << PORT3);  // toggle LED
            tca_trigger = 0;
        }
    }
}

ISR(TCA0_OVF_vect)
{
    tca_trigger = 1;

    TCA0.SINGLE.INTFLAGS = (1u << TCA_SINGLE_INTFLAGS_OVF_Pos);  // "cleared ... by writing a one to its bit location."
}