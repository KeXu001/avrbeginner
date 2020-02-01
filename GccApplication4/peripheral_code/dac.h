/*
 * dac.h
 *
 * Created: 1/31/2020 8:36:44 PM
 *  Author: Kevin
 */ 


#ifndef PERIPHERAL_CODE_DAC_H_
#define PERIPHERAL_CODE_DAC_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void setup_dac(void);
void dac_convert(uint8_t val);

#endif /* PERIPHERAL_CODE_DAC_H_ */