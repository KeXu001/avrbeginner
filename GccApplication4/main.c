/*
 * GccApplication4.c
 *
 * Created: 1/14/2020 11:46:36 AM
 * Author : Kevin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define TCA_SINGLE_CTRLA_CLKSEL_Pos     1
#define TCA_SINGLE_CTRLA_ENABLE_Pos     0
#define TCA_SINGLE_INTCTRL_OVF_Pos      0
#define TCA_SINGLE_INTFLAGS_OVF_Pos     0

#define TWI_CTRLA_SDASETUP_Pos          4
#define TWI_CTRLA_SDAHOLD_Pos           2
#define TWI_CTRLA_FMPEN_Pos             1
#define TWI_MCTRLA_RIEN_Pos             7
#define TWI_MCTRLA_WIEN_Pos             6
#define TWI_MCTRLA_ENABLE_Pos           0

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
    
    /* TWI (Two-Wire Interface e.g. I2C) */
    TWI0.CTRLA = (0 << TWI_CTRLA_SDASETUP_Pos) |                    // SDA setup time is 4 clock cycles
                 (0x3 << TWI_CTRLA_SDAHOLD_Pos) |                   // hold SDA after SCL 500ns
                 (0 << TWI_CTRLA_FMPEN_Pos);                        // fast mode plus disabled

    TWI0.MBAUD = 161;                                               // target 10kHz f_SCL , T_rise ~= 300ns --> 2*BAUD = (3333333/10000) - 10 - 1

    TWI0.MCTRLA = (1 << TWI_MCTRLA_RIEN_Pos) |                      // enable master read interrupt
                  (1 << TWI_MCTRLA_WIEN_Pos);                       // enable master write interrupt
    TWI0.MCTRLA |= (1 << TWI_MCTRLA_ENABLE_Pos);                    // enable

    /* interrupts */
    sei();  // enable global interrupts



    while (1) 
    {
    }
}

ISR(TCA0_OVF_vect)
{
    PORTA.OUTTGL = (1u << PORT3);  // toggle LED
    TCA0.SINGLE.INTFLAGS = (1u << TCA_SINGLE_INTFLAGS_OVF_Pos);  // "cleared ... by writing a one to its bit location."
}

ISR(TWI0_TWIM_vect)
{
    
}