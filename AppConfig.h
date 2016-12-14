/* 
 * File:   AppConfig.h
 * Author: Luca
 *
 * Created on 24 luglio 2015, 10.16
 */

#ifndef APPCONFIG_H
#define	APPCONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

#include "TCPIP Stack/TCPIP.h"
#include "TCPIPConfig.h"

    
#define MY_DEFAULT_DDNS_USERNAME        ""
#define MY_DEFAULT_DDNS_PASSWORD        ""
#define MY_DEFAULT_DDNS_HOSTNAME        ""
    
void InitAppConfig(void);
void clearEEPROMContet(void);
void saveAppConfig(void);
void saveThisAppConfig(APP_CONFIG *ptrConfig);
void loadAppConfig(void);
void loadThisAppConfig(APP_CONFIG *ptrConfig);
void cloneAppConfig(APP_CONFIG *ptrSrc, APP_CONFIG *ptrDest);
void loadDefaultAppConfig(void);

void checkManualRestoreDefault(void);


#ifdef	__cplusplus
}
#endif

#endif	/* APPCONFIG_H */

