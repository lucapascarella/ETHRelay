/********************************************************************
 FileName:		GenericTCPClient.c
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

#define __GENERICTCPCLIENT_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)

#include "TCPIPStack/TCPIP.h"

#include "VLSICodec.h"
//#include "SPIRAM2.h"

enum _TCPClientState {
    SM_OGG_TCP_CLIENT_HOME = 0,

    SM_OGG_TCP_CLIENT_SOCKET_OBTAINED,
    
    SM_OGG_PROCESS_STREAM_FILL_ETH_BUFFER,

    SM_OGG_TCP_CLIENT_CONNECTED_GET,
    SM_OGG_TCP_CLIENT_CONNECTED_PUT,
    SM_OGG_TCP_CLIENT_CONNECTED_CTRL,

    SM_OGG_TCP_CLIENT_DISCONNECT,

    SM_OGG_TCP_CLIENT_DONE
} TCPClientState = SM_OGG_TCP_CLIENT_DONE;

/*****************************************************************************
  Function:
        void GenericTCPClient(void)
 ***************************************************************************/
void OggTCPClient(void) {

    static DWORD Timer, reconnectionDelay = 0;
    static TCP_SOCKET ClientSocket = INVALID_SOCKET;
    //extern BYTE Buffer[];
    extern BYTE ringBuffer[];
    extern APP_CONFIG AppConfig;
    static int ByteRead, buf_len_half;
    static BYTE reconnectionTimes = 0;

    switch (TCPClientState) {

        case SM_OGG_TCP_CLIENT_HOME:
            // Connect a socket to the remote TCP server
            XEEReadArray(URL_NAME_ADDRESS, ringBuffer, URL_NAME_LENGTH);
            ClientSocket = TCPOpen((DWORD) & ringBuffer[0], TCP_OPEN_RAM_HOST, AppConfig.Stream.RemotePort, TCP_PURPOSE_GENERIC_TCP_CLIENT);

            // Abort operation if no TCP socket of type TCP_PURPOSE_GENERIC_TCP_CLIENT is available
            // If this ever happens, you need to go add one to TCPIPConfig.h
            if (ClientSocket == INVALID_SOCKET)
                break;

            TCPClientState = SM_OGG_TCP_CLIENT_SOCKET_OBTAINED;
            Timer = TickGet();
            break;

        case SM_OGG_TCP_CLIENT_SOCKET_OBTAINED:
            // Wait for the remote server to accept our connection request
            if (!TCPIsConnected(ClientSocket)) {
                // Time out if too much time is spent in this state
                if (TickGet() - Timer > AppConfig.Stream.ConnectTimeout * TICK_SECOND) {
                    // Close the socket so it can be used by other modules
                    TCPClientState = SM_OGG_TCP_CLIENT_DISCONNECT;
                }
                break;
            }

	    // Adjust the TCP FIFOs for optimal reception of audio stream
	    TCPAdjustFIFOSize(ClientSocket, 128, 128, TCP_ADJUST_GIVE_REST_TO_RX);

            AppConfig.Flags.bConncetcionStatus = TRUE;
            buf_len_half = AppConfig.Stream.BufferLen / 16;
            ByteRead = 0;

            //TCPClientState = SM_OGG_TCP_CLIENT_CONNECTED_GET;
            TCPClientState = SM_OGG_PROCESS_STREAM_FILL_ETH_BUFFER;
            break;

	case SM_OGG_PROCESS_STREAM_FILL_ETH_BUFFER:

	    // Check to see if the remote node has disconnected from us or sent us any application data
	    if (!TCPIsConnected(ClientSocket)) {
		TCPClientState = SM_OGG_TCP_CLIENT_DISCONNECT;
	    } else if (TCPIsGetReady(ClientSocket) >= 25000)
		// Wait until ETH RX buffer is filled
		TCPClientState++;
	    break;


        case SM_OGG_TCP_CLIENT_CONNECTED_GET:

            ByteRead += TCPGetArray(ClientSocket, &ringBuffer[ByteRead], AppConfig.Stream.BufferLen - ByteRead);

            if (ByteRead < buf_len_half)
                break;

            TCPClientState = SM_OGG_TCP_CLIENT_CONNECTED_PUT;
            break;

        case SM_OGG_TCP_CLIENT_CONNECTED_PUT:

            VLSIPutArray(ringBuffer, ByteRead);
            //TCPClientState = SM_OGG_TCP_CLIENT_CONNECTED_GET;
            ByteRead = 0;

            // No break;


        case SM_OGG_TCP_CLIENT_CONNECTED_CTRL:

            TCPClientState = SM_OGG_TCP_CLIENT_CONNECTED_GET;

            if ((BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStop) && AppConfig.Flags.bConncetcionStatus == TRUE) {
                while (BUTTON0_IO == 0);
                AppConfig.Stream.Flags.bits.bWebStop = FALSE;
                TCPClientState = SM_OGG_TCP_CLIENT_DISCONNECT;
            }

            // Check to see if the remote node has disconnected from us or sent us any application data
            if (!TCPIsConnected(ClientSocket)) {
                TCPClientState = SM_OGG_TCP_CLIENT_DISCONNECT;
            }
            break;


        case SM_OGG_TCP_CLIENT_DISCONNECT:

            AppConfig.Flags.bConncetcionStatus = FALSE;
            // Increment reconnection times
            reconnectionTimes++;
            // Take current time
            reconnectionDelay = TickGet();

            // Do a soft reset
            VLSI_SoftReset();

            // Close the socket so it can be used by other modules
            TCPDisconnect(ClientSocket);
            TCPDisconnect(ClientSocket);
            ClientSocket = INVALID_SOCKET;
            TCPClientState = SM_OGG_TCP_CLIENT_DONE;
            break;


        case SM_OGG_TCP_CLIENT_DONE:
            // Connect automatically if set in the software
            if (AppConfig.Stream.Flags.bits.bConnAtStartUp == TRUE && reconnectionTimes <= AppConfig.Stream.ReconnectTimes) {
                // Wait for the delay before retrying the connection
                if ((TickGet() - reconnectionDelay) >= AppConfig.Stream.ReconnectDelay * TICK_SECOND)
                    TCPClientState = SM_OGG_TCP_CLIENT_HOME;

            }
            // Do nothing unless the user pushes BUTTON0 and wants to restart the whole connection/download process
            if (BUTTON0_IO == 0u || AppConfig.Stream.Flags.bits.bWebStart == TRUE) {
                reconnectionTimes = 0;
                AppConfig.Stream.Flags.bits.bWebStart = FALSE;
                TCPClientState = SM_OGG_TCP_CLIENT_HOME;
            }
            break;
    }
}

#endif	//#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)
