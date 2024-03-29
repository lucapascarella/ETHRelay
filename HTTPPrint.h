/*********************************************************************
 * FileName: HTTPPrint.h
 * Provides callback headers and resolution for user's custom
 * HTTP Application.
 * Processor: PIC18,PIC24E, PIC24F, PIC24H, dsPIC30F, dsPIC33F, dsPIC33E,PIC32
 * Compiler:  Microchip C18, C30, C32
 * 
 * This file is automatically generated by the MPFS Utility
 * ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS GENERATOR
 *
 * Software License Agreement
 *
 * Copyright (C) 2012 Microchip Technology Inc.  All rights
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
 *********************************************************************/

#ifndef __HTTPPRINT_H
#define __HTTPPRINT_H

#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_HTTP2_SERVER)

extern HTTP_STUB httpStubs[MAX_HTTP_CONNECTIONS];
extern BYTE curHTTPID;

void HTTPPrint(DWORD callbackID);
void HTTPPrint_rebootaddr(void);
void HTTPPrint_user(void);
void HTTPPrint_pass(void);
void HTTPPrint_mac(void);
void HTTPPrint_hostname(void);
void HTTPPrint_icmp(void);
void HTTPPrint_dhcp(void);
void HTTPPrint_ip(void);
void HTTPPrint_gw(void);
void HTTPPrint_subnet(void);
void HTTPPrint_dns1(void);
void HTTPPrint_dns2(void);
void HTTPPrint_ddnsPublic(void);
void HTTPPrint_ddnsEnabled(void);
void HTTPPrint_ddnsService(void);
void HTTPPrint_ddnsHost(void);
void HTTPPrint_ddnsUser(void);
void HTTPPrint_ddnsPass(void);
void HTTPPrint_pingIsComp(void);
void HTTPPrint_pingIsTimeout(void);
void HTTPPrint_pingIsUnresolved(void);
void HTTPPrint_pingTime(void);
void HTTPPrint_pingAdd(void);
void HTTPPrint_cs(void);
void HTTPPrint_ddnsStatus(void);
void HTTPPrint_time(void);
void HTTPPrint_date(void);
void HTTPPrint_gmt(void);
void HTTPPrint_dste(void);
void HTTPPrint_dst(void);
void HTTPPrint_ntpe(void);
void HTTPPrint_ntps(void);
void HTTPPrint_rl(WORD);
void HTTPPrint_in(WORD);
void HTTPPrint_io(WORD);
void HTTPPrint_st(WORD);
void HTTPPrint_an(WORD);
void HTTPPrint_ao(WORD);
void HTTPPrint_ar(WORD);
void HTTPPrint_ad(WORD);
void HTTPPrint_is(WORD);
void HTTPPrint_ioAdd(WORD);
void HTTPPrint_ioUser(WORD);
void HTTPPrint_ioPass(WORD);
void HTTPPrint_ioSel(WORD);
void HTTPPrint_ah(WORD);
void HTTPPrint_al(WORD);
void HTTPPrint_ron1h(WORD);
void HTTPPrint_ron1m(WORD);
void HTTPPrint_roff1h(WORD);
void HTTPPrint_roff1m(WORD);
void HTTPPrint_ron2h(WORD);
void HTTPPrint_ron2m(WORD);
void HTTPPrint_roff2h(WORD);
void HTTPPrint_roff2m(WORD);
void HTTPPrint_oon1h(WORD);
void HTTPPrint_oon1m(WORD);
void HTTPPrint_ooff1h(WORD);
void HTTPPrint_ooff1m(WORD);
void HTTPPrint_oon2h(WORD);
void HTTPPrint_oon2m(WORD);
void HTTPPrint_ooff2h(WORD);
void HTTPPrint_ooff2m(WORD);

void HTTPPrint(DWORD callbackID)
{
	switch(callbackID)
	{
        case 0x00000002:
			HTTPIncFile((ROM BYTE*)"footer.inc");
			break;
        case 0x0000001c:
			HTTPPrint_rebootaddr();
			break;
        case 0x00000045:
			HTTPIncFile((ROM BYTE*)"menu.inc");
			break;
        case 0x00000047:
			HTTPPrint_user();
			break;
        case 0x00000048:
			HTTPPrint_pass();
			break;
        case 0x00000049:
			HTTPPrint_mac();
			break;
        case 0x0000004a:
			HTTPPrint_hostname();
			break;
        case 0x0000004b:
			HTTPPrint_icmp();
			break;
        case 0x0000004c:
			HTTPPrint_dhcp();
			break;
        case 0x0000004d:
			HTTPPrint_ip();
			break;
        case 0x0000004e:
			HTTPPrint_gw();
			break;
        case 0x0000004f:
			HTTPPrint_subnet();
			break;
        case 0x00000050:
			HTTPPrint_dns1();
			break;
        case 0x00000051:
			HTTPPrint_dns2();
			break;
        case 0x00000056:
			HTTPPrint_ddnsPublic();
			break;
        case 0x00000057:
			HTTPPrint_ddnsEnabled();
			break;
        case 0x00000058:
			HTTPPrint_ddnsService();
			break;
        case 0x00000059:
			HTTPPrint_ddnsHost();
			break;
        case 0x0000005a:
			HTTPPrint_ddnsUser();
			break;
        case 0x0000005b:
			HTTPPrint_ddnsPass();
			break;
        case 0x0000005c:
			HTTPPrint_pingIsComp();
			break;
        case 0x0000005d:
			HTTPPrint_pingIsTimeout();
			break;
        case 0x0000005e:
			HTTPPrint_pingIsUnresolved();
			break;
        case 0x0000005f:
			HTTPPrint_pingTime();
			break;
        case 0x00000060:
			HTTPPrint_pingAdd();
			break;
        case 0x00000061:
			HTTPPrint_cs();
			break;
        case 0x00000062:
			HTTPPrint_ddnsStatus();
			break;
        case 0x00000063:
			HTTPPrint_time();
			break;
        case 0x00000064:
			HTTPPrint_date();
			break;
        case 0x00000065:
			HTTPPrint_gmt();
			break;
        case 0x00000066:
			HTTPPrint_dste();
			break;
        case 0x00000067:
			HTTPPrint_dst();
			break;
        case 0x00000068:
			HTTPPrint_ntpe();
			break;
        case 0x00000069:
			HTTPPrint_ntps();
			break;
        case 0x0000006e:
			HTTPPrint_rl(1);
			break;
        case 0x0000006f:
			HTTPPrint_rl(2);
			break;
        case 0x00000070:
			HTTPPrint_rl(3);
			break;
        case 0x00000071:
			HTTPPrint_rl(4);
			break;
        case 0x00000072:
			HTTPPrint_in(1);
			break;
        case 0x00000073:
			HTTPPrint_in(2);
			break;
        case 0x00000074:
			HTTPPrint_in(3);
			break;
        case 0x00000075:
			HTTPPrint_in(4);
			break;
        case 0x00000076:
			HTTPPrint_in(5);
			break;
        case 0x00000077:
			HTTPPrint_in(6);
			break;
        case 0x00000078:
			HTTPPrint_in(7);
			break;
        case 0x00000079:
			HTTPPrint_in(8);
			break;
        case 0x00000082:
			HTTPPrint_io(1);
			break;
        case 0x00000083:
			HTTPPrint_io(2);
			break;
        case 0x00000084:
			HTTPPrint_io(3);
			break;
        case 0x00000085:
			HTTPPrint_io(4);
			break;
        case 0x00000086:
			HTTPPrint_io(5);
			break;
        case 0x00000087:
			HTTPPrint_io(6);
			break;
        case 0x00000088:
			HTTPPrint_io(7);
			break;
        case 0x00000089:
			HTTPPrint_io(8);
			break;
        case 0x00000096:
			HTTPPrint_st(1);
			break;
        case 0x00000097:
			HTTPPrint_st(2);
			break;
        case 0x00000098:
			HTTPPrint_st(3);
			break;
        case 0x00000099:
			HTTPPrint_st(4);
			break;
        case 0x0000009a:
			HTTPPrint_an(1);
			break;
        case 0x0000009b:
			HTTPPrint_an(2);
			break;
        case 0x0000009c:
			HTTPPrint_an(3);
			break;
        case 0x0000009d:
			HTTPPrint_an(4);
			break;
        case 0x000000a2:
			HTTPPrint_ao(1);
			break;
        case 0x000000a3:
			HTTPPrint_ao(2);
			break;
        case 0x000000a4:
			HTTPPrint_ao(3);
			break;
        case 0x000000a5:
			HTTPPrint_ao(4);
			break;
        case 0x000000a6:
			HTTPPrint_ar(1);
			break;
        case 0x000000a7:
			HTTPPrint_ar(2);
			break;
        case 0x000000a8:
			HTTPPrint_ar(3);
			break;
        case 0x000000a9:
			HTTPPrint_ar(4);
			break;
        case 0x000000aa:
			HTTPPrint_ad(1);
			break;
        case 0x000000ab:
			HTTPPrint_ad(2);
			break;
        case 0x000000ac:
			HTTPPrint_ad(3);
			break;
        case 0x000000ad:
			HTTPPrint_ad(4);
			break;
        case 0x000000b2:
			HTTPPrint_is(1);
			break;
        case 0x000000b3:
			HTTPPrint_is(2);
			break;
        case 0x000000b4:
			HTTPPrint_is(3);
			break;
        case 0x000000b5:
			HTTPPrint_is(4);
			break;
        case 0x000000b6:
			HTTPPrint_is(5);
			break;
        case 0x000000b7:
			HTTPPrint_is(6);
			break;
        case 0x000000b8:
			HTTPPrint_is(7);
			break;
        case 0x000000b9:
			HTTPPrint_is(8);
			break;
        case 0x000000be:
			HTTPPrint_ioAdd(1);
			break;
        case 0x000000bf:
			HTTPPrint_ioAdd(2);
			break;
        case 0x000000c0:
			HTTPPrint_ioAdd(3);
			break;
        case 0x000000c1:
			HTTPPrint_ioAdd(4);
			break;
        case 0x000000c2:
			HTTPPrint_ioAdd(5);
			break;
        case 0x000000c3:
			HTTPPrint_ioAdd(6);
			break;
        case 0x000000c4:
			HTTPPrint_ioAdd(7);
			break;
        case 0x000000c5:
			HTTPPrint_ioAdd(8);
			break;
        case 0x000000c6:
			HTTPPrint_ioUser(1);
			break;
        case 0x000000c7:
			HTTPPrint_ioUser(2);
			break;
        case 0x000000c8:
			HTTPPrint_ioUser(3);
			break;
        case 0x000000c9:
			HTTPPrint_ioUser(4);
			break;
        case 0x000000ca:
			HTTPPrint_ioUser(5);
			break;
        case 0x000000cb:
			HTTPPrint_ioUser(6);
			break;
        case 0x000000cc:
			HTTPPrint_ioUser(7);
			break;
        case 0x000000cd:
			HTTPPrint_ioUser(8);
			break;
        case 0x000000ce:
			HTTPPrint_ioPass(1);
			break;
        case 0x000000cf:
			HTTPPrint_ioPass(2);
			break;
        case 0x000000d0:
			HTTPPrint_ioPass(3);
			break;
        case 0x000000d1:
			HTTPPrint_ioPass(4);
			break;
        case 0x000000d2:
			HTTPPrint_ioPass(5);
			break;
        case 0x000000d3:
			HTTPPrint_ioPass(6);
			break;
        case 0x000000d4:
			HTTPPrint_ioPass(7);
			break;
        case 0x000000d5:
			HTTPPrint_ioPass(8);
			break;
        case 0x000000d6:
			HTTPPrint_ioSel(1);
			break;
        case 0x000000d7:
			HTTPPrint_ioSel(2);
			break;
        case 0x000000d8:
			HTTPPrint_ioSel(3);
			break;
        case 0x000000d9:
			HTTPPrint_ioSel(4);
			break;
        case 0x000000da:
			HTTPPrint_ioSel(5);
			break;
        case 0x000000db:
			HTTPPrint_ioSel(6);
			break;
        case 0x000000dc:
			HTTPPrint_ioSel(7);
			break;
        case 0x000000dd:
			HTTPPrint_ioSel(8);
			break;
        case 0x000000e6:
			HTTPPrint_ah(1);
			break;
        case 0x000000e7:
			HTTPPrint_ah(2);
			break;
        case 0x000000e8:
			HTTPPrint_ah(3);
			break;
        case 0x000000e9:
			HTTPPrint_ah(4);
			break;
        case 0x000000ea:
			HTTPPrint_al(1);
			break;
        case 0x000000eb:
			HTTPPrint_al(2);
			break;
        case 0x000000ec:
			HTTPPrint_al(3);
			break;
        case 0x000000ed:
			HTTPPrint_al(4);
			break;
        case 0x0000010e:
			HTTPPrint_ron1h(1);
			break;
        case 0x0000010f:
			HTTPPrint_ron1m(1);
			break;
        case 0x00000110:
			HTTPPrint_roff1h(1);
			break;
        case 0x00000111:
			HTTPPrint_roff1m(1);
			break;
        case 0x00000112:
			HTTPPrint_ron1h(2);
			break;
        case 0x00000113:
			HTTPPrint_ron1m(2);
			break;
        case 0x00000114:
			HTTPPrint_roff1h(2);
			break;
        case 0x00000115:
			HTTPPrint_roff1m(2);
			break;
        case 0x00000116:
			HTTPPrint_ron1h(3);
			break;
        case 0x00000117:
			HTTPPrint_ron1m(3);
			break;
        case 0x00000118:
			HTTPPrint_roff1h(3);
			break;
        case 0x00000119:
			HTTPPrint_roff1m(3);
			break;
        case 0x0000011a:
			HTTPPrint_ron1h(4);
			break;
        case 0x0000011b:
			HTTPPrint_ron1m(4);
			break;
        case 0x0000011c:
			HTTPPrint_roff1h(4);
			break;
        case 0x0000011d:
			HTTPPrint_roff1m(4);
			break;
        case 0x0000011e:
			HTTPPrint_ron2h(1);
			break;
        case 0x0000011f:
			HTTPPrint_ron2m(1);
			break;
        case 0x00000120:
			HTTPPrint_roff2h(1);
			break;
        case 0x00000121:
			HTTPPrint_roff2m(1);
			break;
        case 0x00000122:
			HTTPPrint_ron2h(2);
			break;
        case 0x00000123:
			HTTPPrint_ron2m(2);
			break;
        case 0x00000124:
			HTTPPrint_roff2h(2);
			break;
        case 0x00000125:
			HTTPPrint_roff2m(2);
			break;
        case 0x00000126:
			HTTPPrint_ron2h(3);
			break;
        case 0x00000127:
			HTTPPrint_ron2m(3);
			break;
        case 0x00000128:
			HTTPPrint_roff2h(3);
			break;
        case 0x00000129:
			HTTPPrint_roff2m(3);
			break;
        case 0x0000012a:
			HTTPPrint_ron2h(4);
			break;
        case 0x0000012b:
			HTTPPrint_ron2m(4);
			break;
        case 0x0000012c:
			HTTPPrint_roff2h(4);
			break;
        case 0x0000012d:
			HTTPPrint_roff2m(4);
			break;
        case 0x0000012e:
			HTTPPrint_oon1h(1);
			break;
        case 0x0000012f:
			HTTPPrint_oon1m(1);
			break;
        case 0x00000130:
			HTTPPrint_ooff1h(1);
			break;
        case 0x00000131:
			HTTPPrint_ooff1m(1);
			break;
        case 0x00000132:
			HTTPPrint_oon1h(2);
			break;
        case 0x00000133:
			HTTPPrint_oon1m(2);
			break;
        case 0x00000134:
			HTTPPrint_ooff1h(2);
			break;
        case 0x00000135:
			HTTPPrint_ooff1m(2);
			break;
        case 0x00000136:
			HTTPPrint_oon1h(3);
			break;
        case 0x00000137:
			HTTPPrint_oon1m(3);
			break;
        case 0x00000138:
			HTTPPrint_ooff1h(3);
			break;
        case 0x00000139:
			HTTPPrint_ooff1m(3);
			break;
        case 0x0000013a:
			HTTPPrint_oon1h(4);
			break;
        case 0x0000013b:
			HTTPPrint_oon1m(4);
			break;
        case 0x0000013c:
			HTTPPrint_ooff1h(4);
			break;
        case 0x0000013d:
			HTTPPrint_ooff1m(4);
			break;
        case 0x0000013e:
			HTTPPrint_oon1h(5);
			break;
        case 0x0000013f:
			HTTPPrint_oon1m(5);
			break;
        case 0x00000140:
			HTTPPrint_ooff1h(5);
			break;
        case 0x00000141:
			HTTPPrint_ooff1m(5);
			break;
        case 0x00000142:
			HTTPPrint_oon1h(6);
			break;
        case 0x00000143:
			HTTPPrint_oon1m(6);
			break;
        case 0x00000144:
			HTTPPrint_ooff1h(6);
			break;
        case 0x00000145:
			HTTPPrint_ooff1m(6);
			break;
        case 0x00000146:
			HTTPPrint_oon1h(7);
			break;
        case 0x00000147:
			HTTPPrint_oon1m(7);
			break;
        case 0x00000148:
			HTTPPrint_ooff1h(7);
			break;
        case 0x00000149:
			HTTPPrint_ooff1m(7);
			break;
        case 0x0000014a:
			HTTPPrint_oon1h(8);
			break;
        case 0x0000014b:
			HTTPPrint_oon1m(8);
			break;
        case 0x0000014c:
			HTTPPrint_ooff1h(8);
			break;
        case 0x0000014d:
			HTTPPrint_ooff1m(8);
			break;
        case 0x0000014e:
			HTTPPrint_oon2h(1);
			break;
        case 0x0000014f:
			HTTPPrint_oon2m(1);
			break;
        case 0x00000150:
			HTTPPrint_ooff2h(1);
			break;
        case 0x00000151:
			HTTPPrint_ooff2m(1);
			break;
        case 0x00000152:
			HTTPPrint_oon2h(2);
			break;
        case 0x00000153:
			HTTPPrint_oon2m(2);
			break;
        case 0x00000154:
			HTTPPrint_ooff2h(2);
			break;
        case 0x00000155:
			HTTPPrint_ooff2m(2);
			break;
        case 0x00000156:
			HTTPPrint_oon2h(3);
			break;
        case 0x00000157:
			HTTPPrint_oon2m(3);
			break;
        case 0x00000158:
			HTTPPrint_ooff2h(3);
			break;
        case 0x00000159:
			HTTPPrint_ooff2m(3);
			break;
        case 0x0000015a:
			HTTPPrint_oon2h(4);
			break;
        case 0x0000015b:
			HTTPPrint_oon2m(4);
			break;
        case 0x0000015c:
			HTTPPrint_ooff2h(4);
			break;
        case 0x0000015d:
			HTTPPrint_ooff2m(4);
			break;
        case 0x0000015e:
			HTTPPrint_oon2h(5);
			break;
        case 0x0000015f:
			HTTPPrint_oon2m(5);
			break;
        case 0x00000160:
			HTTPPrint_ooff2h(5);
			break;
        case 0x00000161:
			HTTPPrint_ooff2m(5);
			break;
        case 0x00000162:
			HTTPPrint_oon2h(6);
			break;
        case 0x00000163:
			HTTPPrint_oon2m(6);
			break;
        case 0x00000164:
			HTTPPrint_ooff2h(6);
			break;
        case 0x00000165:
			HTTPPrint_ooff2m(6);
			break;
        case 0x00000166:
			HTTPPrint_oon2h(7);
			break;
        case 0x00000167:
			HTTPPrint_oon2m(7);
			break;
        case 0x00000168:
			HTTPPrint_ooff2h(7);
			break;
        case 0x00000169:
			HTTPPrint_ooff2m(7);
			break;
        case 0x0000016a:
			HTTPPrint_oon2h(8);
			break;
        case 0x0000016b:
			HTTPPrint_oon2m(8);
			break;
        case 0x0000016c:
			HTTPPrint_ooff2h(8);
			break;
        case 0x0000016d:
			HTTPPrint_ooff2m(8);
			break;
		default:
			// Output notification for undefined values
			TCPPutROMArray(sktHTTP, (ROM BYTE*)"!DEF", 4);
	}

	return;
}

void HTTPPrint_(void)
{
	TCPPut(sktHTTP, '~');
	return;
}

#endif

#endif
