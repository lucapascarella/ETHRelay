
#include "IO.h"
#include "AppConfig.h"
#include "HTTPClient.h"


#if ((XEEPROM_ADDRESS_SIZE + XEEPROM_USERNAME_SIZE + XEEPROM_PASSWORD_SIZE) > XEEPROM_PAGE_SIZE)
#error XEEPROM page exception
#endif

#if ((XEEPROM_ADDRESS_SIZE + XEEPROM_USERNAME_SIZE + XEEPROM_PASSWORD_SIZE) * 8 > INPUT_RESERVED_BLOCK)
#error XEEPROM reserved block exception
#endif

IO_CONFIG io[8];

void InitIO(void) {
    BYTE i;

    // Initialize output pins
    OUTPUT_1_TRIS = OUTPUT;
    OUTPUT_2_TRIS = OUTPUT;
    OUTPUT_3_TRIS = OUTPUT;
    OUTPUT_4_TRIS = OUTPUT;
    OUTPUT_5_TRIS = OUTPUT;
    OUTPUT_6_TRIS = OUTPUT;
    OUTPUT_7_TRIS = OUTPUT;
    OUTPUT_8_TRIS = OUTPUT;

    // Initialize inputs pins
    INPUT_1_TRIS = INPUT;
    INPUT_2_TRIS = INPUT;
    INPUT_3_TRIS = INPUT;
    INPUT_4_TRIS = INPUT;
    INPUT_5_TRIS = INPUT;
    INPUT_6_TRIS = INPUT;
    INPUT_7_TRIS = INPUT;
    INPUT_8_TRIS = INPUT;

    // Initialize the output pins to "high impedance" for input control
    OUTPUT_1_O = IO_LOW;
    OUTPUT_2_O = IO_LOW;
    OUTPUT_3_O = IO_LOW;
    OUTPUT_4_O = IO_LOW;
    OUTPUT_5_O = IO_LOW;
    OUTPUT_6_O = IO_LOW;
    OUTPUT_7_O = IO_LOW;
    OUTPUT_8_O = IO_LOW;

    // Define default direction
    for (i = 0; i < 8; i++)
        setDirectionIO(IO_1 + i, appConfig.gpio.fields.ioBits[i].ioDirection);

    // Define default status
    for (i = 0; i < 8; i++)
        setOutputStateIO(IO_1 + i, appConfig.gpio.fields.ioBits[i].ioDefault);

    for (i = 0; i < 8; i++) {
        io[i].bits.hasChanged = FALSE;
        io[i].bits.status = readInputIO(i + 1);
    }
}

void IOTask(void) {
    WORD cTime, on1, off1, on2, off2;
    BYTE i, hour, minute, onh, onm, offh, offm;
    BOOL status, act, dir;

    for (i = 0; i < 8; i++) {
        if (getDirectionIO(i + 1) == IO_INPUT) {
            if (appConfig.gpio.fields.ioBits[i].ioNotice != 0) {
                status = readInputIO(i + 1);
                if (io[i].bits.status != status) {
                    io[i].bits.status = status;
                    io[i].bits.hasChanged = TRUE;
                }
            }
        }
    }
    // Check to see if the HTTP Client is free and a request must be sent
    for (i = 0; i < 8; i++) {
        if (io[i].bits.hasChanged && HTTPClientIsReady() == 0) {
            io[i].bits.hasChanged = FALSE;
            HTTPClientSendRequest(i + 1, io[i].bits.status);
        }
    }

    // Check timer
    hour = ExtRTCCGetHours();
    minute = ExtRTCCGetMinutes();
    cTime = getTime(hour, minute);
    for (i = 0; i < 8; i++) {
        if (getDirectionIO(i + 1) == IO_OUTPUT) {
            act = FALSE;
            // Get minutes from timer 1
            onh = getOutOn1Hour(i + 1);
            onm = getOutOn1Minute(i + 1);
            offh = getOutOff1Hour(i + 1);
            offm = getOutOff1Minute(i + 1);
            on1 = getTime(onh, onm);
            off1 = getTime(offh, offm);
            // Get minutes from timer 2
            onh = getOutOn2Hour(i + 1);
            onm = getOutOn2Minute(i + 1);
            offh = getOutOff2Hour(i + 1);
            offm = getOutOff2Minute(i + 1);
            on2 = getTime(onh, onm);
            off2 = getTime(offh, offm);
            if (on1 == off1 && on2 == off2) {
                // Disabled
                Nop();
            } else if (on1 > off1 && on2 <= off2) {
                // T1 inverted and T2 normal or disabled
                if (cTime < off1) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
                if (cTime >= off1) {
                    act = TRUE;
                    dir = IO_LOW;
                }

                if (cTime >= on2 && cTime < off2) {
                    act = TRUE;
                    dir = IO_HIGH;
                }

                if (cTime >= on1) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
            } else if (on2 > off2 && on1 <= off1) {
                // T2 inverted and T1 normal or disabled
                if (cTime < off2) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
                if (cTime >= off2) {
                    act = TRUE;
                    dir = IO_LOW;
                }

                if (cTime >= on1 && cTime < off1) {
                    act = TRUE;
                    dir = IO_HIGH;
                }

                if (cTime >= on2) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
            } else if (on1 <= off1 && on2 <= off2) {
                // Both normal or one disabled
                if (cTime < on1 && cTime < on2) {
                    act = TRUE;
                    dir = IO_LOW;
                }

                if (on1 < on2 && cTime >= on1 && cTime < off1) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
                if (on1 < on2 && cTime > on1 && cTime >= off1) {
                    act = TRUE;
                    dir = IO_LOW;
                }
                if (on2 < on1 && cTime >= on2 && cTime < off2) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
                if (on2 < on1 && cTime > on2 && cTime >= off2) {
                    act = TRUE;
                    dir = IO_LOW;
                }

                if (on1 < on2 && cTime >= on2 && cTime < off2) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
                if (on1 < on2 && cTime > on2 && cTime >= off2) {
                    act = TRUE;
                    dir = IO_LOW;
                }
                if (on2 < on1 && cTime >= on1 && cTime < off1) {
                    act = TRUE;
                    dir = IO_HIGH;
                }
                if (on2 < on1 && cTime > on1 && cTime >= off1) {
                    act = TRUE;
                    dir = IO_LOW;
                }

                if (cTime >= off1 && cTime >= off2) {
                    act = TRUE;
                    dir = IO_LOW;
                }
            } else {
                // Not allowed!
                Nop();
            }
            // Perform action
            if (act == TRUE) {
                setOutputStateIO(i + 1, dir);
                act = FALSE;
            }
        }
    }
}

void setDirectionIO(BYTE io, BYTE dir) {
    if (dir == IO_OUTPUT)
        setDirectionOutputIO(io);
    else if (dir == IO_INPUT)
        setDirectionInputIO(io);
}

void setDirectionInputIO(BYTE io) {
    switch (io) {
        case IO_1:
            // INPUT_1_TRIS = INPUT;
            // OUTPUT_1_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_1_O = IO_LOW;
            break;
        case IO_2:
            // INPUT_2_TRIS = INPUT;
            // OUTPUT_2_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_2_O = IO_LOW;
            break;
        case IO_3:
            // INPUT_3_TRIS = INPUT;
            // OUTPUT_3_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_3_O = IO_LOW;
            break;
        case IO_4:
            // INPUT_4_TRIS = INPUT;
            // OUTPUT_4_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_4_O = IO_LOW;
            break;
        case IO_5:
            // INPUT_5_TRIS = INPUT;
            // OUTPUT_5_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_5_O = IO_LOW;
            break;
        case IO_6:
            // INPUT_6_TRIS = INPUT;
            // OUTPUT_6_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_6_O = IO_LOW;
            break;
        case IO_7:
            // INPUT_7_TRIS = INPUT;
            // OUTPUT_7_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_7_O = IO_LOW;
            break;
        case IO_8:
            // INPUT_8_TRIS = INPUT;
            // OUTPUT_8_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_INPUT;
            OUTPUT_8_O = IO_LOW;
            break;
    }
    saveAppConfig();
}

void setDirectionOutputIO(BYTE io) {
    switch (io) {
        case IO_1:
            // INPUT_1_TRIS = INPUT;
            // OUTPUT_1_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_2:
            // INPUT_2_TRIS = INPUT;
            // OUTPUT_2_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_3:
            // INPUT_3_TRIS = INPUT;
            // OUTPUT_3_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_4:
            // INPUT_4_TRIS = INPUT;
            // OUTPUT_4_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_5:
            // INPUT_5_TRIS = INPUT;
            // OUTPUT_5_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_6:
            // INPUT_6_TRIS = INPUT;
            // OUTPUT_6_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_7:
            // INPUT_7_TRIS = INPUT;
            // OUTPUT_7_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
        case IO_8:
            // INPUT_8_TRIS = INPUT;
            // OUTPUT_8_TRIS = OUTPUT;
            appConfig.gpio.fields.ioBits[io - 1].ioDirection = IO_OUTPUT;
            break;
    }
    saveAppConfig();
}

//BOOL getDirectionIO(BYTE io) {
//    switch (io) {
//        case IO_1:
//            return appConfig.gpio.fields.flags.bits.ioDirection1;
//        case IO_2:
//            return appConfig.gpio.fields.flags.bits.ioDirection2;
//        case IO_3:
//            return appConfig.gpio.fields.flags.bits.ioDirection3;
//        case IO_4:
//            return appConfig.gpio.fields.flags.bits.ioDirection4;
//        case IO_5:
//            return appConfig.gpio.fields.flags.bits.ioDirection5;
//        case IO_6:
//            return appConfig.gpio.fields.flags.bits.ioDirection6;
//        case IO_7:
//            return appConfig.gpio.fields.flags.bits.ioDirection7;
//        case IO_8:
//            return appConfig.gpio.fields.flags.bits.ioDirection8;
//    }
//}

BOOL readInputIO(BYTE io) {
    switch (io) {
        case IO_1:
            return ~INPUT_1_IO & 0x01;
        case IO_2:
            return ~INPUT_2_IO & 0x01;
        case IO_3:
            return ~INPUT_3_IO & 0x01;
        case IO_4:
            return ~INPUT_4_IO & 0x01;
        case IO_5:
            return ~INPUT_5_IO & 0x01;
        case IO_6:
            return ~INPUT_6_IO & 0x01;
        case IO_7:
            return ~INPUT_7_IO & 0x01;
        case IO_8:
            return ~INPUT_8_IO & 0x01;
    }
}

void setOutputStateHighIO(BYTE io) {
    setOutputStateIO(io, IO_HIGH);
}

void setOutputStateLowIO(BYTE io) {
    setOutputStateIO(io, IO_LOW);
}

void setOutputStateIO(BYTE io, BOOL state) {

    switch (io) {
        case IO_1:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_1_O = ~state;
            break;
        case IO_2:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_2_O = ~state;
            break;
        case IO_3:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_3_O = ~state;
            break;
        case IO_4:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_4_O = ~state;
            break;
        case IO_5:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_5_O = ~state;
            break;
        case IO_6:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_6_O = ~state;
            break;
        case IO_7:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_7_O = ~state;
            break;
        case IO_8:
            if (appConfig.gpio.fields.ioBits[io - 1].ioDirection == IO_OUTPUT)
                OUTPUT_8_O = ~state;
            break;
    }
}

//BOOL getStartupIO(BYTE io) {
//    switch (io) {
//        case IO_1:
//            return appConfig.gpio.fields.flags.bits.ioDefault1;
//        case IO_2:
//            return appConfig.gpio.fields.flags.bits.ioDefault2;
//        case IO_3:
//            return appConfig.gpio.fields.flags.bits.ioDefault3;
//        case IO_4:
//            return appConfig.gpio.fields.flags.bits.ioDefault4;
//        case IO_5:
//            return appConfig.gpio.fields.flags.bits.ioDefault5;
//        case IO_6:
//            return appConfig.gpio.fields.flags.bits.ioDefault6;
//        case IO_7:
//            return appConfig.gpio.fields.flags.bits.ioDefault7;
//        case IO_8:
//            return appConfig.gpio.fields.flags.bits.ioDefault8;
//    }
//}

//void setStartupIO(BYTE io, BOOL state) {
//
//    switch (io) {
//        case IO_1:
//            appConfig.gpio.fields.flags.bits.ioDefault1 = state;
//            break;
//        case IO_2:
//            appConfig.gpio.fields.flags.bits.ioDefault2 = state;
//            break;
//        case IO_3:
//            appConfig.gpio.fields.flags.bits.ioDefault3 = state;
//            break;
//        case IO_4:
//            appConfig.gpio.fields.flags.bits.ioDefault4 = state;
//            break;
//        case IO_5:
//            appConfig.gpio.fields.flags.bits.ioDefault5 = state;
//            break;
//        case IO_6:
//            appConfig.gpio.fields.flags.bits.ioDefault6 = state;
//            break;
//        case IO_7:
//            appConfig.gpio.fields.flags.bits.ioDefault7 = state;
//            break;
//        case IO_8:
//            appConfig.gpio.fields.flags.bits.ioDefault8 = state;
//            break;
//    }
//    saveAppConfig();
//}
