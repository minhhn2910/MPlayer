#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__
#define PATH_BUF_SIZE   80

#define MAX_MP3 100
//#define MAX_FILE 100
#define MAX_DIR 20
#define MAX_TXT 100
#define FILE_NAME_FORMAT 12
#define DIR_NAME_FORMAT  12

extern char txt_file_list[MAX_TXT][FILE_NAME_FORMAT+1];
extern unsigned long ulTxtFileCount;
extern char mp3_file_list[MAX_MP3][FILE_NAME_FORMAT+1];
extern unsigned long ulMp3FileCount;
extern char dir_list[MAX_DIR][DIR_NAME_FORMAT+1];
extern unsigned long ulDirCount;

extern void GetListFileName(void);
extern void init_CARD(void);

#endif 
