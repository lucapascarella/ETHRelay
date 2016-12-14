/*********************************************************************
 *
 *  Application to Demo HTTP2 Server
 *  Support for HTTP2 module in Microchip TCP/IP Stack
 *	 -Implements the application 
 *	 -Reference: RFC 1002
 *
 *********************************************************************
 * FileName:        CustomHTTPApp.c
 * Dependencies:    TCP/IP stack
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Elliott Wood     	6/18/07	Original
 ********************************************************************/
#define __CUSTOMHTTPAPP_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_HTTP2_SERVER)

#include "TCPIP Stack/TCPIP.h"
#include "HTTPApp/HTTPApp.h"

#include "AppConfig.h"

#define cmin(a, b)	((a<b)?a:b)
#define cmax(a, b)	((a>b)?a:b)


/****************************************************************************
  Section:
    Function Prototypes and Memory Globalizers
 ***************************************************************************/



extern APP_CONFIG appConfig;


unsigned short ASCIItoNumeric(unsigned char*);



/*****************************************************************************
  Function:
    BYTE HTTPNeedsAuth(BYTE* cFile)
	
  Internal:
    See documentation in the TCP/IP Stack API or HTTP2.h for details.
 ***************************************************************************/
#if defined(HTTP_USE_AUTHENTICATION)

BYTE HTTPNeedsAuth(BYTE* cFile) {
    // If the filename begins with the folder "protect", then require auth
    if (memcmppgm2ram(cFile, (ROM void*) "prt", 3) == 0)
        return 0x00; // Authentication will be needed later

#if defined(HTTP_MPFS_UPLOAD_REQUIRES_AUTH)
    if (memcmppgm2ram(cFile, (ROM void*) "mpfsupload", 10) == 0)
        return 0x00;
#endif

    // You can match additional strings here to password protect other files.
    // You could switch this and exclude files from authentication.
    // You could also always return 0x00 to require auth for all files.
    // You can return different values (0x00 to 0x79) to track "realms" for below.

    return 0x80; // No authentication required
}
#endif

/*****************************************************************************
  Function:
    BYTE HTTPCheckAuth(BYTE* cUser, BYTE* cPass)
	
  Internal:
    See documentation in the TCP/IP Stack API or HTTP2.h for details.
 ***************************************************************************/
#if defined(HTTP_USE_AUTHENTICATION)

BYTE HTTPCheckAuth(BYTE* cUser, BYTE* cPass) {
    int minUserLength, minPassLength;
    //if(strcmppgm2ram((char *)cUser,(ROM char *)"admin") == 0
    //	&& strcmppgm2ram((char *)cPass, (ROM char *)"microchip") == 0)
    //	return 0x80;		// We accept this combination

    minUserLength = cmin(cmax(strlen((char*) cUser), strlen((char*) appConfig.auth.fields.user.username)), SIZE_USERNAME);
    minPassLength = cmin(cmax(strlen((char*) cPass), strlen((char*) appConfig.auth.fields.user.password)), SIZE_PASSWORD);

    if (strncmp((char*) cUser, (char*) appConfig.auth.fields.user.username, minUserLength) == 0 && strncmp((char*) cPass, (char*) appConfig.auth.fields.user.password, minPassLength) == 0)
        return 0x80; // We accept this combination


    // You can add additional user/pass combos here.
    // If you return specific "realm" values above, you can base this
    //   decision on what specific file or folder is being accessed.
    // You could return different values (0x80 to 0xff) to indicate
    //   various users or groups, and base future processing decisions
    //   in HTTPExecuteGet/Post or HTTPPrint callbacks on this value.

    return 0x00; // Provided user/pass is invalid
}
#endif

/****************************************************************************
  Section:
    GET Form Handlers
 ***************************************************************************/

/*****************************************************************************
  Function:
    HTTP_IO_RESULT HTTPExecuteGet(void)
	
  Internal:
    See documentation in the TCP/IP Stack API or HTTP2.h for details.
 ***************************************************************************/
HTTP_IO_RESULT HTTPExecuteGet(void) {
    BYTE *ptr;
    BYTE filename[20];

    // Load the file name
    // Make sure BYTE filename[] above is large enough for your longest name
    MPFSGetFilename(curHTTP.file, filename, sizeof (filename));

    // If its the forms.htm page
    if (!memcmppgm2ram(filename, "forms.htm", 9)) {
        // Seek out each of the four LED strings, and if it exists set the LED states
        ptr = HTTPGetROMArg(curHTTP.data, (ROM BYTE *) "led2");
        if (ptr)
            LED_0_O = (*ptr == '1');

        ptr = HTTPGetROMArg(curHTTP.data, (ROM BYTE *) "led1");
        if (ptr)
            LED_0_O = (*ptr == '1');
    }// If it's the LED updater file

    if (!memcmppgm2ram(filename, "prt/local.cgi", 13)) {
        HTTPGetPing(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/date.cgi", 12)) {
        HTTPGetDateTime(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/relay.htm", 13)) {
        HTTPAppGetRelay(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/relay.cgi", 13)) {
        HTTPAppGetRelay(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/analog.htm", 14)) {
        HTTPAppGetAnalog(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/analog.cgi", 14)) {
        HTTPAppGetAnalog(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/io.htm", 10)) {
        HTTPAppGetIO(&curHTTP);
    } else if (!memcmppgm2ram(filename, "prt/io.cgi", 10)) {
        HTTPAppGetIO(&curHTTP);
    }
    return HTTP_IO_DONE;
}


/****************************************************************************
  Section:
    POST Form Handlers
 ***************************************************************************/
#if defined(HTTP_USE_POST)

/*****************************************************************************
  Function:
    HTTP_IO_RESULT HTTPExecutePost(void)
	
  Internal:
    See documentation in the TCP/IP Stack API or HTTP2.h for details.
 ***************************************************************************/
HTTP_IO_RESULT HTTPExecutePost(void) {
    // Resolve which function to use and pass along
    BYTE filename[20];

    // Load the file name
    // Make sure BYTE filename[] above is large enough for your longest name
    MPFSGetFilename(curHTTP.file, filename, sizeof (filename));

    if (!strcmppgm2ram((char*) filename, "prt/index.htm"))
        return HTTPPostAuthentication();
    else if (!strcmppgm2ram((char*) filename, "prt/local.htm"))
        return HTTPPostLocalNetwork();
    else if (!strcmppgm2ram((char*) filename, "prt/io.htm"))
        return HTTPPostIO();

    return HTTP_IO_DONE;
}


//**********************************************************************************
//		static unsigned char ASCIItoNumeric(unsigned char *)
//
//		Routine per la conversione di una numero espresso in ASCII in 
//		valore numerico
//**********************************************************************************

unsigned short ASCIItoNumeric(unsigned char* textascii) {
    unsigned short numeric;
    unsigned char value;
    unsigned char counter;

    numeric = 0;
    value = 0;
    counter = 0;

    //Scansione dei caratteri e determinazione del valore numerico
    while (textascii[counter] != '\0') {
        switch (textascii[counter]) {
            case '0': value = 0;
                break;
            case '1': value = 1;
                break;
            case '2': value = 2;
                break;
            case '3': value = 3;
                break;
            case '4': value = 4;
                break;
            case '5': value = 5;
                break;
            case '6': value = 6;
                break;
            case '7': value = 7;
                break;
            case '8': value = 8;
                break;
            case '9': value = 9;
                break;
            default: value = 0;
                break; //******* ERRORE ********
        }
        if (numeric != 0 && counter != 0) numeric *= 10;
        numeric += value;
        ++counter;
    }
    //if(numeric>255) return(255);		//ERROR MESSAGE
    return (numeric);
}



size_t strlcpy(char *dst, const char *src, size_t size) {
    register char *d = dst;
    register const char *s = src;
    register size_t n = size;

    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
        do {
            if ((*d++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (size != 0)
            *d = '\0'; /* NUL-terminate dst */
        while (*s++);
    }

    return (s - src - 1); /* count does not include NUL */
}


/*********************************

#endif //(use_post)

 * 
 * 
/****************************************************************************
  Section:
    Dynamic Variable Callback Functions
 ***************************************************************************/

/*****************************************************************************
  Function:
    void HTTPPrint_varname(void)
	
  Internal:
    See documentation in the TCP/IP Stack API or HTTP2.h for details.
 ***************************************************************************/

void HTTPPrint_builddate(void) {
    curHTTP.callbackPos = 0x01;
    if (TCPIsPutReady(sktHTTP) < strlenpgm((ROM char*) __DATE__" "__TIME__))
        return;

    curHTTP.callbackPos = 0x00;
    TCPPutROMString(sktHTTP, (ROM void*) __DATE__" "__TIME__);
}

void HTTPPrint_version(void) {
    TCPPutROMString(sktHTTP, (ROM void*) TCPIP_STACK_VERSION);
}

void HTTPPrint_reboot(void) {
    // This is not so much a print function, but causes the board to reboot
    // when the configuration is changed.  If called via an AJAX call, this
    // will gracefully reset the board and bring it back online immediately
    Reset();
}

void HTTPPrint_rebootaddr(void) {// This is the expected address of the board upon rebooting
    TCPPutString(sktHTTP, curHTTP.data);
}

void HTTPPrint_cs(void) {
    TCPPutROMString(sktHTTP, (rom BYTE*) "0");
}

#endif
