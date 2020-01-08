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

#include "HTTPApp.h"
#include "TCPIPConfig.h"
#include "AppConfig.h"
#include "Relay.h"

void HTTPAppGetRelay(HTTP_CONN *curHTTP) {

    BYTE *relay, *action, *startup, *on1h, *on1m, *off1h, *off1m, *on2h, *on2m, *off2h, *off2m;

    relay = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "relay");
    action = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "act");
    startup = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "st");
    on1h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on1h");
    on1m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on1m");
    off1h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off1h");
    off1m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off1m");
    on2h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on2h");
    on2m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on2m");
    off2h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off2h");
    off2m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off2m");
    if (relay && action) {
        setRelay(*relay - '0', *action - '0');
    }
    if (relay && startup) {
        setStartUpRelay(*relay - '0', *startup - '0');
    }
    if (relay && on1h && on1m && off1h && off1m) {
        setRelayOn1Hour((*relay - '0'), atob((char*) on1h));
        setRelayOn1Minute(*relay - '0', atob((char*) on1m));
        setRelayOff1Hour(*relay - '0', atob((char*) off1h));
        setRelayOff1Minute(*relay - '0', atob((char*) off1m));
        saveAppConfig();
    }
    if (relay && on2h && on2m && off2h && off2m) {
        setRelayOn2Hour((*relay - '0'), atob((char*) on2h));
        setRelayOn2Minute(*relay - '0', atob((char*) on2m));
        setRelayOff2Hour(*relay - '0', atob((char*) off2h));
        setRelayOff2Minute(*relay - '0', atob((char*) off2m));
        saveAppConfig();
    }
}

/**
 * Print the status of @param relay
 * @param relay
 */
void HTTPPrint_rl(WORD relay) {
    BOOL stat = getRelay(relay);
    TCPPut(sktHTTP, (BYTE*) stat ? '1' : '0');
}

void HTTPPrint_st(WORD relay) {
    BOOL stat = getStartUpRelay(relay);
    TCPPut(sktHTTP, (BYTE*) stat ? '1' : '0');
}

void HTTPPrint_ron1h(WORD relay) {
    char str[8];
    btoa(getRelayOn1Hour(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ron1m(WORD relay) {
    char str[8];
    btoa(getRelayOn1Minute(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_roff1h(WORD relay) {
    char str[8];
    btoa(getRelayOff1Hour(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_roff1m(WORD relay) {
    char str[8];
    btoa(getRelayOff1Minute(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ron2h(WORD relay) {
    char str[8];
    btoa(getRelayOn2Hour(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ron2m(WORD relay) {
    char str[8];
    btoa(getRelayOn2Minute(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_roff2h(WORD relay) {
    char str[8];
    btoa(getRelayOff2Hour(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_roff2m(WORD relay) {
    char str[8];
    btoa(getRelayOff2Minute(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}



