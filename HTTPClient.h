/* 
 * File:   HTTPClient.h
 * Author: Luca
 *
 * Created on 29 agosto 2016, 12.38
 */

#ifndef HTTPCLIENT_H
#define	HTTPCLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "TCPIP Stack/TCPIP.h"
#include "TCPIPConfig.h"
    
typedef enum {
    HTTPC_SM_HOME = 0,
    HTTPC_SM_IS_SELECTED,
    HTTPC_SM_INIT,
            
    HTTPC_SM_OPEN_SOCKET,
    HTTPC_SM_SOCKET_OBTAINED,
    HTTPC_SM_SEND_HTTP_REQEUST,
    HTTPC_SM_WAIT_SEND,
    HTTPC_SM_WAIT_REPSONE,
            
    HTTPC_SM_DISCONNECT,
    HTTPC_SM_ERROR,
    HTTPC_SM_DONE
} HTTP_CLIENT_STATE_MACHINE;

typedef enum {
    HTTPC_NO_ERROR = 0,
    HTTPC_CABLE_ERROR,
    HTTPC_ADDRESS_ERROR,
    HTTPC_USERNAME_ERROR,
    HTTPC_PASSWORD_ERROR,
    HTTPC_SOCKET_ERROR,
    HTTPC_TIMEOUT_ERROR,
    HTTPC_DISCONNECTION_ERROR
} HTTP_CLIENT_ERROR;

typedef struct __attribute__((__packed__)) {

    HTTP_CLIENT_STATE_MACHINE sm;       // HTTP Client state machine indicator
    HTTP_CLIENT_ERROR error;
    TCP_SOCKET socket;                  // TCP Socket
    DWORD timeout;                      // Timeout value
    BYTE num;
    BOOL stauts;
} HTTP_CLIENT_CONFIG;

void HTTPClientInit(void);
void HTTPClientTask(void);
void HTTPClientSendRequest(BYTE num, BOOL status);
BYTE HTTPClientIsReady(void);

#ifdef	__cplusplus
}
#endif

#endif	/* HTTPCLIENT_H */

