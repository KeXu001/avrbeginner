/*
 * GccApplication4.c
 *
 * Created: 1/14/2020 11:46:36 AM
 * Author : Kevin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define TCA0_SINGLE_CTRLA_CLKSEL_Pos    1
#define TCA0_SINGLE_CTRLA_ENABLE_Pos    0
#define TCA0_SINGLE_INTCTRL_OVF_Pos     0
#define TCA0_SINGLE_INTFLAGS_OVF_Pos    0

int main(void)
{
    /* PORT */
    PORTA.DIRSET = (1u << PORT3);
    PORTA.OUTSET = (1u << PORT3);

    /*  
     *  CLK_PER runs at 3.33MHz by default
     *  since CLKCTRL.MCLKCTRLB.PDIV is 0x8 (div 6) of CLK_MAIN (20MHz) by default
     *  this can be changed but requires going through Configuration Change Protection (Key=IOREG)
     */

    /* TCA (Timer/Counter Type A) */
    TCA0.SINGLE.PER = 0xCB74;                                       // 3,333,333 * 1/64 = 52084
    TCA0.SINGLE.INTCTRL = (1u << TCA0_SINGLE_INTCTRL_OVF_Pos);      // enable overflow interrupts
    TCA0.SINGLE.CTRLA = (0x5 << TCA0_SINGLE_CTRLA_CLKSEL_Pos) |     // div clk_per by 64
                        (1u << TCA0_SINGLE_CTRLA_ENABLE_Pos);       // enable

    /* interrupts */
    sei();  // enable global interrupts

    while (1) 
    {
    }
}

ISR(TCA0_OVF_vect)
{
    PORTA.OUTTGL = (1u << PORT3);  // toggle LED
    TCA0.SINGLE.INTFLAGS = (1u << TCA0_SINGLE_INTFLAGS_OVF_Pos);  // "cleared ... by writing a one to its bit location."
}
