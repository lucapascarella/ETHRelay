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

#ifndef __DDNS_C
#define __DDNS_C

#include "ddns.h"
#include "TCPIP Stack/TCPIP.h"

void ddnsInit(void) {

    // Sets defaults for the system
    DDNSSetService(0);
    DDNSClient.Host.szROM = NULL;
    DDNSClient.Username.szROM = NULL;
    DDNSClient.Password.szROM = NULL;
    DDNSClient.ROMPointers.Host = FALSE;
    DDNSClient.ROMPointers.Username = FALSE;
    DDNSClient.ROMPointers.Password = FALSE;

    DDNSClient.Host.szRAM = appConfig.ip.fields.host;
    DDNSClient.Username.szRAM = appConfig.ip.fields.user;
    DDNSClient.Password.szRAM = appConfig.ip.fields.pass;

    // Convert to a service ID
    DDNSSetService(appConfig.ip.fields.service);

    // Since user name and password changed, force an update immediately
    if (appConfig.ip.fields.flags.bits.enableDDNS)
        DDNSForceUpdate();
}

#endif
