#include "Board.h"

/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.  It is a generic initialization
    routine for many of the Microchip development boards, using definitions
    in HardwareProfile.h to determine specific initialization.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
 ***************************************************************************/

#include "Board.h"
#include "Relay.h"
#include "IO.h"
#include "ADC.h"
#include "Input.h"
#include "HardwareProfile.h"
#include "TCPIP Stack/TCPIP.h"

void InitializeBoard(void) {

    // Enable 3x POST DIV, enable 5x PLL, disable POST DIV and 3x PRE DIV
    OSCTUNE = 0x40;

    // Initialize LEDs
    LED_0_TRIS = OUTPUT;

    // Turn off LEDs
    LED_0_O = 1;





    // Postscaler don't care, 5x PLL enabled, Postscaler disabled, Prescaler 2x => 41.667 MHz
    //OSCTUNE = 0x50;

    // Set up analog features of PORTA

    //    // PICDEM.net 2 board has POT on AN2, Temp Sensor on AN3
    //#if defined(PICDEMNET2)
    //    ADCON0 = 0x09; // ADON, Channel 2
    //    ADCON1 = 0x0B; // Vdd/Vss is +/-REF, AN0, AN1, AN2, AN3 are analog
    //#elif defined(PICDEMZ)
    //    ADCON0 = 0x81; // ADON, Channel 0, Fosc/32
    //    ADCON1 = 0x0F; // Vdd/Vss is +/-REF, AN0, AN1, AN2, AN3 are all digital
    //#elif defined(__18F87J11) || defined(_18F87J11) || defined(__18F87J50) || defined(_18F87J50)
    //    ADCON0 = 0x01; // ADON, Channel 0, Vdd/Vss is +/-REF
    //    WDTCONbits.ADSHR = 1;
    //    ANCON0 = 0xFC; // AN0 (POT) and AN1 (temp sensor) are anlog
    //    ANCON1 = 0xFF;
    //    WDTCONbits.ADSHR = 0;
    //#else
    //    ADCON0 = 0x01; // ADON, Channel 0
    //    ADCON1 = 0x0E; // Vdd/Vss is +/-REF, AN0 is analog
    //#endif
    //    ADCON2 = 0xBE; // Right justify, 20TAD ACQ time, Fosc/64 (~21.0kHz)


    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;

    // Configure USART
    TXSTA = 0x20;
    RCSTA = 0x90;
    //
    //    // See if we can use the high baud rate setting
    //#if ((GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1) <= 255
    //    SPBRG = (GetPeripheralClock() + 2 * BAUD_RATE) / BAUD_RATE / 4 - 1;
    //    TXSTAbits.BRGH = 1;
    //#else	// Use the low baud rate setting
    //    SPBRG = (GetPeripheralClock() + 8 * BAUD_RATE) / BAUD_RATE / 16 - 1;
    //#endif


    // Enable Interrupts
    RCONbits.IPEN = 1; // Enable interrupt priorities
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

    //    // Do a calibration A/D conversion
    //#if defined(__18F87J10) || defined(__18F86J15) || defined(__18F86J10) || defined(__18F85J15) || defined(__18F85J10) || defined(__18F67J10) || defined(__18F66J15) || defined(__18F66J10) || defined(__18F65J15) || defined(__18F65J10) || defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) || \
//	     defined(_18F87J10) ||  defined(_18F86J15) || defined(_18F86J10)  ||  defined(_18F85J15) ||  defined(_18F85J10) ||  defined(_18F67J10) ||  defined(_18F66J15) ||  defined(_18F66J10) ||  defined(_18F65J15) ||  defined(_18F65J10) ||  defined(_18F97J60) ||  defined(_18F96J65) ||  defined(_18F96J60) ||  defined(_18F87J60) ||  defined(_18F86J65) ||  defined(_18F86J60) ||  defined(_18F67J60) ||  defined(_18F66J65) ||  defined(_18F66J60)
    //    ADCON0bits.ADCAL = 1;
    //    ADCON0bits.GO = 1;
    //    while (ADCON0bits.GO);
    //    ADCON0bits.ADCAL = 0;
    //#elif defined(__18F87J11) || defined(__18F86J16) || defined(__18F86J11) || defined(__18F67J11) || defined(__18F66J16) || defined(__18F66J11) || \
//		   defined(_18F87J11) ||  defined(_18F86J16) ||  defined(_18F86J11) ||  defined(_18F67J11) ||  defined(_18F66J16) ||  defined(_18F66J11) || \
//		  defined(__18F87J50) || defined(__18F86J55) || defined(__18F86J50) || defined(__18F67J50) || defined(__18F66J55) || defined(__18F66J50) || \
//		   defined(_18F87J50) ||  defined(_18F86J55) ||  defined(_18F86J50) ||  defined(_18F67J50) ||  defined(_18F66J55) ||  defined(_18F66J50)
    //    ADCON1bits.ADCAL = 1;
    //    ADCON0bits.GO = 1;
    //    while (ADCON0bits.GO);
    //    ADCON1bits.ADCAL = 0;
    //#endif


}

REBOOT_CONFIG reboot;

void rebootInit(void) {
    reboot.milliseconds = 0;
    reboot.timer = 0;
    reboot.flags.reboot = FALSE;
    reboot.sm = RB_SM_HOME;
}

void rebootTask(void) {

    switch (reboot.sm) {
        case RB_SM_HOME:
            reboot.sm++;
            break;

        case RB_SM_WAIT_COMMAND:
            if (reboot.flags.reboot) {
                SetSystemLedHigh();
                reboot.flags.reboot = FALSE;
                reboot.timer = TickGet();
                reboot.sm++;
            }
            break;

        case RB_SM_START_WAIT:
            if (TickGet() - reboot.timer > reboot.milliseconds) {
                reboot.sm++;
            }
            break;

        case RB_SM_REBOOT:
            Reset();
            break;
    }
}

void askRebootInMilliSeconds(DWORD msec) {
    reboot.milliseconds = (float) (msec / 1000) * TICK_SECOND;
    reboot.flags.reboot = TRUE;
}