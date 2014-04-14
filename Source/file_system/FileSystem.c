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
#include "Global.h"
#include "peripheral.h"
#include "FileSystem.h"
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

//list file
char mp3_file_list[MAX_MP3][FILE_NAME_FORMAT+1];
unsigned long ulMp3FileCount;
char txt_file_list[MAX_TXT][FILE_NAME_FORMAT+1];
unsigned long ulTxtFileCount;
char dir_list[MAX_DIR][DIR_NAME_FORMAT+1];
unsigned long ulDirCount;

static DIR g_sDirObject;
static FATFS g_sFatFs;
static FRESULT fresult;
static FILINFO g_sFileInfo;

// ***************************************
// init_CARD
// Prototype: void init_CARD()
// Description: init SD card module, mount it to system.
// Returns: None.
// ***************************************
void init_CARD()
{
	FRESULT fresult;
	SysTickPeriodSet(SysCtlClockGet() / 100);
	SysTickEnable();
	SysTickIntEnable();

	//
	// Enable Interrupts
	//
	IntMasterEnable();
	
	select_CARD();
	while(1)
	{
		fresult = f_mount(0, &g_sFatFs);
		if(fresult != FR_OK)
		UARTprintf("f_mount error:\n");
		else
		break;
	}
	GetListFileName();
	deselect_CARD();
}

//*******************************************************//
//GetListFileName
//Prototype: void GetListFileName()
//Description: Read all file information in SD_Card and sort them to mp3_file_list or txt_file_list, this is a part of middle ware for sdcard, must be called in initial routine.
//Returns: None.
//*******************************************************//
void GetListFileName()
{
	const char* sub_a;
	const char* sub_b = ".mp3";
	const char* sub_c = ".MP3";
	const char* sub_d = ".txt";
	const char* sub_e = ".TXT";
	int lenght;
	fresult = f_opendir(&g_sDirObject, g_cCwdBuf);
	if(fresult != FR_OK)
	{
		return;
	}
	ulMp3FileCount = 0;
	ulTxtFileCount = 0;
	ulDirCount = 0;
	for(;;)
	{
		fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

		if(fresult != FR_OK)
		{
			return;
		}

		if(!g_sFileInfo.fname[0])
		{
			break;
		}

		if(g_sFileInfo.fattrib & AM_DIR)
		{
			strcpy(dir_list[ulDirCount],g_sFileInfo.fname);
			ulDirCount++;						
			if(ulDirCount==MAX_DIR)
			ulDirCount = 0;
		}
		else
		{
			lenght = strlen(g_sFileInfo.fname);
			sub_a = &g_sFileInfo.fname[lenght-4];
			if ((strcmp(sub_a, sub_b) == 0)||(strcmp(sub_a, sub_c) == 0))
			{							
				strcpy(mp3_file_list[ulMp3FileCount],g_sFileInfo.fname);			
				ulMp3FileCount ++;							
				if(ulMp3FileCount == MAX_MP3)
				ulMp3FileCount = 0;	
			}
			else if ((strcmp(sub_a, sub_d) == 0)||(strcmp(sub_a, sub_e) == 0))
			{							
				strcpy(txt_file_list[ulTxtFileCount],g_sFileInfo.fname);			
				ulTxtFileCount ++;							
				if(ulTxtFileCount == MAX_TXT)
				ulTxtFileCount = 0;	
			}	          
		}
		
	}   

}


