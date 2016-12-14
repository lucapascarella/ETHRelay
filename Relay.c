
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

void RelayTesk(void) {
    BYTE i, hour, minute, hon, mon, hoff, moff;
    BOOL act[4], dir[4];

    hour = ExtRTCCGetHours();
    minute = ExtRTCCGetMinutes();
    for (i = 0; i < 4; i++) {
        if (getOnHour(i + 1) != getOffHour(i + 1) || getOnMinute(i + 1) != getOffMinute(i + 1)) {
            hon = getOnHour(i + 1);
            mon = getOnMinute(i + 1);
            hoff = getOffHour(i + 1);
            moff = getOffMinute(i + 1);
            if (getOnHour(i + 1) < getOffHour(i + 1)) {
                if (hour >= getOnHour(i + 1) && minute >= getOnMinute(i + 1)) {
                    act[i] = TRUE;
                    dir[i] = RELAY_HIGH;
                }
                if (hour >= getOffHour(i + 1) && minute >= getOffMinute(i + 1)) {
                    act[i] = TRUE;
                    dir[i] = RELAY_LOW;
                }
            } else if (getOnHour(i + 1) == getOffHour(i + 1)) {
                if (getOnMinute(i + 1) < getOffMinute(i + 1)) {
                    if (hour >= getOnHour(i + 1) && minute >= getOnMinute(i + 1)) {
                        act[i] = TRUE;
                        dir[i] = RELAY_HIGH;
                    }
                    if (hour >= getOffHour(i + 1) && minute >= getOffMinute(i + 1)) {
                        act[i] = TRUE;
                        dir[i] = RELAY_LOW;
                    }
                } else {
                    // Turn on time is less than Turn off
                    if (hour <= getOnHour(i + 1) && minute <= getOnMinute(i + 1)) {
                        act[i] = TRUE;
                        dir[i] = RELAY_HIGH;
                    }
                    if (hour <= getOffHour(i + 1) && minute <= getOffMinute(i + 1)) {
                        act[i] = TRUE;
                        dir[i] = RELAY_LOW;
                    }
                }
            } else {
                // Turn on time is less than Turn off
                if (hour <= getOnHour(i + 1) && minute <= getOnMinute(i + 1)) {
                    act[i] = TRUE;
                    dir[i] = RELAY_HIGH;
                }
                if (hour <= getOffHour(i + 1) && minute <= getOffMinute(i + 1)) {
                    act[i] = TRUE;
                    dir[i] = RELAY_LOW;
                }
            }
        }
    }

    for (i = 0; i < 4; i++) {
        if (act[i] == TRUE) {
            act[i] = FALSE;
            setRelay(i + 1, dir[i]);
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

//BYTE getOnHour(BYTE relay) {
//    return appConfig.gpio.fields.relay[relay - 1].onh;
//}
//
//BYTE getOnMinute(BYTE relay) {
//    return appConfig.gpio.fields.relay[relay - 1].onm;
//}
//
//BYTE getOffHour(BYTE relay) {
//    return appConfig.gpio.fields.relay[relay - 1].offh;
//}
//
//BYTE getOffMinute(BYTE relay) {
//    return appConfig.gpio.fields.relay[relay - 1].offm;
//}
