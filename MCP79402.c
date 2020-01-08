
#include <p18f67j60.h>

#include "MCP79402.h"
#include "NTP.h"
#include "TCPIP Stack/SNMP.h"

//************************************************************************************
//                      I2C RTCC DRIVERS 
//************************************************************************************

BOOL updateDate;

// initialization of the RTCC(MCP79410) 

BOOL InitExtRTCC(void) {
    DWORD timeout;
    BYTE SSPxADD;

    EXT_RTCC_CONTROL ctrl;
    EXT_RTCC_OSCTRIM osctrim;
    EXT_RTCC_RTCSEC sec;
    EXT_RTCC_RTCWKDAY wday;

    EXT_RTCC_PWRMIN pwrdownMin, pwrupMin;
    EXT_RTCC_PWRHOUR pwrdownHour, pwrupHour;
    EXT_RTCC_PWRDATE pwrdownDate, pwrupDate;
    EXT_RTCC_PWRMTH pwrdownMonth, pwrupMonth;

    SSPxADD = SSP1ADD;
    SSP1ADD = 0x19; // clock = FOSC/(4 * (SSPxADD + 1)) // 400 kHz

    // Read back the current CONTROL value
    ctrl.Val = ExtRTCCRead(CONTROL);
    // Square wave on MFP, no alarms, MFP = 32.768 Hz 
    ctrl.bits.OUT = FALSE; // 0 = MFP signal level is logic low
    ctrl.bits.SQWEN = TRUE; // 1 = Enable Square Wave Clock Output mode
    ctrl.bits.ALM1EN = FALSE; // 0 = Alarm 1 disabled
    ctrl.bits.ALM0EN = FALSE; // 0 = Alarm 0 disabled
    ctrl.bits.EXTOSC = FALSE; // 0 = Disable external 32.768 kHz input
    ctrl.bits.CRSTRIM = FALSE; // 0 = Disable Coarse Trim mode
    ctrl.bits.SQWFS = 3; // 0b11 = 32.768 kHz
    ExtRTCCWrite(ctrl.Val, CONTROL);

    // Keep the previous seconds value and start oscillator
    sec.Val = ExtRTCCRead(RTCSEC);
    if (sec.bits.ST == FALSE) {
        sec.bits.ST = TRUE; // 1 = Oscillator enabled
        ExtRTCCWrite(sec.Val, RTCSEC);
    }

    // Read back the current OSCTRIM value
    osctrim.Val = ExtRTCCRead(OSCTRIM);
    // osctrim.bits.SIGN = FALSE; // 0 = Subtract clocks to correct for fast time
    if (osctrim.bits.TRIMVAL != 0x00) {
        osctrim.bits.TRIMVAL = 0x00; // Disable digital trimming
        ExtRTCCWrite(ctrl.Val, OSCTRIM);
    }

    // Keep the previous week day value and clear events
    wday.Val = ExtRTCCRead(RTCWKDAY);
    if (wday.bits.PWRFAIL) {
        // Read Power-fail and Power-up timestamp registers
        pwrdownMin.Val = ExtRTCCRead(PWRDNMIN);
        pwrdownHour.Val = ExtRTCCRead(PWRDNHOUR);
        pwrdownDate.Val = ExtRTCCRead(PWRDNDATE);
        pwrdownMonth.Val = ExtRTCCRead(PWRDNMTH);

        pwrupMin.Val = ExtRTCCRead(PWRUPMIN);
        pwrupHour.Val = ExtRTCCRead(PWRUPHOUR);
        pwrupDate.Val = ExtRTCCRead(PWRUPDATE);
        pwrupMonth.Val = ExtRTCCRead(PWRUPMTH);
    }
    // wday.bits.OSCRUN = TRUE; // 1 = Oscillator is enabled and running
    if (wday.bits.PWRFAIL != FALSE || wday.bits.VBATEN != TRUE) {
        wday.bits.PWRFAIL = FALSE; // 0 = Primary power has not been lost
        wday.bits.VBATEN = TRUE; // 1 = VBAT input is enabled
        ExtRTCCWrite(sec.Val, RTCWKDAY);
    }

    timeout = TickGet();
    while (wday.bits.OSCRUN == FALSE) {
        if (TickGet() - timeout < 2 * TICK_SECOND) {
            SSP1ADD = SSPxADD; // Restore the previous baud rate
            return FALSE;
        }
        DelayMs(10);
        wday.Val = ExtRTCCRead(RTCWKDAY);
    }
    SSP1ADD = SSPxADD; // Restore the previous baud rate
    
    // Update the RTCC date as soon as the NTP is sync
    updateDate = TRUE;
    
    return TRUE;
}

BOOL ExtRTCCWriteSequential(BYTE *data, BYTE size, BYTE address, BYTE reg) {
    BYTE i, byte, SSPxADD;
    BOOL ret;

    SSPxADD = SSP1ADD;
    SSP1ADD = 0x19; // clock = FOSC/(4 * (SSPxADD + 1)) // 400 kHz

    // start I2C communication 
    i2c_start();
    // write DEVICE ADDRESS for RTCC WRITES (EEPROM write requires special address)
    ret = i2c_wr(address & 0xFE);
    // write the register's ADDRESS
    ret = i2c_wr(reg);
    // Write the data stream
    for (i = 0; i < size && ret == TRUE; i++) {
        byte = data[i];
        ret = i2c_wr(byte);
        //ret &= i2c_ack();
    }
    // stop I2C communication
    i2c_stop();
    SSP1ADD = SSPxADD; // Restore the previous baud rate

    return ret;
}

BOOL ExtRTCCReadSequential(BYTE *data, BYTE size, BYTE address, BYTE reg) {

    BYTE i, SSPxADD;
    BOOL ret;

    if (size == 0)
        return TRUE;

    SSPxADD = SSP1ADD;
    SSP1ADD = 0x19; // clock = FOSC/(4 * (SSPxADD + 1)) // 400 kHz

    // start I2C communication
    i2c_start();
    // write DEVICE ADDR for RTCC WRITES
    i2c_wr(address & 0xFE);
    // write the register ADDRESS 
    i2c_wr(reg);
    // Send restart operation for read sequence
    i2c_restart();
    // send the DEVICE ADDRESS for RTCC READS.
    ret = i2c_wr(address | 0x01);
    // Read size - 1 bytes with ACK
    size--;
    for (i = 0; i < size && ret == TRUE; i++) {
        data[i] = i2c_rd();
        i2c_ack(&ret);
    }
    // Read last byte with NACK
    data[i] = i2c_rd();
    i2c_nack(&ret);
    // stop I2C communication
    i2c_stop();
    SSP1ADD = SSPxADD; // Restore the previous baud rate

    return ret;
}

void ExtRTCCWrite(BYTE time_var, BYTE rtcc_reg) {
    BYTE SSPxADD;
    SSPxADD = SSP1ADD;
    SSP1ADD = 0x19; // clock = FOSC/(4 * (SSPxADD + 1)) // 400 kHz
    i2c_start(); // start I2C communication   
    i2c_wr(ADDR_RTCC_WRITE); // write DEVICE ADDR for RTCC WRITES
    i2c_wr(rtcc_reg); // write the register's ADDRESS
    i2c_wr(time_var); // write byte variable in the register 
    i2c_stop(); // stop I2C communication
    SSP1ADD = SSPxADD; // Restore the previous baud rate
}
//.....................................................................................                       

BYTE ExtRTCCRead(BYTE rtcc_reg) {
    BYTE rtcc_buf, SSPxADD;
    BOOL ret;

    SSPxADD = SSP1ADD;
    SSP1ADD = 0x19; // clock = FOSC/(4 * (SSPxADD + 1)) // 400 kHz

    i2c_start(); // start I2C communication
    i2c_wr(ADDR_RTCC_WRITE); // write DEVICE ADDR for RTCC WRITES
    i2c_wr(rtcc_reg); // write the register ADDRESS 
    i2c_restart(); // RESTART for READS 
    i2c_wr(ADDR_RTCC_READ); // send the DEVICE ADDRESS for RTCC READS.
    rtcc_buf = i2c_rd(); // read register (stored in 'rtcc_buf')
    i2c_nack(&ret); // NOACK from MASTER (last read byte)
    i2c_stop(); // stop I2C communication
    SSP1ADD = SSPxADD; // Restore the previous baud rate

    return rtcc_buf;
}




//.........................................................................................                            

//void ini_time(void) // initialization of time/date:2016_03_01 , 9am.
//{ // it initializes also :24H format, and START OSC.
//    if ((day & OSCRUN) == OSCRUN) {
//        ;
//    }// if oscillator = already running, do nothing. 
//    else { // if oscillator = not running, set time/date(arbitrary)
//        // and START oscillator/ crystal    
//        rtcc_wr(0x16, RTCYEAR); // initialize YEAR  register           
//        rtcc_wr(0x03, RTCMTH); // initialize MONTH register  
//        rtcc_wr(0x01, RTCDATE); // initialize DATE  register  
//        rtcc_wr(0x09, RTCHOUR); // initialize HOUR  register  
//        rtcc_wr(0x00, RTCMIN); // initialize MIN   register  
//        rtcc_wr(0x30 | ST, RTCSEC);
//    } //init SEC register, set START bit 

BOOL ExtRTCCReadRAM(BYTE *data, BYTE size) {
    return ExtRTCCReadSequential(data, size, ADDR_RTCC, SRAM_PTR);
}

BOOL ExtRTCCWriteRAM(BYTE *data, BYTE size) {
    return ExtRTCCWriteSequential(data, size, ADDR_RTCC, SRAM_PTR);
}

BOOL ExtRTCCReadMACAddress(BYTE *data, BYTE size) {

#if defined(EXT_RTCC_MCP79400_DEVICE)
#error This device does not have unique EUI-48 or EUI-64 MAC address
#elif defined(EXT_RTCC_MCP79401_DEVICE)
    if (size != 6)
        return FALSE;
    return ExtRTCCReadSequential(array, size, ADDR_EEPROM, EEPROM_PTR + 2);
#elif defined(EXT_RTCC_MCP79402_DEVICE)
    if (size < 6 || size > 8)
        return FALSE;
    return ExtRTCCReadSequential(data, size, ADDR_EEPROM, EEPROM_PTR);
#endif
}



//...................................................................................                              

BYTE binaryByteToBCDbyte(BYTE binary) {
    BYTE bcd = 0;
    bcd = 0x0F & (binary % 10);
    bcd |= (binary / 10) << 4;
    return bcd;
}

BYTE BCDByteToBinaryByte(BYTE bcd) {
    BYTE binary = 0;
    binary = (bcd & 0x0F);
    binary += ((bcd >> 4) & 0x0F) * 10;
    return binary;
}

BYTE ExtRTCCGetSeconds(void) {
    static EXT_RTCC_RTCSEC sec;
    sec.Val = ExtRTCCRead(RTCSEC);
    return sec.bits.SECONE + sec.bits.SECTEN * 10;
}

BYTE ExtRTCCGetMinutes(void) {
    static EXT_RTCC_RTCMIN min;
    min.Val = ExtRTCCRead(RTCMIN);
    return min.bits.MINONE + min.bits.MINTEN * 10;
}

BYTE ExtRTCCGetHours(void) {
    static EXT_RTCC_RTCHOUR hour;
    hour.Val = ExtRTCCRead(RTCHOUR);
    return hour.bits.HRONE + hour.bits.HRTEN * 10;
}

BYTE ExtRTCCGetDay(void) {
    static EXT_RTCC_RTCDATE day;
    day.Val = ExtRTCCRead(RTCDATE);
    return day.bits.DATEONE + day.bits.DATETEN * 10;
}

BYTE ExtRTCCGetMonth(void) {
    static EXT_RTCC_RTCMTH month;
    month.Val = ExtRTCCRead(RTCMTH);
    return month.bits.MTHONE + month.bits.MTHTEN * 10;
}

BYTE ExtRTCCGetYear(void) {
    static EXT_RTCC_RTCYEAR year;
    year.Val = ExtRTCCRead(RTCYEAR);
    return year.bits.YRONE + year.bits.YRTEN * 10;
}

void ExtRTCCGetTime(EXT_RTCC *rtcc) {
    rtcc->sec = ExtRTCCGetSeconds();
    rtcc->min = ExtRTCCGetMinutes();
    rtcc->hour = ExtRTCCGetHours();
    rtcc->day = ExtRTCCGetDay();
    rtcc->month = ExtRTCCGetMonth();
    rtcc->year = ExtRTCCGetYear() + 2000;
}

void ExtRTCCSetExtendedTimeAndDate(BYTE hh, BYTE mm, BYTE ss, BYTE dd, BYTE MM, INT16 yyyy, BYTE _wday) {
    EXT_RTCC_RTCSEC sec;
    EXT_RTCC_RTCMIN min;
    EXT_RTCC_RTCHOUR hour;
    EXT_RTCC_RTCWKDAY wday;
    EXT_RTCC_RTCDATE date;
    EXT_RTCC_RTCMTH month;
    EXT_RTCC_RTCYEAR year;

    sec.bits.SECONE = ss % 10;
    sec.bits.SECTEN = ss / 10;
    sec.bits.ST = TRUE; // 1 = Oscillator enabled
    ExtRTCCWrite(sec.Val, RTCSEC);
    min.bits.MINONE = mm % 10;
    min.bits.MINTEN = mm / 10;
    ExtRTCCWrite(min.Val, RTCMIN);
    hour.bits.HRONE = hh % 10;
    hour.bits.HRTEN = hh / 10;
    hour.bits.sel12_24 = FALSE; // 24 hours format
    ExtRTCCWrite(hour.Val, RTCHOUR);

    wday.bits.WKDAY = (_wday + 1); // +1
    wday.bits.VBATEN = TRUE; // 1 = VBAT input is enabled
    wday.bits.PWRFAIL = FALSE; // 0 = Primary power has not been lost
    //wday.bits.OSCRUN  = FALSE; // Read only bit
    ExtRTCCWrite(wday.Val, RTCWKDAY);

    date.bits.DATEONE = dd % 10;
    date.bits.DATETEN = dd / 10;
    ExtRTCCWrite(date.Val, RTCDATE);
    month.bits.MTHONE = (MM) % 10; //
    month.bits.MTHTEN = (MM) / 10;
    // month.bits.LPYR = FALSE; // Read only value
    ExtRTCCWrite(month.Val, RTCMTH);
    year.bits.YRONE = (yyyy - 2000) % 10;
    year.bits.YRTEN = (yyyy - 2000) / 10;
    ExtRTCCWrite(year.Val, RTCYEAR);

    //        // Read back the saved values
    //        ctrl.Val = ExtRTCCRead(CONTROL); // read day + OSCRUN bit
    //        sec.Val = ExtRTCCRead(RTCSEC);
    //        min.Val = ExtRTCCRead(RTCMIN);
    //        hour.Val = ExtRTCCRead(RTCHOUR);
    //
    //        wday.Val = ExtRTCCRead(RTCWKDAY);
    //        date.Val = ExtRTCCRead(RTCDATE);
    //        month.Val = ExtRTCCRead(RTCMTH);
    //        year.Val = ExtRTCCRead(RTCYEAR);
}

void rtccUpdateDate(void) {
    updateDate = TRUE;
}

void ExtRTCCSetTimeFromNTPTask(void) {

    DWORD utc;
    TIME_TM time_tm;
    INT16 dst, gmt1;

    if (updateDate == TRUE && ntpIsSync()) {
        updateDate = FALSE;
        utc = SNTPGetUTCSeconds();
        //memset(&time_tm, 0x00, sizeof (TIME_TM));
        // if (appConfig.ip.fields.flags.bits.bIsDSTEnabled)
        dst = (INT16) (appConfig.ip.fields.DST * 3600);
        gmt1 = returnGMTValue(appConfig.ip.fields.GMT);
        offtime(&time_tm, utc, dst + gmt1);
        ExtRTCCSetExtendedTimeAndDate(time_tm.tm_hour, time_tm.tm_min, time_tm.tm_sec, time_tm.tm_mday, time_tm.tm_mon, time_tm.tm_year, time_tm.tm_wday);
    }
}

#ifdef MORE_TIME_STUFF

// Three character representation of month names
rom char *month_str[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// Three character representation of the day of the week
rom char *wday_str[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


#endif

// Days on each month for regular and leap years
ROM char days_month[2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

// isleap() takes the argument year and returns a non zero value if it is a
// leap year. 
//

BYTE isleap(int year) {
    BYTE y;

    y = ((year % 4 == 0) && (year % 100 != 0 || year % 400 == 0));
    return (y);
}

#ifdef MORE_TIME_STUFF
//*****************************************************************************
// mktime() take a tm time structure and generates the equivalent time_t
// value.
//

time_t mktime(tm *p) {
    time_t j;
    int i;

    j = 0;

    for (i = START_YEAR; i < p->tm_year; i++) {
        j += isleap(i) ? 366 : 365;
    }

    for (i = 0; i < p->tm_mon; i++) {
        j += days_month[isleap(p->tm_year)][i];
    }

    j = j + p->tm_mday - 1;
    j *= 86400; // convert to seconds

    j = j + (time_t) (p->tm_hour * 3600L);
    j = j + (time_t) (p->tm_min * 60L);
    j = j + (time_t) (p->tm_sec);

    return (j);
}
#endif

//*****************************************************************************
// offtime() takes a time_t value and generates the corresponding tm time
// structure taking in account leap years. It also takes an offset value
// as argument to facilitate correction for Time Zone and Day Light Savings
// Time.
//

#define START_YEAR           1970
#define START_WDAY           4

void offtime(TIME_TM *ts, DWORD t, INT16 offset) {
    long days, j;
    int i, k;
    //char *p;

    t += offset; // Correct for TZ/DST offset
    days = t / 86400; // Integer number of days
    j = t % 86400; // Fraction of a day
    ts->tm_hour = j / 3600; // Integer number of hours
    j %= 3600; // Fraction of hour
    ts->tm_min = j / 60; // Integer number of minutes
    ts->tm_sec = j % 60; // Remainder seconds
    ts->tm_wday = (days + START_WDAY) % 7; // Day of the week

    i = START_YEAR;

    // Count the number of days per year taking in account leap years
    // to determine the year number and remaining days
    while (days >= (j = isleap(i) ? 366 : 365)) {
        i++;
        days -= j;
    }

    while (days < 0) {
        i--;
        days += isleap(i) ? 366 : 365;
    }

    ts->tm_year = i; // We have the year  
    ts->tm_yday = days; // And the number of days

    i = isleap(i);

    // Count the days for each month in this year to determine the month
    for (k = 0; days >= days_month[i][k]; ++k) {
        days -= days_month[i][k];
    }

    ts->tm_mon = k; // We have the month
    ts->tm_mday = days + 1; // And the day of the month
}

INT16 returnGMTValue(BYTE gmt) {
    INT16 time = 0;

    switch (gmt) {
        case GMT_MINUS_12_00: time -= 12l * 60l * 60l;
            break;
        case GMT_MINUS_11_00: time -= 11l * 60l * 60l;
            break;
        case GMT_MINUS_10_00: time -= 10l * 60l * 60l;
            break;
        case GMT_MINUS_09_00: time -= 9l * 60l * 60l;
            break;
        case GMT_MINUS_08_00: time -= 8l * 60l * 60l;
            break;
        case GMT_MINUS_07_00: time -= 7l * 60l * 60l;
            break;
        case GMT_MINUS_06_00: time -= 6l * 60l * 60l;
            break;
        case GMT_MINUS_05_00: time -= 5l * 60l * 60l;
            break;
        case GMT_MINUS_04_30: time -= 4l * 60l * 60l + 30l * 60l;
            break;
        case GMT_MINUS_04_00: time -= 4l * 60l * 60l;
            break;
        case GMT_MINUS_03_30: time -= 3l * 60l * 60l + 30l * 60l;
            break;
        case GMT_MINUS_03_00: time -= 3l * 60l * 60l;
            break;
        case GMT_MINUS_02_00: time -= 2l * 60l * 60l;
            break;
        case GMT_MINUS_01_00: time = -1l * 60l * 60l;
            break;
        case GMT_00_00: time = 0;
            break;
        case GMT_PLUS_01_00: time += 1l * 60l * 60l;
            break;
        case GMT_PLUS_02_00: time += 2l * 60l * 60l;
            break;
        case GMT_PLUS_03_00: time += 3l * 60l * 60l;
            break;
        case GMT_PLUS_03_30: time += 3l * 60l * 60l + 30l * 60l;
            break;
        case GMT_PLUS_04_00: time += 4l * 60l * 60l;
            break;
        case GMT_PLUS_04_30: time += 4l * 60l * 60l + 30l * 60l;
            break;
        case GMT_PLUS_05_00: time += 5l * 60l * 60l;
            break;
        case GMT_PLUS_05_30: time += 5l * 60l * 60l + 30l * 60l;
            break;
        case GMT_PLUS_05_45: time += 5l * 60l * 60l + 45l * 60l;
            break;
        case GMT_PLUS_06_00: time += 6l * 60l * 60l;
            break;
        case GMT_PLUS_06_30: time += 6l * 60l * 60l + 30l * 60l;
            break;
        case GMT_PLUS_07_00: time += 7l * 60l * 60l;
            break;
        case GMT_PLUS_08_00: time += 8l * 60l * 60l;
            break;
        case GMT_PLUS_09_00: time += 9l * 60l * 60l;
            break;
        case GMT_PLUS_09_30: time += 9l * 60l * 60l + 30l * 60l;
            break;
        case GMT_PLUS_10_00: time += 10l * 60l * 60l;
            break;
        case GMT_PLUS_11_00: time += 11l * 60l * 60l;
            break;
        case GMT_PLUS_12_00: time += 12l * 60l * 60l;
            break;
        case GMT_PLUS_13_00: time += 13l * 60l * 60l;
            break;
    }
    return time;
}