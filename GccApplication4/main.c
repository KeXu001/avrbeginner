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

#define TCA_SINGLE_CTRLA_CLKSEL_Pos     1
#define TCA_SINGLE_CTRLA_ENABLE_Pos     0
#define TCA_SINGLE_INTCTRL_OVF_Pos      0
#define TCA_SINGLE_INTFLAGS_OVF_Pos     0

int main(void)
{
    /* PORT */
    PORTA.DIRSET = (1u << PORT3);
    PORTA.OUTSET = (1u << PORT3);

    /*  
     *  CLK_PER runs at 3.33MHz by default
     *   since CLKCTRL.MCLKCTRLB.PDIV is 0x8 (div 6) of CLK_MAIN (20MHz) by default
     *  this can be changed but requires going through Configuration Change Protection (Key=IOREG)
     */

    /* TCA (Timer/Counter Type A) */
    TCA0.SINGLE.PER = 0xCB74;                                       // 3,333,333 * 1/64 = 52084
    TCA0.SINGLE.INTCTRL = (1 << TCA_SINGLE_INTCTRL_OVF_Pos);        // enable overflow interrupts
    TCA0.SINGLE.CTRLA = (0x5 << TCA_SINGLE_CTRLA_CLKSEL_Pos) |      // div clk_per by 64
                        (1 << TCA_SINGLE_CTRLA_ENABLE_Pos);         // enable

    setup_i2c();

    /* interrupts */
    sei();  // enable global interrupts
    
    uint8_t gyrodata[6] = {0};
    bmg_read(0x68, 0x12, gyrodata, 6, 0);
    while(i2c_routine_done==0);

    while (1) 
    {
        
    }
}

ISR(TCA0_OVF_vect)
{
    PORTA.OUTTGL = (1u << PORT3);  // toggle LED
    TCA0.SINGLE.INTFLAGS = (1u << TCA_SINGLE_INTFLAGS_OVF_Pos);  // "cleared ... by writing a one to its bit location."
}