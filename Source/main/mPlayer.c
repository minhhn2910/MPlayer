/*

THIS FILE IS MAIN PROGRAM OF MPLAYER APPLICATION

*/

#include <stdlib.h>
#include <string.h>
#include "Windows.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"

#include "utils/uartstdio.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "drivers/rit128x96x4.h"
#include "FileSystem.h"
#include "Global.h"
#include "ebook.h"
#include "peripheral.h"
#include "mp3driver.h"
#include "radio.h"
#include "buttons.h"
unsigned char temp;

// Flag indicate on changing window or element
unsigned char flagOnChangeWindow;
unsigned char flagOnChangeElement;
unsigned char flagOnChangeHeader;
unsigned char flagOnChangeBody;
unsigned char flagOnChangeSetting;
unsigned char flagOnChangeTime;
unsigned char flagOnChangeVolume;
unsigned char	flagOnChangeLyrics = W_REMAIN;

//state variables
unsigned char playing_state = STOP;
unsigned char reading_state = READ_STOP;
unsigned char radio_state = RADIO_STOP;
unsigned char percent_played;

//lyrics variables
unsigned char flagLyricsPrinted = 0;
unsigned char lyrics_exist = 0;

//for mp3 driver and player
char  volume = 40; //0x3F;
char random = false;
int current_position=0;
int current_book=0;
char timeout=5; //for 10ms *5 = 50ms
char second_tick = 100; //10ms * 100 = 1s every tick
char volume_change=0;

unsigned int random_number=0;
//for fatfs
static FIL	g_sFileObject;
static char g_cTmpBuf[FILE_BUFF_SIZE];

//static char* mp3_file_list[7];


//char *playing_song;
/*
* Playing window variable
*/
// Status of playing or pause
unsigned char stsPlaying;

// Status of Repeat option
unsigned char stsRepeat;

// Status of Shuffle option
unsigned char stsShuffle;

// Status of Auto turn off screen option
unsigned char stsAutoOff;


// Status of volume indicator on screen
unsigned char stsVolume;
unsigned char stsVolumePre;

// Variable to track time played.
unsigned long minutePlayed = 0;
unsigned long secondPlayed = 0;



void updateScreen(void);


//*******************************************//
//initUART
//Prototype:  void initUART(void);
//Parameters: None
//Description: Init UART for debug
//Returns: None.
//*******************************************//
void initUART(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTStdioInit(0);
}

//******************************************//
//SysTickIntHandler
//Prototype:  void SysTickIntHandler (void);
//Parameters: None
//Description: Method control timer interrupt for button reading, Ethernet timer, DiskIO,… 
//Returns: None.
//******************************************//
void
SysTickIntHandler(void)
{
	//
	// Call the FatFs tick timer.
	random_number++;
	timeout--;
	second_tick--;
	if(timeout==0)
	{
		timeout =5;
		buttons_process();
		//
		if((onWindow == W_PLAYING && playing_state == PLAY )|| onWindow == W_RADIO_INTERNET)
		{
			if(UP_PRESSED )
			{
				btnUp = 0;
				if(volume >5)
				volume-=2;
				else
				volume =0;
				volume_change = 1;
			}
			else if(DOWN_PRESSED )
			{
				btnDown = 0;
				if(volume < 0xfa)
				volume+=2;
				else
				volume =0xff;
				volume_change = 1;
			}
		}	
	}
	
	if(second_tick==0)// 1s tick
	{
		second_tick=100;
		if (playing_state == PLAY){
			secondPlayed++;
			if (secondPlayed == 60){
				secondPlayed = 0;
				minutePlayed++;
				if (minutePlayed == 60)
					minutePlayed = 0;
			}
			flagOnChangeTime = W_CHANGE;
			//UARTprintf("%d : %d", minutePlayed, secondPlayed);
			}
	}
	lwIPTimer(SYSTICKMS);
}


//**********************************************//
//Init 
//Prototype:  void Init(void);
//Parameters: None
//Description:  Initialize all needed functions, peripherals.
//Returns: None
//*********************************************//
void init_Timer()
{
	SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
	SysTickEnable();
	SysTickIntEnable();

	// Enable processor interrupts.
	IntMasterEnable();
}
void Init()
{
	
	onWindow = W_WELCOME;
	flagOnChangeWindow = W_CHANGE;
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	

	GPIOPinTypeGPIOOutput(CARD_CS); 

	deselect_CARD();
	deselect_MP3();
	initSSI();
	Delay_ms(500);
	initUART(); //for uart, debug
	UARTprintf("successful init uart \n");
	// 		Delay_ms(500);
	//	init_CARD();
	// 		Delay_ms(500);
	//desel lcd
	GPIOPinTypeGPIOOutput(LCD_CS); 
	GPIOPinWrite(LCD_CS,GPIO_PIN_3);
	//for buttons
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);  
	init_buttons();
	init_Ethernet();
	MP3_Init();
	//	Delay_ms(500);
	//	GPIOPinWrite(LED,1);
	//	select_LCD();
	RIT128x96x4Init(2000000);
	deselect_LCD();	
	//MP3_sineTest();
	//IntMasterEnable();
	init_CARD();
	
	temp = 0;
	stsRepeat = P_REPEAT_ALL;
	stsShuffle = P_SHUF_OFF ;

	UARTprintf("txt %d 	mp3 %d \n",ulTxtFileCount,ulMp3FileCount);
	
}


//*******************************************//
//stateMachine
//Prototype:  void stateMachine (void);
//Parameters: None
//Description:  Main state machine for the application.
//Returns: None.
//*******************************************//
void stateMachine(){
	FRESULT fresult;
	int i;
	unsigned short usBytesRead;
	switch (onWindow){
	case W_WELCOME:
		// Debug
		if(btnUp ==1)
		{
			onWindow = W_WELCOME_PLAYING;
			flagOnChangeWindow = W_CHANGE;
			btnUp = 0;
		}
		break;
	case W_WELCOME_PLAYING:
		if(btnRight== 1 )
			{
				onWindow = W_WELCOME_SETTING;
				flagOnChangeWindow = W_CHANGE;
				btnRight = 0;	
			}
		else if(btnLeft== 1 )
			{
				onWindow = W_WELCOME_RADIO_INTERNET;
				flagOnChangeWindow = W_CHANGE;
				btnLeft = 0;
			}
		else if(btnSelect == 1)
			{
				onWindow = W_PLAYING;
				flagOnChangeWindow = W_CHANGE;
				playing_state = START;
				btnSelect = 0;	
			}	
			break;
	case W_PLAYING:
		switch (playing_state)
		{
		case START:
			MP3_soft_reset(0x30);
			select_CARD();
			Delay_ms(10);
			fresult = f_open(&g_sFileObject, mp3_file_list[current_position], FA_READ);
			if(fresult != FR_OK)
				UARTprintf("f_mount error: \n");
			else
			{	
				UARTprintf("playing file %s \n", mp3_file_list[current_position]);
				playing_state = PLAY;
				percent_played = 0;
				flagOnChangeHeader = W_CHANGE;
				// Reset playing time
				minutePlayed = 0;
				secondPlayed = 0;			
				lyrics_exist = lyric_open(mp3_file_list[current_position]);
				if(lyrics_exist == 0)
				{
						strcpy(lyric_buffer,"No lyrics found!");
						flagOnChangeLyrics = W_CHANGE;
				}
				else
						lyric_read_next();
				
			}
			break;
		case PLAY:
			select_CARD();
			fresult = f_read(&g_sFileObject, g_cTmpBuf, FILE_BUFF_SIZE,&usBytesRead);
			percent_played = g_sFileObject.fptr*100/ g_sFileObject.fsize;

			if(lyrics_exist == 1)
			{
				if((minutePlayed == lyric_mm)&&(secondPlayed==(lyric_ss+2)))
				{
					if(flagLyricsPrinted ==0)
						flagOnChangeLyrics = W_CHANGE;
				}
				else
				{
					while(((minutePlayed == lyric_mm)&&(secondPlayed>(lyric_ss+2)))||((minutePlayed > lyric_mm)))
					{
						lyric_read_next();
						flagLyricsPrinted =0;
					}
				//	
				}
			}
			deselect_CARD();
			if(volume_change !=0)
			{
				MP3_Set_Volume(volume, volume);
				
				stsVolumePre = stsVolume;
				stsVolume = (255-volume+30)/(VOL_RANGE/VOL_STAT_NO);
				if (stsVolume !=  stsVolumePre){
					flagOnChangeVolume= W_CHANGE;
					}
				volume_change =0;

			}
			if(usBytesRead < FILE_BUFF_SIZE)
			{
				for(i=0;i<usBytesRead;i++)
				{
					MP3_SDI_Write(g_cTmpBuf[i]);
				}
				playing_state = NEXT;
			}
			else
			{
				send_audio_data(g_cTmpBuf);
				//		
				
			}
			if(btnRight==1)
			{
				playing_state = NEXT;	
				btnRight = 0;	
			}
			else if(btnLeft== 1)
			{
				playing_state = PREVIOUS;
				btnLeft = 0;					
			}
			else if(btnSelect == 1)
			{
				playing_state = PAUSE;
				flagOnChangeHeader = W_CHANGE;
				btnSelect = 0;	
			}		
			else if(btnSelectHold ==1)
			{
					playing_state = STOP;
					onWindow = W_WELCOME_PLAYING;
					flagOnChangeWindow = W_CHANGE;
					btnSelectHold = 0;
			}
			break;
		case NEXT:
			//choose next song
			switch(stsRepeat)
			{
				case P_REPEAT_NONE:
					// test
					if(current_position == ulMp3FileCount-1)
						current_position =0;
					else
					{
						if(stsShuffle == P_SHUF_OFF)
							current_position++;
						else
							current_position = random_number % ulMp3FileCount;
					}				
				break;
				case P_REPEAT_ONE :
				
				break;
				case P_REPEAT_ALL :
					if(current_position == ulMp3FileCount-1)
						current_position =0;
					else
					{
						if(stsShuffle == P_SHUF_OFF)
							current_position++;
						else
							current_position = random_number % ulMp3FileCount;
					}
				break;
			}
					songListPos = current_position;
					flagOnChangeBody = W_CHANGE;
					playing_state = START;
			break;
		case PREVIOUS:
					switch(stsRepeat)
			{
				case P_REPEAT_NONE:
				//test
					if(current_position == 0)
							current_position = ulMp3FileCount-1;
					else
					{
						if(stsShuffle == P_SHUF_OFF)
							current_position--;
						else
							current_position = random_number % ulMp3FileCount;
					}				
				break;
				case P_REPEAT_ONE :
				
				break;
				case P_REPEAT_ALL :
					if(current_position == 0)
						current_position = ulMp3FileCount-1;
					else
					{
						if(stsShuffle == P_SHUF_OFF)
							current_position--;
						else
							current_position = random_number % ulMp3FileCount;
					}
				break;
			}	

			songListPos = current_position;
			flagOnChangeBody = W_CHANGE;
			playing_state = START;
			break;
		case PAUSE:
			playing_state = PAUSE;
			if(btnRight==1)
			{
				playing_state = NEXT;	
				btnRight = 0;	
			}
			else if(btnLeft== 1)
			{
				playing_state = PREVIOUS;
				btnLeft = 0;					
			}
			else if (btnUp ==1){
				if (songListPos<=0) songListPos = ulMp3FileCount-1;
				else songListPos--;
				flagOnChangeBody = W_CHANGE;
				btnUp = 0;
			}
			else if (btnDown ==1){
				if (songListPos>= ulMp3FileCount-1) songListPos = 0;
				else songListPos++;
				flagOnChangeBody = W_CHANGE;
				btnDown = 0;
			}
			else if(btnSelect == 1)
			{
				if(current_position != songListPos)
				{
					current_position = songListPos;
					playing_state = START;
				}
				else
					playing_state = PLAY;
				
				flagOnChangeHeader = W_CHANGE;
				
				btnSelect = 0;	
			}	
			else if(btnSelectHold ==1)
			{
					playing_state = STOP;
					onWindow = W_WELCOME_PLAYING;
					flagOnChangeWindow = W_CHANGE;
					btnSelectHold = 0;
			}
			break;
		default:
			break;
		}

		break;

	case W_SONGSLIST:
		//UARTprintf("Songlist Window\n");
		// Button check
		if (btnRight==1){
			onWindow = W_RADIO_INTERNET;
			flagOnChangeWindow= W_CHANGE;

			// clear button
			btnRight = 0;
			
		} else if (btnLeft==1){
			onWindow = W_PLAYING;
			flagOnChangeWindow = W_CHANGE;
			// clear button					
			btnLeft = 0;
		} else if (btnUp==1){
			if (songListPos > 0){
				songListPos--;
			}
			else {
				songListPos = songListSize-1;
			}
			flagOnChangeWindow = W_CHANGE;

			btnUp = 0;

		} else if (btnDown==1){
			if (songListPos < (songListSize-1)){
				songListPos++;
			}
			else {
				songListPos = 0;
			}
			flagOnChangeWindow = W_CHANGE;
			
			btnDown = 0;
		} else if (btnSelect){
			//	selectSong(songListPos);
			current_position=songListPos;
			onWindow = W_PLAYING;
			stsPlaying = P_PLAYING;
			flagOnChangeWindow = W_CHANGE;

			btnSelect = 0;
		}
		break;

		// 			case W_FILEBROWSER:
		// 								UARTprintf("fileBrowser Window\n");
		// 				// Button check
		// 				if (btnRight==1){
		// 					// Chose file
		// 					// clear button
		// 					btnRight = 0;
		// 					
		// 				} else if (btnLeft==1){
		// 					onWindow = W_OPTIONS;
		// 					flagOnChangeWindow = W_CHANGE;
		// 					// clear button					
		// 					btnLeft = 0;
		// 				} else if (btnUp==1){
		// 					if (fileListPos > 0){
		// 						fileListPos--;
		// 						}
		// 					else {
		// 						fileListPos = fileListSize;
		// 						}
		// 					flagOnChangeWindow = W_CHANGE;
		// 					
		// 					btnUp = 0;

		// 				} else if (btnDown==1){
		// 					if (fileListPos < (fileListSize-1)){
		// 						fileListPos++;
		// 						}
		// 					else {
		// 						fileListPos = 0;
		// 						}
		// 					flagOnChangeWindow = W_CHANGE;
		// 					
		// 					btnDown = 0;
		// 				} else if (btnSelect){
		// 					onWindow = W_PLAYING;
		// 					flagOnChangeWindow = W_CHANGE;

		// 					btnSelect = 0;
		// 				}
		//  				break;
	case W_WELCOME_SETTING:
		 if (btnRight){
			onWindow = W_WELCOME_EBOOK;
			flagOnChangeWindow = W_CHANGE;
			
			btnRight= 0;

		} else if (btnLeft){
			onWindow = W_WELCOME_PLAYING;
			flagOnChangeWindow = W_CHANGE;
			
			btnLeft= 0;
		}
		else if (btnSelect){
			onWindow = W_SETTING;
			flagOnChangeWindow = W_CHANGE;
			btnSelect = 0;
			}
		break;
	case W_SETTING:
		//								UARTprintf("Seting Window\n");
		// Button check
		if (btnDown){
			// Move around the icons
			if (settingHighline <NUM_OF_SETTING-1) settingHighline++;
			else settingHighline = 0;
			flagOnChangeWindow = W_CHANGE;

			// clear button
			btnDown= 0;
			
		}
		else if (btnUp){
			if (settingHighline >0) settingHighline--;
			else settingHighline = NUM_OF_SETTING-1;
			flagOnChangeWindow = W_CHANGE;

			// clear button					
			btnUp= 0;
		}
		else if (btnSelect){
			if (settingHighline == STG_REPEAT) {// Repeat
				if (stsRepeat<2) stsRepeat++;
				else stsRepeat= 0;
			}
			else if (settingHighline == STG_SHUFFLE){ // Shuffle
				if (stsShuffle<1) stsShuffle++;
				else stsShuffle= 0;
			}
			else if (settingHighline == STG_AUTOOFF){
				if (stsAutoOff == 0) stsAutoOff = 1;
				else stsAutoOff = 0;
			}
			else if (settingHighline == STG_ABOUTUS){
				onWindow = W_WELCOME;
				flagOnChangeWindow= W_CHANGE;
			}
			flagOnChangeSetting = W_CHANGE;

			// change to Playing Window
			//onWindow = W_PLAYING;
			//flagOnChangeWindow = W_CHANGE;
			
			btnSelect = 0;
		}
		else if(btnSelectHold ==1)
		{
					playing_state = STOP;
					onWindow = W_WELCOME_SETTING;
					flagOnChangeWindow = W_CHANGE;
					btnSelectHold = 0;
		}
		
		break;
	case W_WELCOME_EBOOK:
		if (btnLeft)
		{		
				onWindow = W_WELCOME_SETTING;
				
				flagOnChangeWindow = W_CHANGE;
				
				btnLeft= 0;

		}
		else if (btnRight)
		{
				onWindow = W_WELCOME_RADIO_INTERNET;
				flagOnChangeWindow = W_CHANGE;
				
				btnRight= 0;
		}
		else if(btnSelect)
		{
				onWindow = W_EBOOK;
				flagOnChangeWindow = W_CHANGE;
				reading_state = READ_STOP;
				btnSelect= 0;
		}
		break;
	case W_EBOOK:
	
		switch(reading_state)
		{		
		case READ_STOP:
			if (btnUp)
			{
				if(ebookListPos == 0 )
					ebookListPos= ulTxtFileCount-1;
				else 
					ebookListPos--;
				flagOnChangeBody = W_CHANGE;
				btnUp = 0;
			}
			else if (btnDown)
			{
				if(ebookListPos == ulTxtFileCount-1)
					ebookListPos = 0;
				else 
					ebookListPos++;
				flagOnChangeBody = W_CHANGE;
				btnDown = 0;
			}
			else if (btnSelect)
			{
				current_book = ebookListPos;
				UARTprintf("catch ebook %s \n",txt_file_list[current_book]);
				reading_state = READ_START;
				btnSelect = 0;
			}
			else if(btnSelectHold ==1)
			{
					UARTprintf("catch return from ebook \n");
					onWindow = W_WELCOME_EBOOK;
					flagOnChangeWindow = W_CHANGE;
					btnSelectHold = 0;
					reading_state = READ_STOP;
			}
			break;
		case READ_START:
			ebook_open(txt_file_list[current_book]);
			ebook_read_next();
			flagOnChangeBody = W_CHANGE;
			reading_state = READ_WAIT;
			break;
		case READ_NEXT_PAGE:
			ebook_read_next();
			flagOnChangeBody = W_CHANGE;
			reading_state = READ_WAIT;
			break;
		case READ_PRE_PAGE:
			ebook_read_pre();
			flagOnChangeBody = W_CHANGE;
			reading_state = READ_WAIT;
			break;
		case READ_WAIT:
			if(btnSelect == 1)
			{
				reading_state = READ_STOP;
				flagOnChangeWindow= W_CHANGE;
				btnSelect=0;
			}
			else if (btnLeft)
			{		
				reading_state = READ_PRE_PAGE;
				
				btnLeft= 0;

			}
			else if (btnRight)
			{
				reading_state = READ_NEXT_PAGE;	
				btnRight= 0;
			}
			else if(btnSelectHold ==1)
			{
					playing_state = STOP;
					onWindow = W_WELCOME_EBOOK;
					flagOnChangeWindow = W_CHANGE;
					btnSelectHold = 0;
					reading_state = READ_STOP;
			}
			break;
		default:
			reading_state = READ_WAIT;
			break;
		}
		break;
	case W_WELCOME_RADIO_INTERNET:
		if (btnLeft)
		{		
				onWindow = W_WELCOME_EBOOK;
				
				flagOnChangeWindow = W_CHANGE;
				
				btnLeft= 0;

		}
		else if (btnRight)
		{
				onWindow = W_WELCOME_PLAYING;
				flagOnChangeWindow = W_CHANGE;
				
				btnRight= 0;
		}
		else if(btnSelect)
		{
				onWindow = W_RADIO_INTERNET;
				flagOnChangeWindow = W_CHANGE;
				radio_state = RADIO_STOP;
				btnSelect= 0;
		}
		break;
	case W_RADIO_INTERNET:
		//("Radio Window\n");
		// Button check
				start_internet_radio();
				//				radio_state	 = RADIO_START;
				while(!SELECT_PRESSED);
				stop_internet_radio();
				while(SELECT_PRESSED);					

				//				Delay_ms(400);
		
				btnSelect= 0;
				btnSelectHold = 0;
				onWindow = W_WELCOME_RADIO_INTERNET;
				flagOnChangeWindow = W_CHANGE;	

		break;

	default:
		//								UARTprintf("Default Window\n");
		onWindow = W_WELCOME;
		flagOnChangeWindow = W_CHANGE;
	break;
	}
}

//*****************************************************************************//
//updateScreen
//Prototype:  void updateScreen (void);
//Parameters: None
//Description:  Main process to update screen after any change need to display.
//Returns: None.
//*****************************************************************************//
void updateScreen(void){

	select_LCD();
	switch (onWindow){
	case W_WELCOME:
		

		// Base on button pressed, we will change the situation
		if (flagOnChangeWindow == W_CHANGE){
			welcomeWindow();
			//					UARTprintf("Welcome Window\n");
			flagOnChangeWindow = W_REMAIN;
		}	
		
		break;
	case W_WELCOME_PLAYING:
		if (flagOnChangeWindow == W_CHANGE){
					draw_welcome_playing();
			//					UARTprintf("Welcome Window\n");
			flagOnChangeWindow = W_REMAIN;
		}	

		break;
	case W_PLAYING:
		//				UARTprintf("Playing Window\n");
		if (flagOnChangeWindow == W_CHANGE){
			playingWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		if (flagOnChangeHeader == W_CHANGE){
			updatePlayingHeader();
			flagOnChangeHeader = W_REMAIN;
		}
		if (flagOnChangeBody == W_CHANGE){
			updatePlayingBody();
			flagOnChangeBody = W_REMAIN;
		}
		if(flagOnChangeLyrics == W_CHANGE)
		{
			updateLyrics();
			flagOnChangeLyrics = W_REMAIN;
		}
		if (flagOnChangeTime == W_CHANGE){
			updatePlayingTime();
			flagOnChangeTime = W_REMAIN;
			}
		if (flagOnChangeVolume== W_CHANGE){
			updateVolumeArea();
			flagOnChangeVolume= W_REMAIN;
			}
		drawPlayingStateBarFromBegining(percent_played);
		break;

	case W_OPTIONS:
		if (flagOnChangeWindow == W_CHANGE){
			optionsWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		if (flagOnChangeBody == W_CHANGE){
			updateOptionsList();
			flagOnChangeBody = W_REMAIN;
		}
		
		break;

	case W_SONGSLIST:
		if (flagOnChangeWindow == W_CHANGE){
			songListWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		
		
		break;

	case W_FILEBROWSER:
		if (flagOnChangeWindow == W_CHANGE){
			fileBrowserWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		
		
		break;
	case W_WELCOME_SETTING:
		
		if (flagOnChangeWindow == W_CHANGE){
								draw_welcome_setting();
			//					UARTprintf("Welcome Window\n");
			flagOnChangeWindow = W_REMAIN;
		}	

		break;
	case W_SETTING:
		if (flagOnChangeWindow == W_CHANGE){
			settingWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		if (flagOnChangeBody == W_CHANGE){
			updateFileList();
			flagOnChangeBody = W_REMAIN;
		}
		if (flagOnChangeSetting == W_CHANGE){
			updateSettingState();
			flagOnChangeSetting = W_REMAIN;
		}
		
		break;
	case W_WELCOME_EBOOK:
		if (flagOnChangeWindow == W_CHANGE){

			draw_welcome_ebook();
			//					UARTprintf("Welcome Window\n");
			flagOnChangeWindow = W_REMAIN;
		}	

		break;
	case W_EBOOK:
		if (flagOnChangeWindow == W_CHANGE){
			ebookWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		if (reading_state == READ_STOP){
			if (flagOnChangeBody == W_CHANGE){
				updateEbookBody();
				flagOnChangeBody = W_REMAIN;
				}
			}
		else {
			if (flagOnChangeBody == W_CHANGE){
			//updateFileList();
			
			print_ebook();
			flagOnChangeBody = W_REMAIN;
				}
		}
		if (flagOnChangeHeader == W_CHANGE){
			updateEbookHeader();
			flagOnChangeHeader = W_REMAIN;
			}
		break;
	case W_WELCOME_RADIO_INTERNET:
		if (flagOnChangeWindow == W_CHANGE){

			draw_welcome_radio_internet();
			//					UARTprintf("Welcome Window\n");
			flagOnChangeWindow = W_REMAIN;
		}	
		break;
	case W_RADIO_INTERNET:
		if (flagOnChangeWindow == W_CHANGE){
			radioWindow();
			flagOnChangeWindow = W_REMAIN;
		}
		if (flagOnChangeBody == W_CHANGE){
			//updateFileList();
			flagOnChangeBody = W_REMAIN;
		}
		break;

	default:
		welcomeWindow();

	}
	deselect_LCD();
}

//********************************************//
//main
//Prototype:  void main(void);
//Parameters: None
//Description:  Main function of this project.
//Returns: None.
//********************************************//
int main(){

	// *********
	// INITIALIZE
	// *********
	Init();

	while(1)
	{
		stateMachine();
		updateScreen();
		
	}
	
}



