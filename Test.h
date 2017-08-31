/* 
 * File:   Test.h
 * Author: Luca
 *
 * Created on August 30, 2017, 10:59 PM
 */

#ifndef TEST_H
#define	TEST_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

    void TestStart(void);
    void TestRelay(void);
    void TestADC(void);
    void TestInputOutput(void);
    void TestEEPROM(void);
    void TestEthGet(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TEST_H */

