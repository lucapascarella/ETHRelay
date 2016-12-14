
#include "HTTPApp.h"
#include "TCPIPConfig.h"
#include "ddns.h"
#include "AppConfig.h"


/*****************************************************************************
  Function:
    static HTTP_IO_RESULT HTTPPostStream(void)
 *****************************************************************************/
HTTP_IO_RESULT HTTPPostDDNS(void) {


    BOOL saveOp = FALSE, applyOp = FALSE;

    if (curHTTP.byteCount > TCPIsGetReady(sktHTTP) + TCPGetRxFIFOFree(sktHTTP))
        goto ConfigFailure;

    // Ensure that all data is waiting to be parsed.  If not, keep waiting for
    // all of it to arrive.
    if (TCPIsGetReady(sktHTTP) < curHTTP.byteCount)
        return HTTP_IO_NEED_DATA;

    // Use current config in non-volatile memory as defaults
    loadThisAppConfig(&newAppConfig);
    // Start out assuming that Connetcts at startup is disabled.  This is necessary since the
    // browser doesn't submit this field if it is unchecked (meaning zero).
    // However, if it is checked, this will be overridden since it will be
    // submitted.
    newAppConfig.ip.fields.flags.bits.enableDDNS = 0;

    // Read all browser POST data
    while (curHTTP.byteCount) {
        // Read a form field name
        if (HTTPReadPostName(curHTTP.data, 6) != HTTP_READ_OK)
            goto ConfigFailure;

        // Read a form field value
        if (HTTPReadPostValue(curHTTP.data + 6, sizeof (curHTTP.data) - 6 - 2) != HTTP_READ_OK)
            goto ConfigFailure;

        // Parse the value that was read
        if (!strcmp((char*) curHTTP.data, (char*) "serv")) {
            // Read new DDNS Service value: 0 = DynDNS, 1 = No-IP, 2 = DNS-O-Matic
            newAppConfig.ip.fields.flags.bits.service = atoi((char*) curHTTP.data + 6);
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "user")) {
            // Read new Username
            strncpy((char*) newAppConfig.ip.fields.user, (char*) curHTTP.data + 6, DDNS_SIZE_USER);
            newAppConfig.ip.fields.user[DDNS_SIZE_USER - 1] = '\0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "pass")) {
            // Read new Password
            strncpy((char*) newAppConfig.ip.fields.pass, (char*) curHTTP.data + 6, DDNS_SIZE_PASS);
            newAppConfig.ip.fields.pass[DDNS_SIZE_PASS - 1] = '\0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "host")) {
            // Read new Host name
            strncpy((char*) newAppConfig.ip.fields.host, (char*) curHTTP.data + 6, DDNS_SIZE_HOST);
            newAppConfig.ip.fields.host[DDNS_SIZE_HOST - 1] = '\0';
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "ddns")) {
            if (curHTTP.data[6] == '1')
                newAppConfig.ip.fields.flags.bits.enableDDNS = 1;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "save")) {
            // Read save operation
            saveOp = TRUE;
        } else if (!strcmppgm2ram((char*) curHTTP.data, (ROM char*) "appl")) {
            // Read apply operation
            applyOp = TRUE;
        }

    }

    // All parsing complete!
    if (saveOp) {
        // Save new settings in external flash memory
        saveThisAppConfig(&newAppConfig);
    }
    if (applyOp) {
        // Apply new values
        strncpy((char*) appConfig.ip.fields.user, (char*) newAppConfig.ip.fields.user, DDNS_SIZE_USER);
        strncpy((char*) appConfig.ip.fields.pass, (char*) newAppConfig.ip.fields.pass, DDNS_SIZE_PASS);
        strncpy((char*) appConfig.ip.fields.host, (char*) newAppConfig.ip.fields.host, DDNS_SIZE_HOST);
        appConfig.ip.fields.flags.bits.service = newAppConfig.ip.fields.flags.bits.service;
        appConfig.ip.fields.flags.bits.enableDDNS = newAppConfig.ip.fields.flags.bits.enableDDNS;

        // Do a DDNS refresh
        ddnsInit();
    }

    return HTTP_IO_DONE;


ConfigFailure:
    //    lastFailure = TRUE;
    strcpypgm2ram((char*) curHTTP.data, (ROM char*) "/protect/error.htm");
    curHTTP.httpStatus = HTTP_REDIRECT;

    return HTTP_IO_DONE;
}

//void HTTPPrint_ddnsStatus(void) {
//    DDNS_STATUS s;
//    s = DDNSGetLastStatus();
//
//    switch (s) {
//        case DDNS_STATUS_GOOD: TCPPutString(sktHTTP, (BYTE*) "Update successful, hostname is now updated");
//            break;
//        case DDNS_STATUS_NOCHG: TCPPutString(sktHTTP, (BYTE*) "Update changed no setting and is considered abusive.  Additional 'nochg' updates will cause hostname to be blocked.");
//            break;
//
//        case DDNS_STATUS_ABUSE: TCPPutString(sktHTTP, (BYTE*) "The hostname specified is blocked for update abuse.");
//            break;
//        case DDNS_STATUS_BADSYS: TCPPutString(sktHTTP, (BYTE*) "System parameter not valid. Should be dyndns, statdns or custom.");
//            break;
//        case DDNS_STATUS_BADAGENT: TCPPutString(sktHTTP, (BYTE*) "The user agent was blocked or not sent.");
//            break;
//        case DDNS_STATUS_BADAUTH: TCPPutString(sktHTTP, (BYTE*) "The username and password pair do not match a real user.");
//            break;
//        case DDNS_STATUS_NOT_DONATOR: TCPPutString(sktHTTP, (BYTE*) "An option available only to credited users (such as offline URL) was specified, but the user is not a credited user. If multiple hosts were specified, only a single !donator will be returned.");
//            break;
//        case DDNS_STATUS_NOT_FQDN: TCPPutString(sktHTTP, (BYTE*) "The hostname specified is not a fully-qualified domain name (not in the form hostname.dyndns.org or domain.com).");
//            break;
//        case DDNS_STATUS_NOHOST: TCPPutString(sktHTTP, (BYTE*) "The hostname specified does not exist in this user account (or is not in the service specified in the system parameter).");
//            break;
//        case DDNS_STATUS_NOT_YOURS: TCPPutString(sktHTTP, (BYTE*) "The hostname specified does not belong to this user account.");
//            break;
//        case DDNS_STATUS_NUMHOST: TCPPutString(sktHTTP, (BYTE*) "Too many hosts specified in an update.");
//            break;
//        case DDNS_STATUS_DNSERR: TCPPutString(sktHTTP, (BYTE*) "Unspecified DNS error encountered by the DDNS service.");
//            break;
//        case DDNS_STATUS_911: TCPPutString(sktHTTP, (BYTE*) "There is a problem or scheduled maintenance with the DDNS service.");
//            break;
//
//        case DDNS_STATUS_UPDATE_ERROR: TCPPutString(sktHTTP, (BYTE*) "Error communicating with Update service.");
//            break;
//        case DDNS_STATUS_UNCHANGED: TCPPutString(sktHTTP, (BYTE*) "The IP Check indicated that no update was necessary.");
//            break;
//        case DDNS_STATUS_CHECKIP_ERROR: TCPPutString(sktHTTP, (BYTE*) "Error communicating with CheckIP service.");
//            break;
//        case DDNS_STATUS_INVALID: TCPPutString(sktHTTP, (BYTE*) "DDNS Client data is not valid.");
//            break;
//        case DDNS_STATUS_UNKNOWN: TCPPutString(sktHTTP, (BYTE*) "DDNS client has not yet been executed with this configuration.");
//            break;
//    }
//}

//void HTTPPrint_ddnsPublic(void) {
//    HTTPPrintIP(DDNSGetLastIP());
//}

//void HTTPPrint_ddnsEnabled(void) {
//    if (appConfig.ip.fields.flags.bits.bEnableDDNS)
//        TCPPutString(sktHTTP, (BYTE*) "checked");
//}

void HTTPPrint_ddns_user(void) {
#if defined(STACK_USE_DYNAMICDNS_CLIENT)
    if (DDNSClient.ROMPointers.Username || !DDNSClient.Username.szRAM)
        return;
    if (curHTTP.callbackPos == 0x00u)
        curHTTP.callbackPos = (PTR_BASE) DDNSClient.Username.szRAM;
    curHTTP.callbackPos = (PTR_BASE) TCPPutString(sktHTTP, (BYTE*) (PTR_BASE) curHTTP.callbackPos);
    if (*(BYTE*) (PTR_BASE) curHTTP.callbackPos == '\0')
        curHTTP.callbackPos = 0x00;
#endif
}

void HTTPPrint_ddns_pass(void) {
#if defined(STACK_USE_DYNAMICDNS_CLIENT)
    if (DDNSClient.ROMPointers.Password || !DDNSClient.Password.szRAM)
        return;
    if (curHTTP.callbackPos == 0x00u)
        curHTTP.callbackPos = (PTR_BASE) DDNSClient.Password.szRAM;
    curHTTP.callbackPos = (PTR_BASE) TCPPutString(sktHTTP, (BYTE*) (PTR_BASE) curHTTP.callbackPos);
    if (*(BYTE*) (PTR_BASE) curHTTP.callbackPos == '\0')
        curHTTP.callbackPos = 0x00;
#endif
}

void HTTPPrint_ddns_host(void) {
#if defined(STACK_USE_DYNAMICDNS_CLIENT)
    if (DDNSClient.ROMPointers.Host || !DDNSClient.Host.szRAM)
        return;
    if (curHTTP.callbackPos == 0x00u)
        curHTTP.callbackPos = (PTR_BASE) DDNSClient.Host.szRAM;
    curHTTP.callbackPos = (PTR_BASE) TCPPutString(sktHTTP, (BYTE*) (PTR_BASE) curHTTP.callbackPos);
    if (*(BYTE*) (PTR_BASE) curHTTP.callbackPos == '\0')
        curHTTP.callbackPos = 0x00;
#endif
}

//void HTTPPrint_ddnsService(WORD i) {
//    extern ROM char * ROM ddnsServiceHosts[];
//#if defined(STACK_USE_DYNAMICDNS_CLIENT)
//    if (!DDNSClient.ROMPointers.UpdateServer || !DDNSClient.UpdateServer.szROM)
//        return;
//    if ((ROM char*) DDNSClient.UpdateServer.szROM == ddnsServiceHosts[i])
//        TCPPutROMString(sktHTTP, (ROM BYTE*) "selected");
//#endif
//}

void HTTPPrint_ddns_status(void) {
#if defined(STACK_USE_DYNAMICDNS_CLIENT)
    DDNS_STATUS s;
    s = DDNSGetLastStatus();
    if (s == DDNS_STATUS_GOOD || s == DDNS_STATUS_UNCHANGED || s == DDNS_STATUS_NOCHG)
        TCPPutROMString(sktHTTP, (ROM BYTE*) "ok");
    else if (s == DDNS_STATUS_UNKNOWN)
        TCPPutROMString(sktHTTP, (ROM BYTE*) "unk");
    else
        TCPPutROMString(sktHTTP, (ROM BYTE*) "fail");
#else
    TCPPutROMString(sktHTTP, (ROM BYTE*) "fail");
#endif
}

void HTTPPrint_ddns_status_msg(void) {
    if (TCPIsPutReady(sktHTTP) < 75u) {
        curHTTP.callbackPos = 0x01;
        return;
    }

#if defined(STACK_USE_DYNAMICDNS_CLIENT)
    switch (DDNSGetLastStatus()) {
        case DDNS_STATUS_GOOD:
        case DDNS_STATUS_NOCHG:
            TCPPutROMString(sktHTTP, (ROM BYTE*) "The last update was successful.");
            break;
        case DDNS_STATUS_UNCHANGED:
            TCPPutROMString(sktHTTP, (ROM BYTE*) "The IP has not changed since the last update.");
            break;
        case DDNS_STATUS_UPDATE_ERROR:
        case DDNS_STATUS_CHECKIP_ERROR:
            TCPPutROMString(sktHTTP, (ROM BYTE*) "Could not communicate with DDNS server.");
            break;
        case DDNS_STATUS_INVALID:
            TCPPutROMString(sktHTTP, (ROM BYTE*) "The current configuration is not valid.");
            break;
        case DDNS_STATUS_UNKNOWN:
            TCPPutROMString(sktHTTP, (ROM BYTE*) "The Dynamic DNS client is pending an update.");
            break;
        default:
            TCPPutROMString(sktHTTP, (ROM BYTE*) "An error occurred during the update.<br />The DDNS Client is suspended.");
            break;
    }
#else
    TCPPutROMString(sktHTTP, (ROM BYTE*) "The Dynamic DNS Client is not enabled.");
#endif

    curHTTP.callbackPos = 0x00;
}