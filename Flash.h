/* 
 * File:   Flash.h
 * Author: Luca
 *
 * Created on 20 agosto 2016, 10.20
 */

#ifndef FLASH_H
#define	FLASH_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

#include "FlashAsm.inc"

#define FLASH_WRITE_SIZE	(64ul)
#define FLASH_ERASE_SIZE	(1024ul)
    
#define min(a, b)           ((a<b)?a:b)    
    
void EraseFlash(INT16 len);
BOOL WriteFlash(BYTE *data, INT16 size);

static void RawEraseFlashBlock(DWORD dwAddress);
static BOOL RawWriteFlashBlock(DWORD Address, BYTE *BlockData, BYTE *StatusData);    
    
#ifdef	__cplusplus
}
#endif

#endif	/* FLASH_H */

