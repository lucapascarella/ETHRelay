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

#ifndef NTP_H
#define	NTP_H

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"
#include "TCPIP Stack/TCPIP.h"
#include "TCPIPConfig.h"

typedef enum _NTPClientState {
    SM_NTP_HOME = 0,

    //SM_NTP_NAME_RESOLVE,
    SM_NTP_CREATE_SOCKET,
    SM_NTP_SOCKET_OBTAINED,
    //SM_NTP_BIND,	// Not required since we are sending the first packet
    SM_NTP_UDP_SEND,
    SM_NTP_UDP_RECV,
    SM_NTP_SHORT_WAIT,
    SM_NTP_WAIT,

    SM_NTP_DISCONNECT
} _NTP_STATE_MACHINE;

// Defines the structure of an NTP packet

typedef struct {

    struct {
        BYTE mode : 3; // NTP mode
        BYTE versionNumber : 3; // SNTP version number
        BYTE leapIndicator : 2; // Leap second indicator
    } flags; // Flags for the packet

    BYTE stratum; // Stratum level of local clock
    CHAR poll; // Poll interval
    CHAR precision; // Precision (seconds to nearest power of 2)
    DWORD root_delay; // Root delay between local machine and server
    DWORD root_dispersion; // Root dispersion (maximum error)
    DWORD ref_identifier; // Reference clock identifier
    DWORD ref_ts_secs; // Reference timestamp (in seconds)
    DWORD ref_ts_fraq; // Reference timestamp (fractions)
    DWORD orig_ts_secs; // Origination timestamp (in seconds)
    DWORD orig_ts_fraq; // Origination timestamp (fractions)
    DWORD recv_ts_secs; // Time at which request arrived at sender (seconds)
    DWORD recv_ts_fraq; // Time at which request arrived at sender (fractions)
    DWORD tx_ts_secs; // Time at which request left sender (seconds)
    DWORD tx_ts_fraq; // Time at which request left sender (fractions)
} NTP_PACKET;

typedef struct __attribute__((__packed__)) {

    _NTP_STATE_MACHINE stateMachine;    // NTP state machine indicator
    BYTE ntpServerName;                 // Server index name
    BYTE ntpSyncInterval;               // Ntp sync interval in hours
    UDP_SOCKET socket;                  // NTP UDP Socket
    DWORD dwTimer;                      // Timeout value
    DWORD lastSync;                     // Seconds (from 00:00:00 Jan 1 1970) of last NTP sync
    
    struct __PACKED {
        BYTE reserved : 6;
        BYTE ntpEnabled : 1;
        BYTE ntpSync : 1;
    } bits;
} NTP_CONFIG;


void NTPInit(void);
void UDPClientNTPHandler(void);
void ntpForceUpdate(void);
DWORD SNTPGetUTCSeconds(void);
BOOL ntpIsSync(void);
DWORD ntpLastSync(void);

#endif	/* NTP_H */

