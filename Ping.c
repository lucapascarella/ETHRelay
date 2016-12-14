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

#define __PING_C

#include "Ping.h"

static PING_CLIENT ping;

void InitPing(void) {
    ping.flags.gateway = FALSE;
    ping.flags.cannotResolveIP = FALSE;
    ping.flags.timeout = FALSE;
    ping.sm = SM_PING_DONE;
}

/*****************************************************************************
  Function:
    void PingTask(void)

  Summary:
    Demonstrates use of the ICMP (Ping) client.
	
  Description:
    This function implements a simple ICMP client.  The function is called
    periodically by the stack, and it checks if flag has been pressed.  
    If the button is pressed, the function sends an ICMP Echo Request (Ping)
    to a Microchip web server.  The round trip time is displayed on the UART
    when the response is received.
	
    This function can be used as a model for applications requiring Ping 
    capabilities to check if a host is reachable.

  Precondition:
    TCP is initialized.

  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
void PingTask(void) {

    LONG ret;

    switch (ping.sm) {
        case SM_PING_HOME:
            if (ping.flags.send)
                // Don't ping flood: wait at least 1 second between ping requests
                if ((ping.timeout - TickGet()) > 1ul * TICK_SECOND)
                    // Obtain ownership of the ICMP module
                    if (ICMPBeginUsage())
                        ping.sm++;
            break;

        case SM_PING_SEND_ICMP_REQUEST:
            ping.flags.cannotResolveIP = FALSE;
            ping.flags.timeout = FALSE;
            // Send ICMP echo request
            if (ping.flags.gateway)
                ICMPSendPing(appConfig.ip.fields.MyGateway.Val);
            else
                ICMPSendPingToHost(ping.host);
            ping.sm++;
            break;

        case SM_PING_GET_ICMP_RESPONSE:
            // Get the status of the ICMP module
            ret = ICMPGetReply();
            if (ret == -2) {
                // Do nothing: still waiting for echo
                break;
            } else if (ret == -1) {
                // Request timed out
                ping.flags.timeout = TRUE;
            } else if (ret == -3) {
                // DNS address not resolvable, can't resolve IP
                ping.flags.cannotResolveIP = TRUE;
            } else {
                // Echo received.  Time elapsed is stored in ret (Tick units).
                ping.millisec = TickConvertToMilliseconds((DWORD) ret);
            }
            ping.sm++;
            break;

        case SM_PING_CLOSE_ICMP:
            // Finished with the ICMP module, release it so other apps can begin using it
            ICMPEndUsage();
            ping.sm++;
            break;

        case SM_PING_DONE:
            // Reset the variables and return to home state
            ping.timeout = TickGet();
            ping.flags.send = FALSE;
            ping.sm = SM_PING_HOME;
            break;
    }
}

void sendPing(void) {
    ping.flags.send = TRUE;
}

void setGatewayPing(BOOL gw) {
    ping.flags.gateway = gw;
}

BOOL pingIsComplete(void) {
    return !ping.flags.send;
}

BOOL pingIsTimeout(void) {
    return ping.flags.timeout;
}

BOOL pingIsUnresolved(void) {
    return ping.flags.cannotResolveIP;
}

WORD pingTime(void) {
    return ping.millisec;
}

void pingSaveHostAddress(char *host) {
    strncpy((char*) ping.host, host, sizeof (ping.host));
}

char * pingGetAddress(void) {
    NODE_INFO *ptr_node_info;
    if (ping.flags.send == FALSE) {
        ptr_node_info = (NODE_INFO *) ICMPGetRemoteAddress();
        sprintf((char*) ping.address, "%d.%d.%d.%d", ptr_node_info->IPAddr.v[0], ptr_node_info->IPAddr.v[1], ptr_node_info->IPAddr.v[2], ptr_node_info->IPAddr.v[3]);
    } else {
        ping.address[0] = '\0';
    }
    return (char*) ping.address;
}