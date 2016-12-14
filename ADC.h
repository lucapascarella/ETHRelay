/* 
 * File:   ADC.h
 * Author: Luca
 *
 * Created on 19 agosto 2016, 11.35
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

typedef struct __attribute__((__packed__)) {

    struct {
        BYTE send : 1;                     // Ask for reboot
        BYTE highTh : 1;                   // Ask for reboot
        BYTE lowTh : 1;                    // Ask for reboot
    } flags[4];

} ADC_CONFIG;    
    
#define ADC_1       1
#define ADC_2       2
#define ADC_3       3
#define ADC_4       4
    
void InitADC(void);
void ADCTask(void);
INT16 readADC(BYTE adc);

#define setHighThreshold(adc, value)        {appConfig.gpio.fields.analog[adc - 1].upThreshold = value;}
#define getHighThreshold(adc)               (appConfig.gpio.fields.analog[adc - 1].upThreshold)

#define setLowThreshold(adc, value)         {appConfig.gpio.fields.analog[adc - 1].lowThreshold = value;}
#define getLowThreshold(adc)                (appConfig.gpio.fields.analog[adc - 1].lowThreshold)

#define adcSetOutput(adc, value)            {appConfig.gpio.fields.analog[adc - 1].output = value;}
#define adcGetOutput(adc)                   (appConfig.gpio.fields.analog[adc - 1].output)

#define adcSetRelay(adc, value)             {appConfig.gpio.fields.analog[adc - 1].relay = value;}
#define adcGetRelay(adc)                    (appConfig.gpio.fields.analog[adc - 1].relay)

void setDefault(BYTE adc, BOOL state);
BOOL getDefault(BYTE adc);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

