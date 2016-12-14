/* 
 * File:   IOHandler.h
 * Author: Luca
 *
 * Created on 28 settembre 2016, 13.22
 */

#ifndef IOHANDLER_H
#define	IOHANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

typedef enum {
    SM_IOH_HOME = 0,
    SM_IOH_DONE
} IOH_STATE_MACHINE;

typedef struct __attribute__((__packed__)) {

    DWORD timer;                            // Timer used to timeout indicator

    IOH_STATE_MACHINE sm;
    
    struct {
        BYTE start : 1;                     // Ask for reboot
    } flags;

} IOH_CONFIG;

    
void IOHandlerInit(void);
void IOHandlerTask(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IOHANDLER_H */

