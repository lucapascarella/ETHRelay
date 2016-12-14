/* 
 * File:   WebRadio.h
 * Author: Luca
 *
 * Created on 12 gennaio 2014, 17.38
 */

#ifndef WEBRADIO_H
#define	WEBRADIO_H

typedef enum _WebRadioClientState {
    SM_WR_HOME = 0,
    SM_WR_SOCKET_OBTAINED,
    SM_WR_PROCESS_HTTP_RESPONSE,
    SM_WR_PROCESS_STREAM_FILL_RING_BUFFER,
    SM_WR_PROCESS_STREAM_GET_BYTE,
    SM_WR_PROCESS_STREAM_PUT_BYTE,
    SM_WR_PROCESS_STREAM_CHECK_CLOSE,
    SM_WR_DISCONNECT,
    SM_WR_DONE
} _WEB_RADIO_CLIENT_STATE;


void WebRadioClient(void);



#endif	/* WEBRADIO_H */

