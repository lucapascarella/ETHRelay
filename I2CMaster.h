/* 
 * File:   I2CMaster.h
 * Author: Luca
 *
 * Created on 11 agosto 2016, 11.34
 */

#ifndef I2CMASTER_H
#define	I2CMASTER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"
    
void InitI2C(void);

void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
BOOL i2c_wr(BYTE i2c_data);
BYTE i2c_rd(void);
void i2c_ack(BOOL *ret);
void i2c_nack(BOOL *ret);

BOOL poll_if_tim1(void);

//#define I2C_NORMAL_FUNCTIONS

#if !defined (I2C_NORMAL_FUNCTIONS)
    #define i2c_start()             { SSP1CON2bits.SEN = 1; poll_if_tim1(); }
    #define i2c_restart()           { SSP1CON2bits.RSEN = 1; poll_if_tim1(); }
    #define i2c_stop()              { SSP1CON2bits.PEN = 1; poll_if_tim1(); }
    #define i2c_ack(ret)            { SSP1CON2bits.ACKDT = 0; SSP1CON2bits.ACKEN = 1; *ret = poll_if_tim1(); }
    #define i2c_nack(ret)           { SSP1CON2bits.ACKDT = 1; SSP1CON2bits.ACKEN = 1; *ret = poll_if_tim1(); }
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* I2CMASTER_H */

