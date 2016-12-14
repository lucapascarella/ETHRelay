 /*********************************************************************
 *
 *	Hardware specific definitions for:
 *    - PICDEM.net 2
 *    - PIC18F97J60
 *    - Internal 10BaseT Ethernet
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    Compiler.h
 * Processor:       PIC18
 * Compiler:        Microchip C18 v3.36 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		09/16/2010	Regenerated for specific boards
 ********************************************************************/
#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#define FIRMWARE_VERSION_MAJOR              (0u)
#define FIRMWARE_VERSION_MINOR              (1u)

#define HARDWARE_VERSION_MAJOR              (1u)
#define HARDWARE_VERSION_MINOR              (0u)

#define FIRMWARE_VERSION                    "0.1"
#define HARDWARE_VERSION                    "1.0"

#include "Compiler.h"


// Clock frequency values
// These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
#define GetSystemClock()                    (41666667ul) // Hz
#define GetInstructionClock()               (GetSystemClock() / 4)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
#define GetPeripheralClock()                (GetSystemClock() / 4)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.

// Some preliminary importance definitions
#ifndef INPUT
#define INPUT                           1
#endif

#ifndef OUTPUT
#define OUTPUT                          0
#endif

//#ifndef HIGH
//#define HIGH                            1
//#endif
//
//#ifndef LOW
//#define LOW                             0
//#endif
//
//#ifndef TRUE
//#define TRUE                            1
//#endif
//
//#ifndef FALSE
//#define FALSE                           0
//#endif

// Hardware I/O pin mappings

#define BUTTON_PRESSED                  (0u)
#define BUTTON_RELEASED                 (1u)

// BUTTONS
#define BUTTON_1_TRIS       (TRISBbits.TRISB5)
#define	BUTTON_1_IO         (PORTBbits.RB5)
#define BUTTON_2_TRIS       (TRISBbits.TRISB4)
#define	BUTTON_2_IO         (PORTBbits.RB4)

// System LED
#define LED_0_TRIS          (TRISCbits.TRISC1)
#define LED_0_O             (LATCbits.LATC1)
#define LED_0_ON()          {LED_0_O = 1;}
#define LED_0_OFF()         {LED_0_O = 0;}
#define LED_0_INV()         {LED_0_O ^= 1;}
// System LED faility
#define SetSystemLedHigh()  {LED_0_O = 1;}
#define SetSystemLedLow()   {LED_0_O = 0;}
#define SetSystemLed(state) {LED_0_O = state;}

// RELAIES
#define RELAY_1_TRIS        (TRISFbits.TRISF2)
#define RELAY_1_O           (LATFbits.LATF2)
#define RELAY_2_TRIS        (TRISFbits.TRISF3)
#define RELAY_2_O           (LATFbits.LATF3)
#define RELAY_3_TRIS        (TRISFbits.TRISF4)
#define RELAY_3_O           (LATFbits.LATF4)
#define RELAY_4_TRIS        (TRISFbits.TRISF5)
#define RELAY_4_O           (LATFbits.LATF5)

// INPUT
#define INPUT_1_TRIS        (TRISFbits.TRISF6)
#define	INPUT_1_IO			(PORTFbits.RF6)
#define INPUT_2_TRIS        (TRISFbits.TRISF7)
#define	INPUT_2_IO			(PORTFbits.RF7)
#define INPUT_3_TRIS        (TRISGbits.TRISG4)
#define	INPUT_3_IO			(PORTGbits.RG4)
#define INPUT_4_TRIS        (TRISBbits.TRISB3)
#define	INPUT_4_IO			(PORTBbits.RB3)
#define INPUT_5_TRIS        (TRISBbits.TRISB2)
#define	INPUT_5_IO			(PORTBbits.RB2)
#define INPUT_6_TRIS        (TRISBbits.TRISB1)
#define	INPUT_6_IO			(PORTBbits.RB1)
#define INPUT_7_TRIS        (TRISBbits.TRISB0)
#define	INPUT_7_IO			(PORTBbits.RB0)
#define INPUT_8_TRIS        (TRISEbits.TRISE0)
#define	INPUT_8_IO			(PORTEbits.RE0)

// OUTPUT
#define OUTPUT_1_TRIS       (TRISEbits.TRISE1)
#define OUTPUT_1_O          (LATEbits.LATE1)
#define OUTPUT_2_TRIS       (TRISEbits.TRISE2)
#define OUTPUT_2_O          (LATEbits.LATE2)
#define OUTPUT_3_TRIS       (TRISEbits.TRISE3)
#define OUTPUT_3_O          (LATEbits.LATE3)
#define OUTPUT_4_TRIS       (TRISEbits.TRISE4)
#define OUTPUT_4_O          (LATEbits.LATE4)
#define OUTPUT_5_TRIS       (TRISEbits.TRISE5)
#define OUTPUT_5_O          (LATEbits.LATE5)
#define OUTPUT_6_TRIS       (TRISDbits.TRISD0)
#define OUTPUT_6_O          (LATDbits.LATD0)
#define OUTPUT_7_TRIS       (TRISDbits.TRISD1)
#define OUTPUT_7_O          (LATDbits.LATD1)
#define OUTPUT_8_TRIS       (TRISDbits.TRISD2)
#define OUTPUT_8_O          (LATDbits.LATD2)

// Uncomment the following declaration to add the I2C EEPROM support
#define XEEPROM_CS_TRIS

#endif // #ifndef HARDWARE_PROFILE_H
