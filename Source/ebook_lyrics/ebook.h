#ifndef	__EBOOK_H__
#define	__EBOOK_H__
#include "Global.h"
/*====================================*/
/* Prototype declaration      		  */
/*====================================*/


//for ebook reader
//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code,
// and a string represenation.  FRESULT codes are returned from the FatFs
// FAT file system driver.
//
//*****************************************************************************

#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))
//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and
// it's name as a string.  This is used for looking up error codes for
// printing to the console.
//
//*****************************************************************************

#define READ_BUF_SIZE   189
#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))
extern unsigned char lyric_mm;
extern unsigned char lyric_ss;

extern char lyric_buffer[150];


extern char ebook_buffer[NUM_LINE][CHAR_PER_LINE+1];

extern void ebook_run(void);
extern const char *StringFromFresult(FRESULT );
void ebook_open(char* filename);
void ebook_read_next(void);
void ebook_read_pre(void);
void lyric_read_next(void);
unsigned char lyric_open(char*);
#endif


/****************END FILE**************/
