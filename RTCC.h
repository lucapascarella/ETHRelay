/* 
 * File:   RTCC.h
 * Author: Luca
 *
 * Created on 12 aprile 2016, 15.55
 */

#ifndef RTCC_H
#define	RTCC_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum {SUNDAY = 1, MONNDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY} WEEKDAYS;

    

    
void InitRTCC(void);
void UpdateRTCC(void);



#ifdef	__cplusplus
}
#endif

#endif	/* RTCC_H */

