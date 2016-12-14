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

    BYTE *relay, *action, *startup, *onh, *onm, *offh, *offm;

    relay = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "relay");
    action = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "act");
    startup = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "st");
    onh = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "onh");
    onm = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "onm");
    offh = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "offh");
    offm = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "offm");
    if (relay && action) {
        setRelay(*relay - '0', *action - '0');
    }
    if (relay && startup) {
        setStartUpRelay(*relay - '0', *startup - '0');
    }
    if (relay && onh && onm && offh && offm) {
        setOnHour((*relay - '0'), atob((char*) onh));
        setOnMinute(*relay - '0', atob((char*) onm));
        setOffHour(*relay - '0', atob((char*) offh));
        setOffMinute(*relay - '0', atob((char*) offm));
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

void HTTPPrint_onh(WORD relay) {
    char str[8];
    btoa(getOnHour(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_onm(WORD relay) {
    char str[8];
    btoa(getOnMinute(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_offh(WORD relay) {
    char str[8];
    btoa(getOffHour(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_offm(WORD relay) {
    char str[8];
    btoa(getOffMinute(relay), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}



