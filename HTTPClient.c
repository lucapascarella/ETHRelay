
#include "HTTPClient.h"
#include "TCPIP Stack/XEEPROM.h"
#include "IO.h"

HTTP_CLIENT_CONFIG hc;

void HTTPClientInit(void) {
    hc.sm = HTTPC_SM_HOME;
    hc.error = HTTPC_NO_ERROR;
    hc.socket = INVALID_SOCKET;
    hc.num = 0;
}

void HTTPClientTask(void) {

    static BYTE str[64];
    BYTE i, j, destination[32];
    WORD length;

    switch (hc.sm) {
        case HTTPC_SM_HOME:
            hc.sm++;
            break;

        case HTTPC_SM_IS_SELECTED:
            if (hc.num == 0)
                break;
            hc.sm++;
            break;

        case HTTPC_SM_INIT:
            // Read address from XEEPROM 
            XEEReadArray(XEEPROM_ADDRESS((WORD) (hc.num - 1)), str, XEEPROM_ADDRESS_SIZE - 1);
            i = str[0] == 0xFF ? 0 : XEEPROM_ADDRESS_SIZE - 1;
            str[i] = '\0';
            if (str[0] == '\0') {
                hc.error = HTTPC_ADDRESS_ERROR;
                hc.sm = HTTPC_SM_ERROR;
                break;
            }
            hc.sm++;
            break;

        case HTTPC_SM_OPEN_SOCKET:
            // The following code does not work
            ////            if (MACIsLinked()) {
            ////                hc.error = HTTPC_CABLE_ERROR;
            ////                hc.sm = HTTPC_SM_ERROR;
            ////                break;
            ////            }
            // Open a socket connection
            hc.socket = TCPOpen((DWORD) & str[0], TCP_OPEN_RAM_HOST, HTTP_PORT, TCP_PURPOSE_HTTP_CLIENT);
            hc.timeout = TickGet();
            hc.error = HTTPC_NO_ERROR;
            hc.sm++;
            break;

        case HTTPC_SM_SOCKET_OBTAINED:
            // Time out if too much time is spent in this state
            if (TickGet() - hc.timeout > 20 * TICK_SECOND) {
                // Close the socket so it can be used by other modules
                hc.error = HTTPC_TIMEOUT_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break;
            }
            // Abort operation if no TCP socket of type TCP_PURPOSE_GENERIC_TCP_CLIENT is available
            // If this ever happens, you need to go add one to TCPIPConfig.h
            // This must be first, to allow check socket validity
            if (hc.socket == INVALID_SOCKET) {
                hc.error = HTTPC_SOCKET_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break; // This is an error catch that
            }
            // Wait for the remote server to accept our connection request
            if (!TCPIsConnected(hc.socket)) {
                // Always do break, if no connection is established yet
                break;
            }

            // Adjust FIFO sizes to half and half. Default state must remain
            TCPAdjustFIFOSize(hc.socket, 32, 32, TCP_ADJUST_GIVE_REST_TO_TX);

            hc.timeout = TickGet();
            hc.sm++;
            break;

        case HTTPC_SM_SEND_HTTP_REQEUST:
            // Time out if too much time is spent in this state
            if (TickGet() - hc.timeout > 20 * TICK_SECOND) {
                // Close the socket so it can be used by other modules
                hc.error = HTTPC_TIMEOUT_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break;
            }
            // Make certain the socket can be written to
            if (TCPIsPutReady(hc.socket) < 125u)
                break;
            // Read username and password from XEEPROM and put them in the same string
            XEEReadArray(XEEPROM_USERNAME((WORD) (hc.num - 1)), str, XEEPROM_USERNAME_SIZE - 1);
            i = str[0] == 0xFF ? 0 : XEEPROM_USERNAME_SIZE - 1;
            str[i] = '\0';
            if (str[0] == '\0') {
                hc.error = HTTPC_USERNAME_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break;
            }
            i = strlen((char*) str);
            str[i++] = ':';
            str[i] = '\0';
            j = i;
            XEEReadArray(XEEPROM_PASSWORD((WORD) (hc.num - 1)), &str[i], XEEPROM_PASSWORD_SIZE - 1);
            i += str[0] == 0xFF ? 0 : XEEPROM_PASSWORD_SIZE - 1;
            str[i] = '\0';
            if (str[j] == '\0') {
                hc.error = HTTPC_PASSWORD_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break;
            }
            // Encode Username and Password in Base64 format
            length = Base64Encode((BYTE*) str, strlen((char*) str), (BYTE*) destination, sizeof (destination));

            TCPPutROMString(hc.socket, (ROM BYTE*) "GET ");
            if (appConfig.gpio.fields.ioBits[hc.num - 1].ioNotice <= 8) {
                TCPPutROMString(hc.socket, (ROM BYTE*) "/prt/io.cgi");
                TCPPutROMString(hc.socket, (ROM BYTE*) "?io=");
                btoa(appConfig.gpio.fields.ioBits[hc.num - 1].ioNotice, (char*) str);
                TCPPutString(hc.socket, str);
            } else {
                TCPPutROMString(hc.socket, (ROM BYTE*) "/prt/relay.cgi");
                TCPPutROMString(hc.socket, (ROM BYTE*) "?relay=");
                btoa(appConfig.gpio.fields.ioBits[hc.num - 1].ioNotice - 8, (char*) str);
                TCPPutString(hc.socket, str);
            }
            TCPPutROMString(hc.socket, (ROM BYTE*) "&act=");
            btoa(hc.stauts, (char*) str);
            TCPPutString(hc.socket, str);
            TCPPutROMString(hc.socket, (ROM BYTE*) " HTTP/1.0\r\nHost: ");
            // Read address from XEEPROM 
            XEEReadArray(XEEPROM_ADDRESS((WORD) (hc.num - 1)), str, XEEPROM_ADDRESS_SIZE - 1);
            i = str[0] == 0xFF ? 0 : XEEPROM_ADDRESS_SIZE - 1;
            str[i] = '\0';
            TCPPutString(hc.socket, str);
            TCPPutROMString(hc.socket, (ROM BYTE*) "\r\nAuthorization: Basic ");
            TCPPutArray(hc.socket, destination, length);
            TCPPutROMString(hc.socket, (ROM BYTE*) "\r\nConnection: close\r\n\r\n");

            // Send the packet
            TCPFlush(hc.socket);
            hc.timeout = TickGet();
            hc.sm++;
            break;

        case HTTPC_SM_WAIT_SEND:
            // Time out if too much time is spent in this state
            if (TickGet() - hc.timeout > 20 * TICK_SECOND) {
                // Close the socket so it can be used by other modules
                hc.error = HTTPC_TIMEOUT_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break;
            }
            if (!TCPIsConnected(hc.socket)) {
                hc.error = HTTPC_DISCONNECTION_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                break;
            }
            if ((length = TCPGetTxFIFOFull(hc.socket)) == 0) {
                TCPAdjustFIFOSize(hc.socket, 16, 16, TCP_ADJUST_GIVE_REST_TO_RX);
                hc.timeout = TickGet();
                hc.sm++;
            }
            break;

        case HTTPC_SM_WAIT_REPSONE:
            // Time out if too much time is spent in this state
            if (TickGet() - hc.timeout > 20 * TICK_SECOND) {
                // Close the socket so it can be used by other modules
                hc.error = HTTPC_TIMEOUT_ERROR;
                hc.sm = HTTPC_SM_DISCONNECT;
                // No break here
                // break;
            }
            if ((length = TCPIsGetReady(hc.socket)) > 0) {
                if ((length = TCPFindROMArrayEx(hc.socket, (ROM BYTE*) "\r\n\r\n", 4, 0, 0, FALSE)) != 0xFFFF) {
                    length += 4;
                    // "\r\n\r\n" is found
                    hc.sm = HTTPC_SM_DISCONNECT;
                }
            }
            break;


        case HTTPC_SM_DISCONNECT:
            if (hc.socket != INVALID_SOCKET)
                //if (TCPIsConnected(hc.socket))
                TCPClose(hc.socket);
            hc.socket = INVALID_SOCKET;
            hc.sm = HTTPC_SM_DONE;
            break;

        case HTTPC_SM_ERROR:
            hc.sm = HTTPC_SM_DISCONNECT;
            break;

        case HTTPC_SM_DONE:
            hc.num = 0;
            hc.sm = HTTPC_SM_HOME;
            break;
    }
}

void HTTPClientSendRequest(BYTE num, BOOL status) {
    hc.num = num;
    hc.stauts = status;
}

BYTE HTTPClientIsReady(void) {
    return hc.num;
}
