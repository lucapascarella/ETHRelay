/* 
 * File:   EMAIL.h
 * Author: Luca
 *
 * Created on 25 settembre 2016, 9.45
 */

#ifndef EMAIL_H
#define	EMAIL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

typedef enum {
    SM_EMAIL_HOME = 0,
    SM_EMAIL_INIT,
    SM_EMAIL_CLAIM_MODULE, 
    SM_EMAIL_PUT_IGNORED,
    SM_EMAIL_FINISHING,    
    SM_EMAIL_ERROR,
    SM_EMAIL_DONE
} EMAIL_STATE_MACHINE;

typedef struct __attribute__((__packed__)) {

    DWORD timer;                            // Timer used to timeout indicator

    BYTE server[32];
    BYTE user[32];
    BYTE pass[32];
    BYTE from[64];
    BYTE to[64];
    BYTE subject[32];
    BYTE body[256];
    
    WORD error;
    EMAIL_STATE_MACHINE sm;
    
    struct {
        BYTE start : 1;                     // Ask for reboot
    } flags;

} EMAIL_CONFIG;

#define XEEPROM_SMTP_SERVER_SIZE            32u
#define XEEPROM_SMTP_USER_SIZE              32u
#define XEEPROM_SMTP_PASS_SIZE              32u
#define XEEPROM_SMTP_TO_SIZE                32u
#define XEEPROM_SMTP_SUBJECT_SIZE           32u

#define XEEPROM_SMTP_SERVER_ADDRESS         SMTP_START_ADDRESS
#define XEEPROM_SMTP_USER_ADDRESS           SMTP_START_ADDRESS + XEEPROM_SMTP_SERVER_SIZE
#define XEEPROM_SMTP_PASS_ADDRESS           SMTP_START_ADDRESS + XEEPROM_SMTP_SERVER_SIZE + XEEPROM_SMTP_USER_SIZE
#define XEEPROM_SMTP_TO_ADDRESS             SMTP_START_ADDRESS + XEEPROM_SMTP_SERVER_SIZE + XEEPROM_SMTP_USER_SIZE + XEEPROM_SMTP_PASS_SIZE
#define XEEPROM_SMTP_SUBJECT_ADDRESS        SMTP_START_ADDRESS + XEEPROM_SMTP_SERVER_SIZE + XEEPROM_SMTP_USER_SIZE + XEEPROM_SMTP_PASS_SIZE + XEEPROM_SMTP_TO_SIZE

void EmailInit(void);
void sendEmailTask(void);
void setSendEmailFlag(void);
WORD emailGetLastStatus(void);



#ifdef	__cplusplus
}
#endif

#endif	/* EMAIL_H */
