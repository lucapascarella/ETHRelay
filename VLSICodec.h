/********************************************************************
 FileName:		VLSICodec.h
 Processor:		PIC18F67J60 Microcontrollers
 Hardware:		Filodiffusione
 Complier:  	Microchip C18 (for PIC18)
 Company:		Elettronica In.
 Author:		Luca Pascarella www.lucasproject.it

********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   09/11/2009   Initial release

********************************************************************/
#ifndef __VLSICODEC_H
#define __VLSICODEC_H

#include "GenericTypeDefs.h"

/****************************************************************************
  Function:
    WORD VLSI_GetBitrate(void)

  Description:
    Get bitrate of currently playing stream

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
WORD VLSI_GetBitrate(void);

/****************************************************************************
  Function:
    void VLSI_Init(void)

  Description:
    This routine configures the VLSI codec.  If basic SPI communication with
    the codec is not working, this function will NOT return.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/

/****************************************************************************
  Function:
    void VLSI_Init_In(void)

  Description:
    This routine configures the VLSI codec.  If basic SPI communication with
    the codec is not working, this function will NOT return.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
//void VLSI_Init_In(void);

void VLSI_Init(void);

/****************************************************************************
  Function:
    void VLSI_SetBassBoost(BYTE bass, BYTE gfreq)

  Description:
    This function sets the bass boost.

  Precondition:
    None

  Parameters:
    BYTE bass   - Bass gain in dB, range from 0 to 15
    BYTE gfreq  - Limit frequency for bass boost, 10 Hz steps (range from
                    0 to 15)

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void VLSI_SetBassBoost(BYTE bass, BYTE gfreq);

/****************************************************************************
  Function:
    void VLSI_SetVolume(BYTE vRight, BYTE vLeft)

  Description:
    This function set volume for analog outputs on the VLSI codec.

  Precondition:
    None

  Parameters:
    BYTE vRight - right channel attenuation from maximum volume, 0.5dB steps
                        (0x00 = full volume, 0xFF = muted)
    BYTE vLeft  - left channel attenuation from maximum volume, 0.5dB steps
                        (0x00 = full volume, 0xFF = muted)

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void VLSI_SetVolume(BYTE vRight, BYTE vLeft);

/****************************************************************************
  Function:
    void VLSI_SineTest()

  Description:
    SDI Test Mode must be enabled in MODE Register (0x00). Enters infinite
    loop, should only be used for VLSI function test

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    This function is for debug purposes.  To enable compilation, define the
    label VLSI_SINE_TEST.
  ***************************************************************************/
#if defined VLSI_SINE_TEST
void VLSI_SineTest(void);
#endif

WORD VLSIReadReg(BYTE vAddress);
void VLSIWriteReg(BYTE vAddress, WORD wValue);
BYTE WriteSPI(BYTE output);

//#define WriteSPI(output) MP3_SPI_IF = 0; MP3_SSPBUF = output; while(!MP3_SPI_IF); return MP3_SSPBUF;

//#define VLSIWriteData(output) MP3_XDCS_IO = 0; WriteSPI(output) MP3_XDCS_IO = 1;
//#define VLSIWaitDreqHight() while(!MP3_DREQ_IO);

#define RING_BUFFER_SIZE                (1024)


#define dreqIsNonFull()                 MP3_DREQ_I
#define dreqIsFull()                    !MP3_DREQ_I

#define WEB_PAGE_SIZE                   0x14000     // 80kByte
#define PLUGIN_SIZE                     0x08000     // 32kByte
#define OTHERS_SIZE                     0x04000     // 16kByte
#define EEPROM_SIZE                     0x20000     // 128kByte
// Spazio disponibile per
#define PLUGIN_START_ADDRESS    	0x14000     // Start at 80kByte
#define CODEC_START_ADDRESS             0x14020     // 32 Byte
#define URL_NAME_ADDRESS                0x1C000     // Start at 80+32=112kByte
#define URL_NAME_LENGTH                 0x00100     // 256 Byte
#define URL_RESOURCE_ADDRESS            0x1C100     // Start at 112kByte + 256Byte
#define URL_RESOURCE_LENGTH     	0x00100     // 256 Byte
#define URL_DDNS_ADDRESS                0x1C200     //
#define URL_DDNS_LENGTH                 0x00200     // 256 Byte
#define HTTP_RESPONSE_ADDRESS           0x1C300     // Start at 112kByte + 512Byte
#define HTTP_RESPONSE_LENGTH     	0x00200     // 512 Byte

void VLSI_Init_In(void);
void VLSI_SetBassBoost_In(BYTE bass, BYTE gfreq);
void VLSI_SetVolume_In(BYTE vRight, BYTE vLeft);
WORD VLSIReadRegIn(BYTE vAddress);
void VLSIWriteRegIn(BYTE vAddress, WORD wValue);
BYTE WriteSPIIn(BYTE output);

BOOL LoadUserCode(void);

unsigned int VLSIGetArray(BYTE*, WORD);
void VLSIPutArray(BYTE*, WORD);
WORD VLSIPutRingBuffer(void);

BOOL VLSIInitEncoder(void);
void VLSI_SoftReset(void);
void VLSIStartEncoder(void);
void VLSIStopEncoder(void);
BOOL VLSIIsStopped(void);

unsigned int VLSIPlugin(DWORD);
DWORD VLSISizePlugin(void);

#define VLSI_MODE           0x00
#define VLSI_STATUS         0x01
#define VLSI_BASS           0x02
#define VLSI_CLOCKF         0x03
#define VLSI_DECODE_TIME    0x04
#define VLSI_AUDATA         0x05
#define VLSI_WRAM           0x06
#define VLSI_WRAMADDR       0x07
#define VLSI_HDAT0          0x08
#define VLSI_HDAT1          0x09
#define VLSI_AIADDR         0x0A
#define VLSI_VOL            0x0B
#define VLSI_AICTRL0        0x0C
#define VLSI_AICTRL1        0x0D
#define VLSI_AICTRL2        0x0E
#define VLSI_AICTRL3        0x0F

// SCI_MODE is used to control the operation of VS1063a and defaults to 0x0800
#define VLSI_VAL_MODE_DECODE          0x0802
typedef union {
    WORD word;

    struct __PACKED {
        BYTE SM_DIFF : 1;       //  bit 0
        BYTE SM_LAYER12 : 1;    //  bit 1
        BYTE SM_RESET : 1;      //  bit 2
        BYTE SM_CANCEL : 1;     //  bit 3
        BYTE SM_EARSPEAKER_LO : 1;//  bit 4
        BYTE SM_TESTS : 1;      //  bit 5
        BYTE SM_STREAM : 1;     //  bit 6
        BYTE SM_EARSPEAKER_HI : 1;//  bit 7
        BYTE SM_DACT : 1;       //  bit 8
        BYTE SM_SDIORD : 1;     //  bit 9
        BYTE SM_SDISHARE : 1;   //  bit 10
        BYTE SM_SDINEW : 1;     //  bit 11
        BYTE SM_ADPCM : 1;      //  bit 12
        BYTE none4 : 1;         //  bit 13
        BYTE SM_LINE1 : 1;      //  bit 14
        BYTE SM_CLK_RANGE : 1;  //  bit 15
    } bits;
} SCI_MODE;

// SCI_STATUS contains information on the current status of VS1063a.
typedef union {
    WORD word;

    struct __PACKED {
        BYTE SS_REFERENCE_SEL : 1;  //  bit 0
        BYTE SS_AD_CLOCK : 1;
        BYTE SS_APDOWN1 : 1;
        BYTE SS_APDOWN2 : 1;
        BYTE SS_VER : 4;
        BYTE none1 : 2;
        BYTE SS_VCM_DISABLE : 1;
        BYTE SS_VCM_OVERLOAD : 1;
        BYTE SS_SWING : 3;
        BYTE SS_DO_NOT_JUMP : 1;    //  bit 15
    } bits;
} SCI_STATUS;

// SCI_AICTRL3 offers run-time controls, channel selection for mono input, the maximum
// number of samples that are allowed in one Ogg frame with 4096 sample granularity, and
// the next unread byte, if any.
typedef union {
    WORD word;

    struct __PACKED {
        BYTE finishRecording : 1;   // bit 0
        BYTE recordingFinished : 1; // bit 1
        BYTE lastOneByteToRead : 1; // bit 2
        BYTE channelSelect : 1;     // bit 3
        BYTE maxSamplesInFrame : 4; // bit 7:4
        BYTE nextDataByte : 2;      // bit 8:15
   } bits;
} SCI_AICTRL3;

#endif


