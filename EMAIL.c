
#include "EMAIL.h"
#include "AppConfig.h"
#include "Relay.h"
#include "IO.h"
#include "ADC.h"

EMAIL_CONFIG email;

void EmailInit(void) {
    email.sm = SM_EMAIL_HOME;
    email.flags.start = FALSE;
    email.error = 0x0000;

    email.timer = TickGet();
}

void sendEmailTask(void) {

    WORD len, i;

    switch (email.sm) {
        case SM_EMAIL_HOME:
            if (email.flags.start && (TickGet() - email.timer) > 30 * TICK_SECOND) {
                email.flags.start = FALSE;
                email.sm++;
            }
            break;

        case SM_EMAIL_INIT:
            email.error = 0x0000;
            XEEReadArray(XEEPROM_SMTP_SERVER_ADDRESS, email.server, XEEPROM_SMTP_SERVER_SIZE);
            if (email.server[0] == 0xFF || email.server[0] == 0x00) {
                email.sm = SM_EMAIL_ERROR;
                email.error = 0x0001;
                break;
            }
            email.server[XEEPROM_SMTP_SERVER_SIZE - 1] = '\0';
            XEEReadArray(XEEPROM_SMTP_USER_ADDRESS, email.user, XEEPROM_SMTP_USER_SIZE);
            if (email.user[0] == 0xFF || email.user[0] == 0x00) {
                email.sm = SM_EMAIL_ERROR;
                email.error = 0x0002;
                break;
            }
            email.user[XEEPROM_SMTP_USER_SIZE - 1] = '\0';
            XEEReadArray(XEEPROM_SMTP_PASS_ADDRESS, email.pass, XEEPROM_SMTP_PASS_SIZE);
            if (email.pass[0] == 0xFF || email.pass[0] == 0x00) {
                email.sm = SM_EMAIL_ERROR;
                email.error = 0x0003;
                break;
            }
            email.pass[XEEPROM_SMTP_PASS_SIZE - 1] = '\0';
            XEEReadArray(XEEPROM_SMTP_TO_ADDRESS, email.to, XEEPROM_SMTP_TO_SIZE);
            if (email.to[0] == 0xFF || email.to[0] == 0x00) {
                email.sm = SM_EMAIL_ERROR;
                email.error = 0x0004;
                break;
            }
            email.to[XEEPROM_SMTP_TO_SIZE - 1] = '\0';
            XEEReadArray(XEEPROM_SMTP_SUBJECT_ADDRESS, email.subject, XEEPROM_SMTP_SUBJECT_SIZE);
            if (email.subject[0] == 0xFF || email.subject[0] == 0x00) {
                email.sm = SM_EMAIL_ERROR;
                email.error = 0x0005;
                break;
            }
            email.subject[XEEPROM_SMTP_SUBJECT_SIZE - 1] = '\0';

            // Create the body of the email with the status of: IO, Relay and analog
            len = 0;
            len += sprintf((char*) &email.body[len], "IO status:");
            for (i = 0; i < 8; i++) {
                len += sprintf((char*) &email.body[len], "\nIO %d: ");
                if (readInputIO(IO_1 + i))
                    len += sprintf((char*) &email.body[len], "ON");
                else
                    len += sprintf((char*) &email.body[len], "OFF");
            }

            len += sprintf((char*) &email.body[len], "\n\nRelay status:");
            for (i = 0; i < 4; i++) {
                len += sprintf((char*) &email.body[len], "\nRelay %d: ", i);
                if (getRelay(RELAY_1 + i))
                    len += sprintf((char*) &email.body[len], "ON");
                else
                    len += sprintf((char*) &email.body[len], "OFF");
            }

            len += sprintf((char*) &email.body[len], "\n\nAnalog status:\n");
            for (i = 0; i < 4; i++) {
                len += sprintf((char*) &email.body[len], "Analog %d: %d\n", i, readADC(ADC_1 + i));
            }

            email.body[sizeof (email.body) - 1] = '\0';
            email.sm = SM_EMAIL_CLAIM_MODULE;
            break;

        case SM_EMAIL_CLAIM_MODULE:
            if (SMTPBeginUsage()) {
                // Prepare SMTP parameters

                SMTPClient.Server.szRAM = email.server;
                SMTPClient.ServerPort = appConfig.ip.fields.smtpPort;
                if (sprintf((char*) email.from, "\"SMTP Service\" <%s>", email.user) > sizeof (email.from)) {
                    email.sm = SM_EMAIL_ERROR;
                    email.error = 0x0006;
                    break;
                }
                SMTPClient.From.szRAM = email.from;
                SMTPClient.Username.szRAM = email.user;
                SMTPClient.Password.szRAM = email.pass;
                SMTPClient.To.szRAM = email.to;
                SMTPClient.Subject.szRAM = email.subject;
                SMTPClient.Body.szRAM = email.body;
                // Start sending the message
                SMTPSendMail();
                email.sm = SM_EMAIL_PUT_IGNORED;
            }
            break;

        case SM_EMAIL_PUT_IGNORED:
            // This section puts a message that is ignored by compatible clients.
            // This text will not display unless the receiving client is obselete 
            // and does not understand the MIME structure.
            // The "--frontier" indicates the start of a section, then any
            // needed MIME headers follow, then two CRLF pairs, and then
            // the actual content (which will be the body text in the next state).

            // Check to see if a failure occurred
            if (!SMTPIsBusy()) {
                email.sm = SM_EMAIL_FINISHING;
                break;
            }
            break;

        case SM_EMAIL_FINISHING:
            // Wait for status
            if (!SMTPIsBusy()) {
                // Release the module and check success
                // Redirect the user based on the result
                if ((email.error = SMTPEndUsage()) == SMTP_SUCCESS) {
                    email.sm = SM_EMAIL_DONE;
                } else {
                    email.sm = SM_EMAIL_ERROR;
                }
            }
            break;

        case SM_EMAIL_ERROR:
            email.sm = SM_EMAIL_DONE;
            break;

        case SM_EMAIL_DONE:
            email.timer = TickGet();
            email.sm = SM_EMAIL_HOME;
            break;
    }
}

void setSendEmailFlag(void) {
    email.flags.start = TRUE;
}

WORD emailGetLastStatus(void) {
    return email.error;
}