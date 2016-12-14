
#include "IOHandler.h"

IOH_CONFIG ioh;

void IOHandlerInit(void) {
    ioh.sm = SM_IOH_HOME;
}

void IOHandlerTask(void) {
    switch(ioh.sm){
        case SM_IOH_HOME:
            
            break;
    }
}
