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

#include <stdlib.h>

#include "HTTPApp.h"
#include "AppConfig.h"
#include "TCPIPConfig.h"
#include "Main.h"
#include "MCP79402.h"// Needed for SaveAppConfig() prototype
#include "NTP.h"

void HTTPGetDateTime(HTTP_CONN *curHTTP) {

    BYTE *ptr;

    // Determine which commands are requests
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "man"))) {
        if (*ptr == '0') { // Manual sync
            int hh, mm, ss, dd, MM, yyyy;
            if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "hh")))
                hh = atoi((char*) ptr);
            if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "mm")))
                mm = atoi((char*) ptr);
            if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ss")))
                ss = atoi((char*) ptr);
            if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "dd")))
                dd = atoi((char*) ptr);
            if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "MM")))
                MM = atoi((char*) ptr);
            if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "yy")))
                yyyy = atoi((char*) ptr);
            // Save in the RTCC
            ExtRTCCSetExtendedTimeAndDate(hh, mm, ss, dd, MM, yyyy, 0x00);
        } else if (*ptr == '1') { // NTP sync
            ntpForceUpdate();
        }
    }
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "gmt"))) {
        appConfig.ip.fields.GMT = atoi((char*) ptr);
    }
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "dste"))) {
        appConfig.ip.fields.flags.bits.bIsDSTEnabled = *ptr == '1' ? TRUE : FALSE;
    }
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "dst"))) {
        appConfig.ip.fields.DST = atoi((char*) ptr);
    }
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ntpe"))) {
        appConfig.ip.fields.flags.bits.bIsNTPEnabled = *ptr == '1' ? TRUE : FALSE;
    }
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ntp"))) {
        appConfig.ip.fields.ntpServerName = atoi((char*) ptr);
        ntpForceUpdate();
        rtccUpdateDate();
    }
    if ((ptr = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "force"))) {
        if (*ptr == '1') { // Force update
            ntpForceUpdate();
            rtccUpdateDate();
        }
    }
    saveThisAppConfig(&appConfig);
}

void HTTPPrint_time(void) {
    char str[16];
    sprintf(str, "%02d:%02d:%02d", ExtRTCCGetHours(), ExtRTCCGetMinutes(), ExtRTCCGetSeconds());
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_date(void) {
    char str[16];
    sprintf(str, "%02d/%02d/%04d", ExtRTCCGetDay(), ExtRTCCGetMonth(), ExtRTCCGetYear() + 2000);
    TCPPutString(sktHTTP, (BYTE*) str);}

void HTTPPrint_hour(void) {
    BYTE str[8];
    btoa(ExtRTCCGetHours(), (char*) str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_minute(void) {
    BYTE str[8];
    btoa(ExtRTCCGetMinutes(), (char*) str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_second(void) {
    BYTE str[8];
    btoa(ExtRTCCGetSeconds(),(char*)  str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_day(void) {
    BYTE str[8];
    btoa(ExtRTCCGetDay(),(char*)  str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_month(void) {
    BYTE str[8];
    btoa(ExtRTCCGetMonth(),(char*)  str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_year(void) {
    BYTE str[8];
    uitoa(ExtRTCCGetYear() + 2000, str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

/*
 * GMT selection
 */
void HTTPPrint_gmt(void) {
    BYTE str[8];
    uitoa(appConfig.ip.fields.GMT, str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

/*
 * Daylight saving time
 * 1 = Enabled
 * 0 = Disabled 
 */
void HTTPPrint_dste(void) {
    TCPPut(sktHTTP, appConfig.ip.fields.flags.bits.bIsDSTEnabled ? '1' : '0');
}

/**
 * Daylight saving time selection
 */
void HTTPPrint_dst(void) {
    BYTE str[8];
    uitoa(appConfig.ip.fields.DST, str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

/**
 * NTP server selection
 */
void HTTPPrint_ntps(void) {
    BYTE str[8];
    uitoa(appConfig.ip.fields.ntpServerName, str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

/**
 * Network time protocol
 * 1 = Enabled
 * 0 = Disabled
 */
void HTTPPrint_ntpe(void) {
    TCPPut(sktHTTP, appConfig.ip.fields.flags.bits.bIsNTPEnabled ? '1' : '0');
}

