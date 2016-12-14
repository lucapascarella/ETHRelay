
#include "I2CMaster.h"
#include "TCPIP Stack/Tick.h"

void InitI2C(void) {
    SSP1CON1bits.SSPEN = 0; // Disable MSSP1 module

    TRISCbits.TRISC3 = INPUT; // SCL
    TRISCbits.TRISC4 = INPUT; // SDA

    //SSP1ADD = 0x67; // clock = FOSC/(4 * (SSPxADD + 1)) // 100 kHz
    //SSP1ADD = 0x19; // clock = FOSC/(4 * (SSPxADD + 1)) // 400 kHz
    SSP1ADD = 0x09; // clock = FOSC/(4 * (SSPxADD + 1)) // 1000 kHz

    // SSP1STAT = 0x80; // slew rate dis, SMBUS disabled  
    SSP1STATbits.SMP = 1; // 1 = Slew rate control is disabled for Standard Speed mode (100 kHz and 1 MHz)
    SSP1STATbits.CKE = 0; // 0 = Disable SMBus-specific inputs

    SSP1CON2 = 0x00; // clear all control bits

    // Clear MSSP interrupt flag
    PIR1bits.SSP1IF = 0;

    //SSP1CON1 = 0x28; // enable module, I2C master SSP1ADD=baud rate
    SSP1CON1bits.WCOL = 0; // 0 = No collision
    SSP1CON1bits.SSPOV = 0; // 0 = No overflow
    SSP1CON1bits.SSPOV = 0; // 0 = No overflow
    SSP1CON1bits.SSPM = 0b1000; // 0b1000 = I2C Master mode, Clock = FOSC/(4 * (SSPADD + 1))
    SSP1CON1bits.SSPEN = 1; // 1 = Enables the serial port and configures the SDAx and SCLx pins as the serial port pins
}

#if defined (I2C_NORMAL_FUNCTIONS)

/*
 *  START I2C communication 
 */
void i2c_start(void) {
    SSP1CON2bits.SEN = 1; // START bit (cleared by hw in the end) 
    poll_if_tim1();
}

/*
 * RESTART I2C communication (change to 'reads') 
 */
void i2c_restart(void) {
    SSP1CON2bits.RSEN = 1; // REPEATED START bit (cleared by hw in the end) 
    poll_if_tim1();
}

/*
 *  STOP I2C communication 
 */
void i2c_stop(void) {
    SSP1CON2bits.PEN = 1; // STOP bit (cleared by hw in the end) 
    poll_if_tim1();
}

/*
 *  ACK=0 from MASTER 
 */
void i2c_ack(BOOL *ret) {
    SSP1CON2bits.ACKDT = 0; // set the MASTER ACK bit 
    SSP1CON2bits.ACKEN = 1; // enable MASTER ACK sequence  
    *ret = poll_if_tim1();
}

/*
 *  NACK=1 from MASTER 
 */
void i2c_nack(BOOL *ret) {
    SSP1CON2bits.ACKDT = 1; // set the MASTER NOACK bit  
    SSP1CON2bits.ACKEN = 1; // enable MASTER ACK sequence 
    *ret = poll_if_tim1();
}

#endif

BOOL i2c_wr(BYTE i2c_data) {
    SSP1BUF = i2c_data; // load char in data buffer ; start streaming
    poll_if_tim1(); // poll MSSP1IF with 1msec timeout  
    if (SSP1CON2bits.ACKSTAT) {
        // if NOACK from slave  
        return FALSE;
    } else {
        return TRUE;
    }
}
//.................................................................................                              
// reads a byte on I2C bus ; returns 'rtcc_buf'  

BYTE i2c_rd(void) {
    BYTE read;

    SSP1CON2bits.RCEN = 1; // enable I2C receive mode(RCEN=0 after 8cks by hw)
    poll_if_tim1(); // poll MSSP1IF with 1msec timeout 
    read = SSP1BUF;

    return read;
}

BOOL poll_if_tim1(void) { // poll 2 flags: MSSP1IF & T1

    DWORD t = TickGet();

    while (PIR1bits.SSP1IF == FALSE && (TickGet() - t < TICK_SECOND / 2ul)); // wait at least on flag to rise
    if (PIR1bits.SSP1IF == FALSE)
        return FALSE; // Error
    PIR1bits.SSP1IF = 0; // Clear MSSP interrupt flag
    return TRUE;
}
