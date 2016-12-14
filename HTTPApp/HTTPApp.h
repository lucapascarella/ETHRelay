/* 
 * File:   HTTPApp.h
 * Author: Luca
 *
 * Created on 13 aprile 2016, 15.23
 */

#ifndef HTTPAPP_H
#define	HTTPAPP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"
#include "TCPIP Stack/HTTP2.h"
    
HTTP_IO_RESULT HTTPPostAuthentication(void);
HTTP_IO_RESULT HTTPPostLocalNetwork(void);
HTTP_IO_RESULT HTTPPostIO(void);

void HTTPGetPing(HTTP_CONN * curHTTP);
void HTTPAppGetIO(HTTP_CONN *curHTTP);
void HTTPAppGetRelay(HTTP_CONN *curHTTP);
void HTTPAppGetAnalog(HTTP_CONN *curHTTP);
void HTTPGetDateTime(HTTP_CONN *curHTTP);

size_t strlcpy(char *dst, const char *src, size_t size);

#ifdef	__cplusplus
}
#endif

#endif	/* HTTPAPP_H */

