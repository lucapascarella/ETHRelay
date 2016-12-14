
#include "Flash.h"

// Erase Flash and data size check
#if DATA_FLASH_SIZE < FLASH_ERASE_SIZE
#error Data Flash size must be bigger than Flash erase page size
#endif

// Insure EMULATION PAGES are on Erase page boundary
#if (FLASH_START_ADDRESS % 1024) != 0
#error Emulation page start address must be aligned on a 1024 byte boundary.
#endif

extern far rom unsigned char FlashSpace[DATA_FLASH_SIZE];

void EraseFlash(INT16 len) {
    INT16 i;

    for (i = 0; i < len; i += FLASH_ERASE_SIZE)
        RawEraseFlashBlock(FLASH_START_ADDRESS + i);
}

BOOL WriteFlash(BYTE *data, INT16 size) {

    INT16 i, lenght;
    BYTE res;
    //BYTE StatusData[DATA_FLASH_SIZE / 8];

    lenght = min(size, DATA_FLASH_SIZE);

    // Check the content, if it is the same skip the write operation
    res = memcmppgm2ram(data, (const rom void*) FlashSpace, lenght);
    if (res != 0) {
        // Check if erase operation is required
        res = 0xFF;
        for (i = 0; i < lenght; i++) {
            res &= (BYTE) FlashSpace[i] | (BYTE) data[i];
        }
        if (res != 0xFF) {
            // Erase an whole block of minimum 1024 byte
            EraseFlash(lenght);
        }
        // Write the content of RAM array in block of 64 byte at time
        res = TRUE;
        for (i = 0; i < lenght; i += FLASH_WRITE_SIZE) {
            res &= RawWriteFlashBlock(FLASH_START_ADDRESS + i, &data[i], NULL);
        }
    } else {
        // No write operation is required
        res = TRUE;
    }
    if (res == 0x00) {
        Nop();
        Nop();
    }

    return res;
}

/******************************************************************************
 * Function:        static void RawEraseFlashBlock(DWORD dwAddress)
 *
 * PreCondition:    None
 *
 * Input:           dwAddress: Begining byte address to start erasing at.  Note 
 *							   that this address must be page aligned (ie: evenly 
 *							   divisible by FLASH_ERASE_SIZE). 
 *
 * Output:          Erases the specified page, limited by the Flash 
 *					technology's FLASH_ERASE_SIZE.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function is bootloader safe.  If an attempt to erase 
 *					the addresses containing the bootloader is made, the 
 *					function will not perform the erase.
 *****************************************************************************/
static void RawEraseFlashBlock(DWORD dwAddress) {

    WORD w;
    BYTE vAndSum;

    // Guarantee that we make calculations using the real erase address 
    // supported by the hardware
    dwAddress &= ~(FLASH_ERASE_SIZE - 1ul);

    ////    // Calculate non-erasable addresses
    ////    dwBootloaderStart = (DWORD) & main & ~(FLASH_ERASE_SIZE - 1ul);
    ////    dwBootloaderEnd = dwBootloaderStart + BOOTLOADER_SIZE - 1ul;
    ////
    ////    // Make sure that we do not erase the Bootloader
    ////    if ((dwAddress >= dwBootloaderStart) && (dwAddress <= dwBootloaderEnd))
    ////        return;

    // Blank checking and erasing can take considerable time
    ClrWdt();

    // Do a blank check.  No need to waste time erasing a blank page.
    vAndSum = 0xFF;
    TBLPTR = dwAddress;
#if FLASH_ERASE_SIZE % 8 == 0
    for (w = 0; w < FLASH_ERASE_SIZE / 8; w++) {
        // Read 8 bytes per loop iteration to increase performance
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;
        _asm TBLRDPOSTINC _endasm
        vAndSum &= TABLAT;

        // Check to see if any of the 8 bytes has data in it
        if (vAndSum != 0xFF) {
            // Erasing can take considerable time
            ClrWdt();

            // Data found, erase this page
            TBLPTR = dwAddress;
            EECON1bits.WREN = 1;
            EECON1bits.FREE = 1;
            _asm
            movlw 0x55
                    movwf EECON2, ACCESS
                    movlw 0xAA
                    movwf EECON2, ACCESS
                    bsf EECON1, 1, ACCESS //WR
                    _endasm
                    EECON1bits.WREN = 0;

            break;
        }
    }
#else
    for (w = 0; w < FLASH_ERASE_SIZE; w++) {
        _asm TBLRDPOSTINC _endasm

                // Check to see if any of the 8 bytes has data in it
        if (TABLAT != 0xFF) {
            // Erasing can take considerable time
            ClrWdt();

            // Data found, erase this page
            TBLPTR = dwAddress;
            EECON1bits.WREN = 1;
            EECON1bits.FREE = 1;
            _asm
            movlw 0x55
                    movwf EECON2, ACCESS
                    movlw 0xAA
                    movwf EECON2, ACCESS
                    bsf EECON1, 1, ACCESS //WR
                    _endasm
                    EECON1bits.WREN = 0;

            break;
        }
    }
#endif

    // Blank checking and erasing can take considerable time
    ClrWdt();
}

/******************************************************************************
 * Function:        static BOOL RawWriteFlashBlock(DWORD Address, BYTE *BlockData, BYTE *StatusData)
 *
 * PreCondition:    None
 *
 * Input:           Address: Location to write to.  This address MUST be an 
 *							 integer multiple of the FLASH_WRITE_SIZE constant.
 *					*BlockData: Pointer to an array of bytes to write.  The 
 *								array is assumed to be exactly FLASH_WRITE_SIZE 
 *								in length (or longer).  If longer, only the 
 *								first FLASH_WRITE_SIZE bytes will be written.  
 *								You must call WriteFlashBlock() again with an 
 *								updated Address to write more bytes.
 *					*StatusData: Pointer to a byte array of size FLASH_WRITE_SIZE/8 
 *								 which will be written with the results of the 
 *								 write.  Each bit in the array represents one 
 *								 data byte successfully or unsuccessfully 
 *								 written.  If the bit is set, the write was 
 *								 successful.  Pointer can be NULL if individual 
 *								 byte status data is not needed.
 *
 * Output:          TRUE: If the write was successful or no write was needed 
 *						  because the data was already matching.
 *					FALSE: If the write failed either because some other data 
 *						   was already in the memory (need to perform Erase 
 *						   first), or if, following the write, the data did 
 *						   not verify correctly (may have run out of Flash 
 *						   Erase-Write cycles).
 *
 * Side Effects:    None
 *
 * Overview:        Writes one FLASH_WRITE_SIZE block of data to the Flash 
 *					memory.  The memory must be erased prior to writing.  
 *
 * Note:            None
 *****************************************************************************/
static BOOL RawWriteFlashBlock(DWORD Address, BYTE *BlockData, BYTE *StatusData) {
    BYTE i;
    WORD w;
    BOOL WriteNeeded;
    BOOL WriteAllowed;
    BOOL VerifyPassed;


    WriteNeeded = FALSE;
    WriteAllowed = TRUE;
    VerifyPassed = TRUE;

    // Load up the internal Flash holding registers in preperation for the write
    TBLPTR = Address;
    for (w = 0; w < FLASH_WRITE_SIZE; w++) {
        _asm TBLRD _endasm
        if (TABLAT != 0xFF)
            WriteAllowed = FALSE;
        i = BlockData[w];
        if (TABLAT != i) {
            TABLAT = i;
            WriteNeeded = TRUE;
        }
        _asm TBLWTPOSTINC _endasm
    }
    TBLPTR = Address;

    // Start out assuming that all bytes were written successfully
    if (StatusData != NULL)
        memset(StatusData, 0xFF, FLASH_WRITE_SIZE >> 3);

    if (WriteNeeded) {
        // Perform the write
        if (WriteAllowed) {
            // Writing can take some real time
            ClrWdt();

            // Copy the holding registers into FLASH.  This takes approximately 2.8ms.
            EECON1bits.FREE = 0;
            EECON1bits.WREN = 1;
            _asm
            movlw 0x55
                    movwf EECON2, ACCESS
                    movlw 0xAA
                    movwf EECON2, ACCESS
                    bsf EECON1, 1, ACCESS //WR
                    _endasm
                    EECON1bits.WREN = 0;

            // Writing can take some real time
            ClrWdt();
        }

        // Verify that the write was performed successfully
        for (w = 0; w < FLASH_WRITE_SIZE; w++) {
            _asm TBLRDPOSTINC _endasm
            if (TABLAT != BlockData[w]) {
                // Uhh oh.  Data is invalid.  
                // You need to perform an erase first 
                // if WriteAllowed is FALSE.
                // If WriteAllowed is TRUE maybe FLASH 
                // ran out of Erase/Write cycles 
                // (endurance problem).
                if (StatusData != NULL)
                    StatusData[w >> 3] &= ~(1 << (w & 0x07));
                VerifyPassed = FALSE;
            }
        }
        return VerifyPassed;
    }
    return TRUE;
}
