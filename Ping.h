/*
 * Copyright (C) 2016 LP Systems
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 * 
 * Author: Luca Pascarella www.lucapascarella.it
 */

#ifndef PING_H
#define	PING_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "HardwareProfile.h"
#include "TCPIP Stack/TCPIP.h"

typedef enum _SM_PING{
	SM_PING_HOME,
	SM_PING_SEND_ICMP_REQUEST,
	SM_PING_GET_ICMP_RESPONSE,
	SM_PING_CLOSE_ICMP,
	SM_PING_DONE,
} SM_PING;

#define PING_SERVER_SIZE        32

//PING global struct descriptor details
typedef struct _FTP_DCPT {
    SM_PING sm;             // current status
    DWORD timeout;          // timeout ping interval
    BYTE host[PING_SERVER_SIZE];
    BYTE address[32];
    LONG millisec;          // Time elapsed to give reply

    struct {
        BYTE send : 1;
        BYTE gateway : 1;
        BYTE timeout : 1;
        BYTE cannotResolveIP : 1;
    } flags;
} PING_CLIENT;

void InitPing(void);
void PingTask(void);

void sendPing(void);
void setGatewayPing(BOOL gw);
BOOL pingIsComplete(void);
BOOL pingIsTimeout(void);
BOOL pingIsUnresolved(void);
WORD pingTime(void);
void pingSaveHostAddress(char *host);

char * pingGetAddress(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PING_H */

