/* 
 * File:   IO.h
 * Author: Luca
 *
 * Created on 19 agosto 2016, 10.30
 */

#ifndef IO_H
#define	IO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"
#include "TCPIP Stack/StackTsk.h"

#define IO_1    1
#define IO_2    2
#define IO_3    3
#define IO_4    4
#define IO_5    5
#define IO_6    6
#define IO_7    7
#define IO_8    8

#define IO_OUTPUT           OUTPUT
#define IO_INPUT            INPUT

#define IO_LOW             0
#define IO_HIGH            1

    typedef struct {

        struct {
            // LSB
            BYTE status : 1;
            BYTE hasChanged : 1;
            // MSB
        } bits;
    } IO_CONFIG;

    void InitIO(void);
    void IOTask(void);

    void setDirectionIO(BYTE io, BYTE dir);
    void setDirectionInputIO(BYTE io);
    void setDirectionOutputIO(BYTE io);
    //BOOL getDirectionIO(BYTE io);

    BOOL readInputIO(BYTE io);

    void setOutputStateHighIO(BYTE io);
    void setOutputStateLowIO(BYTE io);
    void setOutputStateIO(BYTE io, BOOL state);

    //BOOL getStartupIO(BYTE io);
    //void setStartupIO(BYTE io, BOOL state);

#define getStartupIO(i)                     (appConfig.gpio.fields.ioBits[i - 1].ioDefault)
#define setStartupIO(i, value)              {appConfig.gpio.fields.ioBits[i - 1].ioDefault = value; saveAppConfig();}

#define getDirectionIO(i)                   (appConfig.gpio.fields.ioBits[i - 1].ioDirection)

#define getNoticeIO(i)                      (appConfig.gpio.fields.ioBits[i - 1].ioNotice)
    /***/

#define XEEPROM_ADDRESS_SIZE                64ul
#define XEEPROM_USERNAME_SIZE               16ul
#define XEEPROM_PASSWORD_SIZE               16ul

#define XEEPROM_ADDRESS_1_ADDRESS           INPUT_START_ADDRESS
#define XEEPROM_ADDRESS_1_USERNAME          XEEPROM_ADDRESS_1_ADDRESS + XEEPROM_ADDRESS_SIZE
#define XEEPROM_ADDRESS_1_PASSWORD          XEEPROM_ADDRESS_1_ADDRESS + XEEPROM_ADDRESS_SIZE + XEEPROM_USERNAME_SIZE

#define XEEPROM_ADDRESS(i)                  (INPUT_START_ADDRESS + (i * XEEPROM_PAGE_SIZE))
#define XEEPROM_USERNAME(i)                 (XEEPROM_ADDRESS_1_ADDRESS + XEEPROM_ADDRESS_SIZE + (i * XEEPROM_PAGE_SIZE))
#define XEEPROM_PASSWORD(i)                 (XEEPROM_ADDRESS_1_ADDRESS + XEEPROM_ADDRESS_SIZE + XEEPROM_USERNAME_SIZE + (i * XEEPROM_PAGE_SIZE))

#define setOut1OnHour(i, value)             {appConfig.gpio.fields.ioBits[i - 1].on1h = value;}
#define setOut1OnMinute(i, value)           {appConfig.gpio.fields.ioBits[i - 1].on1m = value;}
#define setOut1OffHour(i, value)            {appConfig.gpio.fields.ioBits[i - 1].off1h = value;}
#define setOut1OffMinute(i, value)          {appConfig.gpio.fields.ioBits[i - 1].off1m = value;}
#define setOut2OnHour(i, value)             {appConfig.gpio.fields.ioBits[i - 1].on2h = value;}
#define setOut2OnMinute(i, value)           {appConfig.gpio.fields.ioBits[i - 1].on2m = value;}
#define setOut2OffHour(i, value)            {appConfig.gpio.fields.ioBits[i - 1].off2h = value;}
#define setOut2OffMinute(i, value)          {appConfig.gpio.fields.ioBits[i - 1].off2m = value;}

#define getOutOn1Hour(i)                    (appConfig.gpio.fields.ioBits[i - 1].on1h)
#define getOutOn1Minute(i)                  (appConfig.gpio.fields.ioBits[i - 1].on1m)
#define getOutOff1Hour(i)                   (appConfig.gpio.fields.ioBits[i - 1].off1h)
#define getOutOff1Minute(i)                 (appConfig.gpio.fields.ioBits[i - 1].off1m)
#define getOutOn2Hour(i)                    (appConfig.gpio.fields.ioBits[i - 1].on2h)
#define getOutOn2Minute(i)                  (appConfig.gpio.fields.ioBits[i - 1].on2m)
#define getOutOff2Hour(i)                   (appConfig.gpio.fields.ioBits[i - 1].off2h)
#define getOutOff2Minute(i)                 (appConfig.gpio.fields.ioBits[i - 1].off2m)

#define getTime(hour, min)                  ((WORD)((WORD)((WORD)(hour) * 60) + (WORD)(min)))

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */

