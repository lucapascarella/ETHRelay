/**
 * @brief           External Data EEPROM Access Routines
 * @file            xeeprom.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 **********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microctrller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-12-21, David Hosken (DH):
 *    - External EEPROM code completely redone. Put all I2C code in seperate file
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2002-05-20, Nilesh Rajbharti:
 *    - Original
 * 2008-02-02, JM Delprat: added 24LC1025 compatibility
 *********************************************************************/



#include "HardwareProfile.h"
#include "TCPIP Stack/TCPIP.h"
#include "I2CMaster.h"

#if defined(XEEPROM_CS_TRIS)

// SPI Serial EEPROM buffer size.  To enhance performance while
// cooperatively sharing the SPI bus with other peripherals, bytes
// read and written to the memory are locally buffered. Legal
// sizes are 1 to the EEPROM page size.
#define EEPROM_BUFFER_SIZE              (64)

// Must be the EEPROM write page size, or any binary power of 2 divisor.  If 
// using a smaller number, make sure it is at least EEPROM_BUFFER_SIZE big for 
// max performance.  Microchip 25LC256 uses 64 byte page size, 25LC1024 uses 
// 256 byte page size, so 64 is compatible with both.
#if defined (USE_EEPROM_25LC1024)
#define EEPROM_PAGE_SIZE				(128)
#else
#define EEPROM_PAGE_SIZE				(64)
#endif

static void DoWrite(void);

static DWORD EEPROMAddress;
static BYTE EEPROMBuffer[EEPROM_BUFFER_SIZE];
static BYTE vBytesInBuffer;

BYTE bank; //JMD24LC1025 B0 memory for XEEIsBusy

/**
 * Overview:        Initialize I2C module to communicate to serial
 *                  EEPROM.
 * @param baud    - SSPADD value for bit rate.
 */
void XEEInit(void) {
    //Open I2C port
    //InitI2C();
}

BYTE XEEClose(void) {
    return XEE_SUCCESS;
}

XEE_RESULT XEEBeginRead(DWORD address) {
    // Save the address and emptry the contents of our local buffer
    EEPROMAddress = address;
    vBytesInBuffer = 0;
    return XEE_SUCCESS;
}

unsigned char XEERead(void) {
    // Check if no more bytes are left in our local buffer
    if (vBytesInBuffer == 0u) {
        // Get a new set of bytes
        XEEReadArray(EEPROMAddress, EEPROMBuffer, EEPROM_BUFFER_SIZE);
        EEPROMAddress += EEPROM_BUFFER_SIZE;
        vBytesInBuffer = EEPROM_BUFFER_SIZE;
    }

    // Return a byte from our local buffer
    return EEPROMBuffer[EEPROM_BUFFER_SIZE - vBytesInBuffer--];
}

XEE_RESULT XEEEndRead(void) {
    return XEE_SUCCESS;
}

/**
 * Reads desired number of bytes in sequential mode. This function
 * performs all necessary steps and releases the bus when finished.
 *
 * @preCondition    XEEInit() is already called.
 *
 * @param ctrl   EEPROM ctrl and address code.
 * @param address   Address from where array is to be read
 * @param buffer    Caller supplied buffer to hold the data
 * @param length    Number of bytes to read.
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
XEE_RESULT XEEReadArray(DWORD address, BYTE *buffer, WORD length) {
    WORD i, lenTemp;
    BOOL ret;

    struct {
        BYTE ctrl;
        BYTE LSB;
        BYTE MSB;
    } tempAddress;

_complete:

    lenTemp = 0;
    // Select bank in accord to address value
#if defined (USE_EEPROM_25LC1024)

    if ((address < 0x10000)) {
        if ((address + length) >= 0x10000) {
            lenTemp = length;
            length = 0x10000 - address;
        }
    }

    bank = ((address >= 0x10000) ? 0x08 : 0x00);

#else
    bank = 0x08;
#endif

    // control sequence + B0 + A1 + A0 + Write operation
    tempAddress.ctrl = 0xA0 | bank | 0x06 | 0x00;
    tempAddress.LSB = address & 0xFF;
    tempAddress.MSB = (address >> 8) & 0xFF;

    // Wait for write to complete
    //while (XEEIsBusy());
_start:
    // start I2C communication
    i2c_start();
    // write DEVICE ADDR for RTCC WRITES
    if (i2c_wr(tempAddress.ctrl) == FALSE)
        goto _start;
    // write the register ADDRESS High
    if (i2c_wr(tempAddress.MSB) == FALSE)
        goto _start;
    // write the register ADDRESS Low
    if (i2c_wr(tempAddress.LSB) == FALSE)
        goto _start;
    // Send restart operation for read sequence
    i2c_restart();
    // send the DEVICE ADDRESS for RTCC READS.
    ret = i2c_wr(tempAddress.ctrl | 0x01);
    // Read size - 1 bytes with ACK
    length--;
    for (i = 0; i < length && ret == TRUE; i++) {
        *buffer++ = i2c_rd();
        i2c_ack(&ret);
    }
    // Read last byte with NACK
    *buffer++ = i2c_rd();
    i2c_nack(&ret);
    // stop I2C communication
    i2c_stop();

    if (lenTemp != 0) {
        length++;
        address = address + length;
        length = lenTemp - length;
        goto _complete;
    }

    return ret;
}

/**
 * Writes given value 'val' at current address. Current address is set
 * by XEEBeginWrite() and is incremented by every XEEWrite().
 *
 * This function does not initiate the write cycle; it simply loads given
 * value into internal page buffer. This function does not release the
 * I2C bus. User must close XEEEndWrite() after this function
 * is called to relase the I2C bus.
 *
 * @preCondition    XEEInit() && XEEBeginWrite() are already called.
 *
 * @param val       Value to be written
 *
 * @return          XEE_SUCCESS if successful other value if failed.
 */
XEE_RESULT XEEBeginWrite(DWORD address) {
    vBytesInBuffer = 0;
    EEPROMAddress = address;
    return XEE_SUCCESS;
}

void XEEWriteArray(BYTE *val, WORD wLen) {
    while (wLen--)
        XEEWrite(*val++);
}

XEE_RESULT XEEWrite(BYTE val) {
    EEPROMBuffer[vBytesInBuffer++] = val;
    if (vBytesInBuffer >= sizeof (EEPROMBuffer))
        DoWrite();
    else if ((((BYTE) EEPROMAddress + vBytesInBuffer) & (EEPROM_PAGE_SIZE - 1)) == 0u)
        DoWrite();
    return XEE_SUCCESS;
}

XEE_RESULT XEEEndWrite(void) {
    if (vBytesInBuffer)
        DoWrite();

    return XEE_SUCCESS;
}

static void DoWrite(void) {
    WORD i;
    BOOL ret;

    struct {
        BYTE ctrl;
        BYTE LSB;
        BYTE MSB;
    } tempAddress;

#if defined (USE_EEPROM_25LC1024)
    bank = ((EEPROMAddress >= 0x10000) ? 0x08 : 0x00);
#else
    bank = 0x08;
#endif

    // control sequence + B0 + A1 + A0 + Write operation
    tempAddress.ctrl = 0xA0 | bank | 0x06 | 0x00;
    tempAddress.LSB = EEPROMAddress & 0xFF;
    tempAddress.MSB = (EEPROMAddress >> 8) & 0xFF;

    // Wait for write to complete
    //while (XEEIsBusy());
_start2:
    // start I2C communication
    i2c_start();
    // write DEVICE ADDR for RTCC WRITES
    if (i2c_wr(tempAddress.ctrl) == FALSE)
        goto _start2;
    // write the register ADDRESS High
    if (i2c_wr(tempAddress.MSB) == FALSE)
        goto _start2;
    // write the register ADDRESS Low
    if (i2c_wr(tempAddress.LSB) == FALSE)
        goto _start2;

    ret = TRUE;
    for (i = 0; i < vBytesInBuffer && ret == TRUE; i++) {
        // Send the byte to write
        ret = i2c_wr(EEPROMBuffer[i]);
    }

    // stop I2C communication
    i2c_stop();

    // Update write address and clear write cache
    EEPROMAddress += vBytesInBuffer;
    vBytesInBuffer = 0;

    //    // Wait for write to complete
    //    while (XEEIsBusy());
}

/**
 * Requests ack from EEPROM.
 *
 * @preCondition    XEEInit() is already called.
 *
 * @param ctrl   EEPROM ctrl and address code.
 *
 * @return          XEE_READY if EEPROM is not busy <br>
 *                  XEE_BUSY if EEPROM is busy <br>
 *                  other value if failed.
 */
BOOL XEEIsBusy(void) {

    BOOL ret;

    struct {
        BYTE ctrl;
        BYTE LSB;
        BYTE MSB;
    } tempAddress;

#if defined (USE_EEPROM_25LC1024)
    bank = ((EEPROMAddress >= 0x10000) ? 0x08 : 0x00);
#else
    bank = 0x08;
#endif

    // control sequence + B0 + A1 + A0 + Write operation
    tempAddress.ctrl = 0xA0 | bank | 0x06 | 0x00;
    tempAddress.LSB = 0x00; // Do not used
    tempAddress.MSB = 0x00; // Do not used

    // start I2C communication
    i2c_start();
    // write DEVICE ADDR for RTCC WRITES
    if ((ret = i2c_wr(tempAddress.ctrl)) == FALSE)
        return XEE_BUSY;
    if ((ret = i2c_wr(tempAddress.MSB)) == FALSE)
        return XEE_BUSY;

    // stop I2C communication
    i2c_stop();

    return XEE_READY; // return with no error
}
#endif
