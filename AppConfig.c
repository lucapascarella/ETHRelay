/********************************************************************
 * FileName:        AppConfig.c
 * Dependencies:    See INCLUDES section
 * Processor:       PIC32 USB Micro-controllers
 * Hardware:        DB Board V1.2
 * Complier:        Microchip XC32 (for PIC32) v1.40
 * Company:         Bruitparif
 *
 ********************************************************************
 * File Description:
 *
 * Change History:
 *  Rev     Description
 *  1.0     Initial release
 ********************************************************************/

#include "AppConfig.h"
#include "TCPIP Stack/XEEPROM.h"
#include "MCP79402.h"


BOOL restoreDefaultSettingsGloablFlag = FALSE;

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/

void InitAppConfig(void) {

    BOOL def;
    extern HASH_SUM md5;
    BYTE configMD5HashTemp[16];
    WORD i, termA, termB;

    // Ensure adequate space has been reserved in non-volatile storage to 
    // store the entire AppConfig structure.  If you get stuck in this while(1) 
    // trap, it means you have a design time miss-configuration in TCPIPConfig.h.
    // You must increase MPFS_RESERVE_BLOCK to allocate more space.
    termA = sizeof (appConfig);
    if (termA > (MPFS_RESERVE_BLOCK - INPUT_RESERVED_BLOCK))
        while (1);

    termA = sizeof (appConfig.firmware.fields);
    termB = sizeof (appConfig.firmware.array);
    if (termA > termB)
        while (1);
    termA = sizeof (appConfig.ip.fields);
    termB = sizeof (appConfig.ip.array);
    if (termA > termB)
        while (1);
    termA = sizeof (appConfig.auth.fields);
    termB = sizeof (appConfig.auth.array);
    if (termA > termB)
        while (1);
    termA = sizeof (appConfig.gpio.fields);
    termB = sizeof (appConfig.gpio.array);
    if (termA > termB)
        while (1);


    do {
        do {
            // Start out zeroing all AppConfig bytes to ensure all fields are 
            // deterministic for checksum generation
            memset((void*) &appConfig, 0x00, sizeof (appConfig));

            // Check if Load default value is asked
            def = FALSE;
            // Load AppConfig structure
            loadAppConfig();

            // Check MD5 value
            memcpy((void*) configMD5HashTemp, (void*) appConfig.firmware.fields.configMD5Hash, sizeof (configMD5HashTemp));
            memset((void*) & appConfig.firmware.fields.configMD5Hash, 0x00, sizeof (appConfig.firmware.fields.configMD5Hash));
            MD5Initialize(&md5);
            MD5AddData(&md5, (BYTE*) & appConfig, sizeof (appConfig));
            MD5Calculate(&md5, appConfig.firmware.fields.configMD5Hash);
            //memcpy(appConfig.firmware.fields.configMD5Hash, configMD5HashTemp, sizeof (configMD5HashTemp));

            for (i = 0; i<sizeof (configMD5HashTemp); i++)
                if (configMD5HashTemp[i] != appConfig.firmware.fields.configMD5Hash[i]) {
                    clearEEPROMContet();
                    loadDefaultAppConfig();
                    saveAppConfig();
                    def = TRUE;
                    break;
                }
        } while (def);

        if (appConfig.firmware.fields.flags.bits.restoreDefault || restoreDefaultSettingsGloablFlag) {
            restoreDefaultSettingsGloablFlag = FALSE;
            clearEEPROMContet();
            loadDefaultAppConfig();
            saveAppConfig();
            def = TRUE;
        }
    } while (def);

    if (appConfig.firmware.fields.flags.bits.upgradeAsk) {
        appConfig.firmware.fields.flags.bits.upgradeAsk = FALSE;
        appConfig.firmware.fields.flags.bits.upgradeSucc = TRUE;
        appConfig.firmware.fields.flags.bits.upgradeErr = FALSE;
        // Save the previous version
        appConfig.firmware.fields.prevFirmMajor = appConfig.firmware.fields.firmMajor;
        appConfig.firmware.fields.prevFirmMinor = appConfig.firmware.fields.firmMinor;
        // Update with new version
        appConfig.firmware.fields.firmMajor = FIRMWARE_VERSION_MAJOR;
        appConfig.firmware.fields.firmMinor = FIRMWARE_VERSION_MINOR;

        // Save
        saveAppConfig();
    }
}

void clearEEPROMContet(void) {
    WORD i;

    // Clear Input space
    XEEBeginWrite(INPUT_START_ADDRESS);
    for (i = 0; i < INPUT_RESERVED_BLOCK; i++)
        XEEWrite(0xFF);
    XEEEndWrite();
    // Clear APP space
    XEEBeginWrite(APP_CONFIG_START_ADDRESS);
    for (i = 0; i < APP_CONFIG_RESERVED_BLOCK; i++)
        XEEWrite(0xFF);
    XEEEndWrite();
}

void saveAppConfig(void) {
    saveThisAppConfig(&appConfig);
}

void saveThisAppConfig(APP_CONFIG *ptrConfig) {

    extern HASH_SUM md5;

    // Get current time
    ExtRTCCGetTime(&ptrConfig->firmware.fields.lastSave);

    // Set MD5 to zero and update value
    memset((void*) &(ptrConfig->firmware.fields.configMD5Hash), 0x00, sizeof (appConfig.firmware.fields.configMD5Hash));
    MD5Initialize(&md5);
    MD5AddData(&md5, (BYTE*) ptrConfig, sizeof (APP_CONFIG));
    MD5Calculate(&md5, (BYTE*)&(ptrConfig->firmware.fields.configMD5Hash));

    // Write the current AppConfig contents to EEPROM/Flash
#if defined (USE_INTERNAL_FLASH)
    WriteFlash((BYTE*) ptrConfig, sizeof (APP_CONFIG));
#else
    XEEBeginWrite(APP_CONFIG_START_ADDRESS);
    XEEWriteArray((BYTE*) ptrConfig, sizeof (APP_CONFIG));
    XEEEndWrite();
#endif
}

void loadAppConfig(void) {
    loadThisAppConfig(&appConfig);
}

void loadThisAppConfig(APP_CONFIG *ptrConfig) {
#if defined (USE_INTERNAL_FLASH)
    extern far rom unsigned char FlashSpace[];
    memcpypgm2ram((void*) ptrConfig, (const rom void*) FlashSpace, sizeof (APP_CONFIG));
#else
    //XEEBeginRead(INPUT_RESERVED_BLOCK);
    XEEReadArray(APP_CONFIG_START_ADDRESS, (void*) ptrConfig, sizeof (APP_CONFIG));
    //XEEEndRead();
#endif
}

void cloneAppConfig(APP_CONFIG *ptrSrc, APP_CONFIG *ptrDest) {
    int i;
    BYTE *s, *d;

    s = (BYTE*) ptrSrc;
    d = (BYTE*) ptrDest;
    for (i = 0; i<sizeof (appConfig); i++)
        d[i] = s[i];
}

void loadDefaultAppConfig(void) {
    WORD i;
    //BYTE array2[6];
    memset((void*) &appConfig, 0x00, sizeof (appConfig));

    // Set default firmware and hardware versions
    appConfig.firmware.fields.firmMajor = FIRMWARE_VERSION_MAJOR;
    appConfig.firmware.fields.firmMinor = FIRMWARE_VERSION_MINOR;
    appConfig.firmware.fields.prevFirmMajor = FIRMWARE_VERSION_MAJOR;
    appConfig.firmware.fields.prevFirmMinor = FIRMWARE_VERSION_MINOR;
    appConfig.firmware.fields.hardMajor = HARDWARE_VERSION_MAJOR;
    appConfig.firmware.fields.hardMinor = HARDWARE_VERSION_MINOR;

    appConfig.firmware.fields.configVersionMajor = APP_CONFIG_VERSION_MAJOR;
    appConfig.firmware.fields.configVersionMinor = APP_CONFIG_VERSION_MINOR;

    appConfig.firmware.fields.settingsLoadedCheck = APP_CONFIG_LOADED_CHECK;

    // Firmware upgrade flags
    appConfig.firmware.fields.flags.bits.upgradeAsk = FALSE;
    appConfig.firmware.fields.flags.bits.upgradeSucc = FALSE;
    appConfig.firmware.fields.flags.bits.upgradeErr = FALSE;
    appConfig.firmware.fields.flags.bits.restoreDefault = FALSE;

    // Clear MD5 array
    memset((void*) &appConfig.firmware.fields.configMD5Hash, 0x00, sizeof (appConfig.firmware.fields.configMD5Hash));


    // Initialize default IP values
    appConfig.ip.fields.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2 << 8ul | MY_DEFAULT_IP_ADDR_BYTE3 << 16ul | MY_DEFAULT_IP_ADDR_BYTE4 << 24ul;
    appConfig.ip.fields.DefaultIPAddr.Val = appConfig.ip.fields.MyIPAddr.Val;
    appConfig.ip.fields.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2 << 8ul | MY_DEFAULT_MASK_BYTE3 << 16ul | MY_DEFAULT_MASK_BYTE4 << 24ul;
    appConfig.ip.fields.DefaultMask.Val = appConfig.ip.fields.MyMask.Val;
    appConfig.ip.fields.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2 << 8ul | MY_DEFAULT_GATE_BYTE3 << 16ul | MY_DEFAULT_GATE_BYTE4 << 24ul;
    // Initialize default MAC Address
    //memcpypgm2ram((void*) &appConfig.ip.fields.MyMACAddr, (rom void*) SerializedMACAddress, sizeof (appConfig.ip.fields.MyMACAddr));
    ExtRTCCReadMACAddress((BYTE*) & appConfig.ip.fields.MyMACAddr, sizeof (appConfig.ip.fields.MyMACAddr));
    //ExtRTCCReadMACAddress((BYTE*) array2, sizeof (array2));
    // Load the default NetBIOS Host Name
    memset(appConfig.ip.fields.NetBIOSName, '\0', sizeof (appConfig.ip.fields.NetBIOSName));
    memcpypgm2ram(appConfig.ip.fields.NetBIOSName, (rom void*) MY_DEFAULT_HOST_NAME, 8);
    FormatNetBIOSName(appConfig.ip.fields.NetBIOSName);
    // Initialize default SMTP values
    appConfig.ip.fields.smtpPort = 25;
    // Initialize default DNS values
    appConfig.ip.fields.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2 << 8ul | MY_DEFAULT_PRIMARY_DNS_BYTE3 << 16ul | MY_DEFAULT_PRIMARY_DNS_BYTE4 << 24ul;
    appConfig.ip.fields.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2 << 8ul | MY_DEFAULT_SECONDARY_DNS_BYTE3 << 16ul | MY_DEFAULT_SECONDARY_DNS_BYTE4 << 24ul;
    // Initialize default IP flags
    appConfig.ip.fields.flags.bits.bIsDHCPEnabled = TRUE;
    appConfig.ip.fields.flags.bits.bInConfigMode = TRUE;
    // NTP section
    appConfig.ip.fields.flags.bits.bIsNTPEnabled = TRUE;
    appConfig.ip.fields.flags.bits.bIsDSTEnabled = TRUE;
    appConfig.ip.fields.ntpServerName = 0x00; // 0 = pool.ntp.org
    appConfig.ip.fields.syncInterval = 3; // Each 3 hours
    appConfig.ip.fields.GMT = 14; // (GMT +0:00 hour)
    appConfig.ip.fields.DST = 0; // Daylight Saving Time
    // DDNS Dynamic DNS Service
    strcpypgm2ram((char*) appConfig.ip.fields.user, MY_DEFAULT_DDNS_HOSTNAME);
    strcpypgm2ram((char*) appConfig.ip.fields.pass, MY_DEFAULT_DDNS_PASSWORD);
    strcpypgm2ram((char*) appConfig.ip.fields.host, MY_DEFAULT_DDNS_HOSTNAME);
    appConfig.ip.fields.flags.bits.enableDDNS = FALSE;

    // Default Authentication
    strcpypgm2ram((char*) appConfig.auth.fields.user.username, (rom char*) MY_DEFAULT_USERNAME);
    strcpypgm2ram((char*) appConfig.auth.fields.user.password, (rom char*) MY_DEFAULT_PASSWORD);

    // IO
    for (i = 0; i < 8; i++) {
        appConfig.gpio.fields.ioBits[i].ioNotice = 0; // 0 = Disabled
        appConfig.gpio.fields.ioBits[i].ioDirection = 1; // 1 = Input
        appConfig.gpio.fields.ioBits[i].ioDefault = 0; // 0 = Low
        appConfig.gpio.fields.ioBits[i].on1h = 0;
        appConfig.gpio.fields.ioBits[i].on1m = 0;
        appConfig.gpio.fields.ioBits[i].off1h = 0;
        appConfig.gpio.fields.ioBits[i].off1m = 0;
        appConfig.gpio.fields.ioBits[i].on2h = 0;
        appConfig.gpio.fields.ioBits[i].on2m = 0;
        appConfig.gpio.fields.ioBits[i].off2h = 0;
        appConfig.gpio.fields.ioBits[i].off2m = 0;
    }

    // Relay
    for (i = 0; i < 4; i++) {
        appConfig.gpio.fields.relay[i].bits.startUp = FALSE;
        appConfig.gpio.fields.relay[i].on1h = 0;
        appConfig.gpio.fields.relay[i].on1m = 0;
        appConfig.gpio.fields.relay[i].off1h = 0;
        appConfig.gpio.fields.relay[i].off1m = 0;
        appConfig.gpio.fields.relay[i].on2h = 0;
        appConfig.gpio.fields.relay[i].on2m = 0;
        appConfig.gpio.fields.relay[i].off2h = 0;
        appConfig.gpio.fields.relay[i].off2m = 0;
    }

    // Analog
    for (i = 0; i < 4; i++) {
        appConfig.gpio.fields.analog[i].bits.startUp = TRUE;
        appConfig.gpio.fields.analog[i].upThreshold = 700;
        appConfig.gpio.fields.analog[i].lowThreshold = 300;
        appConfig.gpio.fields.analog[i].output = 0;
        appConfig.gpio.fields.analog[i].relay = 0;
    }

    // Get default date
    ////ExtRTCCGetTime(&appConfig.firmware.fields.lastUpgarde);
}

void restoreDefaultSettings(BOOL reboot) {

    if (appConfig.firmware.fields.settingsLoadedCheck == APP_CONFIG_LOADED_CHECK) {
        // load and save default settings if the App Config is already loaded
        loadDefaultAppConfig();
        saveAppConfig();
        if (reboot)
            Reset();
    } else {
        restoreDefaultSettingsGloablFlag = TRUE;
    }
}

void checkManualRestoreDefault(void) {

    DWORD time, t;

    // Invalidate the EEPROM contents if BUTTON_1 and BUTTON_2 are held down for more than 4 seconds
    time = t = TickGet();
    while (BUTTON_1_IO == BUTTON_PRESSED && BUTTON_2_IO == BUTTON_PRESSED) {
        // Check if the buttons are pressed for up to 4 seconds
        if (TickGet() - time > 4 * TICK_SECOND) {
            // If the buttons are pressed to up 4 seconds, restore default settings
            time = t = TickGet();
            LED_0_ON();
            while (TickGet() - time < 4 * TICK_SECOND) {
                if (BUTTON_1_IO == BUTTON_RELEASED && BUTTON_2_IO == BUTTON_RELEASED) {
                    // Invalidate the Flash contents
                    LED_0_ON();
                    restoreDefaultSettingsGloablFlag = TRUE;
                }
            }
            LED_0_OFF();
            break;
        } else {
            if (TickGet() - t >= TICK_SECOND / 6ul) {
                t = TickGet();
                LED_0_INV();
            }
        }
    }
}
