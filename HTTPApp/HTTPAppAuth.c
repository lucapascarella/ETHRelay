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

/*****************************************************************************
  Function:
        static HTTP_IO_RESULT HTTPPostAuthentication(void)
 *****************************************************************************/
HTTP_IO_RESULT HTTPPostAuthentication(void) {

    BOOL saveOp = FALSE, applyOp = FALSE;

    if (curHTTP.byteCount > TCPIsGetReady(sktHTTP) + TCPGetRxFIFOFree(sktHTTP))
        goto ConfigFailure;

    // Ensure that all data is waiting to be parsed.  If not, keep waiting for
    // all of it to arrive.
    if (TCPIsGetReady(sktHTTP) < curHTTP.byteCount)
        return HTTP_IO_NEED_DATA;

    // Use current config in non-volatile memory as defaults
    loadThisAppConfig(&newAppConfig);

    // Read all browser POST data
    while (curHTTP.byteCount) {
        // Read a form field name
        if (HTTPReadPostName(curHTTP.data, 6) != HTTP_READ_OK)
            goto ConfigFailure;

        // Read a form field value
        if (HTTPReadPostValue(curHTTP.data + 6, sizeof (curHTTP.data) - 6 - 2) != HTTP_READ_OK)
            goto ConfigFailure;

        // Parse the value that was read
        if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "user")) {// Read new Username
            strncpy((char*) newAppConfig.auth.fields.user.username, (char*) curHTTP.data + 6, SIZE_USERNAME - 1);
            newAppConfig.auth.fields.user.username[SIZE_USERNAME - 1] = '\0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "pass")) {// Read new Password
            strncpy((char*) newAppConfig.auth.fields.user.password, (char*) curHTTP.data + 6, SIZE_PASSWORD - 1);
            newAppConfig.auth.fields.user.password[SIZE_PASSWORD - 1] = '\0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "save")) {
            // Read "Save" request
            saveOp = TRUE;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "appl")) {
            // Read "Apply" request
            applyOp = TRUE;
        }
    }


    // All parsing are completed!
    if (saveOp) {
        // Do a action to Save request
        saveThisAppConfig(&newAppConfig);
    }
    if (applyOp) {
        // Do a action to Apply request
        strncpy((char*) appConfig.auth.fields.user.username, (char*) newAppConfig.auth.fields.user.username, SIZE_USERNAME);
        strncpy((char*) appConfig.auth.fields.user.password, (char*) newAppConfig.auth.fields.user.password, SIZE_PASSWORD);
    }
    return HTTP_IO_DONE;


ConfigFailure:
    //    lastFailure = TRUE;
    strcpypgm2ram((char*) curHTTP.data, (ROM char*) "/prt/index.htm");
    curHTTP.httpStatus = HTTP_REDIRECT;

    return HTTP_IO_DONE;
}

void HTTPPrint_user(void) {
    TCPPutString(sktHTTP, appConfig.auth.fields.user.username);
}

void HTTPPrint_pass(void) {
    TCPPutString(sktHTTP, appConfig.auth.fields.user.password);
}


