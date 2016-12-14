
#include "Input.h"
#include "TCPIP Stack/Tick.h"
#include "HTTPClient.h"

// TODO Remove EMAIL
#include "EMAIL.h"

INPUT_CONFIG in[2];

void InitInput(void) {

    // Initialize Buttons
    BUTTON_1_TRIS = INPUT;
    BUTTON_2_TRIS = INPUT;
}

BOOL readInput(BYTE io) {
    switch (io) {
        case INPUT_1:
            return BUTTON_1_IO;
        case INPUT_2:
            return BUTTON_2_IO;
    }
    
    
}

void InputTask(void) {
    BYTE i;

    for (i = 0; i < 2; i++) {
        if (in[i].bits.pressedTemp == FALSE) {
            if (readInput(INPUT_1 + i) == BUTTON_PRESSED) {
                in[i].timeout = TickGet();
                in[i].bits.pressedTemp = TRUE;
            }
        } else {
            if (TickGet() - in[i].timeout > TICK_SECOND / 5) {
                if (readInput(INPUT_1 + i) == BUTTON_PRESSED) {
                    in[i].bits.pressedNotice = TRUE;
                    // Button pressed do something
                    setSendEmailFlag();
                    //HTTPClientSendRequest(1);
                }
                in[i].bits.pressedTemp = FALSE;
            }
        }
    }
}