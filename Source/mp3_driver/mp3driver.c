#include <string.h>
#include "inc/hw_ssi.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "driverlib/interrupt.h"
#include "Global.h"
#include "peripheral.h"
#include "mp3driver.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


//static char g_cTmpBuf[FILE_BUFF_SIZE];
/**************************************************************************************************
* Function void Delay_ms(int n)
* -------------------------------------------------------------------------------------------------
* Overview: Delay in n milisecond
* Input:  int n
* Output: nothing
**************************************************************************************************/
void Delay_ms(int n)
{
	int i,j;
	for(i =0;i<n;i++)
	for(j=0;j<1000;j++)
	;

}
/**************************************************************************************************
* Function unsigned char SPI0_Read(void)
* -------------------------------------------------------------------------------------------------
* Overview: Function read one byte of data from SPI0 
* Input:  nothing
* Output: data read from SPI
**************************************************************************************************/
unsigned char SPI0_Read(void)
{
	unsigned long temp_result;
	SSIDataPut(SSI0_BASE, 0xff);
	SSIDataGet(SSI0_BASE, &temp_result);
	return (unsigned char)temp_result;
}
/**************************************************************************************************
*Function void SPI0_Write(char)
* -------------------------------------------------------------------------------------------------
* Overview: Function send one byte of data to SPI0 
* Input:  data to send
* Output: Nothing
**************************************************************************************************/
void SPI0_Write(char data)
{
	unsigned long temp_result;
	SSIDataPut(SSI0_BASE, data);
	SSIDataGet(SSI0_BASE, &temp_result);
}

/**************************************************************************************************
* Function waitSPI()
* -------------------------------------------------------------------------------------------------
* Overview: Function Waiting for SSI line idle, implemented but not use
* Input:  None
* Output: Nothing
**************************************************************************************************/

void waitSPI()
{
	while(SSIBusy(SSI0_BASE))
	;
}

/**************************************************************************************************
* Function set_BSYNC()
* -------------------------------------------------------------------------------------------------
* Overview: Function Deselect Data interface of VS1011E
* Input:  None
* Output: Nothing
**************************************************************************************************/
void set_BSYNC()
{
	GPIOPinWrite(BSYNC,BSYNC_PIN);
}
/**************************************************************************************************
* Function clear_BSYNC()
* -------------------------------------------------------------------------------------------------
* Overview: Function Select Data interface of VS1011E
* Input:  None
* Output: Nothing
**************************************************************************************************/
void clear_BSYNC()
{
	GPIOPinWrite(BSYNC,0);
}

/**************************************************************************************************
* Function MP3_SCI_Write()
* -------------------------------------------------------------------------------------------------
* Overview: Function writes one byte to MP3 SCI
* Input: register address in codec, data
* Output: Nothing
**************************************************************************************************/
void MP3_SCI_Write(char address, unsigned int data_in) 
{

	unsigned char highbyte,lowbyte;
	highbyte =(unsigned char)(data_in>>8);
	lowbyte = (unsigned char)(data_in);
	set_BSYNC();	//		BSYNC = 1;
	select_MP3();		//MP3_CS = 0;  select MP3 SCI
	//Delay_ms(10);
	SPI0_Write(WRITE_CODE);
	SPI0_Write(address);
	SPI0_Write(highbyte);       // high byte
	SPI0_Write(lowbyte);       // low byte
	//waitSPI();
	deselect_MP3(); //MP3_CS = 1;// deselect MP3 SCI
	while (DREQ == 0);             // wait until DREQ becomes 1, see MP3 codec datasheet, Serial Protocol for SCI
}

/**************************************************************************************************
* Function MP3_Set_Volume()
* -------------------------------------------------------------------------------------------------
* Overview: Function set volume on the left and right channel
* Input: left channel volume, right channel volume
* Output: Nothing
**************************************************************************************************/
void MP3_Set_Volume(char left, char right) {
	unsigned int volume;

	volume = (left<<8) + right;             // calculate value
	MP3_SCI_Write(SCI_VOL_ADDR, volume);    // Write value to VOL register
}

/**************************************************************************************************
* Function MP3_SCI_Read()
* -------------------------------------------------------------------------------------------------
* Overview: Function reads words_count words from MP3 SCI
* Input: start address, word count to be read
* Output: words are stored to data_buffer
**************************************************************************************************/
void MP3_SCI_Read(char start_address, char words_count, unsigned int *data_buffer) {
	unsigned int temp;

	select_MP3();// MP3_CS = 0; // select MP3 SCI
	SPI0_Write(READ_CODE);
	SPI0_Write(start_address);
	waitSPI();
	while (words_count--) {        // read words_count words byte per byte
		temp = SPI0_Read();
		temp <<= 8;
		temp += SPI0_Read();
		*(data_buffer++) = temp;
	}
	
	deselect_MP3();    //MP3_CS = 1;// deselect MP3 SCI
	while (DREQ == 0);             // wait until DREQ becomes 1, see MP3 codec datasheet, Serial Protocol for SCI
}



unsigned int Mp3ReadRegister	(	unsigned char 	addressbyte	 ) 	
{
	unsigned int resultvalue = 0;
	unsigned char temp1,temp2;
	select_MP3();
	SPI0_Write(READ_CODE);
	SPI0_Write((addressbyte));
	temp1 = SPI0_Read();
	resultvalue = temp1; 
	resultvalue = resultvalue<<8;
	temp2 = SPI0_Read();
	waitSPI();
	resultvalue |= (temp2);
	deselect_MP3();
	return resultvalue;
	
}

/**************************************************************************************************
* Function MP3_SDI_Write()
* -------------------------------------------------------------------------------------------------
* Overview: Function write one byte to MP3 SDI
* Input: data to be writed
* Output: Nothing
**************************************************************************************************/
void MP3_SDI_Write(char data_) {

	clear_BSYNC();	// BSYNC = 0;
	deselect_MP3();		// MP3_CS = 1;
	

	while (!DREQ);             // wait until DREQ becomes 1, see MP3 codec datasheet, Serial Protocol for SCI

	SPI0_Write(data_);
	
	set_BSYNC();					//BSYNC = 1;
}

/**************************************************************************************************
* Function MP3_SDI_Write_32
* -------------------------------------------------------------------------------------------------
* Overview: Function Write 32 bytes to MP3 SDI
* Input: data buffer
* Output: Nothing
**************************************************************************************************/
void MP3_SDI_Write_32(char *data_) {
	char i;

	deselect_MP3();
	clear_BSYNC();

	while (!DREQ) // wait until DREQ becomes 1, see MP3 codec datasheet, Serial Protocol for SCI
	{
		//other stuff ?
	}            

	for (i=0; i<32; i++)
	SPI0_Write(data_[i]);
	waitSPI();
	set_BSYNC();
}

//************************************************//
//MP3_reset 
//Prototype: void MP3_reset(void)
//Description: perform hardware reset vs1011e.
//Returns: None.
//************************************************//
void MP3_reset()
{
	GPIOPinWrite(MP3_RST,0); 
	Delay_ms(10);
	GPIOPinWrite(MP3_RST,MP3_RST_PIN);
}

//***********************************************//
//MP3_pin_init 
//Prototype: void MP3_pin_init(void)
//Description: initial pins for communicate with vs1011e.
//Returns: None.
//***********************************************//
void MP3_pin_init()
{
	GPIOPinTypeGPIOOutput(MP3_CS);              // Configure MP3_CS as output
	deselect_MP3();							              // Deselect MP3_CS
	GPIOPinTypeGPIOOutput(MP3_RST);               // Configure MP3_RST as output
	GPIOPinWrite(MP3_RST,MP3_RST_PIN);               // Set MP3_RST pin
	
	GPIOPinTypeGPIOOutput(BSYNC);               // Configure BSYNC as output
	clear_BSYNC();		
	GPIOPinTypeGPIOInput(DREQPIN);                  // Configure DREQ as input
	GPIOPadConfigSet(DREQPIN, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}


//***********************************************//
//MP3_Init 
//Prototype: void  MP3_Init(void)
//Description: initial vs1011e chip
//Returns: None.
//***********************************************//
void  MP3_Init()
{
	MP3_pin_init();
	// Clear BSYNC

	// Hardware reset
	MP3_reset();
	while (!DREQ);

	MP3_SCI_Write(SCI_MODE_ADDR, 0x0800); //mode new
	MP3_SCI_Write(SCI_BASS_ADDR, 0x7A00);
	MP3_SCI_Write(SCI_CLOCKF_ADDR,0x9800);   // default 12 288 000 Hz
	//   // Maximum volume is 0x00 and total silence is 0xFE.
	//   
	//  // volume_right = 40; //0x3F;
	MP3_Set_Volume( 40, 40);
	Delay_ms(500);
}


//*************************************************//
//MP3_stream_mode 
//Prototype: void MP3_stream_mode(void)
//Description: set vs1011e run in audio streaming mode
//Returns: None.
//*************************************************//
void MP3_stream_mode(void)
{
	MP3_SCI_Write(SCI_MODE_ADDR ,0x0804) ;
	Delay_ms(1); /* One millisecond delay */
	while (!DREQ) /* wait for startup */
	;
	Delay_ms(100);
	MP3_SCI_Write(SCI_MODE_ADDR, 0x0840); //mode new stream
	MP3_SCI_Write(SCI_BASS_ADDR, 0x7A00);
	MP3_SCI_Write(SCI_CLOCKF_ADDR,0x9800);   // default 12 288 000 Hz
	//	Delay_ms(200); /* One millisecond delay */

	MP3_Set_Volume(volume, volume);
	Delay_ms(100); 

	MP3_SDI_Write(0x00);
	MP3_SDI_Write(0x00);
	MP3_SDI_Write(0x00);
	MP3_SDI_Write(0x00);
	Delay_ms(100);
}


//************************************************//
//MP3_soft_reset 
//Prototype: void MP3_soft_reset(unsigned char)
//Parameters: volume value after reset.
//Description: perform software reset for vs1011e, and set new volume.
//Returns: None.
//***********************************************//
void MP3_soft_reset(unsigned char volume)
{
	MP3_SCI_Write(SCI_MODE_ADDR ,0x0804) ;
	Delay_ms(1); /* One millisecond delay */
	while (!DREQ) /* wait for startup */
	;
	Delay_ms(100);
	MP3_SCI_Write(SCI_MODE_ADDR, 0x0800); //mode new
	MP3_SCI_Write(SCI_BASS_ADDR, 0x7A00);
	MP3_SCI_Write(SCI_CLOCKF_ADDR,0x9800);   // default 12 288 000 Hz
	//	Delay_ms(200); /* One millisecond delay */

	MP3_Set_Volume(volume, volume);
	Delay_ms(100); 

	MP3_SDI_Write(0x00);
	MP3_SDI_Write(0x00);
	MP3_SDI_Write(0x00);
	MP3_SDI_Write(0x00);
	Delay_ms(100);
}

//*************************************************//
//initSSI 
//Prototype: void initSSI(void)
//Description: init SPI module for communicate with vs1011e.
//Returns: None.
//*************************************************//
void initSSI(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5);
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4| GPIO_PIN_5, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 2000000, 8);
	SSIEnable(SSI0_BASE);
}


//****************************************************//
//send_audio_data
//Prototype: void send_audio_data(char *)
//Parameters: block 512 bytes read from sdcard.
//Description: send block of audio data to vs1011e.
//Returns: None.
//****************************************************//
void send_audio_data( char * g_cTmpBuf)
{
	int i;
	for(i=0;i<FILE_BUFF_SIZE/BUFF_SIZE;i++)
	{
		MP3_SDI_Write_32(g_cTmpBuf + i*BUFF_SIZE);
	}
	
}

//****************************************************//
//MP3_sineTest
//Prototype: void MP3_sineTest()
//Parameters: None.
//Description: test the sine wave function of vs1011e.
//Returns: None.
//****************************************************//
void MP3_sineTest()
{
	MP3_reset();       /* Pull xRESET low -> hardware reset */
	Delay_ms(100);            /* 100 ms delay */

	deselect_MP3();  /* Pull xCS high    */
	set_BSYNC();     /* Pull xDCS high */
	//Mp3DeselectData	( 		 ) 	   {MP3_XDCS=1;}

	Delay_ms(100);             /* 100 ms delay     */


	/* VS10xx Application Notes, chapter 4.8 ---------------------------------*/
	/* As an example, let's write value 0x0820 to register 00 byte by byte    */
	select_MP3();    /* Now SPI writes go to SCI port                   */
	SPI0_Write(0x02);      /* Send SPI Byte, then wait for byte to be sent.   */
	SPI0_Write(0x00);      /* 0x02 was WRITE command, 0x00 is register number */
	SPI0_Write(0x08);      /* This byte goes to MSB                           */
	SPI0_Write(0x20);      /* ..and this is LSB. (0x20=Allow Test Mode)       */
	waitSPI();             /* Wait until Atmel MCU signals SPI write complete */
	deselect_MP3();  /* Now SPI writes don't go to SCI port             */
	while (DREQ==0) {;}     /* Wait for DREQ = 1                               */
	/* Do nothing while waiting for DREQ = 1           */

	/* Send a Sine Test Header to Data port                                   */
	/* Now SPI writes go to SDI port                   */
	UARTprintf("in sine test \n");
	while(1)
	{
		MP3_SDI_Write(0x53);//SPI0_Write(0x53);      /* - This is a special VLSI Solution test header - */
		MP3_SDI_Write(0xef);//SPI0_Write(0xef);      /* - that starts a sine sound. It's good for     - */
		MP3_SDI_Write(0x6e);//SPI0_Write(0x6e);      /* - testing your code, the chip and also for    - */
		MP3_SDI_Write(0x44);//SPI0_Write(0x44);      /* - seeing if your MP3 decoder was manufactured - */
		MP3_SDI_Write(0x00);//SPI0_Write(0x00);      /* - by VLSI Solution oy. ------------------------ */
		MP3_SDI_Write(0x00);// SPI0_Write(0x00);
		MP3_SDI_Write(0x00);//SPI0_Write(0x00);
		MP3_SDI_Write(0x00);//SPI0_Write(0x00);

		Delay_ms(500);
		/* Stop the sine test sound */
		MP3_SDI_Write(0x45); //SPI0_Write(0x45);
		MP3_SDI_Write(0x78);//SPI0_Write(0x78);
		MP3_SDI_Write(0x69);//SPI0_Write(0x69);
		MP3_SDI_Write(0x74);// SPI0_Write(0x74);
		MP3_SDI_Write(0x00);//SPI0_Write(0x00);
		MP3_SDI_Write(0x00);// SPI0_Write(0x00);
		MP3_SDI_Write(0x00);//SPI0_Write(0x00);
		MP3_SDI_Write(0x00);//SPI0_Write(0x00);

		Delay_ms(500);            /* 500 ms delay */
	}
}

