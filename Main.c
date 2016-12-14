/********************************************************************
 FileName:		Main.c
 Processor:		PIC18F67J60 Micro-controllers
 Hardware:		ETHRele
 Complier:  	Microchip C18 v3.47 (for PIC18)
 Company:		Futura Group srl.
 Author:		Luca Pascarella www.lpsystems.eu

 ********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   12/04/2016   Initial release

 ********************************************************************/

/*
 * This macro uniquely defines this file as the main entry point.
 * There should only be one such definition in the entire project,
 * and this file must define the AppConfig variable as described below.
 */
#define THIS_IS_STACK_APPLICATION

#if defined(THIS_IS_STACK_APPLICATION)
#include <p18F67J60.h>

// CONFIG1L
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on SWDTEN bit))
#pragma config STVR = ON        // Stack Overflow/Underflow Reset Enable bit (Reset on stack overflow/underflow enabled)
#pragma config XINST = ON       // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode enabled)

// CONFIG1H
#pragma config CP0 = OFF        // Code Protection bit (Program memory is not code-protected)

// CONFIG2L
#pragma config FOSC = HSPLL     // Oscillator Selection bits (HS oscillator, PLL enabled and under software control)
#pragma config FOSC2 = ON       // Default/Reset System Clock Select bit (Clock selected by FOSC1:FOSC0 as system clock is enabled when OSCCON<1:0> = 00)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor disabled)
#pragma config IESO = ON        // Two-Speed Start-up (Internal/External Oscillator Switchover) Control bit (Two-Speed Start-up enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Timer Postscaler Select bits (1:32768)

// CONFIG3L

// CONFIG3H
#pragma config ETHLED = ON      // Ethernet LED Enable bit (RA0/RA1 are multiplexed with LEDA/LEDB when Ethernet module is enabled and function as I/O when Ethernet is disabled)
#endif

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"

// Include functions specific to this stack application
#include "Main.h"
#include "Board.h"
#include "AppConfig.h"
#include "RTCC.h"
#include "Relay.h"
#include "I2CMaster.h"
#include "IO.h"
#include "MCP79402.h"
#include "ADC.h"
#include "Input.h"
#include "NTP.h"
#include "ddns.h"
#include "Ping.h"
#include "HTTPClient.h"
#include "EMAIL.h"
#include "IOHandler.h"

//
// PIC18 Interrupt Service Routines
// 
#pragma interruptlow LowISR

void LowISR(void) {
    TickUpdate();
    ////UpdateRTCC();
}

#pragma interruptlow HighISR

void HighISR(void) {

}

#pragma code lowVector=0x18

void LowVector(void) {
    _asm goto LowISR _endasm
}
#pragma code highVector=0x8

void HighVector(void) {
    _asm goto HighISR _endasm
}
#pragma code // Return to default code section

// Global variables
// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG appConfig, newAppConfig;
HASH_SUM md5;

/**
 * Main application entry point.
 */
void main(void) {
    static DWORD t = 0;
    static DWORD dwLastIP = 0;

    // 1 - Initialize application specific hardware
    InitializeBoard();
    // 2 - TickInit must be the one of the first initialization to use the software timeouts
    TickInit();
    // 3 - Initialize I2C used by ExtRTCC
    InitI2C();
    // 4 - Initialize External RTCC (MAC Address)
    InitExtRTCC();
    // 5 - Initialize MPFS file system linked to internal Flash
    MPFSInit();
    // 6 - Check user restore trigger
    checkManualRestoreDefault();
    // 7 - Initialize Stack and application related NV variables
    InitAppConfig();
    // 8 - Initialize core stack layers (MAC, ARP, TCP, UDP) and application modules (HTTP, SNMP, etc.)
    StackInit();
    // 9 - NTP Client initialization
    NTPInit();
    // 10 - Ping handler
    InitPing();
    // 11 - Reboot manager
    rebootInit();
    // 12 - Do a DDNS connection
    ddnsInit();
    // 13 - Initialize HTTP Client
    HTTPClientInit();
    // 14 - Initialize EMail
    EmailInit();
    // 15 - Initialize relay
    InitRelaies();
    // 16 - Initialize IO
    InitIO();
    // 17 - Initialize Input
    InitInput();
    // 18 - Initialize ADC
    InitADC();


    // Now that all items are initialized, begin the co-operative
    // multitasking loop.  This infinite loop will continuously
    // execute all stack-related tasks, as well as your own
    // application's functions.  Custom functions should be added
    // at the end of this loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must be broken
    // down into smaller pieces so that other tasks can have CPU time.
    while (1) {
        // Blink LED0 (right most one) every second.
        if (TickGet() - t >= TICK_SECOND / 2ul) {
            t = TickGet();
            LED_0_INV();
        }

        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();
        // This tasks invokes each of the core stack application tasks
        StackApplications();
        // Ping request task handler
        PingTask();
        // NTP Handler
        UDPClientNTPHandler();
        
        InputTask();
        IOTask();
        ADCTask();
        RelayTesk();
        
        ExtRTCCSetTimeFromNTPTask();
        rebootTask();
        HTTPClientTask();
        sendEmailTask();

        // If the local IP address has changed (ex: due to DHCP lease change)
        // write the new IP address to Announce service
        if (dwLastIP != appConfig.ip.fields.MyIPAddr.Val) {
            dwLastIP = appConfig.ip.fields.MyIPAddr.Val;
#if defined(STACK_USE_ANNOUNCE)
            AnnounceIP();
#endif
        }
    }
}



