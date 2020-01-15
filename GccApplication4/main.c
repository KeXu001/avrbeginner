/*
 * GccApplication4.c
 *
 * Created: 1/14/2020 11:46:36 AM
 * Author : Kevin
 */ 

#include <avr/io.h>


int main(void)
{
    PORTA.DIRSET = (1u << PORT3);
    PORTA.OUTSET = (1u << PORT3);

    /* Replace with your application code */
    while (1) 
    {
    }
}

