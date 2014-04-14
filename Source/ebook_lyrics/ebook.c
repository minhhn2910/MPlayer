// File name : ebook.c

#include <string.h>
#include "utils/ustdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "drivers/rit128x96x4.h"
#include "driverlib/ssi.h"
#include "ebook.h"
#include "peripheral.h"
#include "FileSystem.h"


int flag_song_title = 0;
unsigned char lyric_mm=0;
unsigned char lyric_ss=0;
//int micro_ss;
char lyric_buffer[150];

//ebook_buffer
char ebook_buffer[NUM_LINE][CHAR_PER_LINE+1];


static FIL ebook_file;
static FIL lyric_file;
static FRESULT fresult;

//**************************************************************//
//ebook_open
//Prototype: void ebook_open (char* filename).
//Parameters: 
//- filename: name of ebook file to open.
//Description:
//- use f_open function to open file with filename and save information of that file in FIL ebook_file in order to read data after that.
//- This function must be called fisrt, before use ebook_read_next or ebook_read_pre to read the txt file.
//Returns: None.
//*************************************************************//
void ebook_open(char* filename)
{
	select_CARD();
	fresult = f_open(&ebook_file, filename, FA_READ);
	if(fresult != FR_OK)
	UARTprintf("f_mount error: \n");
	UARTprintf("ebook_open \n");
	deselect_CARD();	
}

//***********************************************************//
//ebook_read_next
//Prototype: void ebook_read_next();
//Description:  this function use to read next page of txt file and save data in ebook_buffer to print it in OLED.  
//Returns: None
//***********************************************************//
void ebook_read_next()
{
	int from = 0, to = 21;
 	char i;
	char j, k;
	char temp_buffer[151];
 	WORD usBytesRead;
	select_CARD();
	fresult = f_read(&ebook_file, temp_buffer, 149, &usBytesRead);	
	//UARTprintf("f_read error: %s\n", StringFromFresult(fresult));
	UARTprintf("%s \n", temp_buffer);
	for(i=0;i<NUM_LINE;i++){
		if (temp_buffer[to] != ' ') {
			for (to=from+20; to>=from; to--){
				//UARTprintf("%c \n", temp_buffer[to]);
				if (temp_buffer[to] == 0x20) break;
			}		
		}
	//	UARTprintf("to = %d from = %d i = %d\n", to, from, i );
		k=0;
		for (j=from; j<= (to); j++){
			ebook_buffer[i][k] = temp_buffer[j];
			k++;
			//UARTprintf("%c \n", ebook_buffer[i][j]);
		} 
		ebook_buffer[i][k] = 0x00;
		from = to+1 ;
		to = from + 20;
		//UARTprintf("%s \n", ebook_buffer[i]);
	}
	/*char i;
	WORD usBytesRead;
	select_CARD();
	for(i=0;i<NUM_LINE;i++)
	{
		fresult = f_read(&ebook_file, ebook_buffer[i],CHAR_PER_LINE,&usBytesRead);
		ebook_buffer[i][usBytesRead]=0x00;
	}*/
	deselect_CARD();
}

//***********************************************//
//ebook_read_pre
//Prototype: void ebook_read_pre();
//Description:  this function use to read previous page of txt file and save data in ebook_buffer to print it in OLED.  
//Returns: None
//***********************************************//
void ebook_read_pre()
{
	select_CARD();
	if(ebook_file.fptr > 150*2)
	fresult = f_lseek(&ebook_file, ebook_file.fptr - 2*150);
	else
	fresult = f_lseek(&ebook_file, 0);
	deselect_CARD();
	ebook_read_next();
}
////////////////////////////////////////////////////////////////


//*************************************************//
//lyric_open
//Prototype: void lyric_open (char* filename).
//Parameters: 
//- filename: name of lyric file to open.
//Description:
//- use f_open function to open file with filename and save information of that file in FIL  lyric_file in order to read data after that.
//- This function must be called fisrt, before use lyric_read_next to read the lrc file.
//Returns: None.
//*************************************************//
unsigned char lyric_open(char* mp3filename)
{
	char filename[FILE_NAME_FORMAT+1];
//	char temp [1];
//	char flag_ok = 0;
//		unsigned short usBytesRead;
	
	flag_song_title = 0;
	strcpy(filename,mp3filename);
	filename[FILE_NAME_FORMAT-3] = 'L'; 
	filename[FILE_NAME_FORMAT-2] = 'R';
	filename[FILE_NAME_FORMAT-1] = 'C';
	UARTprintf("lyrics file name: %s \n",filename);
	select_CARD();
	fresult = f_open(&lyric_file, filename, FA_READ);
	if(fresult != FR_OK)
	{
			filename[FILE_NAME_FORMAT-3] = 'l'; 
			filename[FILE_NAME_FORMAT-2] = 'r';
			filename[FILE_NAME_FORMAT-1] = 'c';
			UARTprintf("lyrics file name: %s \n",filename);
			fresult = f_open(&lyric_file, filename, FA_READ);
			if(fresult != FR_OK)
				return 0;
		
	}

	UARTprintf("lyric_opened \n");
	
	
	
	
	deselect_CARD();	
	
	return 1;
}

//**************************************************//
//lyric_read_next
//Prototype: void lyric_read_next();
//Description:  this function use to read next line of lrc file and save data in lyric_buffer to print it in OLED.  
//Returns: None.
//**************************************************//
void lyric_read_next()
{
	int i=0;
	//int k,j = 0;
	int time_pointer = 0;
	int flag_time_detect = 0;
	FRESULT fresult;
	unsigned short usBytesRead;
	char str[2];
	char time_detect[9];
	//char strDisplay[150];
	//int mm = 0;
	//int ss = 0;
	//int micro_ss = 0;
	
	str[1] = '\0';		
	
//	for(j = 0;j < 5; j++)
	usprintf(lyric_buffer, "");
	usprintf(time_detect, "");
	flag_time_detect = 0;
	time_pointer = 0;
	select_CARD();
			// Read the first title of this song 
			if (flag_song_title == 0){
					flag_song_title = 1; 
					do {
						fresult = f_read(&lyric_file, str, 1, &usBytesRead);
						strcat(lyric_buffer,str);	
					}while (str[0] != '\n' && fresult == FR_OK && usBytesRead == 1 );
					//for(j = 0;j < 5; j++)
					usprintf(lyric_buffer, "");
					lyric_mm=0;
					lyric_ss=0;
				}
				// Read content of lyric each sentences...
			else
			{
				do {
					fresult = f_read(&lyric_file, str, 1, &usBytesRead);
					if ((flag_time_detect) && (str[0] != ']') && (time_pointer <= 7)){
						strcat(time_detect, str);
						time_pointer++;
					}
					if (str[0] == '[') flag_time_detect = 1;
					
					// filter content of lyric without time
					if (time_pointer >= 10){
						strcat(lyric_buffer, str);
						i++;
					}
					if ((time_pointer >= 8) && (time_pointer < 10))
						if (time_pointer >= 8)
							time_pointer++;
				}while (str[0] != '\n' && fresult == FR_OK && usBytesRead == 1);	
				if (usBytesRead < 1) return;
			// check time terminated to display this lyric comment or not?
				lyric_mm = (time_detect[0] - 48)*10 + (time_detect[1] - 48);
				lyric_ss = (time_detect[3] - 48)*10 + (time_detect[4] - 48);
			}

	deselect_CARD();	
	//micro_ss = (time_detect[6] - 48)*10 +(time_detect[7] - 48);		
	UARTprintf("mm = %d\n", lyric_mm);
	UARTprintf("ss = %d\n", lyric_ss);
//	UARTprintf("micro_ss = %d\n", micro_ss);
	UARTprintf("data = %s\n", lyric_buffer);
}

