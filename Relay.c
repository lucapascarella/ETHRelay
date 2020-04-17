
#include "Relay.h"
#include "AppConfig.h"
#include "MCP79402.h"

void InitRelaies(void) {

    int i;

    // Set the values in accord to stored values
    for (i = 0; i < 4; i++)
        setRelay(RELAY_1 + i, appConfig.gpio.fields.relay[i].bits.startUp);

    SetRelay1AsOutput();
    SetRelay2AsOutput();
    SetRelay3AsOutput();
    SetRelay4AsOutput();
}

void RelayTask(void) {
    BYTE i, hour, minute, onh, onm, offh, offm;
    WORD cTime, on1, off1, on2, off2;
    BOOL act, dir;

    hour = ExtRTCCGetHours();
    minute = ExtRTCCGetMinutes();
    cTime = getTime(hour, minute);
    for (i = 0; i < 4; i++) {
        act = FALSE;
        // Get minutes from timer 1
        onh = getRelayOn1Hour(i + 1);
        onm = getRelayOn1Minute(i + 1);
        offh = getRelayOff1Hour(i + 1);
        offm = getRelayOff1Minute(i + 1);
        on1 = getTime(onh, onm);
        off1 = getTime(offh, offm);
        // Get minutes from timer 2
        onh = getRelayOn2Hour(i + 1);
        onm = getRelayOn2Minute(i + 1);
        offh = getRelayOff2Hour(i + 1);
        offm = getRelayOff2Minute(i + 1);
        on2 = getTime(onh, onm);
        off2 = getTime(offh, offm);
        if (on1 == off1 && on2 == off2) {
            // Disabled
            Nop();
        } else if (on1 > off1 && on2 <= off2) {
            // T1 inverted and T2 normal or disabled
            if (cTime < off1) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
            if (cTime >= off1) {
                act = TRUE;
                dir = RELAY_LOW;
            }

            if (cTime >= on2 && cTime < off2) {
                act = TRUE;
                dir = RELAY_HIGH;
            }

            if (cTime >= on1) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
        } else if (on2 > off2 && on1 <= off1) {
            // T2 inverted and T1 normal or disabled
            if (cTime < off2) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
            if (cTime >= off2) {
                act = TRUE;
                dir = RELAY_LOW;
            }

            if (cTime >= on1 && cTime < off1) {
                act = TRUE;
                dir = RELAY_HIGH;
            }

            if (cTime >= on2) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
        } else if (on1 <= off1 && on2 <= off2) {
            // Both normal or one disabled
            if (cTime < on1 && cTime < on2) {
                act = TRUE;
                dir = RELAY_LOW;
            }

            if (on1 < on2 && cTime >= on1 && cTime < off1) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
            if (on1 < on2 && cTime > on1 && cTime >= off1) {
                act = TRUE;
                dir = RELAY_LOW;
            }
            if (on2 < on1 && cTime >= on2 && cTime < off2) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
            if (on2 < on1 && cTime > on2 && cTime >= off2) {
                act = TRUE;
                dir = RELAY_LOW;
            }

            if (on1 < on2 && cTime >= on2 && cTime < off2) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
            if (on1 < on2 && cTime > on2 && cTime >= off2) {
                act = TRUE;
                dir = RELAY_LOW;
            }
            if (on2 < on1 && cTime >= on1 && cTime < off1) {
                act = TRUE;
                dir = RELAY_HIGH;
            }
            if (on2 < on1 && cTime > on1 && cTime >= off1) {
                act = TRUE;
                dir = RELAY_LOW;
            }

            if (cTime >= off1 && cTime >= off2) {
                act = TRUE;
                dir = RELAY_LOW;
            }
        } else {
            // Not allowed!
            Nop();
        }
        
        // Perform action
        if (act == TRUE) {
            setRelay(i + 1, dir);
            act = FALSE;
        }
    }
}

void setRelay(BYTE relay, BYTE state) {
    switch (relay) {
        case RELAY_1:
            SetRelay1(state);
            break;
        case RELAY_2:
            SetRelay2(state);
            break;
        case RELAY_3:
            SetRelay3(state);
            break;
        case RELAY_4:
            SetRelay4(state);

            break;
    }
}

BOOL getRelay(BYTE relay) {
    BOOL ret;

    switch (relay) {
        case RELAY_1:
            ret = GetRelay1();
            break;
        case RELAY_2:
            ret = GetRelay2();
            break;
        case RELAY_3:
            ret = GetRelay3();
            break;
        case RELAY_4:
            ret = GetRelay4();

            break;
    }
    return ret;
}

void setStartUpRelay(BYTE relay, BOOL state) {

    appConfig.gpio.fields.relay[relay - 1].bits.startUp = state;
    //    switch (relay) {
    //        case RELAY_1:
    //            appConfig.gpio.fields.flags.bits.startUpRelay1 = state;
    //            break;
    //        case RELAY_2:
    //            appConfig.gpio.fields.flags.bits.startUpRelay2 = state;
    //            break;
    //        case RELAY_3:
    //            appConfig.gpio.fields.flags.bits.startUpRelay3 = state;
    //            break;
    //        case RELAY_4:
    //            appConfig.gpio.fields.flags.bits.startUpRelay4 = state;
    //            break;
    //    }
    saveAppConfig();
}

BOOL getStartUpRelay(BYTE relay) {
    return appConfig.gpio.fields.relay[relay - 1].bits.startUp;
    //    switch (relay) {
    //        case RELAY_1:
    //            return appConfig.gpio.fields.flags.bits.startUpRelay1;
    //        case RELAY_2:
    //            return appConfig.gpio.fields.flags.bits.startUpRelay2;
    //        case RELAY_3:
    //            return appConfig.gpio.fields.flags.bits.startUpRelay3;
    //        case RELAY_4:
    //            return appConfig.gpio.fields.flags.bits.startUpRelay4;
    //    }
}
