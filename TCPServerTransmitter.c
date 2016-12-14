/********************************************************************
 FileName:		GenericTCPServer.c
 Processor:		PIC18F67J60 Microcontrollers
 Hardware:		Filodiffusione
 Complier:  	Microchip C18 (for PIC18)
 Company:		Elettronica In.
 Author:		Luca Pascarella www.lucasproject.it

 ********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   09/11/2009   Initial release

 ********************************************************************/
#define __GENERICTCPSERVER_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE)

#include "TCPIPStack/TCPIP.h"
#include "VLSICodec.h"

enum _TCPServerState {
    SM_SERVER_HOME = 0,
    SM_SERVER_LISTENING_CONNECTION,
    SM_SERVER_GET_BYTE,
    SM_SERVER_SEND_BYTE,
    SM_SERVER_CLOSING,
    SM_SERVER_FINISH_GET,
    SM_SERVER_RESET_VLSI,
} TCPServerState = SM_SERVER_HOME;

/*****************************************************************************
  Function:
	void GenericTCPServer(void)
 ***************************************************************************/
void OggTCPServer(void) {

    static int ByteRead, buf_len_half;
    int sent;
    extern BYTE ringBuffer[];
    extern APP_CONFIG AppConfig;
    static TCP_SOCKET ServerSocket;

    switch (TCPServerState) {
	case SM_SERVER_HOME:
	    // Allocate a socket for this server to listen and accept connections on
	    //ServerSocket = TCPOpen(0, TCP_OPEN_SERVER, AppConfig.Stream.LocalPort, TCP_PURPOSE_GENERIC_TCP_SERVER);
	    ServerSocket = TCPOpen(0, TCP_OPEN_SERVER, AppConfig.Stream.LocalPort, TCP_PURPOSE_GENERIC_TCP_CLIENT);
	    if (ServerSocket == INVALID_SOCKET)
		return;

	    TCPServerState = SM_SERVER_LISTENING_CONNECTION;
	    break;

	case SM_SERVER_LISTENING_CONNECTION:
	    // See if anyone is connected to us
	    if (!TCPIsConnected(ServerSocket))
		break;

	    // Initialize VLSI in Ogg Vorbis Encoder mode
	    VLSIInitEncoder();
	    // Starts the VLSI encoder
	    VLSIStartEncoder();

	    //buf_len_half = AppConfig.Stream.BufferLen / 2;
	    buf_len_half = AppConfig.Stream.BufferLen / 4;
	    ByteRead = 0;
	    // If you get 0x00 0x00 0x67 0x53, you have read data from
	    // SCI_HDAT0 too soon after starting the application.
	    //	    DelayMs(1000);
	    //	    TCPServerState = SM_SERVER_SEND_BYTE;
	    //	    break;

	    // Adjust the TCP FIFOs for optimal transmission of audio stream
	    TCPAdjustFIFOSize(ServerSocket, 128, 128, TCP_ADJUST_GIVE_REST_TO_TX);

	case SM_SERVER_GET_BYTE:
	    ByteRead += VLSIGetArray(&ringBuffer[ByteRead], AppConfig.Stream.BufferLen - ByteRead);

	    if (ByteRead < buf_len_half)
		break;

	    TCPServerState = SM_SERVER_SEND_BYTE;
	    //	    break;


	case SM_SERVER_SEND_BYTE:
	    if (TCPIsPutReady(ServerSocket) < ByteRead) {
		if (!TCPIsConnected(ServerSocket))
		    TCPServerState = SM_SERVER_CLOSING;
		break;
	    }

	    sent = TCPPutArray(ServerSocket, ringBuffer, ByteRead);
	    TCPFlush(ServerSocket);

	    if (sent == ByteRead)
		TCPServerState = SM_SERVER_GET_BYTE;
	    else
		TCPServerState = SM_SERVER_CLOSING;

	    // See if anyone is connected to us
	    if (!TCPIsConnected(ServerSocket))
		TCPServerState = SM_SERVER_CLOSING;

	    ByteRead = 0;
	    break;


	case SM_SERVER_CLOSING:
	    // Close the socket connection.
	    //TCPClose(ServerSocket);
	    TCPDisconnect(ServerSocket);
	    TCPDisconnect(ServerSocket);

	    // Stop the VLSI encoder
	    VLSIStopEncoder();

	    TCPServerState = SM_SERVER_FINISH_GET;
	    break;

	case SM_SERVER_FINISH_GET:
	    ByteRead = VLSIGetArray(ringBuffer, AppConfig.Stream.BufferLen);

	    if (VLSIIsStopped())
		TCPServerState = SM_SERVER_RESET_VLSI;
	    break;

	case SM_SERVER_RESET_VLSI:
	    VLSI_SoftReset();
//	    TCPServerState = SM_SERVER_HOME;
	    TCPServerState = SM_SERVER_LISTENING_CONNECTION;
	    break;
    }
}

#endif //#if defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE)
