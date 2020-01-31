/*
 * i2c.c
 *
 * Created: 1/31/2020 6:07:30 PM
 *  Author: Kevin
 */ 

#include "i2c.h"

/*
 *  This I2C module will have two routines, read and write
 *  When a routine is kicked off it will enter into
 *   a corresponding finite state machine,
 *   which will proceed until the sequence is complete,
 *   at which point it will trigger the callback function if one was provided
 */
enum I2C_States_e
{
    NOSTATE,
    READSTATE_PRE_ADDRESS,
    READSTATE_PRE_WRITE_REG_ADDR,
    READSTATE_PRE_REPEAT_ADDRESS,
    READSTATE_PRE_READ_VALUE,
    WRITESTATE_PRE_ADDRESS,
    WRITESTATE_PRE_WRITE_REG_ADDR,
    WRITESTATE_PRE_WRITE_VALUE,
    WRITESTATE_POST_WRITE_VALUE
};

static int _state;

#define IS_READ_STATE(state) ((state)>=READSTATE_PRE_ADDRESS && (state)<=READSTATE_PRE_READ_VALUE)
#define IS_WRITE_STATE(state) ((state)>=WRITESTATE_PRE_ADDRESS && (state)<=WRITESTATE_POST_WRITE_VALUE)
#define TWI0_BUSSTATE ((TWI0.MSTATUS & TWI_MSTATUS_BUSSTATE_Msk) >> TWI_MSTATUS_BUSSTATE_Pos)

static uint8_t _slave_addr;
static uint8_t _reg_addr;
static uint8_t* _data_ptr;
static uint32_t _data_size;
static void_func_void_t _callback;

volatile int i2c_routine_done = 0;

void bmg_read_routine(void)
{
    if (TWI0_BUSSTATE != 0x2)  // if not current owner of bus
    {
        TWI0.MCTRLB = (1 << TWI_MCTRLB_ACKACT_Pos) | (0x3 << TWI_MCTRLB_CMD_Pos);  // send NACK (to discontinue reading) followed by STOP condition

        _state = READSTATE_PRE_ADDRESS;
    }

    if (_state == READSTATE_PRE_ADDRESS)
    {
        TWI0.MADDR = ((_slave_addr & 0x7F) << 1) | MASTER_WRITE;

        _state++;
    }
    else if(_state == READSTATE_PRE_WRITE_REG_ADDR)
    {
        TWI0.MDATA = _reg_addr & 0xFF;

        _state++;
    }
    else if(_state == READSTATE_PRE_REPEAT_ADDRESS)
    {
        TWI0.MADDR = ((_slave_addr & 0x7F) << 1) | MASTER_READ;

        _state++;
    }
    else if(_state == READSTATE_PRE_READ_VALUE)
    {
        *_data_ptr = TWI0.MDATA;
        _data_ptr++;
        _data_size--;

        if (_data_size == 0)
        {
            TWI0.MCTRLB = (1 << TWI_MCTRLB_ACKACT_Pos) | (0x3 << TWI_MCTRLB_CMD_Pos);  // send NACK (to discontinue reading) followed by STOP condition
            
            i2c_routine_done = 1;
            _state = NOSTATE;

            if (_callback)
            {
                _callback();
            }
        }
        else
        {
            TWI0.MCTRLB = (0 << TWI_MCTRLB_ACKACT_Pos) | (0x2 << TWI_MCTRLB_CMD_Pos);  // send ACK (to continue reading) followed by a byte read operation
        }
    }
}

void bmg_write_routine(void)
{
    if (TWI0_BUSSTATE != 0x2)  // if not current owner of bus
    {
        TWI0.MCTRLB = (1 << TWI_MCTRLB_ACKACT_Pos) | (0x3 << TWI_MCTRLB_CMD_Pos);  // send NACK (to discontinue reading) followed by STOP condition
        
        _state = WRITESTATE_PRE_ADDRESS;
    }

    if (_state == WRITESTATE_PRE_ADDRESS)
    {
        TWI0.MADDR = ((_slave_addr & 0x7F) << 1) | MASTER_WRITE;

        _state++;
    }
    else if(_state == WRITESTATE_PRE_WRITE_REG_ADDR)
    {
        TWI0.MDATA = _reg_addr & 0xFF;

        _state++;
    }
    else if(_state == WRITESTATE_PRE_WRITE_VALUE)
    {
        TWI0.MDATA = (*_data_ptr) & 0xFF;

        _state++;
    }
    else if(_state == WRITESTATE_POST_WRITE_VALUE)
    {
        TWI0.MCTRLB = (1 << TWI_MCTRLB_ACKACT_Pos) | (0x3 << TWI_MCTRLB_CMD_Pos);  // send NACK (to discontinue reading) followed by STOP condition
        
        i2c_routine_done = 1;
        _state = NOSTATE;

        if (_callback)
        {
            _callback();
        }
    }
}

void setup_i2c(void)
{
    /* port */
    // TWI pins should be automatically configured when enabling the TWI

    /* twi */
    TWI0.CTRLA = (0 << TWI_CTRLA_SDASETUP_Pos) |                    // SDA setup time is 4 clock cycles
                 (0x3 << TWI_CTRLA_SDAHOLD_Pos) |                   // hold SDA after SCL 500ns
                 (0 << TWI_CTRLA_FMPEN_Pos);                        // fast mode plus disabled

    TWI0.MBAUD = 161;                                               // target 10kHz f_SCL , T_rise ~= 300ns --> 2*BAUD = (3333333/10000) - 10 - 1

    TWI0.MCTRLA = (1 << TWI_MCTRLA_RIEN_Pos) |                      // enable master read interrupt
                  (1 << TWI_MCTRLA_WIEN_Pos);                       // enable master write interrupt

    TWI0.MSTATUS = (0x1 << TWI_MSTATUS_BUSSTATE_Pos);               // force from unknown state (on startup) to idle

    TWI0.MCTRLA |= (1 << TWI_MCTRLA_ENABLE_Pos);                    // enable
    
    return;
}

void bmg_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* data_ptr, uint32_t data_size, void_func_void_t callback)
{
    _slave_addr = slave_addr;
    _reg_addr = reg_addr;
    _data_ptr = data_ptr;
    _data_size = data_size;
    _callback = callback;
    _state = READSTATE_PRE_ADDRESS;

    i2c_routine_done = 0;

    bmg_read_routine();
}

void bmg_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t* data_ptr, void_func_void_t callback)
{
    _slave_addr = slave_addr;
    _reg_addr = reg_addr;
    _data_ptr = data_ptr;
    _callback = callback;
    _state = WRITESTATE_PRE_ADDRESS;

    i2c_routine_done = 0;

    bmg_write_routine();
}

ISR(TWI0_TWIM_vect)
{
    PORTA.OUTTGL = (1u << PORT3);
    if IS_READ_STATE(_state)
    {
        bmg_read_routine();
    }
    else if IS_WRITE_STATE(_state)
    {
        bmg_write_routine();
    }
}