/* 
 * File:   UDPServerTransmitter.c
 * Author: Luca
 *
 * Created on 15 febbraio 2014, 9.25
 */

#include "VLSICodec.h"
#include "TCPIPStack/TCPIP.h"

enum _UDPState {
    SM_OGG_UDP_SERVER_HOME = 0,
    SM_OGG_UDP_SERVER_SOCKET_OBTAINED,
    SM_OGG_UDP_SERVER_CONNECTED_READ,
    SM_OGG_UDP_SERVER_CONNECTED_SEND,
    SM_OGG_UDP_SERVER_CONNECTED_CTRL,
    SM_OGG_UDP_SERVER_DISCONNECT,
    SM_OGG_UDP_SERVER_DONE
} UDPServerState = SM_OGG_UDP_SERVER_HOME;

void OggUDPServer(void) {
    static UDP_SOCKET MySocket;
    static int ByteRead, buf_len_half;
    static DWORD StartTime;

    char close = 0;
    extern BYTE ringBuffer[];
    extern APP_CONFIG AppConfig;


    switch (UDPServerState) {
	case SM_OGG_UDP_SERVER_HOME:
	    // Open a UDP socket for inbound and outbound transmission
	    // Since we expect to only receive broadcast packets and
	    // only send unicast packets directly to the node we last
	    // received from, the remote NodeInfo parameter can be anything
	    // OLD MySocket = UDPOpen(AppConfig.Stream.LocalPort, NULL, AppConfig.Stream.RemotePort);
	    MySocket = UDPOpenEx(NULL, UDP_OPEN_SERVER, 0, AppConfig.Stream.LocalPort);

	    if (MySocket == INVALID_UDP_SOCKET)
		return;

	    // Initialize VLSI in Ogg Vorbis Encoder mode
	    VLSIInitEncoder();

	    buf_len_half = AppConfig.Stream.BufferLen / 2;
	    ByteRead = 0;

	    StartTime = TickGet();

	    UDPServerState = SM_OGG_UDP_SERVER_SOCKET_OBTAINED;
	    break;

	case SM_OGG_UDP_SERVER_SOCKET_OBTAINED:

	    if ((TickGet() - StartTime) <= 4 * TICK_SECOND)
		break;

	    // Starts the VLSI encoder
	    VLSIStartEncoder();
	    UDPServerState = SM_OGG_UDP_SERVER_CONNECTED_READ;
	    break;

	case SM_OGG_UDP_SERVER_CONNECTED_READ:

	    ByteRead += VLSIGetArray(ringBuffer + ByteRead, AppConfig.Stream.BufferLen - ByteRead);

	    if (ByteRead < buf_len_half)
		break;

	    UDPServerState++;

	case SM_OGG_UDP_SERVER_CONNECTED_SEND:

	    if (UDPIsPutReady(MySocket) < ByteRead)
		break;

	    UDPPutArray(ringBuffer, ByteRead);
	    UDPFlush();
	    ByteRead = 0;

	    UDPServerState = SM_OGG_UDP_SERVER_CONNECTED_READ;
	    break;

	    //	case SM_OGG_UDP_SERVER_CONNECTED_CTRL:
	    //
	    //	    if (close == 0 && (TickGet() - StartTime) > 4 * TICK_SECOND)
	    //		close = 1;
	    //
	    //	    if (close == 1 && BUTTON0_IO == 0u) {
	    //		UDPServerState = SM_OGG_UDP_SERVER_DISCONNECT;
	    //		close = 0;
	    //		StartTime = TickGet();
	    //		break;
	    //	    }
	    //
	    //	    UDPServerState = SM_OGG_UDP_SERVER_CONNECTED_READ;
	    //	    break;
	    //
	    //	case SM_OGG_UDP_SERVER_DISCONNECT:
	    //	    // Close the socket so it can be used by other modules
	    //	    // For this application, we wish to stay connected, but this state will still get entered if the remote server decides to disconnect
	    //	    if ((TickGet() - StartTime) < 4 * TICK_SECOND)
	    //		break;
	    //
	    //	    UDPClose(MySocket);
	    //	    MySocket = INVALID_UDP_SOCKET;
	    //	    UDPServerState = SM_OGG_UDP_SERVER_DONE;
	    //	    break;
	    //
	    //	case SM_OGG_UDP_SERVER_DONE:
	    //	    // Do nothing unless the user pushes BUTTON1 and wants to restart the whole connection/download process
	    //	    if (BUTTON0_IO == 0u) {
	    //		UDPServerState = SM_OGG_UDP_SERVER_HOME;
	    //		StartTime = TickGet();
	    //	    }
	    //	    break;
    }

}

