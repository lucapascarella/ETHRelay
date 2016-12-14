/********************************************************************
 FileName:		VLSICodec.c
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
#include "TCPIPStack/TCPIP.h"
#include "VLSICodec.h"
//#include "CodecPlugin.h"
#include "HardwareProfile.h"

#define ClearSPIDoneFlag()                      {MP3_SPI_IF = 0;}
#define WaitForDataByte()                       {while(!MP3_SPI_IF); MP3_SPI_IF = 0;}
#define SPI_ON_BIT                              (MP3_SPICON1bits.SSPEN)

#define ringIsEmpty(start, end)                 (end == start)
#define ringIsFull(start, end)                  (end == (start ^ RING_BUFFER_SIZE))
#define ringIncr(p)                             ((p + 1)&(2*RING_BUFFER_SIZE-1))

#define ringRead(ringBuffer, start)             ({ \
                                                BYTE ret = ringBuffer[start&(RING_BUFFER_SIZE-1)]; \
                                                start = ringIncr(start); \
                                                ret; \
                                                })

#define ringWrite(ringBuffer, start, end, elem) ( \
                                                ringBuffer[end&(RING_BUFFER_SIZE-1)] = elem; \
                                                end = ringIncr(end); \
                                                )

//******************************************************************************
//******************************************************************************
// Section: Constants
//******************************************************************************
//******************************************************************************

#define VLSI_STATUS_VER     0x00F0u
#define VER_VS1001          (0u<<4)
#define VER_VS1011          (1u<<4)
#define VER_VS1002          (2u<<4)
#define VER_VS1011E         (2u<<4)
#define VER_VS1003          (3u<<4)
#define VER_VS1053          (4u<<4)
#define VER_VS1033          (5u<<4)
#define VER_VS1103          (7u<<4)


//******************************************************************************
//******************************************************************************
// Section: Function Prototypes
//******************************************************************************
//******************************************************************************

//static WORD VLSIReadReg(BYTE vAddress);
//static void VLSIWriteReg(BYTE vAddress, WORD wValue);
//static BYTE WriteSPI(BYTE output);

//******************************************************************************
//******************************************************************************
//******************************************************************************
// Section: Application Callable Functions
//******************************************************************************
//******************************************************************************
//******************************************************************************

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
void VLSI_Init(void) {

    WORD bSPICONSave, bSPISTATSave;

    // Set up SPI port pins
    MP3_XDCS_O = 1; // Make the Data CS pin high
    MP3_XCS_O = 1; // Make the Control CS pin high
    MP3_XRESET_O = 0;

    MP3_XRESET_TRIS = 0;
    MP3_DREQ_TRIS = 1;
    MP3_XDCS_TRIS = 0; // Make the Data CS pin an output
    MP3_XCS_TRIS = 0; // Make the Control CS pin an output
    MP3_SDI_TRIS = 1; // Make the DIN pin an input
    MP3_SDO_TRIS = 0; // Make the DOUT pin an output
    MP3_SCK_TRIS = 0; // Make the SCK pin an output

    // Set up SPI module
    bSPISTATSave = MP3_SPISTAT;
    bSPICONSave = MP3_SPICON1;

    MP3_SPISTATbits.SMP = 0; // Sample at middle
    MP3_SPISTATbits.CKE = 1; // Transmit data on rising edge of clock

    // Use slow SPI speed first, will bump up later
    MP3_SPICON1 = 0x22; // SSPEN = 1, CKP = 0, CLK = Fosc/64, this one doesn't go as fast

    MP3_SPI_IF = 0;

    // Deassert RESET (active low)
    DelayMs(1);
    MP3_XRESET_O = 1;

    // Write configuration MODE register in a loop to verify that the chip is
    // connected and running correctly
    do {
	VLSIWriteReg(VLSI_MODE, 0x0820);
    } while (VLSIReadReg(VLSI_MODE) != 0x0820u);

    if ((VLSIReadReg(VLSI_STATUS) & VLSI_STATUS_VER) == VER_VS1053) {
	// Set the clock to maximum speed (VS1053 only) to allow all audio formats
	// to be decoded without glitching.  Note that this increases power
	// consumption.
	// SC_MULT = XTALI*5.0, SC_ADD = 2.0x, SC_FREQ = 0 (12.288MHz)
	//VLSIWriteReg(VLSI_CLOCKF, 0xE000 | 0x1800 | 0x0000);
	// SC_MULT = XTALI*3.5, SC_ADD = 2.0x, SC_FREQ = 0 (12.288MHz)
	VLSIWriteReg(VLSI_CLOCKF, 0x8000 | 0x1800 | 0x0000);
    } else if ((VLSIReadReg(VLSI_STATUS) & VLSI_STATUS_VER) == VER_VS1011E) {
	// Nothing special to do
	Nop();
    } else {
	// VLSI Chip version not tested, not supported, halt program execution.
	// This trap should be caught during the design phase.
	while (1);
    }


    MP3_SPICON1 = bSPICONSave;
    MP3_SPISTAT = bSPISTATSave;
    MP3_SPICON1 = 0x21; // SSPEN = 1, CKP = 0, CLK = Fosc/16 (2.604MHz)

    // Set decode mode
    VLSIWriteReg(VLSI_MODE, VLSI_VAL_MODE_DECODE);

    // Do a soft reset
    VLSI_SoftReset();
}

BOOL VLSIInitEncoder(void) {

    extern APP_CONFIG AppConfig;
    SCI_AICTRL3 sci_aictrl3;
    SCI_MODE sci_mode;

    // Set the VS1053b clock to the highest value just below 55.3 MHz with 12.288MHz
    VLSIWriteReg(VLSI_CLOCKF, 0xC000);

    // Reset VS1053b by setting bit SM_RESET (2) to 1 in register SCI_MODE (0).
    // At the same time, clear bit SM_ADPCM (12) to 0.
    VLSI_SoftReset();

    // Set SCI_BASS to 0; Clear bass booster
    VLSIWriteReg(VLSI_BASS, 0x0000);

    // Disable any user application
    VLSIWriteReg(VLSI_AIADDR, 0x0000);
    // Disable all interrupt except SCI interrupt
    VLSIWriteReg(VLSI_WRAMADDR, 0xC01A);
    VLSIWriteReg(VLSI_WRAM, 0x0002);

    // Load the plugin profile
    if (!LoadUserCode())
	return FALSE;

    // Set SM_ADPCM bit
    sci_mode.word = VLSIReadReg(VLSI_MODE);
    //sci_mode.word = 0x0000;
    sci_mode.bits.SM_ADPCM = TRUE;
    sci_mode.bits.SM_LINE1 = TRUE;
    VLSIWriteReg(VLSI_MODE, sci_mode.word);
    // Recording gain
    VLSIWriteReg(VLSI_AICTRL1, AppConfig.Stream.recGain);
    // Max level Recording gain
    VLSIWriteReg(VLSI_AICTRL2, AppConfig.Stream.autoGain);
    // Set to zero
    VLSIWriteReg(VLSI_AICTRL0, 0x0000);
    // Prepare encoding
    sci_aictrl3.word = 0x00;
    sci_aictrl3.bits.maxSamplesInFrame = 0; // 4 * 4096 = 16384 samples
    VLSIWriteReg(VLSI_AICTRL3, sci_aictrl3.word);

    return TRUE;
}

void VLSI_SoftReset(void) {

    SCI_MODE sci_mode;

    sci_mode.word = VLSIReadReg(VLSI_MODE);
    sci_mode.word = 0x0000;
    sci_mode.bits.SM_ADPCM = FALSE;
    sci_mode.bits.SM_LINE1 = FALSE;
    sci_mode.bits.SM_LAYER12 = TRUE;
    sci_mode.bits.SM_SDINEW = TRUE;
    sci_mode.bits.SM_RESET = TRUE;
    VLSIWriteReg(VLSI_MODE, sci_mode.word);
    DelayMs(1);
    VLSIWriteReg(VLSI_CLOCKF, 0xC000);

    DelayMs(1);
    while (MP3_DREQ_I == 0);

    // Monitoring volume
    VLSI_SetVolume(AppConfig.Stream.volume.bits.bVolLeft, AppConfig.Stream.volume.bits.bVolLeft);

    // Set Bass and Treble values
    VLSIWriteReg(VLSI_BASS, AppConfig.Stream.bassReg.word);
}

void VLSIStartEncoder(void) {

    // Activate the encoder
    VLSIWriteReg(VLSI_AIADDR, 0x0034);
}

void VLSIStopEncoder(void) {

    SCI_AICTRL3 sci_aictrl3;
    // Deactive the encoder
    sci_aictrl3.word = VLSIReadReg(VLSI_AICTRL3);
    sci_aictrl3.bits.finishRecording = TRUE;
    VLSIWriteReg(VLSI_AICTRL3, sci_aictrl3.word);
}

BOOL VLSIIsStopped(void) {

    SCI_AICTRL3 sci_aictrl3;
    // Wait the deactivation of the encoder
    sci_aictrl3.word = VLSIReadReg(VLSI_AICTRL3);
    if (sci_aictrl3.bits.recordingFinished == TRUE);
    return TRUE;
    return FALSE;
}

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
void VLSI_SetVolume(BYTE vRight, BYTE vLeft) {
    VLSIWriteReg(VLSI_VOL, ((WORD) vLeft) << 8 | vRight);
}


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
////#if defined VLSI_SINE_TEST
//
//void VLSI_SineTest() {
//    // Send Test Sequence
//    while (1) {
//	while (!MP3_DREQ_I);
//	//      MP3_XDCS_IO = 0;
//	// Start Sine Test
//	WriteSPI(0x53);
//	WriteSPI(0xEF);
//	WriteSPI(0x6E);
//	WriteSPI(0x7D);
//	WriteSPI(0x00);
//	WriteSPI(0x00);
//	WriteSPI(0x00);
//	WriteSPI(0x00);
//	//    MP3_XDCS_IO = 1;
//    }
//}
////#endif

//******************************************************************************
//******************************************************************************
//******************************************************************************
// Section: Internal Functions
//******************************************************************************
//******************************************************************************
//******************************************************************************

/****************************************************************************
  Function:
    static WORD VLSIReadReg(BYTE vAddress)

  Description:
    This function reads one register from the VLSI code

  Precondition:
    None

  Parameters:
    BYTE vAddress - register address

  Returns:
    WORD value of the register

  Remarks:
    None
 ***************************************************************************/
WORD VLSIReadReg(BYTE vAddress) {
    WORD wValue;

    while (!MP3_DREQ_I);
    MP3_XCS_O = 0;
    WriteSPI(0x03); // Read
    WriteSPI(vAddress); // Register address
    ((BYTE*) & wValue)[1] = WriteSPI(0xFF); // 16 bit value high byte
    ((BYTE*) & wValue)[0] = WriteSPI(0xFF); // 16 bit value low byte
    MP3_XCS_O = 1;

    EEPROM_SPI_IF = 0;

    return wValue;
}

/****************************************************************************
  Function:
    static void VLSIWriteReg(BYTE vAddress, WORD wValue)

  Description:
    This function writes one register from the VLSI code

  Precondition:
    None

  Parameters:
    BYTE vAddress   - address of register
    WORD wValue     - value to write to the register

  Returns:
    None

  Remarks:
    None
 ***************************************************************************/
void VLSIWriteReg(BYTE vAddress, WORD wValue) {
    while (!MP3_DREQ_I);
    MP3_XCS_O = 0;
    WriteSPI(0x02); // Write
    WriteSPI(vAddress); // Register address
    WriteSPI(((BYTE*) & wValue)[1]); // 16 bit value to write high byte
    WriteSPI(((BYTE*) & wValue)[0]); // 16 bit value to write low byte
    MP3_XCS_O = 1;

    EEPROM_SPI_IF = 0;
}

/****************************************************************************
  Function:
    static BYTE WriteSPI(BYTE output)

  Description:
    This function writes one byte out the MP3 SPI port, as defined by the
    configuration file, HardwareProfile.h.  Since we are the master, we do
    an SPI read by doing an SPI write and checking the SPI buffer register.
    This routine returns that value, so it can also be used to perform an
    SPI read.  For an SPI read, the value output byte is irrelevant.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    Different architectures have different methods of determining when the
    SPI write is complete.
 ***************************************************************************/

BYTE WriteSPI(BYTE output) {
    #if defined( __18CXX )
    MP3_SPI_IF = 0;
    MP3_SSPBUF = output;
    while (!MP3_SPI_IF);
    #elif defined( __C30__ )
    MP3_SPI_IF = 0;
    MP3_SSPBUF = output;
    while (!MP3_SPI_IF);
    MP3_SPI_IF = 0;
    #else
    #error Define SPI Write mechanism
    #endif

    return MP3_SSPBUF;
}

unsigned int VLSIGetArray(BYTE *bfr, WORD len) {

    unsigned int i, WordToRead;
    unsigned char Temp, Ret;

    //Spostare
    while (!MP3_DREQ_I)
	LED1_IO = 1;
    LED1_IO = 0;
    //Fine

    MP3_XCS_O = 0;
    MP3_SPI_IF = 0;
    MP3_SSPBUF = 0x03;
    while (!MP3_SPI_IF);
    Ret = MP3_SSPBUF;
    MP3_SPI_IF = 0;
    MP3_SSPBUF = VLSI_HDAT1;
    while (!MP3_SPI_IF);
    Ret = MP3_SSPBUF;
    MP3_SPI_IF = 0;
    MP3_SSPBUF = 0xFF;
    while (!MP3_SPI_IF);
    ((BYTE*) & WordToRead)[1] = MP3_SSPBUF;
    MP3_SPI_IF = 0;
    MP3_SSPBUF = 0xFF;
    while (!MP3_SPI_IF);
    ((BYTE*) & WordToRead)[0] = MP3_SSPBUF;
    MP3_XCS_O = 1;

    if (WordToRead < len) {
	if ((WordToRead % 2) == 1u)
	    WordToRead--;
	len = WordToRead;
    }

    for (i = 0; i < len; i++) {
	while (!MP3_DREQ_I); //Abilitare nel caso di problemi
	MP3_XCS_O = 0;
	MP3_SPI_IF = 0;
	MP3_SSPBUF = 0x03;
	while (!MP3_SPI_IF);
	Ret = MP3_SSPBUF;
	MP3_SPI_IF = 0;
	MP3_SSPBUF = VLSI_HDAT0;
	while (!MP3_SPI_IF);
	Ret = MP3_SSPBUF;
	MP3_SPI_IF = 0;
	MP3_SSPBUF = 0xFF;
	while (!MP3_SPI_IF);
	Temp = MP3_SSPBUF;
	MP3_SPI_IF = 0;
	MP3_SSPBUF = 0xFF;
	bfr[i] = Temp;
	i++;
	while (!MP3_SPI_IF);
	bfr[i] = MP3_SSPBUF;
	MP3_XCS_O = 1;
    }

    EEPROM_SPI_IF = 0;

    return len;
}

void VLSIPutArray(BYTE *bfr, WORD len) {

    int i;
    volatile BYTE Temp, Ret;
    BYTE vSPIONSave;
    BYTE SPICON1Save;

    // Save SPI state
    SPICON1Save = MP3_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    //MP3_SPICON1 = (0x21); /* SSPEN bit is set, SPI in master mode, FOSC/16 (With 25MHz have >2.5MHz on SPI), IDLE state is low level */
    // Only for SCI -> SDI write CLKI/4, With 12.288MHz and PLL 5x -> 15 MHz
    MP3_SPICON1 = (0x20); /* SSPEN bit is set, SPI in master mode, FOSC/4 (With 25MHz have >10MHz on SPI), IDLE state is low level */
    SPI_ON_BIT = 1;

    MP3_XDCS_O = 0;
    for (i = 0; i < len; i++) {
	while (!MP3_DREQ_I)
	    LED1_IO = 1;
	LED1_IO = 0;
	MP3_SPI_IF = 0;
	MP3_SSPBUF = bfr[i];
	i++;
	Temp = bfr[i];
	while (!MP3_SPI_IF);
	Ret = MP3_SSPBUF;
	MP3_SPI_IF = 0;
	MP3_SSPBUF = Temp;
	while (!MP3_SPI_IF);
	Ret = MP3_SSPBUF;
    }
    MP3_XDCS_O = 1;

    EEPROM_SPI_IF = 0;

    // Restore SPI state
    SPI_ON_BIT = 0;
    MP3_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}

BOOL LoadUserCode(void) {

    DWORD i = 0;
    DWORD c = VLSISizePlugin();

    if (VLSIPlugin(0) == 0xFFFF)
	return FALSE;

    while (i < c) {
	unsigned short addr, n, val;

	//addr = VLSIPlugin(i);
	addr = VLSIPlugin(i++);
	n = VLSIPlugin(i++);
	if (n & 0x8000U) { /* RLE run, replicate n samples */
	    n &= 0x7FFF;
	    //	val = VLSIPlugin(i);
	    val = VLSIPlugin(i++);
	    while (n--) {
		VLSIWriteReg(addr, val);
		//WriteVS10xxRegister(addr, val);
	    }
	} else { /* Copy run, copy n samples */
	    while (n--) {
		//  val = VLSIPlugin(i);
		val = VLSIPlugin(i++);
		VLSIWriteReg(addr, val);
		//WriteVS10xxRegister(addr, val);
	    }
	}
    }
    return TRUE;
}

unsigned int VLSIPlugin(DWORD i) {
    BYTE bfr[2];
    WORD a, b, ret;
    DWORD j;

    j = CODEC_START_ADDRESS + (i * 2);
    XEEReadArray(j, bfr, 2);

    a = bfr[0]&0x00FF;
    b = bfr[1]&0x00FF;
    ret = (a << 8) + b;

    return ret;
    //return plugin[i];
}

DWORD VLSISizePlugin(void) {

    BYTE bfr[5];
    DWORD ret, a;

    XEEReadArray(PLUGIN_START_ADDRESS + 27, bfr, 5);

    ret = (bfr[0] - '0')*10000;
    a = (bfr[1] - '0')*1000;
    ret += a;
    a = bfr[2];
    a = a - '0';
    a = a * 100;
    ret += a;
    a = (bfr[3] - '0')*10;
    ret += a;
    a = bfr[4] - '0';
    ret += a;

    //ret = sizeof(plugin)/sizeof(plugin[0]);

    return ret;
}

WORD VLSIPutRingBuffer() {

    extern BYTE ringBuffer[RING_BUFFER_SIZE];
    extern UINT ringStart, ringEnd;

    WORD i = 0;
    volatile BYTE Dummy, tmp;
    BYTE vSPIONSave;
    BYTE SPICON1Save;

    // Return 0 if buffer is empty or internal VS1053 buffer is full
    if (ringIsEmpty(ringStart, ringEnd) || dreqIsFull())
	return 0;

    LED1_IO = 1;

    // Save SPI state
    SPICON1Save = MP3_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    //MP3_SPICON1 = (0x21); /* SSPEN bit is set, SPI in master mode, FOSC/16 (With 25MHz have >2.5MHz on SPI), IDLE state is low level */
    // Only for SCI -> SDI write CLKI/4, With 12.288MHz and PLL 5x -> 15 MHz
    MP3_SPICON1 = (0x20); /* SSPEN bit is set, SPI in master mode, FOSC/4 (With 25MHz have >10MHz on SPI), IDLE state is low level */
    SPI_ON_BIT = 1;

    // Select Data chip
    MP3_XDCS_O = 0;


    tmp = ringBuffer[ringStart & (RING_BUFFER_SIZE - 1)];
    do {
	MP3_SSPBUF = tmp;
	ringStart = ringIncr(ringStart);
	i++;
	tmp = ringBuffer[ringStart & (RING_BUFFER_SIZE - 1)];
	WaitForDataByte();
	Dummy = EEPROM_SSPBUF;
	if (dreqIsFull())
	    break;
    } while (!ringIsEmpty(ringStart, ringEnd));

    //    // Send some data, exit if the buffer is empty or internal VS1053 buffer is full
    //    do {
    //        MP3_SSPBUF = ringBuffer[ringStart & (RING_BUFFER_SIZE - 1)];
    //        ringStart = ringIncr(ringStart);
    //        i++;
    //        WaitForDataByte();
    //        Dummy = EEPROM_SSPBUF;
    //    } while (!ringIsEmpty(ringStart, ringEnd) && !dreqIsFull());

    // Deselect Data chip
    MP3_XDCS_O = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    MP3_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;

    LED1_IO = 0;

    return i;
}



//int cbIsFull(CircularBuffer *cb) {
//    return cb->end == (cb->start ^ cb->size); /* This inverts the most significant bit of start before comparison */ }
//
//int cbIsEmpty(CircularBuffer *cb) {
//    return cb->end == cb->start; }
//
//int cbIncr(CircularBuffer *cb, int p) {
//    return (p + 1)&(2*cb->size-1); /* start and end pointers incrementation is done modulo 2*size */
//}
//
//void cbWrite(CircularBuffer *cb, BYTE *elem) {
//    cb->elems[cb->end&(cb->size-1)] = *elem;
//    if (cbIsFull(cb)) /* full, overwrite moves start pointer */
//        cb->start = cbIncr(cb, cb->start);
//    cb->end = cbIncr(cb, cb->end);
//}
//
//void cbRead(CircularBuffer *cb, BYTE *elem) {
//    *elem = cb->elems[cb->start&(cb->size-1)];
//    cb->start = cbIncr(cb, cb->start);
//}


