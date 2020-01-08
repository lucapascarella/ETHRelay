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
#include "TCPIP Stack/XEEPROM.h"
#include "IO.h"
#include <stdlib.h>

/*****************************************************************************
  Function:
        static HTTP_IO_RESULT HTTPPostIO(void)
 *****************************************************************************/
HTTP_IO_RESULT HTTPPostIO(void) {

    static BYTE num;
    DWORD address;

#define SM_POST_IO_READ_NAME_NUM        (0u)
#define SM_POST_IO_READ_VALUE_NUM       (1u)
#define SM_POST_IO_READ_NAME_ADD		(2u)
#define SM_POST_IO_READ_VALUE_ADD		(3u)
#define SM_POST_IO_READ_NAME_USER       (4u)
#define SM_POST_IO_READ_VALUE_USER      (5u)
#define SM_POST_IO_READ_NAME_PASS       (6u)
#define SM_POST_IO_READ_VALUE_PASS      (7u)
#define SM_POST_IO_READ_NAME_CHECK      (8u)
#define SM_POST_IO_READ_VALUE_CHECK     (9u)
#define SM_POST_IO_ERROR                (10u)

    switch (curHTTP.smPost) {

        case SM_POST_IO_READ_NAME_NUM:
            if (HTTPReadPostName(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "num"))
                curHTTP.smPost = SM_POST_IO_READ_VALUE_NUM;
            else
                curHTTP.smPost = SM_POST_IO_ERROR;
            // No break...continue reading value

        case SM_POST_IO_READ_VALUE_NUM:
            if (HTTPReadPostValue(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            num = atob((char*) curHTTP.data) - 1;
            address = XEEPROM_ADDRESS(num);
            XEEBeginWrite(address);
            curHTTP.smPost = SM_POST_IO_READ_NAME_ADD;
            break;

        case SM_POST_IO_READ_NAME_ADD:
            if (HTTPReadPostName(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "add"))
                curHTTP.smPost = SM_POST_IO_READ_VALUE_ADD;
            else
                curHTTP.smPost = SM_POST_IO_ERROR;
            // No break...continue reading value

        case SM_POST_IO_READ_VALUE_ADD:
            if (HTTPReadPostValue(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            XEEWriteArray((BYTE*) curHTTP.data, XEEPROM_ADDRESS_SIZE);
            curHTTP.smPost = SM_POST_IO_READ_NAME_USER;
            break;

        case SM_POST_IO_READ_NAME_USER:
            if (HTTPReadPostName(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "user"))
                curHTTP.smPost = SM_POST_IO_READ_VALUE_USER;
            else
                curHTTP.smPost = SM_POST_IO_ERROR;
            // No break...continue reading value


        case SM_POST_IO_READ_VALUE_USER:
            if (HTTPReadPostValue(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            XEEWriteArray((BYTE*) curHTTP.data, XEEPROM_USERNAME_SIZE);
            curHTTP.smPost = SM_POST_IO_READ_NAME_PASS;
            break;

        case SM_POST_IO_READ_NAME_PASS:
            if (HTTPReadPostName(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "pass"))
                curHTTP.smPost = SM_POST_IO_READ_VALUE_PASS;
            else
                curHTTP.smPost = SM_POST_IO_ERROR;
            // No break...continue reading value

        case SM_POST_IO_READ_VALUE_PASS:
            if (HTTPReadPostValue(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            XEEWriteArray((BYTE*) curHTTP.data, XEEPROM_PASSWORD_SIZE);
            XEEEndWrite();
            curHTTP.smPost++;
            break;

        case SM_POST_IO_READ_NAME_CHECK:
            if (HTTPReadPostName(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "sel"))
                curHTTP.smPost++;
            else
                curHTTP.smPost = SM_POST_IO_ERROR;
            // No break...continue reading value

        case SM_POST_IO_READ_VALUE_CHECK:
            if (HTTPReadPostValue(curHTTP.data, HTTP_MAX_DATA_LEN) == HTTP_READ_INCOMPLETE)
                return HTTP_IO_NEED_DATA;
            appConfig.gpio.fields.ioBits[num].ioNotice = atob((char*) curHTTP.data);
            saveAppConfig();

            // This is the only expected value, so callback is done
            //strcpypgm2ram((char*) curHTTP.data, "/prt/io.htm");
            //curHTTP.httpStatus = HTTP_REDIRECT;
            return HTTP_IO_DONE;

        case SM_POST_IO_ERROR:
            strcpypgm2ram((char*) curHTTP.data, (ROM char*) "/prt/io.htm");
            curHTTP.httpStatus = HTTP_REDIRECT;
            return HTTP_IO_DONE;
    }

    // Default assumes that we're returning for state machine convenience.
    // Function will be called again later.
    return HTTP_IO_WAITING;
}

void HTTPAppGetIO(HTTP_CONN * curHTTP) {

    BYTE *io, *action, *direction, *startup;
    BYTE *out, *on1h, *on1m, *off1h, *off1m, *on2h, *on2m, *off2h, *off2m;
    
    io = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "io");
    action = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "act");
    direction = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "dir");
    startup = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "st");
    if (io && action) {
        setOutputStateIO(*io - '0', *action - '0');
    }
    if (io && direction) {
        setDirectionIO(*io - '0', *direction - '0');
    }
    if (io && startup) {
        setStartupIO(*io - '0', *startup - '0');
    }

    out = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "out");
    on1h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on1h");
    on1m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on1m");
    off1h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off1h");
    off1m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off1m");
    on2h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on2h");
    on2m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "on2m");
    off2h = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off2h");
    off2m = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "off2m");
    if (out && on1h && on1m && off1h && off1m) {
        setOut1OnHour((*out - '0'), atob((char*) on1h));
        setOut1OnMinute(*out - '0', atob((char*) on1m));
        setOut1OffHour(*out - '0', atob((char*) off1h));
        setOut1OffMinute(*out - '0', atob((char*) off1m));
        saveAppConfig();
    }
    if (out && on2h && on2m && off2h && off2m) {
        setOut2OnHour((*out - '0'), atob((char*) on2h));
        setOut2OnMinute(*out - '0', atob((char*) on2m));
        setOut2OffHour(*out - '0', atob((char*) off2h));
        setOut2OffMinute(*out - '0', atob((char*) off2m));
        saveAppConfig();
    }
}

void HTTPPrint_io(WORD io) {
    BOOL stat = getDirectionIO(io);
    TCPPut(sktHTTP, stat ? '1' : '0');
}

void HTTPPrint_in(WORD in) {
    BOOL stat = readInputIO(in);
    TCPPut(sktHTTP, stat ? '1' : '0');
}

void HTTPPrint_is(WORD is) {
    BOOL stat = getStartupIO(is);
    TCPPut(sktHTTP, stat ? '1' : '0');
}

void HTTPPrint_ioAdd(WORD num) {
    BYTE i;
    XEEReadArray(XEEPROM_ADDRESS((WORD) (num - 1)), curHTTP.data, XEEPROM_ADDRESS_SIZE - 1);
    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_ADDRESS_SIZE - 1;
    curHTTP.data[i] = '\0';
    TCPPutString(sktHTTP, curHTTP.data);
}

void HTTPPrint_ioUser(WORD num) {
    BYTE i;
    XEEReadArray(XEEPROM_USERNAME((WORD) (num - 1)), curHTTP.data, XEEPROM_USERNAME_SIZE - 1);
    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_USERNAME_SIZE - 1;
    curHTTP.data[i] = '\0';
    TCPPutString(sktHTTP, curHTTP.data);
}

void HTTPPrint_ioPass(WORD num) {
    BYTE i;
    XEEReadArray(XEEPROM_PASSWORD((WORD) (num - 1)), curHTTP.data, XEEPROM_PASSWORD_SIZE - 1);
    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_PASSWORD_SIZE - 1;
    curHTTP.data[i] = '\0';
    TCPPutString(sktHTTP, curHTTP.data);
}

void HTTPPrint_ioSel(WORD num) {
    BYTE aaa = getNoticeIO(num);
    btoa(aaa, (char*) curHTTP.data);
    TCPPutString(sktHTTP, curHTTP.data);
}

void HTTPPrint_oon1h(WORD out) {
    char str[8];
    btoa(getOutOn1Hour(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_oon1m(WORD out) {
    char str[8];
    btoa(getOutOn1Minute(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ooff1h(WORD out) {
    char str[8];
    btoa(getOutOff1Hour(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ooff1m(WORD out) {
    char str[8];
    btoa(getOutOff1Minute(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_oon2h(WORD out) {
    char str[8];
    btoa(getOutOn2Hour(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_oon2m(WORD out) {
    char str[8];
    btoa(getOutOn2Minute(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ooff2h(WORD out) {
    char str[8];
    btoa(getOutOff2Hour(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ooff2m(WORD out) {
    char str[8];
    btoa(getOutOff2Minute(out), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}
