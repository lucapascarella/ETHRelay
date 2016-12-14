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
void RelayTesk(void);

void setRelay(BYTE relay, BYTE state);
BOOL getRelay(BYTE relay);

void setStartUpRelay(BYTE relay, BOOL state);
BOOL getStartUpRelay(BYTE relay);

//BYTE getOnHour(BYTE relay);
//BYTE getOnMinute(BYTE relay);
//BYTE getOffHour(BYTE relay);
//BYTE getOffMinute(BYTE relay);

#define setOnHour(i, value)                 {appConfig.gpio.fields.relay[i - 1].onh = value;}
#define setOnMinute(i, value)               {appConfig.gpio.fields.relay[i - 1].onm = value;}
#define setOffHour(i, value)                {appConfig.gpio.fields.relay[i - 1].offh = value;}
#define setOffMinute(i, value)              {appConfig.gpio.fields.relay[i - 1].offm = value;}

#define getOnHour(i)                        (appConfig.gpio.fields.relay[i - 1].onh)
#define getOnMinute(i)                      (appConfig.gpio.fields.relay[i - 1].onm)
#define getOffHour(i)                       (appConfig.gpio.fields.relay[i - 1].offh)
#define getOffMinute(i)                     (appConfig.gpio.fields.relay[i - 1].offm)


#ifdef	__cplusplus
}
#endif

#endif	/* RELAY_H */

