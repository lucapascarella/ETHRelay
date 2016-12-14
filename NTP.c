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

#ifndef __NTP_C
#define __NTP_C

#include "NTP.h"


// Defines how frequently to resynchronize the date/time (default: 10 minutes)
//#define NTP_QUERY_INTERVAL          (10ull*60ull * TICK_SECOND)

// Defines how long to wait to retry an update after a failure.
// Updates may take up to 6 seconds to fail, so this 14 second delay is actually only an 8-second retry.
#define NTP_FAST_QUERY_INTERVAL		(14ull * TICK_SECOND)

// Port for contacting NTP servers
#define NTP_SERVER_PORT             (123ul)

// Reference Epoch to use.  (default: 01-Jan-1970 00:00:00)
#define NTP_EPOCH                   (86400ul * (365ul * 70ul + 17ul))

// Defines how long to wait before assuming the query has failed
#define NTP_REPLY_TIMEOUT           (6ul * TICK_SECOND)

// Defines how long to wait before assuming the connection is in timeout
#define NTP_CONNECTION_TIMEOUT      (20ul * TICK_SECOND)

const rom char *serverName[] = {"pool.ntp.org", "europe.pool.ntp.org", "asia.pool.ntp.org", "oceania.pool.ntp.org", "north-america.pool.ntp.org", "south-america.pool.ntp.org", "africa.pool.ntp.org"};

// Seconds value obtained by last update
static DWORD dwSNTPSeconds = 0;

// Tick count of last update
static DWORD dwLastUpdateTick = 0;

// NTP configs
NTP_CONFIG ntp;

void NTPInit(void) {

    // Initialize to Wait Command state
    ntp.stateMachine = SM_NTP_SHORT_WAIT;

    // Init timeout variable
    ntp.dwTimer = TickGetDiv64K();

    // Clear Sync indicator
    ntp.bits.ntpSync = FALSE;
}

/*****************************************************************************
  Function:
    void BerkeleyUDPClientDemo(void)

  Summary:
    Periodically checks the current time from a pool of servers.

  Description:
    This function periodically checks a pool of time servers to obtain the
    current date/time.

  Precondition:
    UDP is initialized.

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function requires once available UDP socket while processing, but
    frees that socket when the SNTP module is idle.
 ***************************************************************************/
void UDPClientNTPHandler(void) {

    NTP_PACKET pkt;
    WORD w;

    switch (ntp.stateMachine) {
        case SM_NTP_HOME:
            // Wait until the NTP is enabled
            if (appConfig.ip.fields.flags.bits.bIsNTPEnabled != 1)
                break;
            // Copy the ntp server indicator into the local variable
            ntp.ntpServerName = appConfig.ip.fields.ntpServerName;
            ntp.ntpSyncInterval = appConfig.ip.fields.syncInterval;
            // NTP enabled goto next stage
            ntp.stateMachine = SM_NTP_CREATE_SOCKET;
            break;

        case SM_NTP_CREATE_SOCKET:

            // Open a UDP socket for inbound and outbound transmission
            // Since we expect to only receive broadcast packets and
            // only send unicast packets directly to the node we last
            // received from, the remote NodeInfo parameter can be anything
            ntp.socket = UDPOpenEx((DWORD) (ROM_PTR_BASE) serverName[ntp.ntpServerName], UDP_OPEN_ROM_HOST, 0, NTP_SERVER_PORT);

            // If this ever happens, you need to go add one to TCPIPConfig.h
            if (ntp.socket == INVALID_UDP_SOCKET)
                break; // This is an error catch that

            // Get current time and goto next stage
            ntp.dwTimer = TickGet();
            ntp.stateMachine++;
            break;


        case SM_NTP_SOCKET_OBTAINED:
            // Wait for the remote server to accept our connection request
            if (UDPIsOpened(ntp.socket) == FALSE) {
                // Time out if too much time is spent in this state
                if (TickGet() - ntp.dwTimer > NTP_CONNECTION_TIMEOUT) {
                    // Close the socket so it can be used by other modules
                    ntp.stateMachine = SM_NTP_DISCONNECT;
                }
                // Always do break, if no connection is established yet
                break;
            }

            // A connection has been well established, go to next step
            ntp.stateMachine++;

            //break;
            // No break needed

        case SM_NTP_UDP_SEND:

            // Make certain the socket can be written to
            if (!UDPIsPutReady(ntp.socket)) {
                UDPClose(ntp.socket);
                ntp.dwTimer = TickGetDiv64K();
                ntp.stateMachine = SM_NTP_SHORT_WAIT;
                //ntp.stateMachine = SM_NTP_HOME;
                ntp.socket = INVALID_UDP_SOCKET;
                break;
            }

            // Transmit a time request packet
            memset(&pkt, 0, sizeof (pkt));
            pkt.flags.versionNumber = 3; // NTP Version 3
            pkt.flags.mode = 3; // NTP Client
            pkt.orig_ts_secs = swapl(NTP_EPOCH);
            UDPPutArray((BYTE*) & pkt, sizeof (pkt));
            UDPFlush();

            ntp.dwTimer = TickGet();
            ntp.stateMachine = SM_NTP_UDP_RECV;
            break;

        case SM_NTP_UDP_RECV:
            // Look for a response time packet
            if (!UDPIsGetReady(ntp.socket)) {
                if ((TickGet()) - ntp.dwTimer > NTP_REPLY_TIMEOUT) {
                    // Abort the request and wait until the next timeout period
                    UDPClose(ntp.socket);
                    ntp.dwTimer = TickGetDiv64K();
                    ntp.stateMachine = SM_NTP_SHORT_WAIT;
                    //ntp.stateMachine = SM_NTP_HOME;
                    ntp.socket = INVALID_UDP_SOCKET;
                    break;
                }
                break;
            }

            // Get the response time packet
            w = UDPGetArray((BYTE*) & pkt, sizeof (pkt));
            UDPClose(ntp.socket);
            ntp.dwTimer = TickGetDiv64K();
            ntp.stateMachine = SM_NTP_WAIT;
            ntp.socket = INVALID_UDP_SOCKET;

            // Validate packet size
            if (w != sizeof (pkt)) {
                break;
            }

            // Set out local time to match the returned time
            dwLastUpdateTick = TickGet();
            dwSNTPSeconds = swapl(pkt.tx_ts_secs) - NTP_EPOCH;
            // Do rounding.  If the partial seconds is > 0.5 then add 1 to the seconds count.
            if (((BYTE*) & pkt.tx_ts_fraq)[0] & 0x80)
                dwSNTPSeconds++;

            // If NTP is enabled update the time and date on RTCC
            ntp.lastSync = SNTPGetUTCSeconds();
            // NTP has a sync, save this
            ntp.bits.ntpSync = TRUE;
            break;

        case SM_NTP_SHORT_WAIT:
            // Attempt to requery the NTP server after a specified NTP_FAST_QUERY_INTERVAL time (ex: 8 seconds) has elapsed.
            if (TickGetDiv64K() - ntp.dwTimer > (NTP_FAST_QUERY_INTERVAL / 65536ull)) {
                ntp.stateMachine = SM_NTP_HOME;
                ntp.socket = INVALID_UDP_SOCKET;
            }
            break;

        case SM_NTP_WAIT:
            // Requery the NTP server after a specified NTP_QUERY_INTERVAL time (ex: 10 minutes) has elapsed.
            if (TickGetDiv64K() - ntp.dwTimer > (ntp.ntpSyncInterval * TICK_HOUR / 65536ull)) {
                ntp.stateMachine = SM_NTP_HOME;
                ntp.socket = INVALID_UDP_SOCKET;
            }
            break;

        case SM_NTP_DISCONNECT:
            UDPClose(ntp.socket);
            ntp.stateMachine = SM_NTP_WAIT;
            break;
    }
}

void ntpForceUpdate(void) {
    if (ntp.stateMachine == SM_NTP_SHORT_WAIT || ntp.stateMachine == SM_NTP_WAIT) {
        ntp.stateMachine = SM_NTP_HOME;
        ntp.bits.ntpSync = FALSE;
    }
}

/*****************************************************************************
  Function:
    DWORD BerkeleySNTPGetUTCSeconds(void)

  Summary:
    Obtains the current time from the SNTP module.

  Description:
    This function obtains the current time as reported by the SNTP module.
    Use this value for absolute time stamping.  The value returned is (by
    default) the number of seconds since 01-Jan-1970 00:00:00.

  Precondition:
    None

  Parameters:
    None

  Returns:
    The number of seconds since the Epoch.  (Default 01-Jan-1970 00:00:00)

  Remarks:
    Do not use this function for time difference measurements.  The Tick
    module is more appropriate for those requirements.
 ***************************************************************************/
DWORD SNTPGetUTCSeconds(void) {
    DWORD dwTickDelta;
    DWORD dwTick;

    // Update the dwSNTPSeconds variable with the number of seconds
    // that has elapsed
    dwTick = TickGet();
    dwTickDelta = dwTick - dwLastUpdateTick;
    while (dwTickDelta > TICK_SECOND) {
        dwSNTPSeconds++;
        dwTickDelta -= TICK_SECOND;
    }

    // Save the tick and residual fractional seconds for the next call
    dwLastUpdateTick = dwTick - dwTickDelta;

    return dwSNTPSeconds;
}

BOOL ntpIsSync(void) {
    return ntp.bits.ntpSync;
}

DWORD ntpLastSync(void) {
    return ntp.lastSync;
}

#endif
