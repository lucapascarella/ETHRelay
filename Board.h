/* 
 * File:   Board.h
 * Author: Luca
 *
 * Created on 12 aprile 2016, 16.24
 */

#ifndef BOARD_H
#define	BOARD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
    
void InitializeBoard(void);

typedef enum {
    RB_SM_HOME = 0,
    RB_SM_WAIT_COMMAND,
    RB_SM_START_WAIT,
    RB_SM_REBOOT
} REBOOT_TASK_STATE_MACHINE;

typedef struct __attribute__((__packed__)) {

    DWORD timer;                            // Timer used to timeout indicator
    DWORD milliseconds;                     // Delay before reboot

    REBOOT_TASK_STATE_MACHINE sm;
    
    struct {
        BYTE reboot : 1;                    // Ask for reboot
    } flags;

} REBOOT_CONFIG;

void rebootInit(void);
void rebootTask(void);
void askRebootInMilliSeconds(DWORD msec);

#ifdef	__cplusplus
}
#endif

#endif	/* BOARD_H */

