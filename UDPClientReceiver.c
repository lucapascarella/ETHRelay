/********************************************************************
 FileName:		UDPBroadcast.c
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

enum {
    SM_OGG_UDP_CLIENT_HOME = 0,

    SM_OGG_UDP_CLIENT_SOCKET_OBTAINED,

    SM_OGG_UDP_CLIENT_CONNECTED_GET,
    SM_OGG_UDP_CLIENT_CONNECTED_PUT,
    SM_OGG_UDP_CLIENT_CONNECTED_CTRL,

    SM_OGG_UDP_CLIENT_DISCONNECT,

    SM_OGG_UDP_CLIENT_DONE
} UDPClientState = SM_OGG_UDP_CLIENT_DONE;

#include "VLSICodec.h"
#include "TCPIPStack/TCPIP.h"

void OggUDPClient(void) {



    static UDP_SOCKET ClientSocket;

    static int ByteRead;
    extern BYTE ringBuffer[];
    extern APP_CONFIG AppConfig;
    static BYTE reconnectionTimes = 0;
    static DWORD reconnectionDelay = 0, length = 0;

    switch (UDPClientState) {

	case SM_OGG_UDP_CLIENT_HOME:
	    // Open a UDP socket for inbound and outbound transmission
	    // Since we expect to only receive broadcast packets and
	    // only send unicast packets directly to the node we last
	    // received from, the remote NodeInfo parameter can be anything

	    // OLD ClientSocket = UDPOpen(AppConfig.Stream.LocalPort, NULL, AppConfig.Stream.RemotePort);
	    // UDP_OPEN_SERVER also for receiving in client from broadcast UDP transmission
	    ClientSocket = UDPOpenEx(NULL, UDP_OPEN_SERVER, AppConfig.Stream.RemotePort, AppConfig.Stream.RemotePort);

	    if (ClientSocket == INVALID_UDP_SOCKET)
		return;
	    else
		UDPClientState = SM_OGG_UDP_CLIENT_SOCKET_OBTAINED;
	    break;

	case SM_OGG_UDP_CLIENT_SOCKET_OBTAINED:

	    if (UDPIsOpened(ClientSocket) == FALSE)
		break;
//	    length = UDPIsGetReady(ClientSocket);
//	    if (length == 0)
//		break;
//
//	    if (length < 1024)
//		break;

	    AppConfig.Flags.bConncetcionStatus = TRUE;
	    ByteRead = 0;

	    UDPClientState = SM_OGG_UDP_CLIENT_CONNECTED_GET;
	    break;

	case SM_OGG_UDP_CLIENT_CONNECTED_GET:
	    // Do nothing if no data is waiting
	    if (!UDPIsGetReady(ClientSocket)) {
		if ((BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStop) && AppConfig.Flags.bConncetcionStatus == TRUE) {
		    while (BUTTON0_IO == 0);
		    AppConfig.Stream.Flags.bits.bWebStop = FALSE;
		    UDPClientState = SM_OGG_UDP_CLIENT_DISCONNECT;
		}
		break;
	    }

	    ByteRead = UDPGetArray(ringBuffer, AppConfig.Stream.BufferLen);
	    UDPClientState = SM_OGG_UDP_CLIENT_CONNECTED_PUT;
	    //break;


	case SM_OGG_UDP_CLIENT_CONNECTED_PUT:

	    VLSIPutArray(ringBuffer, ByteRead);
	    UDPClientState = SM_OGG_UDP_CLIENT_CONNECTED_CTRL;
	    ByteRead = 0;
	    // no break;


	case SM_OGG_UDP_CLIENT_CONNECTED_CTRL:

	    UDPClientState = SM_OGG_UDP_CLIENT_CONNECTED_GET;

	    if (((BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStop) && AppConfig.Flags.bConncetcionStatus == TRUE) || UDPIsOpened(ClientSocket) == FALSE)  {
		while (BUTTON0_IO == 0);
		AppConfig.Stream.Flags.bits.bWebStop = FALSE;
		UDPClientState = SM_OGG_UDP_CLIENT_DISCONNECT;
	    }
	    break;


	case SM_OGG_UDP_CLIENT_DISCONNECT:

	    AppConfig.Flags.bConncetcionStatus = FALSE;
	    // Increment reconnection times
	    reconnectionTimes++;
	    // Take current time
	    reconnectionDelay = TickGet();

	    // Do a soft reset
	    VLSI_SoftReset();

	    // Close the socket so it can be used by other modules
	    UDPClose(ClientSocket);
	    ClientSocket = INVALID_SOCKET;
	    UDPClientState = SM_OGG_UDP_CLIENT_DONE;
	    break;


	case SM_OGG_UDP_CLIENT_DONE:
	    // Connect automatically if set in the software
	    if (AppConfig.Stream.Flags.bits.bConnAtStartUp == TRUE && reconnectionTimes <= AppConfig.Stream.ReconnectTimes) {
		// Wait for the delay before retrying the connection
		if ((TickGet() - reconnectionDelay) >= AppConfig.Stream.ReconnectDelay * TICK_SECOND)
		    UDPClientState = SM_OGG_UDP_CLIENT_HOME;

	    }
	    // Do nothing unless the user pushes BUTTON0 and wants to restart the whole connection/download process
	    if (BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStart == TRUE) {
		reconnectionTimes = 0;
		AppConfig.Stream.Flags.bits.bWebStart = FALSE;
		UDPClientState = SM_OGG_UDP_CLIENT_HOME;
	    }
	    break;
    }

}


//#endif //#if defined(STACK_USE_ANNOUNCE)
