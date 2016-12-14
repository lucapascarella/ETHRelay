/* 
 * File:   MCP79402.h
 * Author: Luca
 *
 * Created on 11 agosto 2016, 11.36
 */

#ifndef MCP79402_H
#define	MCP79402_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "I2CMaster.h"

//#define EXT_RTCC_MCP79400_DEVICE
//#define EXT_RTCC_MCP79401_DEVICE
#define EXT_RTCC_MCP79402_DEVICE
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                    GLOBAL CONSTANTS RTCC - ADDRESSES 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #define  ADDR_EEPROM_WRITE 0xAE       //  DEVICE ADDR for EEPROM (writes)   
    #define  ADDR_EEPROM_READ  0xAF       //  DEVICE ADDR for EEPROM (reads)  
    #define  ADDR_RTCC_WRITE   0xDE       //  DEVICE ADDR for RTCC   (writes) 
    #define  ADDR_RTCC_READ    0xDF       //  DEVICE ADDR for RTCC   (reads)

    #define  ADDR_RTCC         0xDE       //  DEVIcE ADDR on 8 bit ()
    #define  ADDR_EEPROM       0xAE       //  DEVIcE ADDR on 8 bit ()
//.............................................................................  
    #define  SRAM_PTR          0x20       //  pointer of the SRAM area (RTCC) 
    #define  EEPROM_PTR        0xF0       //  EEPROM address
    #define  ADDR_EEPROM_SR    0xFF       //  STATUS REGISTER in the  EEPROM
//.............................................................................
    #define  RTCSEC            0x00       //  address of SECONDS      register 
    #define  RTCMIN            0x01       //  address of MINUTES      register 
    #define  RTCHOUR           0x02       //  address of HOURS        register 
    #define  RTCWKDAY          0x03       //  address of DAY OF WK    register 
    #define  RTCDATE           0x04       //  address of DATE         register  
    #define  RTCMTH            0x05       //  address of MONTH        register 
    #define  RTCYEAR           0x06       //  address of YEAR         register 
    #define  CONTROL           0x07       //  address of CONTROL      register 
    #define  OSCTRIM           0x08       //  address of CALIB        register 
    #define  EEUNLOCK          0x09       //  address of UNLOCK ID    register
//..............................................................................
    #define  ALM0SEC           0x0A       //  address of ALARMO SEC   register 
    #define  ALM0MIN           0x0B       //  address of ALARMO MIN   register 
    #define  ALM0HOUR          0x0C       //  address of ALARMO HOUR  register 
    #define  ALM0WKDAY         0x0D      //  address of ALARM0 DAY   register
    #define  ALM0DATE          0x0E       //  address of ALARMO DATE  register 
    #define  ALM0MTH           0x0F       //  address of ALARMO MONTH register 
//.............................................................................. 
    #define  ALM1SEC           0x11       //  address of ALARM1 SEC   register 
    #define  ALM1MIN           0x12       //  address of ALARM1 MIN   register 
    #define  ALM1HOUR          0x13       //  address of ALARM1 HOUR  register 
    #define  ALM1WKDAY         0x14       //  address of ALARM1 CONTR register
    #define  ALM1DATE          0x15       //  address of ALARM1 DATE  register 
    #define  ALM1MTH           0x16       //  address of ALARM1 MONTH register 
//..............................................................................        
    #define  PWRDNMIN          0x18       //  address of T_SAVER MIN(VDD->BAT)
    #define  PWRDNHOUR         0x19       //  address of T_SAVER HR (VDD->BAT) 
    #define  PWRDNDATE         0x1A       //  address of T_SAVER DAT(VDD->BAT) 
    #define  PWRDNMTH          0x1B       //  address of T_SAVER MTH(VDD->BAT) 
//..............................................................................
    #define  PWRUPMIN          0x1C       //  address of T_SAVER MIN(BAT->VDD)
    #define  PWRUPHOUR         0x1D       //  address of T_SAVER HR (BAT->VDD) 
    #define  PWRUPDATE         0x1E       //  address of T_SAVER DAT(BAT->VDD) 
    #define  PWRUPMTH          0x1F       //  address of T_SAVER MTH(BAT->VDD)      


typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE SECONE : 4;    // Binary-Coded Decimal Value of Second's Ones Digit (LSB)
        BYTE SECTEN : 3;    // Binary-Coded Decimal Value of Second's Tens Digit
        BYTE ST : 1;        // Start Oscillator bit  (MSB)
    } bits;
} EXT_RTCC_RTCSEC;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE MINONE : 4;    // Binary-Coded Decimal Value of Minute's Ones Digit (LSB)
        BYTE MINTEN : 3;    // Binary-Coded Decimal Value of Minute's Tens Digit
        BYTE reserved : 4;  // (MSB)
    } bits;
} EXT_RTCC_RTCMIN;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE HRONE : 4;     // Binary-Coded Decimal Value of Hour's Ones Digit (LSB)
        BYTE HRTEN : 2;     // Binary-Coded Decimal Value of Hour's Tens Digit
        BYTE sel12_24 : 1;  // 12 or 25 Hour Time Format bit
        BYTE reserved : 1;  // (MSB)
    } bits;
} EXT_RTCC_RTCHOUR;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE WKDAY : 3;     // Binary-Coded Decimal Value of the Week (LSB)
        BYTE VBATEN : 1;    // External Battery Backup Supply (Vbat) Enable bit
        BYTE PWRFAIL : 1;   // Power Failure Status bit
        BYTE OSCRUN : 1;    // Oscillator status bit
        BYTE reserved : 2;  // (MSB)
    } bits;
} EXT_RTCC_RTCWKDAY;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE DATEONE : 4;   // Binary-Coded Decimal Value of Date's Ones Digit (LSB)
        BYTE DATETEN : 2;   //  Binary-Coded Decimal Value of Date's Tens Digit
        BYTE reserved : 2;  // (MSB)
    } bits;
} EXT_RTCC_RTCDATE;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE MTHONE : 4;    // Binary-Coded Decimal Value of Month's Ones Digit (LSB)
        BYTE MTHTEN : 1;    // Binary-Coded Decimal Value of Month's Tens Digit
        BYTE LPYR : 1;      // Leap Year bit
        BYTE reserved : 2;  // (MSB)
    } bits;
} EXT_RTCC_RTCMTH;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE YRONE : 4;     // Binary-Coded Decimal Value of Year's Ones Digit (LSB)
        BYTE YRTEN : 4;     // Binary-Coded Decimal Value of Year's Tens Digit(MSB)
    } bits;
} EXT_RTCC_RTCYEAR;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE SQWFS : 2;     // Square Wave Clock Output Frequency Select bits (LSB)
        BYTE CRSTRIM : 1;   // Coarse Trim Mode Enable bit
        BYTE EXTOSC : 1;    // External Oscillator Input bit
        BYTE ALM0EN : 1;    // Alarm 0 Module Enable bit
        BYTE ALM1EN : 1;    // Alarm 1 Module Enable bit
        BYTE SQWEN : 1;     // Square Wave Output Enable bit
        BYTE OUT : 1;       // Logic Level for General Purpose Output bit (MSB))
    } bits;
} EXT_RTCC_CONTROL;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE TRIMVAL : 7;   // Oscillator Trim Value bits (LSB)
        BYTE SIGN : 1;      // Trim Sign bit (MSB)
    } bits;
} EXT_RTCC_OSCTRIM;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE MINONE : 4;    // Binary-Coded Decimal Value of Minute?s Ones Digit Contains a value from 0 to 9 (LSB)
        BYTE MINTEN : 3;    // Binary-Coded Decimal Value of Minute?s Tens Digit Contains a value from 0 to 5
        BYTE reserved : 1;  // (MSB)
    } bits;
} EXT_RTCC_PWRMIN;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE HRONE : 4;     // Binary-Coded Decimal Value of Hour's Ones Digit (LSB)
        BYTE HRTEN : 2;     // Binary-Coded Decimal Value of Hour's Tens Digit
        BYTE sel12_24 : 1;  // 12 or 25 Hour Time Format bit
        BYTE reserved : 1;  // (MSB)
    } bits;
} EXT_RTCC_PWRHOUR;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE DATEONE : 4;   // Binary-Coded Decimal Value of Date's Ones Digit (LSB)
        BYTE DATETEN : 2;   //  Binary-Coded Decimal Value of Date's Tens Digit
        BYTE reserved : 2;  // (MSB)
    } bits;
} EXT_RTCC_PWRDATE;

typedef union __attribute__((__packed__)) {
    BYTE Val;
    struct __PACKED {
        BYTE MTHONE : 4;    // Binary-Coded Decimal Value of Month's Ones Digit (LSB)
        BYTE MTHTEN : 1;    // Binary-Coded Decimal Value of Month's Tens Digit
        BYTE WKDAY : 3;     // Binary-Coded Decimal Value of the Week (LSB)
    } bits;
} EXT_RTCC_PWRMTH;

typedef struct {
    WORD year;
    BYTE month;
    BYTE day;
    BYTE hour; 
    BYTE min;
    BYTE sec;
    BYTE weekday;
} EXT_RTCC; 

// TODO insert the typedef for ALARM registers


typedef struct tm {
    BYTE tm_sec;
    BYTE tm_min;
    BYTE tm_hour;
    BYTE tm_mday;
    BYTE tm_mon;
    INT16 tm_year;
    BYTE tm_wday;
    INT16 tm_yday;
	//BOOL tm_isdst;
} TIME_TM;

BOOL InitExtRTCC(void);

BOOL ExtRTCCWriteSequential(BYTE *data, BYTE size, BYTE address, BYTE reg);
BOOL ExtRTCCReadSequential(BYTE *data, BYTE size, BYTE address, BYTE reg);

void ExtRTCCWrite(BYTE time_var, BYTE rtcc_reg);
BYTE ExtRTCCRead(BYTE rtcc_reg);  
    

BOOL ExtRTCCReadRAM(BYTE *data, BYTE size);
BOOL ExtRTCCWriteRAM(BYTE *data, BYTE size);
BOOL ExtRTCCReadMACAddress(BYTE *data, BYTE size);

BYTE binaryByteToBCDbyte(BYTE binary);
BYTE BCDByteToBinaryByte(BYTE bcd);

BYTE ExtRTCCGetSeconds(void);
BYTE ExtRTCCGetMinutes(void);
BYTE ExtRTCCGetHours(void);
BYTE ExtRTCCGetDay(void);
BYTE ExtRTCCGetMonth(void);
BYTE ExtRTCCGetYear(void);

void ExtRTCCGetTime(EXT_RTCC *rtcc);

void ExtRTCCSetExtendedTimeAndDate(BYTE hh, BYTE mm, BYTE ss, BYTE dd, BYTE MM, INT16 yyyy, BYTE _wday);
void rtccUpdateDate(void);
void ExtRTCCSetTimeFromNTPTask(void);
void offtime(TIME_TM *ts, DWORD t, INT16 offset);


#define GMT_MINUS_12_00 0
#define GMT_MINUS_11_00 1
#define GMT_MINUS_10_00 2
#define GMT_MINUS_09_00 3
#define GMT_MINUS_08_00 4
#define GMT_MINUS_07_00 5
#define GMT_MINUS_06_00 6
#define GMT_MINUS_05_00 7
#define GMT_MINUS_04_30 8
#define GMT_MINUS_04_00 9
#define GMT_MINUS_03_30 10
#define GMT_MINUS_03_00 11
#define GMT_MINUS_02_00 12
#define GMT_MINUS_01_00 13
#define GMT_00_00       14
#define GMT_PLUS_01_00  15
#define GMT_PLUS_02_00  16
#define GMT_PLUS_03_00  17
#define GMT_PLUS_03_30  18
#define GMT_PLUS_04_00  19
#define GMT_PLUS_04_30  20
#define GMT_PLUS_05_00  21
#define GMT_PLUS_05_30  22
#define GMT_PLUS_05_45  23
#define GMT_PLUS_06_00  24
#define GMT_PLUS_06_30  25
#define GMT_PLUS_07_00  26
#define GMT_PLUS_08_00  27
#define GMT_PLUS_09_00  28
#define GMT_PLUS_09_30  29
#define GMT_PLUS_10_00  30
#define GMT_PLUS_11_00  31
#define GMT_PLUS_12_00  32
#define GMT_PLUS_13_00  33

INT16 returnGMTValue(BYTE gmt);

#ifdef	__cplusplus
}
#endif

#endif	/* MCP79402_H */

