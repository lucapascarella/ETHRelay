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
#include "ADC.h"

/*****************************************************************************
  Function:
        static HTTP_IO_RESULT HTTPPostAuthentication(void)
 *****************************************************************************/
void HTTPAppGetAnalog(HTTP_CONN *curHTTP) {

    BYTE *adc, *adcHighThreshold, *adcLowThreshold, *adcOutput, *adcRelay, *adcDefault;
    BYTE num;

    adc = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "adc");
    adcHighThreshold = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ah");
    adcLowThreshold = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "al");
    adcOutput = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ao");
    adcRelay = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ar");
    adcDefault = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "ad");
    if (adc && adcHighThreshold && adcLowThreshold && adcOutput && adcRelay && adcDefault) {
        setHighThreshold((*adc - '0'), atoi((char*) adcHighThreshold));
        setLowThreshold((*adc - '0'), atoi((char*) adcLowThreshold));
        adcSetOutput((*adc - '0'), atob((char*) adcOutput));
        adcSetRelay((*adc - '0'), atob((char*) adcRelay));
        setDefault((*adc - '0'), (*adcDefault - '0'));
        saveAppConfig();
    }
}

void HTTPPrint_an(WORD analog) {
    char str[8];
    INT16 adc = readADC(analog);
    itoa(adc, str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ah(WORD analog) {
    char str[8];
    itoa(getHighThreshold(analog), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_al(WORD analog) {
    char str[8];
    itoa(getLowThreshold(analog), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ao(WORD analog) {
    char str[8];
    btoa(adcGetOutput(analog), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ar(WORD analog) {
    char str[8];
    btoa(adcGetRelay(analog), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ad(WORD analog) {
    char str[8];
    btoa(getDefault(analog), str);
    TCPPutString(sktHTTP, (BYTE*) str);
}




