#include "Test.h"
#include "Relay.h"
#include "ADC.h"
#include "IO.h"
#include "Input.h"
#include "AppConfig.h"

#define DELAY_INTERVAL      100

void TestStart(void) {

    DWORD timeout;
    INT32 i;

    timeout = TickGet();
    if (readInput(INPUT_1) == BUTTON_PRESSED && readInput(INPUT_2) == BUTTON_PRESSED) {
        while (TickGet() - timeout < TICK_SECOND * 2 && readInput(INPUT_1) == BUTTON_PRESSED && readInput(INPUT_2) == BUTTON_PRESSED);
        if (readInput(INPUT_1) == BUTTON_PRESSED && readInput(INPUT_2) == BUTTON_PRESSED) {
            TestRelay();
            TestInputOutput();
            TestADC();
            TestEEPROM();
            //TestEthGet();

            // Clear all EEPROM content
            //            XEEBeginWrite(0x0000);
            //            for (i = 0; i < 0x20000; i++)
            //                XEEWrite(0xFF);
            //            XEEEndWrite();
        }
    }
}

BYTE array[256];

void TestEEPROM(void) {
    INT16 i;

    for (i = 0; i < sizeof (array); i++)
        array[i] = i;

    XEEBeginWrite(0x0000);
    XEEWriteArray(array, sizeof (array));
    XEEEndWrite();

    for (i = 0; i < sizeof (array); i++)
        array[i] = 0x00;

    XEEReadArray(0x0000, array, sizeof (array));
    XEEEndWrite();

    for (i = 0; i < sizeof (array); i++) {
        if (array[i] != i) {
            setOutputStateHighIO(IO_1);
            while (1);
        }
    }
}

void TestEthGet(void) {

    strcpypgm(array, "");
    //XEEReadArray(XEEPROM_ADDRESS((WORD) (hc.num - 1)), str, XEEPROM_ADDRESS_SIZE - 1);
    Nop();
    Nop();
}

void TestRelay(void) {
    SetRelay1High();
    DelayMs(DELAY_INTERVAL);
    SetRelay1Low();
    SetRelay2High();
    DelayMs(DELAY_INTERVAL);
    SetRelay2Low();
    SetRelay3High();
    DelayMs(DELAY_INTERVAL);
    SetRelay3Low();
    SetRelay4High();
    DelayMs(DELAY_INTERVAL);
    SetRelay4Low();
}

void TestADC(void) {
    INT16 a1, a2, a3, a4;

    a1 = readADC(ADC_1);
    a2 = readADC(ADC_2);
    a3 = readADC(ADC_3);
    a4 = readADC(ADC_4);

    if (a1 > 10 || a2 > 10 || a3 > 10 || a4 > 10) {
        setOutputStateHighIO(IO_2);
        while (1);
    }
}

void TestInputOutput(void) {
    BYTE i;
    BOOL input;

    for (i = 0; i < 8; i++)
        setDirectionOutputIO(IO_1 + i);

    for (i = 0; i < 8; i++) {
        setOutputStateHighIO(IO_1 + i);
        Delay10us(10);
        input = readInputIO(IO_1 + i);
        if (input != TRUE)
            while (1);
        DelayMs(DELAY_INTERVAL);
    }

    for (i = 0; i < 8; i++) {
        setOutputStateLowIO(IO_1 + i);
        Delay10us(10);
        input = readInputIO(IO_1 + i);
        if (input != FALSE)
            while (1);
        DelayMs(DELAY_INTERVAL);
    }
}