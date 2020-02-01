/*
 * i2c.h
 *
 * Created: 1/31/2020 6:07:10 PM
 *  Author: Kevin
 */ 


#ifndef PERIPHERAL_CODE_I2C_H_
#define PERIPHERAL_CODE_I2C_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../misc_defs.h"

//#define TWI_CTRLA_SDASETUP_Pos TWI_SDASETUP_bp
//#define TWI_CTRLA_SDASETUP_Pos          4
//#define TWI_CTRLA_SDAHOLD_Pos           2
//#define TWI_CTRLA_FMPEN_Pos             1
//#define TWI_MCTRLA_RIEN_Pos             7
//#define TWI_MCTRLA_WIEN_Pos             6
//#define TWI_MCTRLA_QCEN_Pos             4
//#define TWI_MCTRLA_ENABLE_Pos           0
//#define TWI_MCTRLB_ACKACT_Pos           2
//#define TWI_MCTRLB_CMD_Pos              0
//#define TWI_MSTATUS_BUSSTATE_Msk        0x3
//#define TWI_MSTATUS_BUSSTATE_Pos        0

#define MASTER_WRITE 0x0
#define MASTER_READ 0x1

extern volatile int i2c_routine_done;

void setup_i2c(void);
void bmg_read(uint8_t slave_addr,               \
              uint8_t reg_addr,                 \
              uint8_t* data_ptr,                \
              uint32_t data_size,               \
              void_func_void_t callback);
void bmg_write(uint8_t slave_addr,              \
               uint8_t reg_addr,                \
               uint8_t* data_ptr,               \
               void_func_void_t callback);

#endif /* PERIPHERAL_CODE_I2C_H_ */