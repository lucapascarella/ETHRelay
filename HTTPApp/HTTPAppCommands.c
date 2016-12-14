
#include "HTTPApp/HTTPApp.h"
#include "Relay.h"

void HTTPAppGetCommands(HTTP_CONN *curHTTP) {

    BYTE *arg1, *arg2, *arg3, *arg4, *arg5;
    BYTE relay, state, action;

    arg1 = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "relay");
    arg2 = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "io");
    arg3 = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "state");
    arg4 = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "act");
    arg5 = HTTPGetROMArg(curHTTP->data, (ROM BYTE *) "dir");
    
    if (arg1 && arg3) {
        relay = *arg1 - '0';
        state = *arg2 - '0';
        if (arg4) {
            if (!memcmppgm2ram(arg4, "save", 4)) {
                setStartUpRelay(relay, state);
            } else if (!memcmppgm2ram(arg4, "apply", 5)) {
                setRelay(relay, state);
            }
        } else {
            setRelay(relay, state);
        }
    } else if (arg2 && arg3) {
        
    }
}