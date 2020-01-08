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
#include "AppConfig.h"
#include "TCPIPConfig.h"
#include "ddns.h"
#include "Ping.h"
#include "Board.h"

void HTTPPrintIP(IP_ADDR ip);

HTTP_IO_RESULT HTTPPostLocalNetwork(void) {

    BYTE *ptr;
    BYTE i;
    BOOL saveOp = FALSE, applyOp = FALSE, rebootOp = FALSE;

    // Check to see if the browser is attempting to submit more data than we
    // can parse at once.  This function needs to receive all updated
    // parameters and validate them all before committing them to memory so that
    // orphaned configuration parameters do not get written (for example, if a
    // static IP address is given, but the subnet mask fails parsing, we
    // should not use the static IP address).  Everything needs to be processed
    // in a single transaction.  If this is impossible, fail and notify the user.
    // As a web devloper, if you add parameters to AppConfig and run into this
    // problem, you could fix this by to splitting your update web page into two
    // seperate web pages (causing two transactional writes).  Alternatively,
    // you could fix it by storing a static shadow copy of AppConfig someplace
    // in memory and using it instead of newAppConfig.  Lastly, you could
    // increase the TCP RX FIFO size for the HTTP server.  This will allow more
    // data to be POSTed by the web browser before hitting this limit.
    if (curHTTP.byteCount > TCPIsGetReady(sktHTTP) + TCPGetRxFIFOFree(sktHTTP))
        goto ConfigFailure;

    // Ensure that all data is waiting to be parsed.  If not, keep waiting for
    // all of it to arrive.
    if (TCPIsGetReady(sktHTTP) < curHTTP.byteCount)
        return HTTP_IO_NEED_DATA;

    // Use current appConfig in non-volatile memory as defaults
    //loadThisAppConfig(&newAppConfig);
    cloneAppConfig(&appConfig, &newAppConfig);

    // Read all browser POST data
    while (curHTTP.byteCount) {
        // Read a form field name
        if (HTTPReadPostName(curHTTP.data, 6) != HTTP_READ_OK)
            goto ConfigFailure;

        // Read a form field value
        if (HTTPReadPostValue(curHTTP.data + 6, sizeof (curHTTP.data) - 6 - 2) != HTTP_READ_OK)
            goto ConfigFailure;

        // Parse the value that was read
        if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "mac")) {
            // Read new MAC address
            WORD_VAL w;
            BYTE i;

            ptr = curHTTP.data + 6;

            for (i = 0; i < 12u; i++) {// Read the MAC address
                // Skip non-hex bytes
                while (*ptr != 0x00u && !(*ptr >= '0' && *ptr <= '9') && !(*ptr >= 'A' && *ptr <= 'F') && !(*ptr >= 'a' && *ptr <= 'f'))
                    ptr++;
                // MAC string is over, so zeroize the rest
                if (*ptr == 0x00u) {
                    for (; i < 12u; i++)
                        curHTTP.data[i] = '0';
                    break;
                }
                // Save the MAC byte
                curHTTP.data[i] = *ptr++;
            }
            // Read MAC Address, one byte at a time
            for (i = 0; i < 6u; i++) {
                w.v[1] = curHTTP.data[i * 2];
                w.v[0] = curHTTP.data[i * 2 + 1];
                newAppConfig.ip.fields.MyMACAddr.v[i] = hexatob(w);
            }
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "host")) {// Read new hostname
            FormatNetBIOSName(&curHTTP.data[6]);
            memcpy((void*) newAppConfig.ip.fields.NetBIOSName, (void*) curHTTP.data + 6, 16);
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "icmp")) {// Read new ICMP Enabled flag
            newAppConfig.ip.fields.flags.bits.bIsICMPServerEnabled = curHTTP.data[6] - '0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dhcp")) {// Read new DHCP Enabled flag
            newAppConfig.ip.fields.flags.bits.bIsDHCPEnabled = curHTTP.data[6] - '0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "ip")) {// Read new static IP Address
            if (!StringToIPAddress(curHTTP.data + 6, &newAppConfig.ip.fields.MyIPAddr))
                goto ConfigFailure;
            newAppConfig.ip.fields.DefaultIPAddr.Val = newAppConfig.ip.fields.MyIPAddr.Val;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "gw")) {// Read new gateway address
            if (!StringToIPAddress(curHTTP.data + 6, &newAppConfig.ip.fields.MyGateway))
                goto ConfigFailure;
            newAppConfig.ip.fields.MyGateway.Val = newAppConfig.ip.fields.MyGateway.Val;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "sub")) {// Read new static subnet
            if (!StringToIPAddress(curHTTP.data + 6, &newAppConfig.ip.fields.MyMask))
                goto ConfigFailure;
            newAppConfig.ip.fields.DefaultMask.Val = newAppConfig.ip.fields.MyMask.Val;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dns1")) {// Read new primary DNS server
            if (!StringToIPAddress(curHTTP.data + 6, &newAppConfig.ip.fields.PrimaryDNSServer))
                goto ConfigFailure;
            newAppConfig.ip.fields.PrimaryDNSServer.Val = newAppConfig.ip.fields.PrimaryDNSServer.Val;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dns2")) {// Read new secondary DNS server
            if (!StringToIPAddress(curHTTP.data + 6, &newAppConfig.ip.fields.SecondaryDNSServer))
                goto ConfigFailure;
            newAppConfig.ip.fields.SecondaryDNSServer.Val = newAppConfig.ip.fields.SecondaryDNSServer.Val;

//        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "smse")) {
//            XEEBeginWrite(XEEPROM_SMTP_SERVER_ADDRESS);
//            XEEWriteArray((BYTE*) curHTTP.data + 6, XEEPROM_SMTP_SERVER_SIZE);
//            XEEEndWrite();
//        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "smpo")) {
//            newAppConfig.ip.fields.smtpPort = atoi((char*) curHTTP.data + 6);
//        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "smus")) {
//            XEEBeginWrite(XEEPROM_SMTP_USER_ADDRESS);
//            XEEWriteArray((BYTE*) curHTTP.data + 6, XEEPROM_SMTP_USER_SIZE);
//            XEEEndWrite();
//        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "smpa")) {
//            XEEBeginWrite(XEEPROM_SMTP_PASS_ADDRESS);
//            XEEWriteArray((BYTE*) curHTTP.data + 6, XEEPROM_SMTP_PASS_SIZE);
//            XEEEndWrite();
//        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "smto")) {
//            XEEBeginWrite(XEEPROM_SMTP_TO_ADDRESS);
//            XEEWriteArray((BYTE*) curHTTP.data + 6, XEEPROM_SMTP_TO_SIZE);
//            XEEEndWrite();
//        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "smsu")) {
//            XEEBeginWrite(XEEPROM_SMTP_SUBJECT_ADDRESS);
//            XEEWriteArray((BYTE*) curHTTP.data + 6, XEEPROM_SMTP_SUBJECT_SIZE);
//            XEEEndWrite();

        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "edns")) {
            // Read new DDNS enable flag
            newAppConfig.ip.fields.flags.bits.enableDDNS = curHTTP.data[6] - '0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dsrv")) {
            // Read new DDNS Service value: 0 = DynDNS, 1 = No-IP, 2 = DNS-O-Matic
            newAppConfig.ip.fields.service = atoi((char*) curHTTP.data + 6);
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dhst")) {
            // Read new DDNS Host name
            strlcpy((char*) newAppConfig.ip.fields.host, (char*) curHTTP.data + 6, DDNS_SIZE_HOST);
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dusr")) {
            // Read new DDNS Username
            strlcpy((char*) newAppConfig.ip.fields.user, (char*) curHTTP.data + 6, SIZE_USERNAME);
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "dpss")) {
            // Read new DDNS Password
            strlcpy((char*) newAppConfig.ip.fields.pass, (char*) curHTTP.data + 6, DDNS_SIZE_PASS);
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "save")) {
            // Read "Save" request
            saveOp = TRUE;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "appl")) {
            // Read "Apply" request
            applyOp = TRUE;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (rom char*) "rbt")) {
            // Read "Apply" request
            rebootOp = TRUE;
        }
    }

    // All parsing are completed!
    if (saveOp) {
        // Do a action to Save request
        saveThisAppConfig(&newAppConfig);
    }
    // In Local Network the apply only button is disabled so Apply do a reboot
    if (applyOp) {
        cloneAppConfig(&newAppConfig, &appConfig);
        // Do a DDNS refresh
        ddnsInit();
    }
    if (rebootOp) {
        // Do a action to Apply request
        askRebootInMilliSeconds(1500);
        // Set the board to reboot and display reconnecting information
        strcpypgm2ram((char*) curHTTP.data, (rom char*) "/prt/localreboot.htm?");
        memcpy((void*) (curHTTP.data + 21), (void*) newAppConfig.ip.fields.NetBIOSName, 16);
        curHTTP.data[21 + 16] = 0x00; // Force null termination
        for (i = 21; i < 21u + 16u; i++) {
            if (curHTTP.data[i] == ' ')
                curHTTP.data[i] = 0x00;
        }
        curHTTP.httpStatus = HTTP_REDIRECT;
    }

    return HTTP_IO_DONE;

ConfigFailure:
    //    lastFailure = TRUE;
    strcpypgm2ram((char*) curHTTP.data, (rom char*) "/prt/local.htm");
    curHTTP.httpStatus = HTTP_REDIRECT;

    return HTTP_IO_DONE;
}

void HTTPGetPing(HTTP_CONN * curHTTP) {

    BYTE *ptr, *ptr2;
    // Determine which commands are requests
    if ((ptr = HTTPGetROMArg(curHTTP->data, (rom BYTE *) "act"))) {
        if (strcmppgm2ram((char*) ptr, (rom char*) "ping") == 0) {
            if ((ptr2 = HTTPGetROMArg(curHTTP->data, (rom BYTE *) "res"))) {
                pingSaveHostAddress((char*) ptr2);
                setGatewayPing(FALSE);
                sendPing();
            }
        } else if (strcmppgm2ram((char*) ptr, (rom char*) "url") == 0) {
            if ((ptr2 = HTTPGetROMArg(curHTTP->data, (rom BYTE *) "res"))) {
                pingSaveHostAddress((char*) ptr2);
                setGatewayPing(FALSE);
                sendPing();
            }
        }
    }
}

/*
 * Ping fields
 */
void HTTPPrint_pingIsComp(void) {
    TCPPut(sktHTTP, pingIsComplete() ? '1' : '0');
}

void HTTPPrint_pingIsTimeout(void) {
    TCPPut(sktHTTP, pingIsTimeout() ? '1' : '0');
}

void HTTPPrint_pingIsUnresolved(void) {
    TCPPut(sktHTTP, pingIsUnresolved() ? '1' : '0');
}

void HTTPPrint_pingTime(void) {
    BYTE str[8];
    itoa(pingTime(), (char*) str);
    TCPPutString(sktHTTP, str);
}

void HTTPPrint_pingAdd(void) {
    TCPPutString((TCP_SOCKET) sktHTTP, (BYTE*) pingGetAddress());
}

/*
 * Local network fields
 */
void HTTPPrint_mac(void) {
    BYTE i;

    if (TCPIsPutReady(sktHTTP) < 18u) {//need 17 bytes to write a MAC
        curHTTP.callbackPos = 0x01;
        return;
    }

    // Write each byte
    for (i = 0; i < 6u; i++) {
        if (i)
            TCPPut(sktHTTP, ':');
        TCPPut(sktHTTP, btohexa_high(appConfig.ip.fields.MyMACAddr.v[i]));
        TCPPut(sktHTTP, btohexa_low(appConfig.ip.fields.MyMACAddr.v[i]));
    }

    // Indicate that we're done
    curHTTP.callbackPos = 0x00;
}

void HTTPPrint_hostname(void) {
    TCPPutString(sktHTTP, appConfig.ip.fields.NetBIOSName);
}

void HTTPPrint_icmp(void) {
    // 0 = Disabled, 1 = Enabled
    TCPPut(sktHTTP, (BYTE) (appConfig.ip.fields.flags.bits.bIsICMPServerEnabled + '0'));
}

void HTTPPrint_dhcp(void) {
    // 0 = Disabled, 1 = Enabled
    TCPPut(sktHTTP, (BYTE) (appConfig.ip.fields.flags.bits.bIsDHCPEnabled + '0'));
}

void HTTPPrint_ip(void) {
    HTTPPrintIP(appConfig.ip.fields.MyIPAddr);
}

void HTTPPrint_gw(void) {
    HTTPPrintIP(appConfig.ip.fields.MyGateway);
}

void HTTPPrint_subnet(void) {
    HTTPPrintIP(appConfig.ip.fields.MyMask);
}

void HTTPPrint_dns1(void) {
    HTTPPrintIP(appConfig.ip.fields.PrimaryDNSServer);
}

void HTTPPrint_dns2(void) {
    HTTPPrintIP(appConfig.ip.fields.SecondaryDNSServer);
}

void HTTPPrintIP(IP_ADDR ip) {
    BYTE digits[4];
    BYTE i;

    for (i = 0; i < 4u; i++) {
        if (i)
            TCPPut(sktHTTP, '.');
        uitoa(ip.v[i], digits);
        TCPPutString(sktHTTP, digits);
    }
}

///*
// * SMTP fields
// */
//void HTTPPrint_smtpStatus(void) {
//    BYTE str[8];
//    WORD error;
//    error = emailGetLastStatus();
//    itoa(error, (char*) str);
//    TCPPutString(sktHTTP, str);
//}
//
//void HTTPPrint_smtpServer(void) {
//    BYTE i;
//    XEEReadArray(XEEPROM_SMTP_SERVER_ADDRESS, curHTTP.data, XEEPROM_SMTP_SERVER_SIZE - 1);
//    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_SMTP_SERVER_SIZE - 1;
//    curHTTP.data[i] = '\0';
//    TCPPutString(sktHTTP, curHTTP.data);
//}
//
//void HTTPPrint_smtpPort(void) {
//    BYTE str[8];
//    itoa(appConfig.ip.fields.smtpPort, (char*) str);
//    TCPPutString(sktHTTP, str);
//}
//
//void HTTPPrint_smtpUser(void) {
//    BYTE i;
//    XEEReadArray(XEEPROM_SMTP_USER_ADDRESS, curHTTP.data, XEEPROM_SMTP_USER_SIZE - 1);
//    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_SMTP_USER_SIZE - 1;
//    curHTTP.data[i] = '\0';
//    TCPPutString(sktHTTP, curHTTP.data);
//}
//
//void HTTPPrint_smtpPass(void) {
//    BYTE i;
//    XEEReadArray(XEEPROM_SMTP_PASS_ADDRESS, curHTTP.data, XEEPROM_SMTP_PASS_SIZE - 1);
//    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_SMTP_PASS_SIZE - 1;
//    curHTTP.data[i] = '\0';
//    TCPPutString(sktHTTP, curHTTP.data);
//}
//
//void HTTPPrint_smtpTo(void) {
//    BYTE i;
//    XEEReadArray(XEEPROM_SMTP_TO_ADDRESS, curHTTP.data, XEEPROM_SMTP_TO_SIZE - 1);
//    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_SMTP_TO_SIZE - 1;
//    curHTTP.data[i] = '\0';
//    TCPPutString(sktHTTP, curHTTP.data);
//}
//
//void HTTPPrint_smtpSubject(void) {
//    BYTE i;
//    XEEReadArray(XEEPROM_SMTP_SUBJECT_ADDRESS, curHTTP.data, XEEPROM_SMTP_SUBJECT_SIZE - 1);
//    i = curHTTP.data[0] == 0xFF ? 0 : XEEPROM_SMTP_SUBJECT_SIZE - 1;
//    curHTTP.data[i] = '\0';
//    TCPPutString(sktHTTP, curHTTP.data);
//}

/*
 * DDNS fields
 */
void HTTPPrint_ddnsStatus(void) {
    BYTE str[8];
    DDNS_STATUS s;
    s = DDNSGetLastStatus();

    itoa(s, (char*) str);
    TCPPutString(sktHTTP, str);
}

void HTTPPrint_ddnsPublic(void) {
    HTTPPrintIP(DDNSGetLastIP());
}

void HTTPPrint_ddnsEnabled(void) {
    TCPPut(sktHTTP, appConfig.ip.fields.flags.bits.enableDDNS + '0');
}

void HTTPPrint_ddnsService(void) {
    //    extern char * ddnsServiceHosts[];
    //    WORD i;
    //    if (!DDNSClient.ROMPointers.UpdateServer || !DDNSClient.UpdateServer.szROM)
    //        return;
    //    if ((char*) DDNSClient.UpdateServer.szROM == ddnsServiceHosts[i])
    //        TCPPutString(sktHTTP, (BYTE*) "selected");
    //    // TODO return an integer value

    BYTE str[8];
    itoa((int) appConfig.ip.fields.service, (char*) str);
    TCPPutString(sktHTTP, (BYTE*) str);
}

void HTTPPrint_ddnsHost(void) {
    TCPPutString(sktHTTP, appConfig.ip.fields.host);
}

void HTTPPrint_ddnsUser(void) {
    TCPPutString(sktHTTP, appConfig.ip.fields.user);
}

void HTTPPrint_ddnsPass(void) {
    TCPPutString(sktHTTP, appConfig.ip.fields.pass);
}

