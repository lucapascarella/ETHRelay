#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "RTCC.h"

BYTE rtccYear, rtccMon, rtccMday, rtccHour, rtccMin, rtccSec;

void InitRTCC(void) {

    // Use Timer1 for 8 bit processors
    rtccYear = 2016 - 1980;
    rtccMon = 1;
    rtccMday = 1;
    rtccHour = 0;
    rtccMin = 0;
    rtccSec = 0;

    // Initialize the timer
    TMR1H = 0;
    TMR1L = 0;

    // Set up the timer interrupt
    IPR1bits.TMR1IP = 0; // Low priority
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1; // Enable interrupt

    // RD16: 16-Bit Read/Write Mode Enable bit
    T1CONbits.RD16 = TRUE; // 1 = Enables register read/write of Timer1 in one 16-bit operation
    // T1RUN: Timer1 System Clock Status bit
    T1CONbits.T1RUN = FALSE; // 0 = Device clock is derived from another source
    // T1CKPS<1:0>: Timer1 Input Clock Prescale Select bits
    T1CONbits.T1CKPS = 0x00; // 00 = 1:1 Prescale value
    // T1OSCEN: Timer1 Oscillator Enable bit
    T1CONbits.T1OSCEN = FALSE; // 0 = Timer1 oscillator is shut off 
    // TMR1CS: Timer1 Clock Source Select bit
    T1CONbits.TMR1CS = TRUE; // 1 = External clock from RC0/T1OSO/T13CKI pin (on the rising edge)
    // TMR1ON: Timer1 On bit
    T1CONbits.TMR1ON = TRUE; // 1 = Enables Timer1

}

/*
 * Catch this event every 2 seconds 
 */
void UpdateRTCC(void) {

    static const BYTE dom[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static int div1k;
    BYTE n;

    if (PIR1bits.TMR1IF) {
        // implement a 'fake' RTCC
        if (++div1k >= 1000) {
            div1k = 0;
            if (++rtccSec >= 60) {
                rtccSec = 0;
                if (++rtccMin >= 60) {
                    rtccMin = 0;
                    if (++rtccHour >= 24) {
                        rtccHour = 0;
                        n = dom[rtccMon - 1];
                        if ((n == 28) && !(rtccYear & 3)) n++;
                        if (++rtccMday > n) {
                            rtccMday = 1;
                            if (++rtccMon > 12) {
                                rtccMon = 1;
                                rtccYear++;
                            }
                        }
                    }
                }
            }
        }
        // Reset interrupt flag
        PIR1bits.TMR1IF = 0;
    }
}

void RTCCGetTime(EXT_RTCC *rtcc) {

}

