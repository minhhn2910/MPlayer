#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "fatfs/ff.h"
#include "fatfs/diskio.h"
/*
* Define the code for buttons
*/
#define B_NONE		5
#define B_UP 		0
#define B_DOWN 		1
#define B_LEFT 		2
#define B_RIGHT 	3
#define B_SELECT 	4

#define LEFT    0
#define RIGHT   1
#define UP      2
#define DOWN    3
#define RESET   4
#define NON  5
#define SELECT	6

#define LEFT_HOLD   	 7
#define RIGHT_HOLD  	 8
#define UP_HOLD    		 9
#define DOWN_HOLD  	  10
#define SELECT_HOLD		13

#define SHORT_BUTTON  1
#define MIDDLE_BUTTON  8
#define LONG_BUTTON		15

#define  WRITE_CODE            0x02
#define READ_CODE             0x03

#define SCI_BASE_ADDR          0x00
#define SCI_MODE_ADDR          0x00
#define SCI_STATUS_ADDR        0x01
#define SCI_BASS_ADDR          0x02
#define SCI_CLOCKF_ADDR        0x03
#define SCI_DECODE_TIME_ADDR   0x04
#define SCI_AUDATA_ADDR        0x05
#define SCI_WRAM_ADDR          0x06
#define SCI_WRAMADDR_ADDR      0x07
#define SCI_HDAT0_ADDR         0x08
#define SCI_HDAT1_ADDR         0x09
#define SCI_AIADDR_ADDR        0x0A
#define SCI_VOL_ADDR           0x0B
#define SCI_AICTRL0_ADDR       0x0C
#define SCI_AICTRL1_ADDR       0x0D
#define SCI_AICTRL2_ADDR       0x0E
#define SCI_AICTRL3_ADDR       0x0F

#define UP_PRESSED 			!(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)&GPIO_PIN_0)
#define DOWN_PRESSED		!(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&GPIO_PIN_1)
#define LEFT_PRESSED		!(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&GPIO_PIN_2)
#define RIGHT_PRESSED 	!(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)&GPIO_PIN_3)
#define SELECT_PRESSED 	!(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)&GPIO_PIN_1)
#define LED      GPIO_PORTF_BASE, GPIO_PIN_0

#define BUTTON_REPEAT	3

//ethernet module
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0
//chip select
#define LCD_CS   	GPIO_PORTA_BASE, GPIO_PIN_3
#define CARD_CS		GPIO_PORTD_BASE, GPIO_PIN_0

#define MP3_CS_PIN		 GPIO_PIN_4                
#define BSYNC_PIN      GPIO_PIN_7                 
#define MP3_RST_PIN    GPIO_PIN_5               
#define DREQPIN_PIN    GPIO_PIN_6 

#define MP3_CS     GPIO_PORTD_BASE, GPIO_PIN_4                
#define BSYNC      GPIO_PORTD_BASE, GPIO_PIN_7                 
#define MP3_RST    GPIO_PORTD_BASE, GPIO_PIN_5               
#define DREQPIN    GPIO_PORTD_BASE, GPIO_PIN_6     
#define DREQ       (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6)&GPIO_PIN_6)   

#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))
#define FILE_BUFF_SIZE 512
#define BUFF_SIZE 32

#define NUM_OF_BUTTON	5
#define NUM_OF_OPTION	5
#define LEN_OPTION		32

#define NUM_OF_SETTING	4
#define STG_REPEAT		0
#define STG_SHUFFLE		1
#define STG_AUTOOFF		2
#define STG_ABOUTUS		3
#define SETTING_LEN		4

/*
* Define code for being active Windows
*/
#define W_WELCOME 		0
#define W_PLAYING 		1
#define W_OPTIONS 		2	
#define W_SONGSLIST		3
#define W_FILEBROWSER 	4
#define W_SETTING 		5
#define W_EBOOK			6
#define W_RADIO_INTERNET	7

#define W_WELCOME_PLAYING 		8
#define W_WELCOME_OPTIONS 		9	
#define W_WELCOME_SETTING 		10
#define W_WELCOME_EBOOK			11
#define W_WELCOME_RADIO_INTERNET	12

// define reading state
#define READ_STOP 				0
#define READ_WAIT 				1
#define READ_NEXT_PAGE 		2
#define READ_END 					3	
#define READ_PRE_PAGE			4
#define READ_START 				5
// define playing state
#define START 0
#define PLAY 1
#define STOP 2
#define PAUSE 3
#define NEXT 4
#define PREVIOUS 5
#define FOWARD 6
#define REWIND 7

// define radio state
#define RADIO_STOP 		0
#define RADIO_START 	1


#define W_CHANGE	1
#define W_REMAIN	2
#define W_OFF		0
#define W_ON		3

#define W_WIDTH		128
#define W_HEIGHT	96
#define W_HEADER_HEIGHT	24
#define W_LIST_HEIGHT	12
#define W_OP_LIST_HEIGHT	12
#define W_SET_LIST_HEIGHT	12

#define W_NUM_SONG_SHOW_LIST	4
#define W_NUM_OPTIONS		6

#define W_LINE_LEN		20
#define W_CHAR_PAD_TOP	2
#define W_CHAR_PAD_LEFT	5

#define W_SETTING_ALIGN	100

#define X_PLAYING_TIME 	90
#define Y_PLAYING_TIME 	3


#define COLOR_WHITE	15
#define COLOR_BLACK	0
#define COLOR_GRAY	12
#define COLOR_DARK	7

#define W_FILE_NUM_SHOW			7
#define W_FILE_HEADER_HEIGHT	12
#define W_FILE_LEN				32

#define OPT_SONGLIST	0
#define OPT_FILE_BROWSER	1
#define OPT_EBOOK		2
#define OPT_RADIO		3
#define OPT_SETTING		4
#define OPT_WELCOME		5

/*
* Define code for playing status
*/
#define P_PLAYING 	0
#define P_PAUSEED	1
#define P_STOPPED	2
#define P_PLAY_FROM_BEGIN	3

#define P_PLAY_BAR_BEGIN	15
#define P_PLAY_BAR_END		85
#define P_PLAY_BAR_LENG		(P_PLAY_BAR_END-P_PLAY_BAR_BEGIN)

/*
* Definde code for playing and option
*/
#define VOL_MAX	255
#define VOL_MIN	0
#define VOL_RANGE	(VOL_MAX-VOL_MIN+1)
#define VOL_STAT_NO	4

#define VOL_STAT_0	0
#define VOL_STAT_1	1
#define VOL_STAT_2	2
#define VOL_STAT_3	3
#define VOL_STAT_4	4

#define G_VOL_AREA	105

#define P_REPEAT_ONE	1
#define P_REPEAT_ALL	2
#define P_REPEAT_NONE	0

#define P_SHUF_ON	1
#define P_SHUF_OFF	0

//define for ebook reading
#define CHAR_PER_LINE			21
#define NUM_LINE 					9

/*
* Button constants
*/
#define BTN_REPEAT	3
#define BTN_HOLD	3

#define PRESSED		1
#define RELEASED	0

/*
* Button read value
*/
// #define UP    !(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)&GPIO_PIN_0)
// #define DOWN  !(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&GPIO_PIN_1)
// #define LEFT  !(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&GPIO_PIN_2)
// #define RIGHT  !(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)&GPIO_PIN_3)

// #define SELECT !(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)&GPIO_PIN_1)

/*
* Time control
*/
#define WELCOME_DELAY	400

#define BTN_DELAY	40


/*
* GLOBAL VARIABLE
*/
extern unsigned char  onWindow;
extern unsigned char flagButton;
extern unsigned char pressedButton;
extern int current_position;
extern int current_book;

/*
* BUTTON VARIABLE
*/
extern unsigned char btnUp;
extern unsigned char btnDown;
extern unsigned char btnLeft;
extern unsigned char btnRight;
extern unsigned char btnSelect;

extern unsigned char btnUpHold;
extern unsigned char btnDownHold;
extern unsigned char btnLeftHold;
extern unsigned char btnRightHold;
extern unsigned char btnSelectHold;

extern unsigned char btnUpPre;
extern unsigned char btnDownPre;
extern unsigned char btnLeftPre;
extern unsigned char btnRightPre;
extern unsigned char btnSelectPre;

extern unsigned char temp;
extern unsigned char btnCount[NUM_OF_BUTTON+1];

//state variables
extern unsigned char playing_state;
extern unsigned char reading_state;





/*
* Window variable
*/
// The window that is active on screen
extern unsigned char  onWindow;

// In option window, this position is being choosen
extern unsigned char optionHighline;

// In setting window
extern unsigned char settingHighline;

// Status for the Repeat options
extern unsigned char sRepeatHighline;

// Status for the Shuffle options
extern unsigned char sShuffleHighline;

/*
* Playing window variable
*/
// Status of playing or pause
extern unsigned char stsPlaying;

// Status of Repeat option
extern unsigned char stsRepeat;

// Status of Auto turn off screen option
extern unsigned char stsAutoOff;


// Status of Shuffle option
extern unsigned char stsShuffle;

// Status of volume indicator on screen
extern unsigned char stsVolume;

// Value of volume, range from MIN to MAX
extern unsigned char volumeValue;

// Flag indicate on changing window or element
extern unsigned char flagOnChangeWindow;
extern unsigned char flagOnChangeElement;
extern unsigned char flagOnChangeHeader;
extern unsigned char flagOnChangeBody;
extern unsigned char flagOnChangeSetting;
extern unsigned char flagOnChangePlayingBar;
extern unsigned char flagOnChangeVolume;
extern unsigned char flagLyricsPrinted;
extern unsigned int random_number;


extern char volume_change;
extern char volume;
/*
* Songlist window variable
*/
// Position of selected song
extern unsigned long songListPos;

// Total size of song list from SD card
extern unsigned long songListSize;

// Position beginning of the list that will be shown on screen
extern unsigned long songListShowBegin;

/*
* Ebook window variable
*/
// Position of selected ebook file
extern unsigned long ebookListPos ;

// Total size of file list from SD card
extern unsigned long ebookListSize ;

// Position beginning of the list that will be shown on screen
extern unsigned long ebookListShowBegin;


/*
* File list window variable
*/
// Position of selected file
extern unsigned char fileListPos;

// Total size of file list from SD card
extern unsigned char fileListSize;

// Position beginning of the list that will be shown on screen
extern unsigned char fileListShowBegin;

// Song variable
extern unsigned char songLenCount;
extern unsigned char songPosCount;

// Variable to track time played.
extern unsigned long minutePlayed;
extern unsigned long secondPlayed;





#endif  // Global.h


