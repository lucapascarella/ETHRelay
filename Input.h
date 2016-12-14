/* 
 * File:   Input.h
 * Author: Luca
 *
 * Created on 19 agosto 2016, 12.05
 */

#ifndef INPUT_H
#define	INPUT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

#define INPUT_1         1
#define INPUT_2         2
    
typedef struct __attribute__((__packed__)) {

    DWORD timeout;                      // Timeout value
    
    struct __PACKED {
        BYTE pressedTemp : 1;
        BYTE pressedNotice : 1;
    } bits;
} INPUT_CONFIG;    
    
void InitInput(void);
BOOL readInput(BYTE io);
void InputTask(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_H */

