/*********************************************************************
 *
 *                  Microchip TCP/IP Stack Definitions
 *
 *********************************************************************
 * FileName:        StackTsk.h
 * Dependencies:    Compiler.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/10/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     8/7/03  Rev 2.21 - TFTP Client addition
 * Howard Schlunder		9/30/04	Added MCHP_MAC, MAC_POWER_ON_TEST, 
                                EEPROM_BUFFER_SIZE, USE_LCD
 * Howard Schlunder		8/09/06	Removed MCHP_MAC, added STACK_USE_NBNS, 
 *								STACK_USE_DNS, and STACK_USE_GENERIC_TCP_EXAMPLE
 ********************************************************************/
#ifndef __STACK_TSK_H
#define __STACK_TSK_H

#if defined (WF_CS_TRIS)
#include "WF_Config.h"     
#endif

#include "MCP79402.h"

// Check for potential configuration errors in "TCPIPConfig.h"
#if (MAX_UDP_SOCKETS <= 0 || MAX_UDP_SOCKETS > 255 )
#error Invalid MAX_UDP_SOCKETS value specified
#endif

// Check for potential configuration errors in "TCPIPConfig.h"
#if (MAX_HTTP_CONNECTIONS <= 0 || MAX_HTTP_CONNECTIONS > 255 )
#error Invalid MAX_HTTP_CONNECTIONS value specified.
#endif

// Structure to contain a MAC address

typedef struct __attribute__((__packed__)) {
    BYTE v[6];
}
MAC_ADDR;

// Definition to represent an IP address
#define IP_ADDR		DWORD_VAL

// Address structure for a node

typedef struct __attribute__((__packed__)) {
    IP_ADDR IPAddr;
    MAC_ADDR MACAddr;
}
NODE_INFO;



#define ARRAY_SIZE_FIRMWARE             40
#define ARRAY_SIZE_IP                   128
#define ARRAY_SIZE_AUTH                 40
#define ARRAY_SIZE_GPIO                 120

#define SIZE_USERNAME                   16      // Username max length
#define SIZE_PASSWORD                   16      // Password max length
#define SIZE_ADDRESS                    21      // Password max length

// Customized AppConfig structure
#define DDNS_SIZE_USER                  16      // Username max length
#define DDNS_SIZE_PASS                  16      // Password max length
#define DDNS_SIZE_HOST                  29      // Password max length

#define APP_CONFIG_VERSION_MAJOR        (1u)
#define APP_CONFIG_VERSION_MINOR        (0u)

#define APP_CONFIG_LOADED_CHECK         0xAA997755


// Application-dependent structure used to contain address information

typedef struct __attribute__((__packed__)) configStruct {

    // Firmware structure

    union {
        BYTE array[ARRAY_SIZE_FIRMWARE];
        // Fields structure

        struct {
            // Least Significant byte

            union {
                DWORD allFlags; // 32 bits reserved for flags field

                struct __PACKED {
                    // LSB
                    DWORD upgradeAsk : 1; // Upgrade asked
                    DWORD upgradeSucc : 1; // Upgrade successful
                    DWORD upgradeErr : 1; // Upgrade error
                    DWORD restoreDefault : 1; // Ask to restore default settings
                    // Expand here
                    // MSB
                } bits;
            } flags;

            BYTE configMD5Hash[16]; // MD5 of the config structure

            // Time info
            //EXT_RTCC lastUpgarde;                   // Save the date of the last firmware upgrade
            EXT_RTCC lastSave; // Save the date of the last configurations modification

            // Application info
            BYTE firmMajor; // Firmware major number
            BYTE firmMinor; // Firmware minor number
            BYTE prevFirmMinor;
            BYTE prevFirmMajor;
            BYTE hardMajor; // Hardware major number
            BYTE hardMinor; // Hardware minor number

            BYTE configVersionMajor; // Config major number
            BYTE configVersionMinor; // Config minor number

            DWORD settingsLoadedCheck; // This field must be initialized to 0xAA997755
            // Expand here
            // Most Significant Byte
        } fields;
    } firmware; // Firmware flags


    // IP structure

    union {
        BYTE array[ARRAY_SIZE_IP];
        // Fields structure

        struct {
            // Least Significant byte

            union {
                DWORD allFlags; // 32 bits reserved for flags field

                struct __PACKED {
                    // LSB
                    DWORD bIsDHCPEnabled : 1;
                    DWORD bIsICMPServerEnabled : 1;
                    DWORD bInConfigMode : 1;
                    // NTP
                    DWORD bIsNTPEnabled : 1; // NTP enabled
                    DWORD bIsDSTEnabled : 1; // Daylight saving time enabled
                    // DDNS
                    DWORD enableDDNS : 1; // 0 = Disabled and 1 = Enabled
                    DWORD service : 2; // DDNS Service value: 0 = DynDNS, 1 = No-IP, 2 = DNS-O-Matic
                    // Expand here
                    // MSB
                } bits;
            } flags;

            IP_ADDR MyIPAddr; // IP address
            IP_ADDR DefaultIPAddr; // Default IP address
            IP_ADDR MyMask; // Subnet mask
            IP_ADDR DefaultMask; // Default subnet mask
            IP_ADDR MyGateway; // Default Gateway
            IP_ADDR PrimaryDNSServer; // Primary DNS Server
            IP_ADDR SecondaryDNSServer; // Secondary DNS Server
            MAC_ADDR MyMACAddr; // Application MAC address
            BYTE NetBIOSName[16]; // NetBIOS name
            // NTP
            BYTE ntpServerName; // NTP server name
            BYTE syncInterval; // NTP re-sync interval
            BYTE GMT; // Greenwich Mean Time, minutes WESTWARD of Greenwich. 0 => GMT-12:00, xx => GMT+13:00
            BYTE DST; // minutes of Daylight Saving Time saving time enabled. 0 => 0 hours, 1 => 1 hour, etc
            // SMTP
            WORD smtpPort;

            // DDNS
            BYTE service; // Service name: DynDNS = 0, No-IP = 1 and DNS-O-Matic = 2
            BYTE user[DDNS_SIZE_USER]; // Username
            BYTE pass[DDNS_SIZE_PASS]; // Password
            BYTE host[DDNS_SIZE_HOST]; // Host
            // Expand here
            // Most Significant Byte
        } fields;
    } ip; // IP Union


    // Authentication structure

    union {
        BYTE array[ARRAY_SIZE_AUTH];
        // Fields structure

        struct __PACKED {
            // Least Significant byte

            union {
                DWORD allFlags; // 32 bits reserved for flags field

                struct {
                    // LSB
                    DWORD bit : 1; // not used
                    // Expand here
                    // MSB
                } bits;
            } flags;

            struct __PACKED {
                BYTE username[SIZE_USERNAME]; // Username
                BYTE password[SIZE_PASSWORD]; // Password
            } user;

            // Expand here
            // Most Significant Byte
        } fields;
    } auth; // Authentication fields


    // GPIO structure
    union {
        BYTE array[ARRAY_SIZE_GPIO];
        // Fields structure

        struct __PACKED {
            // Least Significant byte
            struct {
                // LSB
                BYTE ioNotice : 4; // 0 = Disabled, 1 = IO 1 [...] and 12 = Relay 4
                BYTE ioDirection : 1; // 1 = Input and 0 = Output
                BYTE ioDefault : 1; // 1 = High and 0 = Low
                BYTE unused : 2; // 1 = High and 0 = Low
                // Expand here
                // MSB
            } ioBits[8];

            struct __PACKED {
                struct {
                    BYTE startUp : 1;
                    BYTE unused : 7;
                } bits;
                BYTE onh;
                BYTE onm;
                BYTE offh;
                BYTE offm;
            } relay[4];
            
            struct __PACKED {
                WORD upThreshold;
                WORD lowThreshold;
                BYTE output;
                BYTE relay;
                struct {
                    BYTE startUp : 1;
                    BYTE relay : 7;
                } bits;
            } analog[4];

            // Expand here
            // Most Significant Byte
        } fields;
    } gpio; // GPIO fields

} APP_CONFIG;

#ifndef THIS_IS_STACK_APPLICATION
//extern APP_CONFIG AppConfig;
extern APP_CONFIG appConfig;
extern APP_CONFIG newAppConfig;
#endif


void StackInit(void);
void StackTask(void);
void StackApplications(void);
#endif
