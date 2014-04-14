#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "drivers/rit128x96x4.h"
#include "Windows.h"
#include "global.h"
//#include "grlib/grlib.h"
//#include "grlib/widget.h"
#include "images.h"
#include "utils/uartstdio.h"
#include "ebook.h"
#include "FileSystem.h"


unsigned char  onWindow;

unsigned char optionHighline;

unsigned char settingHighline;
unsigned char sRepeatHighline;
unsigned char sShuffleHighline;
char onPlaySongName[100];

/*
* Songlist window variable
*/
// Position of selected song
unsigned long songListPos = 0;

// Total size of song list from SD card
unsigned long songListSize = 0;

// Position beginning of the list that will be shown on screen
unsigned long songListShowBegin = 0;

/*
* Ebook window variable
*/
// Position of selected ebook file
unsigned long ebookListPos = 0;

// Total size of file list from SD card
unsigned long ebookListSize = 0;

// Position beginning of the list that will be shown on screen
unsigned long ebookListShowBegin = 0;


/*
* File list window variable
*/
// Position of selected file
unsigned char fileListPos= 0;

// Total size of file list from SD card
unsigned char fileListSize= 0;

// Position beginning of the list that will be shown on screen
unsigned char fileListShowBegin = 0;

// Current folder path
char* folderPath;

//welcome stuff

// ********************************************************
// draw_welcome_playing
// Prototype: void draw_welcome_playing(void);
// Parameters: 
// Global variable:
// pMusic[] : Array data for playing welcome window.
// Description: Draw the playing welcome window.
// Returns: None.
// ********************************************************
void		draw_welcome_playing()
{
//	RIT128x96x4Clear();
//	RIT128x96x4StringDraw("PLAY MUSIC", 20,40,15);
		// Clear screen before refresh to new window
	RIT128x96x4Clear();
	RIT128x96x4ImageDraw(pMusic,0,0,128,96);
}

// **********************************************************
// draw_welcome_setting
// Prototype: void draw_welcome_setting(void);
// Parameters: 
// Global variable:
// pMusic[] : Array data for setting welcome window.
// Description: Draw the setting welcome window.
// Returns: None.
// **********************************************************
void		draw_welcome_setting()
{
// 	RIT128x96x4Clear();
// 	RIT128x96x4StringDraw("SETTING", 20,40,15);
		RIT128x96x4Clear();
		RIT128x96x4ImageDraw(pSetting,0,0,128,96);
}

// *************************************************************
// draw_welcome_ebook
// Prototype: void draw_welcome_ebook(void);
// Parameters: 
// Global variable:
// pMusic[] : Array data for ebook welcome window.
// Description: Draw the ebook welcome window.
// Returns: None.
// *************************************************************
void		draw_welcome_ebook()
{
// 	RIT128x96x4Clear();
// 	RIT128x96x4StringDraw("READ EBOOK", 20,40,15);
		RIT128x96x4Clear();
		RIT128x96x4ImageDraw(pEbook,0,0,128,96);
}

// *****************************************************************
// draw_welcome_radio_internet
// Prototype: void draw_welcome_radio_internet(void);
// Parameters: 
// Global variable:
// pMusic[] : Array data for radio welcome window.
// Description: Draw the radio welcome window.
// Returns: None.
// *****************************************************************
void		draw_welcome_radio_internet()
{
// 	RIT128x96x4Clear();
// 	RIT128x96x4StringDraw("READ EBOOK", 20,40,15);
		RIT128x96x4Clear();
		RIT128x96x4ImageDraw(pRadio,0,0,128,96);
}


// Option list
char optionList[NUM_OF_OPTION][LEN_OPTION]= 
{
	"Playlist",
	"File Browser",
	"Setting",
	"Ebook Reader",
	"Information"
};

// Setting list
char settingList[NUM_OF_SETTING][LEN_OPTION]= 
{
	"Repeat",
	"Shuffle",
	"Auto Off",
	"About Us"
};


char SongList[11][LEN_OPTION] = 
{
	"1. Love the way you lie",
	"2. We are young",
	"3. Circus",
	"4. Mirrors",
	"5. 22",
	"6. Say it to the world",
	"7. Love you love me",
	"8. Not afraid",
	"9. 25 minutes",
	"10. Heal the world",
	"11. Mocking bird"
};

char fileList[10][LEN_OPTION] = 
{
	"1a. Love the way you lie",
	"2a. We are young",
	"3a. Circus",
	"4a. Mirrors",
	"5a. 22",
	"6a. Say it to the world",
	"7a. Love you love me",
	"8a. Not afraid",
	"9a. 25 minutes",
	"10a. Heal the world"
};



//*****************************************************//
// welcomeWindow
// Prototype:  void welcomeWindow(void);
// Parameters: 
// Global variable:
// G_pucBackground[] : array data for the welcome picture
// Description: Draw welcome screen
// Returns: None
//*****************************************************//

void welcomeWindow(void){
	UARTprintf("draw welcome \n");
	// Clear screen before refresh to new window
	RIT128x96x4Clear();

	// Load background
	loadBackground();

	// Draw the image
	RIT128x96x4ImageDraw(g_pucBackground,0,0,128,96);
}

// ******************************************************
// playingWindow
// Prototype: void playingWindow(void);
// Parameters: None
// Description: Draw the full playing window.
// Returns: None.
// ******************************************************
void playingWindow(void){
	UARTprintf("draw playing \n");
	// Clear screen before refresh to new window
	RIT128x96x4Clear();

	// Load background
	loadBackground();

	// Just for you to know it playing.
	//RIT128x96x4StringDraw("...Playing...", 0,0,15);

	// Draw the header
	updatePlayingHeader();

	// Draw the body
	updatePlayingBody();
	
}

void optionsWindow(void){
	UARTprintf("draw Option \n");
	// Clear screen before refresh to new window
	RIT128x96x4Clear();

	// Load background
	loadBackground();
	
	// Draw the header
	updateOptionHeader();

	// Draw the body
	updateOptionsList();


	// Just show to know
	RIT128x96x4StringDraw("Options", 0,0,15);
	
}

void songListWindow(void){
	UARTprintf("draw songlist \n");
	RIT128x96x4Clear();

	// Load background
	loadBackground();

	// Just for testing
	//RIT128x96x4StringDraw("SongsList!!!", 0,0,15);

	// Get the songslist first

	//Draw the header
	updateSonglistHeader();
	

	// Show the songs list
	updateSongList();
}

void fileBrowserWindow(void){
	
	RIT128x96x4Clear();

	// Load background
	loadBackground();

	// Just for testing
	RIT128x96x4StringDraw("File Browser!!!", 0,0,15);

	// Get the file list

	// Draw the header
	updateFileHeader();

	// Show the file list
	updateFileList	();
}

// ***********************************************************
// settingWindow
// Prototype: void settingWindow(void);
// Parameters:  None
// Description: Draw the setting window.
// Returns: None.
// ***********************************************************
void settingWindow(void){

	RIT128x96x4Clear();

	// Load background
	loadBackground();

	// Just so you know :D
	RIT128x96x4StringDraw("Setting!!!", 0,0,15);

	// Get the setting list

	// Draw the header
	updateSettingHeader();

	// Draw the setting list
	updateSettingList();
	updateSettingState();
	
}



// *************************************************
// ebookWindow
// Prototype: void ebookWindow(void);
// Parameters: None
// Description: Draw the Ebook reader window.
// Returns: None
// *************************************************
void ebookWindow(void){
	RIT128x96x4Clear();
	// Load background
	loadBackground();

	// Draw ebook header
	updateEbookHeader();

	// Draw ebook list in body:
	updateEbookBody();
}

// ********************************************
// radioWindow
// Prototype: void radioWindow(void);
// Parameters: None
// Description: Draw Radio – Internet window
// Returns: None.
// ********************************************
void radioWindow(void){
	RIT128x96x4Clear();
	// Load background
	loadBackground();

	// Draw radio header
	updateRadioHeader();
	
	RIT128x96x4StringDraw("Press select", 20,40,15);
	RIT128x96x4StringDraw("Server: 192.168.1.99", 5,50,15);
	RIT128x96x4StringDraw("<< EBOOK --- PLAY >>", 4,86,15);
}


// This is optional back ground color for all window
void loadBackground(void){
	RIT128x96x4StringDraw(lyric_buffer, 4,86,15);
}

// *********************************************************
// updatePlayingHeader
// Prototype: void updatePlayingHeader(void);
// Parameters: 
// Global variable:
// pHeader24[]: Array data for header background.
// playing_state: current playing status (Play, pause, stop,…)
// pPlay[] & pPause[] : Array data for play or pause state indicator.
// Mp3_file_list[] : Array of song’s file name
// Description: Show the header of playing window: song name, playing status, time, volume indicator.
// Returns: None.
// *********************************************************
void updatePlayingHeader(void){

	// Draw the header background
	RIT128x96x4ImageDraw(pHeader24, 0, 0, 128, 24);

	// Draw the playing/pause button
	if (playing_state == PLAY) {
		RIT128x96x4ImageDraw(pPlay10, 1, 13, 10, 10);
	}
	else {
		RIT128x96x4ImageDraw(pPause10, 1, 13, 10, 10);
	}


	updateVolumeArea();

	// Draw the song name
	RIT128x96x4StringDraw(mp3_file_list[current_position], 4, 3,15);
	//need to improve		
	
	drawPlayingStateBarFromBegining(0);

	updatePlayingTime();
}

// *********************************************************
// updatePlayingBody
// Prototype: void updatePlayingBody(void);
// Parameters: 
// Global variable:
// ulMp3FileCount : number of mp3 files found in SD card
// pBody[] : Array data for body background image.
// pHighline12[] : Array data for highlighted song background.
// songListPos : the current position of the highlighted song in song list.
// Mp3_file_list[]:Array of song’s file name
// Description:  Update playing body that show the list of song in SD card
// Returns: None.
// *********************************************************
void updatePlayingBody(void){
	// Show the list of songs file in SD card
	unsigned char i, xPos, yPos, color;
	
	songListSize = ulMp3FileCount;

	// Draw background of body, clear old data.
	RIT128x96x4ImageDraw(pBody72, 0, W_HEADER_HEIGHT, 128, 72);

	
	//songListPos = current_position;
	// Detect the begining of the show list
	//if (songListPos < 4) songListShowBegin = 0;
	//else if (songListPos > songListSize-5) songListShowBegin = songListSize-W_NUM_SONG_SHOW_LIST;
	//else songListShowBegin = songListPos-3;

	// Update the window for show list
	if ((songListPos<songListShowBegin)) 	
	songListShowBegin = songListPos;
	else if (songListPos == songListSize-1 && songListShowBegin==0) 
	songListShowBegin = songListSize-W_NUM_SONG_SHOW_LIST;
	else if ((songListPos>=songListShowBegin+W_NUM_SONG_SHOW_LIST)) 
	songListShowBegin = songListPos-W_NUM_SONG_SHOW_LIST+1;
	else if (songListPos==0 && songListShowBegin == songListSize-W_NUM_SONG_SHOW_LIST)
	songListShowBegin = 0;
	
	for (i = songListShowBegin; i<songListShowBegin+W_NUM_SONG_SHOW_LIST; i++){
		xPos = 0;
		yPos = (W_HEADER_HEIGHT+(i-songListShowBegin)*W_OP_LIST_HEIGHT);

		// Setting color for text
		if (i == songListShowBegin || i== songListShowBegin+8) color = 15;
		else color = 15;
		
		if (i==songListPos){
			//color = 15;
			RIT128x96x4ImageDraw(pHighline12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(mp3_file_list[i], 4, yPos+W_CHAR_PAD_TOP,color);
		}
		else {
			//RIT128x96x4ImageDraw(pBody12, xPos, yPos, 128, 12);
			//color = 13;
			RIT128x96x4StringDraw(mp3_file_list[i], 4, yPos+W_CHAR_PAD_TOP,color);
		}
		
	}
	UARTprintf("**Update playing list \n");
}

void updateLyrics()
{
	UARTprintf("***Update lyrics \n");
	RIT128x96x4StringDraw("                                   ", 0,82,15);
	RIT128x96x4StringDraw(lyric_buffer, 0,82,15);
	flagLyricsPrinted=1;
	
}

// *******************************************************
// updatePlayingTime
// Prototype: void updatePlayingTime(void);
// Parameters: 
// Global variable:
// minutePlayed & secondePlayed : Minute and second of playing song
// Description:  Show the playing time in the header area.
// Returns: None.
// *******************************************************
void updatePlayingTime(void){
	char time[6];
	// Draw background
	RIT128x96x4ImageDraw(pBody72, X_PLAYING_TIME, Y_PLAYING_TIME, 36, 12);
	
	// Draw the time
	sprintf(time, "%d%d:%d%d",minutePlayed/10, minutePlayed%10, secondPlayed/10, secondPlayed %10);
	RIT128x96x4StringDraw(time, X_PLAYING_TIME+W_CHAR_PAD_TOP, Y_PLAYING_TIME+0, 15);

//	UARTprintf("*#*Update playing time \n");
	
}


void updateOptionHeader(void){
	// Draw the header background
	RIT128x96x4ImageDraw(pHeader24, 0, 0, 128, 24);

	// Draw string of "OPTIONS"
	RIT128x96x4StringDraw("OPTIONS", 5,5,15);
}

void updateOptionsList(void){
	unsigned char i, xPos, yPos;
	for (i = 0; i<NUM_OF_OPTION; i++){
		xPos = 0;
		yPos = (W_HEADER_HEIGHT+i*W_OP_LIST_HEIGHT);
		// If it choosen
		if (i == optionHighline){
			RIT128x96x4ImageDraw(pHighline12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(optionList[i], 5, yPos+W_CHAR_PAD_TOP,15);
		}
		else {
			RIT128x96x4ImageDraw(pBody12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(optionList[i], 5, yPos+W_CHAR_PAD_TOP,15);
		}
	}
	
}

void updateSongList(void){
	unsigned char i, xPos, yPos, color;
	unsigned char yScrollStart, yScrollPos;
	songListSize = 10;
	// Detect the begining of the show list
	if (songListPos < 4) songListShowBegin = 0;
	else if (songListPos > songListSize-5) songListShowBegin = songListSize-8;
	else songListShowBegin = songListPos-3;
	
	for (i = songListShowBegin; i<songListShowBegin+W_NUM_SONG_SHOW_LIST; i++){
		xPos = 0;
		yPos = (0+(i-songListShowBegin)*W_OP_LIST_HEIGHT);

		// Setting color for text
		if (i == songListShowBegin || i== songListShowBegin+8) color = 13;
		else color = 15;
		
		if (i==songListPos){
			//color = 15;
			RIT128x96x4ImageDraw(pHighline12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(SongList[i], 4, yPos+W_CHAR_PAD_TOP,color);
		}
		else {
			//RIT128x96x4ImageDraw(pBody12, xPos, yPos, 128, 12);
			//color = 13;
			RIT128x96x4StringDraw(SongList[i], 4, yPos+W_CHAR_PAD_TOP,color);
		}
	}

	/*/ Draw the scroll bar
	// Still error on display
	if (songListSize > W_NUM_SONG_SHOW_LIST){
		yScrollStart = 0;
		yScrollPos = W_HEIGHT*(songListPos/(songListSize-W_NUM_SONG_SHOW_LIST))+yScrollStart;
		RIT128x96x4ImageDraw(pRoller, 124, yScrollPos, 2, 10);
		} */
	
}

void updateFileList(void){
	unsigned char i, xPos, yPos, color;
	fileListSize = 10;
	// Detect the begining of the show list
	if (fileListPos < 4) fileListShowBegin = 0;
	else if (fileListPos > fileListSize-5) fileListShowBegin = fileListSize-8;
	else fileListShowBegin = fileListPos-3;
	
	for (i = fileListShowBegin; i<fileListShowBegin+W_NUM_SONG_SHOW_LIST; i++){
		xPos = 0;
		yPos = (0+i*W_OP_LIST_HEIGHT);

		// Setting color for text
		if (i == fileListShowBegin || i== fileListShowBegin+8) color = 12;
		else color = 15;
		
		if (i==fileListPos){
			RIT128x96x4ImageDraw(pHighline12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(fileList[i], 5, yPos+W_CHAR_PAD_TOP,color);
		}
		else {
			RIT128x96x4ImageDraw(pBody12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(fileList[i], 5, yPos+W_CHAR_PAD_TOP,color);
		}
	}
}

// *******************************************************
// updateSettingHeader
// Prototype: void updateSettingHeader(void);
// Parameters: 
// Global variable:
// pHeader24[] : Array data for header background.
// Description: Draw the setting header
// Returns: None
// *******************************************************
void updateSettingHeader(void){
	// Draw the header background
	RIT128x96x4ImageDraw(pHeader24, 0, 0, 128, 24);

	// Draw string of "OPTIONS"
	RIT128x96x4StringDraw("SETTING", 5,5,15);
}


// **********************************************************
// updateSettingList
// Prototype: void updateSettingList(void);
// Parameters: 
// Global variable:
// settingList[]: Array of setting name list
// settingHighline: The position of the current setting highlighted.
// Description: Draw and update the setting detail.
// Returns: None.
// **********************************************************
void updateSettingList(void){
	unsigned char i, xPos, yPos;
	for (i = 0; i<NUM_OF_SETTING; i++){
		xPos = 0;
		yPos = (W_HEADER_HEIGHT+i*W_OP_LIST_HEIGHT);
		// If it choosen
		if (i == settingHighline){
			RIT128x96x4ImageDraw(pHighline12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(settingList[i], 4, yPos+W_CHAR_PAD_TOP,15);
		}
		else {
			//RIT128x96x4ImageDraw(pBody12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(settingList[i], 4, yPos+W_CHAR_PAD_TOP,15);
		}
	}
}

// **********************************************************
// updateSettingState
// Prototype: void updateSettingState(void);
// Parameters: 
// stsRepeat: State variable for repeat function.
// stsShuffle: State variable for  shuffle function.
// Description: Update when the user change setting to repeat and shuffle function.
// Returns: None.
// **********************************************************
void updateSettingState(void){
	unsigned char i, xPos, yPos;
	
	char settingString[NUM_OF_SETTING][SETTING_LEN];
	

	switch (stsRepeat){
	case P_REPEAT_NONE:
		sprintf(settingString[STG_REPEAT], "O  ");
		//settingString[STG_REPEAT]= "O";
		break;
	case P_REPEAT_ONE :
		sprintf(settingString[STG_REPEAT],"1  ");
		break;
	case P_REPEAT_ALL :
		sprintf(settingString[STG_REPEAT], "ALL");
		break;
		default : 
		sprintf(settingString[STG_REPEAT], "O  ");
	}

	switch (stsShuffle){
	case P_SHUF_OFF:
		sprintf(settingString[STG_SHUFFLE], "OFF");
		break;
	case P_SHUF_ON : 
		sprintf(settingString[STG_SHUFFLE], "ON ");
		break;
		default : 
		sprintf(settingString[STG_SHUFFLE], "OFF");
	}
	if (stsAutoOff == 1){
		sprintf(settingString[STG_AUTOOFF], "ON ");
	}
	else {
		sprintf(settingString[STG_AUTOOFF], "OFF");
	}

	for (i = 0; i<NUM_OF_SETTING; i++){
		xPos = 0 + W_SETTING_ALIGN+W_CHAR_PAD_LEFT;
		yPos = (W_HEADER_HEIGHT+i*W_OP_LIST_HEIGHT);

		RIT128x96x4StringDraw(settingString[i], xPos, yPos+W_CHAR_PAD_TOP,15);
		
	}
}


void updateFileHeader(void){
	// Draw the header background
	RIT128x96x4ImageDraw(pHeader12, 0, 0, 128, 12);

	// Draw string of folder path
	RIT128x96x4StringDraw("Folder Path", 5,5,15);
}

void updateSonglistHeader(void){
	// In first version, there are no header for songlist window :)	
}

// ***********************************************************
// drawPlayingStateBar
// Prototype: void drawPlayingStateBar(char);
// Parameters: 
// Percent: the percentage of  the song has been played.
// Description: Draw the playing state bar in header. This method draw 1 piece of the bar only.
// Returns: None.
// ***********************************************************
void drawPlayingStateBar(char percent){
	unsigned char xPos, yPos;
	//	UARTprintf("drawing state bar \n");

	// Get the position of the status bar
	xPos = P_PLAY_BAR_BEGIN + percent*P_PLAY_BAR_LENG/100;
	yPos = 17;
	RIT128x96x4ImageDraw(pPiece1x2, xPos, yPos, 2, 2);
}

// ********************************************************
// drawPlayingStateBarFromBegining
// Prototype: void drawPlayingStateBarFromBegining(char);
// Parameters:  None
// Description: Draw the entire playing state bar in header.
// Returns: None.
// ********************************************************
void drawPlayingStateBarFromBegining(char percent){
	unsigned char i, xPos, yPos;
	// Get the position of the status bar
	xPos = P_PLAY_BAR_BEGIN +( percent*P_PLAY_BAR_LENG)/100;
	yPos = 17;

	// Loop to draw from the begining
	for (i=P_PLAY_BAR_BEGIN; i<= xPos; i++){
		RIT128x96x4ImageDraw(pPiece1x2, i, yPos, 2, 2);
	}
}

void fillPlayingStateBar(){
	unsigned char i, xPos, yPos;
	// Get the position of the status bar
	xPos = P_PLAY_BAR_BEGIN + P_PLAY_BAR_LENG;
	yPos = 17;

	// Loop to draw from the begining
	for (i=P_PLAY_BAR_BEGIN; i<= xPos; i++){
		RIT128x96x4ImageDraw(pPiece1x2, i, yPos, 2, 2);
	}
}

// ****************************************************
// clearPlayingStateBar
// Prototype: void clearPlayingStateBar(void);
// Parameters: None
// Description: Clear the playing state bar
// Returns: None.
// ****************************************************
void clearPlayingStateBar(void){
	unsigned char xPos, yPos;
	// Get the position of the status bar
	xPos = P_PLAY_BAR_BEGIN;
	yPos = 17;

	// Loop to draw from the begining
	RIT128x96x4ImageDraw(pClear2x2, xPos, yPos, P_PLAY_BAR_LENG, 2);

}


// *******************************************************
// updateVolumeArea
// Prototype: void updateVolumeArea(void);
// Parameters: 
// Global variable:
// volume: The state of the volume, rank from 0 to 255.
// Description: Update the volume indicator.
// eturns: None.
// *******************************************************
void updateVolumeArea(void){
	stsVolume = (255-volume+30)/(VOL_RANGE/VOL_STAT_NO);
	// Draw back ground for volume area
	RIT128x96x4ImageDraw(p18x12, G_VOL_AREA, 10, 18, 12);

	// Draw volume indicator
	if (stsVolume > VOL_STAT_0){
		RIT128x96x4ImageDraw(pVolume1, G_VOL_AREA+2, 20, 2, 2);
	}
	if (stsVolume > VOL_STAT_1){
		RIT128x96x4ImageDraw(pVolume2, G_VOL_AREA+2+4, 18, 2, 4);
	}
	if (stsVolume > VOL_STAT_2){
		RIT128x96x4ImageDraw(pVolume3, G_VOL_AREA+2+8, 16, 2, 6);
	}
	if (stsVolume >= VOL_STAT_4){
		RIT128x96x4ImageDraw(pVolume4, G_VOL_AREA+2+12, 14, 2, 8);
	}
}

// *****************************************************
// updateEbookHeader
// Prototype: void updateEbookHeader(void);
// Parameters: 
// Global variable:
// pHeader24[]: Array data for header background.
// Description: Draw the header for ebook reader window.
// Returns: None.
// *****************************************************
void updateEbookHeader(void){
	// Draw the header background
	RIT128x96x4ImageDraw(pHeader24, 0, 0, 128, 24);
	
	// Draw string on header
	RIT128x96x4StringDraw("Ebook - Reader", 5,5,15);
}

// ************************************************************
// updateEbookBody
// Prototype: void updateEbookBody(void);
// Parameters: 
// Global variable:
// ulTxtFileCount: Number of .txt file found in SD card.
// Txt_file_list[] : list of .txt file in SD card.
// Description: Draw the list of ebook file in SD card.
// Returns: None.
// ************************************************************
void updateEbookBody(void){
	// Show the list of songs file in SD card
	unsigned char i, xPos, yPos, color;
	
	ebookListSize = ulTxtFileCount;

	
	// Draw background of body, clear old data.
	RIT128x96x4ImageDraw(pBody72, 0, W_HEADER_HEIGHT, 128, 72);

	// Update the window for show list
	if ((ebookListPos<ebookListShowBegin)) 	
	ebookListShowBegin = ebookListPos;
	else if (ebookListPos == ebookListSize-1 && ebookListShowBegin==0) 
	ebookListShowBegin = ebookListSize-W_NUM_SONG_SHOW_LIST;
	else if ((ebookListPos>=ebookListShowBegin+W_NUM_SONG_SHOW_LIST)) 
	ebookListShowBegin = ebookListPos-W_NUM_SONG_SHOW_LIST+1;
	else if (ebookListPos==0 && ebookListShowBegin == ebookListSize-W_NUM_SONG_SHOW_LIST)
	ebookListShowBegin = 0;
	
	for (i = ebookListShowBegin; i<ebookListShowBegin+W_NUM_SONG_SHOW_LIST; i++){
		xPos = 0;
		yPos = (W_HEADER_HEIGHT+(i-ebookListShowBegin)*W_OP_LIST_HEIGHT);

		// Setting color for text
		if (i == ebookListShowBegin || i== ebookListShowBegin+8) color = 15;
		else color = 15;
		
		if (i==ebookListPos){
			//color = 15;
			RIT128x96x4ImageDraw(pHighline12, xPos, yPos, 128, 12);
			RIT128x96x4StringDraw(txt_file_list[i], 4, yPos+W_CHAR_PAD_TOP,color);
		}
		else {
			//RIT128x96x4ImageDraw(pBody12, xPos, yPos, 128, 12);
			//color = 13;
			RIT128x96x4StringDraw(txt_file_list[i], 4, yPos+W_CHAR_PAD_TOP,color);
		}
	}
	UARTprintf("***Update Ebook list \n");

}

// *******************************************************
// print_ebook
// Prototype: void print_ebook(void);
// Parameters: 
// Global variable:
// Txt_file_list[] : list of .txt file in SD card.
// Ebook_buffer[]: buffer for each row of the displaying text.
// Description: Draw the content of txt file on screen.
// Returns: None.
// *******************************************************
void print_ebook(){
	char i;
	RIT128x96x4Clear();
	RIT128x96x4StringDraw(txt_file_list[current_book],20 ,0,15);
	for(i=0;i<NUM_LINE;i++)
	RIT128x96x4StringDraw(ebook_buffer[i], 0,12+8*i,15);
	RIT128x96x4StringDraw("<< prev --- next >>", 5,86,15);
}

// ****************************************************
// updateRadioHeader
// Prototype: void updateRadioHeader(void);
// Parameters: 
// Global variable:
// pHeader24[]: Array data for header background.
// Description: Draw the header for radio internet window.
// Returns: None.
// ****************************************************
void updateRadioHeader(void){
	// Draw the header background
	RIT128x96x4ImageDraw(pHeader24, 0, 0, 128, 24);

	// Draw string on Header
	RIT128x96x4StringDraw("Radio - Internet", 5,5,15);
}



