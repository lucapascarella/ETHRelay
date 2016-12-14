/*********************************************************************
 *
 *  Generic TCP Client Example Application
 *  Module for Microchip TCP/IP Stack
 *   -Implements an example HTTP client and should be used as a basis
 *	  for creating new TCP client applications
 *	 -Reference: None.  Hopefully AN833 in the future.
 *
 *********************************************************************
 * FileName:        GenericTCPClient.c
 * Dependencies:    TCP, DNS, ARP, Tick
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
 * Howard Schlunder     8/01/06	Original
 ********************************************************************/
#define __GENERICTCPCLIENT_C


#include "TCPIPConfig.h"
#include "WebRadio.h"
#include "VLSICodec.h"

#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)

#include "TCPIPStack/TCPIP.h"


//static BYTE ServerName[] = "www.google.com";

_WEB_RADIO_CLIENT_STATE WebRadioClientState = SM_WR_DONE;


#define cmin(a, b)                  ((a<b)?a:b)
#define cmax(a, b)                  ((a>b)?a:b)
#define ringIncr(p)                 ((p + 1)&(2*RING_BUFFER_SIZE-1))
#define ringIsFull(start, end)      (end == (start ^ RING_BUFFER_SIZE))
#define ringIsEmpty(start, end)     (end == start)

/*****************************************************************************
  Function:
	void GenericTCPClient(void)

  Summary:
	Implements a simple HTTP client (over TCP).

  Description:
	This function implements a simple HTTP client, which operates over TCP.
	The function is called periodically by the stack, and waits for BUTTON1
	to be pressed.  When the button is pressed, the application opens a TCP
	connection to an Internet search engine, performs a search for the word
	"Microchip" on "microchip.com", and prints the resulting HTML page to
	the UART.

	This example can be used as a model for many TCP and HTTP client
	applications.

  Precondition:
	TCP is initialized.

  Parameters:
	None

  Returns:
	None
 ***************************************************************************/

void WebRadioClient(void) {

    WORD i;
    static WORD processed, read;
    static DWORD Timer, reconnectionDelay = 0;
    static TCP_SOCKET WebSocket = INVALID_SOCKET;
    static BYTE reconnectionTimes = 0;

    extern BYTE Buffer[];
    extern BYTE ringBuffer[RING_BUFFER_SIZE];
    extern UINT ringStart, ringEnd;

    switch (WebRadioClientState) {
	case SM_WR_HOME:
	    // Connect a socket to the remote TCP server
	    XEEReadArray(URL_NAME_ADDRESS, Buffer, URL_NAME_LENGTH);
	    WebSocket = TCPOpen((DWORD) & Buffer[0], TCP_OPEN_RAM_HOST, AppConfig.Stream.RemotePort, TCP_PURPOSE_GENERIC_TCP_CLIENT);

	    // Abort operation if no TCP socket of type TCP_PURPOSE_GENERIC_TCP_CLIENT is available
	    // If this ever happens, you need to go add one to TCPIPConfig.h
	    if (WebSocket == INVALID_SOCKET)
		break;

	    // Reset static variables
	    processed = read = 0;
	    // Reset ring indicator (Buffer Empty)
	    ringStart = ringEnd = 0;

	    WebRadioClientState++;
	    Timer = TickGet();
	    break;

	case SM_WR_SOCKET_OBTAINED:
	    // Wait for the remote server to accept our connection request
	    if (!TCPIsConnected(WebSocket)) {
		// Time out if too much time is spent in this state
		if (TickGet() - Timer > AppConfig.Stream.ConnectTimeout * TICK_SECOND) {
		    // Close the socket so it can be used by other modules
		    WebRadioClientState = SM_WR_DISCONNECT;
		}
		break;
	    }

	    //Timer = TickGet();

	    // Make certain the socket can be written to
	    if (TCPIsPutReady(WebSocket) < 150u)
		break;

	    // Place the application protocol data into the transmit buffer.  For this example, we are connected to an HTTP server, so we'll send an HTTP GET request.
	    TCPPutROMString(WebSocket, (ROM BYTE*) "GET ");
	    XEEReadArray(URL_RESOURCE_ADDRESS, Buffer, URL_RESOURCE_LENGTH);
	    TCPPutString(WebSocket, Buffer);
	    TCPPutROMString(WebSocket, (ROM BYTE*) " HTTP/1.0\r\nHost: ");
	    XEEReadArray(URL_NAME_ADDRESS, Buffer, URL_NAME_LENGTH);
	    TCPPutString(WebSocket, Buffer);
	    TCPPutROMString(WebSocket, (ROM BYTE*) "\r\nConnection: close\r\n\r\n");

	    // Send the packet
	    TCPFlush(WebSocket);
	    WebRadioClientState++;
	    break;

	case SM_WR_PROCESS_HTTP_RESPONSE:

	    // Read available bytes from RX fifo queue
	    read += TCPGetArray(WebSocket, &Buffer[read], BUFFER_MAX_SIZE - read);

	    // Find the HTTP Header termination indicator
	    for (i = 3; i < read; i++) {
		// Find \r\n\r\n path
		if (Buffer[i - 3] == '\r' && Buffer[i - 2] == '\n' && Buffer[i - 1] == '\r' && Buffer[i] == '\n') {
		    // Save the response in EEPROM to Web server availability
		    XEEBeginWrite(HTTP_RESPONSE_ADDRESS);
		    XEEWriteArray(Buffer, i++);
		    // Update the ring buffer with the last read
		    while (i < read && !ringIsFull(ringStart, ringEnd)) {
			ringBuffer[ringEnd & (RING_BUFFER_SIZE - 1)] = Buffer[i++];
			ringEnd = ringIncr(ringEnd);
		    }
		    // Reset read static variable
		    read = 0;
		    // Goto next stage
		    AppConfig.Flags.bConncetcionStatus = TRUE;
		    WebRadioClientState++;
		    break;
		}
	    }

	    // Timeout, HTTP Header termination not found
	    if (read >= BUFFER_MAX_SIZE) {
		AppConfig.Flags.bConncetcionStatus = TRUE;
		WebRadioClientState++;
	    }

	    break;

	case SM_WR_PROCESS_STREAM_FILL_RING_BUFFER:

	    // Check to see if the remote node has disconnected from us or sent us any application data
	    // If application data is available, write it to the UART
	    if (!TCPIsConnected(WebSocket))
		WebRadioClientState = SM_WR_DISCONNECT;
	    // Do not break;  We might still have data in the TCP RX FIFO waiting for us

	    if (processed == read) {
		read = TCPGetArray(WebSocket, Buffer, BUFFER_MAX_SIZE);
		processed = 0;
	    }
	    while (processed < read && !ringIsFull(ringStart, ringEnd)) {
		ringBuffer[ringEnd & (RING_BUFFER_SIZE - 1)] = Buffer[processed++];
		ringEnd = ringIncr(ringEnd);
	    }

	    // If the ring buffer is full, go to the next stage
	    if (ringIsFull(ringStart, ringEnd))
		WebRadioClientState = SM_WR_PROCESS_STREAM_PUT_BYTE;
	    break;

	case SM_WR_PROCESS_STREAM_GET_BYTE:
	    // Check to see if the remote node has disconnected from us or sent us any application data
	    // If application data is available, write it to the UART
	    if (!TCPIsConnected(WebSocket))
		WebRadioClientState = SM_WR_DISCONNECT;
	    // Do not break;  We might still have data in the TCP RX FIFO waiting for us

	    if (processed == read) {
		read = TCPGetArray(WebSocket, Buffer, BUFFER_MAX_SIZE);
		processed = 0;
	    }
	    while (processed < read && !ringIsFull(ringStart, ringEnd)) {
		ringBuffer[ringEnd & (RING_BUFFER_SIZE - 1)] = Buffer[processed++];
		ringEnd = ringIncr(ringEnd);
	    }

	    WebRadioClientState++;
	    break;

	case SM_WR_PROCESS_STREAM_PUT_BYTE:

	    i = VLSIPutRingBuffer();
	    WebRadioClientState = SM_WR_PROCESS_STREAM_GET_BYTE;

	    // No break;

	case SM_WR_PROCESS_STREAM_CHECK_CLOSE:

	    WebRadioClientState = SM_WR_PROCESS_STREAM_GET_BYTE;

	    if ((BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStop) && AppConfig.Flags.bConncetcionStatus == TRUE) {
		while (BUTTON0_IO == 0);
		AppConfig.Stream.Flags.bits.bWebStop = FALSE;
		WebRadioClientState = SM_WR_DISCONNECT;
	    }
	    // Check to see if the remote node has disconnected from us or sent us any application data
	    if (!TCPIsConnected(WebSocket)) {
		WebRadioClientState = SM_WR_DISCONNECT;
	    }
	    break;

	case SM_WR_DISCONNECT:
	    // Close the socket so it can be used by other modules
	    // For this application, we wish to stay connected, but this state will still get entered if the remote server decides to disconnect
	    TCPDisconnect(WebSocket);
	    WebSocket = INVALID_SOCKET;
	    AppConfig.Flags.bConncetcionStatus = FALSE;
	    // Increment reconnection times
	    reconnectionTimes++;
	    // Take current time
	    reconnectionDelay = TickGet();
	    // End disconnction process
	    WebRadioClientState = SM_WR_DONE;
	    break;

	case SM_WR_DONE:
	    // Connect automatically if set in the software
	    if (AppConfig.Stream.Flags.bits.bConnAtStartUp == TRUE && reconnectionTimes <= AppConfig.Stream.ReconnectTimes) {
		// Wait for the delay before retrying the connection
		if ((TickGet() - reconnectionDelay) >= AppConfig.Stream.ReconnectDelay * TICK_SECOND)
		    WebRadioClientState = SM_WR_HOME;

	    }
	    // Do nothing unless the user pushes BUTTON0 and wants to restart the whole connection/download process
	    if (BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStart == TRUE) {
		reconnectionTimes = 0;
		AppConfig.Stream.Flags.bits.bWebStart = FALSE;
		WebRadioClientState = SM_WR_HOME;
	    }

	    break;
    }
}

#endif	//#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)

