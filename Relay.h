/* 
 * File:   Relay.h
 * Author: Luca
 *
 * Created on 13 aprile 2016, 15.31
 */

#ifndef RELAY_H
#define	RELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

#define SetRelay1AsOutput()     {RELAY_1_TRIS = OUTPUT;}
#define SetRelay2AsOutput()     {RELAY_2_TRIS = OUTPUT;}
#define SetRelay3AsOutput()     {RELAY_3_TRIS = OUTPUT;}
#define SetRelay4AsOutput()     {RELAY_4_TRIS = OUTPUT;}

#define SetRelay1High()         {RELAY_1_O = 1;}
#define SetRelay1Low()          {RELAY_1_O = 0;}
#define SetRelay1(state)        {RELAY_1_O = state;}
#define GetRelay1()             (RELAY_1_O)

#define SetRelay2High()         {RELAY_2_O = 1;}
#define SetRelay2Low()          {RELAY_2_O = 0;}
#define SetRelay2(state)        {RELAY_2_O = state;}
#define GetRelay2()             (RELAY_2_O)

#define SetRelay3High()         {RELAY_3_O = 1;}
#define SetRelay3Low()          {RELAY_3_O = 0;}
#define SetRelay3(state)        {RELAY_3_O = state;}
#define GetRelay3()             (RELAY_3_O)

#define SetRelay4High()         {RELAY_4_O = 1;}
#define SetRelay4Low()          {RELAY_4_O = 0;}
#define SetRelay4(state)        {RELAY_4_O = state;}
#define GetRelay4()             (RELAY_4_O)

#define RELAY_1                 1
#define RELAY_2                 2
#define RELAY_3                 3
#define RELAY_4                 4

#define RELAY_HIGH              1
#define RELAY_LOW               0

    void InitRelaies(void);
    void RelayTask(void);

    void setRelay(BYTE relay, BYTE state);
    BOOL getRelay(BYTE relay);

    void setStartUpRelay(BYTE relay, BOOL state);
    BOOL getStartUpRelay(BYTE relay);

    //BYTE getOnHour(BYTE relay);
    //BYTE getOnMinute(BYTE relay);
    //BYTE getOffHour(BYTE relay);
    //BYTE getOffMinute(BYTE relay);

#define setRelayOn1Hour(i, value)                   {appConfig.gpio.fields.relay[i - 1].on1h = value;}
#define setRelayOn1Minute(i, value)                 {appConfig.gpio.fields.relay[i - 1].on1m = value;}
#define setRelayOff1Hour(i, value)                  {appConfig.gpio.fields.relay[i - 1].off1h = value;}
#define setRelayOff1Minute(i, value)                {appConfig.gpio.fields.relay[i - 1].off1m = value;}
#define setRelayOn2Hour(i, value)                   {appConfig.gpio.fields.relay[i - 1].on2h = value;}
#define setRelayOn2Minute(i, value)                 {appConfig.gpio.fields.relay[i - 1].on2m = value;}
#define setRelayOff2Hour(i, value)                  {appConfig.gpio.fields.relay[i - 1].off2h = value;}
#define setRelayOff2Minute(i, value)                {appConfig.gpio.fields.relay[i - 1].off2m = value;}

#define getRelayOn1Hour(i)                          (appConfig.gpio.fields.relay[i - 1].on1h)
#define getRelayOn1Minute(i)                        (appConfig.gpio.fields.relay[i - 1].on1m)
#define getRelayOff1Hour(i)                         (appConfig.gpio.fields.relay[i - 1].off1h)
#define getRelayOff1Minute(i)                       (appConfig.gpio.fields.relay[i - 1].off1m)
#define getRelayOn2Hour(i)                          (appConfig.gpio.fields.relay[i - 1].on2h)
#define getRelayOn2Minute(i)                        (appConfig.gpio.fields.relay[i - 1].on2m)
#define getRelayOff2Hour(i)                         (appConfig.gpio.fields.relay[i - 1].off2h)
#define getRelayOff2Minute(i)                       (appConfig.gpio.fields.relay[i - 1].off2m)

#define getTime(hour, min)                          ((WORD)((WORD)((WORD)(hour) * 60) + (WORD)(min)))

#ifdef	__cplusplus
}
#endif

#endif	/* RELAY_H */

