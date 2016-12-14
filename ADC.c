
#include "ADC.h"
#include "AppConfig.h"
#include "IO.h"
#include "Relay.h"
#include "EMAIL.h"

ADC_CONFIG adc;

void InitADC(void) {
    BYTE i;

    // Vdd/Vss is +/-REF, (AN0), (AN1), AN2, AN3, AN4 and AN6 are analog
    ADCON1 = 0x08;

    // Right justify, 20TAD ACQ time, Fosc/64 (~390.0kHz)
    ADCON2 = 0xBE;

    // Initialize ADCON0 and enable the ADC
    ADCON0 = 0x00;
    ADCON0bits.ADON = TRUE;

    // Do a calibration A/D conversion
    ADCON0bits.ADCAL = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    ADCON0bits.ADCAL = 0;

    // Initialize adc config
    for (i = 0; i < 4; i++) {
        adc.flags[i].highTh = FALSE;
        adc.flags[i].lowTh = FALSE;
        adc.flags[i].send = FALSE;
    }
}

void ADCTask(void) {
    BYTE i, output, relay;
    WORD adcVal;

    for (i = 0; i < 4; i++) {
        adcVal = readADC(i + 1);
        if (adcVal > getHighThreshold(i + 1) && adc.flags[i].highTh == FALSE) {
            adc.flags[i].highTh = TRUE;
            adc.flags[i].lowTh = FALSE;
            adc.flags[i].send = TRUE;
        } else if (adcVal < getLowThreshold(i + 1) && adc.flags[i].lowTh == FALSE) {
            adc.flags[i].lowTh = TRUE;
            adc.flags[i].highTh = FALSE;
            adc.flags[i].send = TRUE;
        }
    }
    // Check to see if the HTTP Client is free and a request must be sent
    for (i = 0; i < 4; i++) {
        if (adc.flags[i].send) {
            adc.flags[i].send = FALSE;
            // Send notification to local IO o Relay
            output = appConfig.gpio.fields.analog[i].output;
            relay = appConfig.gpio.fields.analog[i].relay;
            if (output > 0 && getDirectionIO(output) == IO_OUTPUT) {
                if (adc.flags[i].lowTh)
                    setOutputStateIO(output, ~getDefault(i + 1));
                else if (adc.flags[i].highTh)
                    setOutputStateIO(output, getDefault(i + 1));
                // Set flag to send email on event change
                setSendEmailFlag();
            }
            if (relay > 0) {
                if (adc.flags[i].lowTh)
                    setRelay(relay, ~getDefault(i + 1));
                else if (adc.flags[i].highTh)
                    setRelay(relay, getDefault(i + 1));
                // Set flag to send email on event change
                setSendEmailFlag();
            }
        }
    }
}

INT16 readADC(BYTE adc) {
    UINT16_VAL read;
    switch (adc) {
        case ADC_1: // AN2
            ADCON0bits.CHS = 0x02;
            break;
        case ADC_2: // AN3
            ADCON0bits.CHS = 0x03;
            break;
        case ADC_3: // AN4
            ADCON0bits.CHS = 0x04;
            break;
        case ADC_4: // AN6
            ADCON0bits.CHS = 0x06;
            break;
    }
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    read.byte.LB = ADRESL;
    read.byte.HB = ADRESH;
    return read.Val;
}

void setDefault(BYTE adc, BOOL state) {
    
    appConfig.gpio.fields.analog[adc - 1].bits.startUp = state;
    
//    switch (adc) {
//        case ADC_1: // AN2
//            appConfig.gpio.fields.flags.bits.analogDefault1 = state;
//            break;
//        case ADC_2: // AN3
//            appConfig.gpio.fields.flags.bits.analogDefault2 = state;
//            break;
//        case ADC_3: // AN4
//            appConfig.gpio.fields.flags.bits.analogDefault3 = state;
//            break;
//        case ADC_4: // AN6
//            appConfig.gpio.fields.flags.bits.analogDefault4 = state;
//            break;
//    }
}

BOOL getDefault(BYTE adc) {
    
    return appConfig.gpio.fields.analog[adc - 1].bits.startUp;
    
//    switch (adc) {
//        case ADC_1: // AN2
//            return appConfig.gpio.fields.flags.bits.analogDefault1;
//        case ADC_2: // AN3
//            return appConfig.gpio.fields.flags.bits.analogDefault2;
//        case ADC_3: // AN4
//            return appConfig.gpio.fields.flags.bits.analogDefault3;
//        case ADC_4: // AN6
//            return appConfig.gpio.fields.flags.bits.analogDefault4;
//    }
}
