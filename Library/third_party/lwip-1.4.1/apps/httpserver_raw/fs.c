/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "lwip/opt.h"
#include "lwip/def.h"
#include "httpd.h"
#include "fs.h"
//#include "fsdata.h"
#include "lmi-fsdata.h"
#include <string.h>
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

/** Set this to 1 to include "fsdata_custom.c" instead of "fsdata.c" for the
 * file system (to prevent changing the file included in CVS) */
#ifndef HTTPD_USE_CUSTOM_FSDATA
#define HTTPD_USE_CUSTOM_FSDATA 0
#endif

#if HTTPD_USE_CUSTOM_FSDATA
#include "fsdata_custom.c"
#else /* HTTPD_USE_CUSTOM_FSDATA */
#include "fsdata.c"
#endif /* HTTPD_USE_CUSTOM_FSDATA */

/*-----------------------------------------------------------------------------------*/

#if LWIP_HTTPD_CUSTOM_FILES
int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);
#if LWIP_HTTPD_FS_ASYNC_READ
u8_t fs_canread_custom(struct fs_file *file);
u8_t fs_wait_read_custom(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
#endif /* LWIP_HTTPD_CUSTOM_FILES */

/*-----------------------------------------------------------------------------------*/
err_t
fs_open(struct fs_file *file, const char *name)
{
  const struct fsdata_file *f;

  if ((file == NULL) || (name == NULL)) {
     return ERR_ARG;
  }

#if LWIP_HTTPD_CUSTOM_FILES
  if (fs_open_custom(file, name)) {
    file->is_custom_file = 1;
    return ERR_OK;
  }
  file->is_custom_file = 0;
#endif /* LWIP_HTTPD_CUSTOM_FILES */

  for (f = FS_ROOT; f != NULL; f = f->next) {
    if (!strcmp(name, (char *)f->name)) {
      file->data = (const char *)f->data;
      file->len = f->len;
      file->index = f->len;
      file->pextension = NULL;
      file->http_header_included = f->http_header_included;
#if HTTPD_PRECALCULATED_CHECKSUM
      file->chksum_count = f->chksum_count;
      file->chksum = f->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
#if LWIP_HTTPD_FILE_STATE
      file->state = fs_state_init(file, name);
#endif /* #if LWIP_HTTPD_FILE_STATE */
      return ERR_OK;
    }
  }
  /* file not found */
  return ERR_VAL;
}

/*-----------------------------------------------------------------------------------*/
void
fs_close(struct fs_file *file)
{
#if LWIP_HTTPD_CUSTOM_FILES
  if (file->is_custom_file) {
    fs_close_custom(file);
  }
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if LWIP_HTTPD_FILE_STATE
  fs_state_free(file, file->state);
#endif /* #if LWIP_HTTPD_FILE_STATE */
  LWIP_UNUSED_ARG(file);
}
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_DYNAMIC_FILE_READ
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_read_async(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg)
#else /* LWIP_HTTPD_FS_ASYNC_READ */
int
fs_read(struct fs_file *file, char *buffer, int count)
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
{
  int read;

  if(file->index == file->len) {
    return FS_READ_EOF;
  }
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
  if (!fs_canread_custom(file)) {
    if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
      return FS_READ_DELAYED;
    }
  }
#else /* LWIP_HTTPD_CUSTOM_FILES */
  LWIP_UNUSED_ARG(callback_fn);
  LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

  read = file->len - file->index;
  if(read > count) {
    read = count;
  }

  MEMCPY(buffer, (file->data + file->index), read);
  file->index += read;

  return(read);
}
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_is_file_ready(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg)
{
  if (file != NULL) {
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
    if (!fs_canread_custom(file)) {
      if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
        return 0;
      }
    }
#else /* LWIP_HTTPD_CUSTOM_FILES */
    LWIP_UNUSED_ARG(callback_fn);
    LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
  }
  return 1;
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
/*-----------------------------------------------------------------------------------*/
int
fs_bytes_left(struct fs_file *file)
{
  return file->len - file->index;
}
//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
static volatile tBoolean g_bFatFsEnabled = false;

//*****************************************************************************
//
// Enable the SSI Port for FatFs usage.
//
//*****************************************************************************
static void
fs_enable(unsigned long ulFrequency)
{
    //
    // Disable the SSI Port.
    //
    SSIDisable(SSI0_BASE);

    //
    // Reconfigure the SSI Port for Fat FS operation.
    //
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, ulFrequency, 8);

    //
    // Eanble the SSI Port.
    //
    SSIEnable(SSI0_BASE);
}
//*****************************************************************************
//
// Write content
//
//*****************************************************************************
int
fs_write(FIL *file, char *buffer, int count)
{		
        unsigned short usBytesRead;
        FRESULT fresult;

        //
        // Ensure that the file system access to the SSI port is active.
        //
        fs_enable(400000);

        //
        // write the data.
        //
        fresult = f_write(file, buffer, count, &usBytesRead);
        if((fresult != FR_OK) || (usBytesRead == 0))
        {
            return(-1);
        }
        return((int)usBytesRead);
}

//*****************************************************************************
//
// Write file
//
//*****************************************************************************
int fs_open_write(char *name, FIL *file){
		FRESULT fresult = FR_OK;
	    //
	    // Check to see if the Fat File System has been enabled.
	    //
		
	    if(g_bFatFsEnabled)
	    {
	        //
	        // Ensure that the file system access to the SSI port is active.
	        //
	        fs_enable(400000);
		
	        //
	        // Attempt to open the file on the Fat File System.
	        //
	        fresult = f_open(file, name, FA_CREATE_ALWAYS | FA_WRITE);
	        if(FR_OK == fresult)
	        {
	            return 1;						
	        }
	    }
			return 0;
}
//*****************************************************************************
//
// Scan dir
//
//*****************************************************************************
int scan_dir(FIL *index){
	FILINFO finfo;
  DIR dirs;
	char tmp[250] ;
	char i = 0;
		
    if (f_opendir(&dirs, "/FSS_Data") == FR_OK) {
       while((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0] ){				 				
            if (finfo.fattrib & AM_ARC) {
                sprintf(tmp, "<tr align=center><td>%d</td><td align=left><a href=\"/FSS_Data/%s\">%s</a></td><td align=right>%d KB</td><td>%d/%d/%d</td><td align=left align=center><a href=\"/delete.cgi?FileName=/FSS_Data/%s\">Delete</a></td>", 
									i++, finfo.fname, finfo.fname, finfo.fsize/1024, 
									finfo.fdate & 31,
									(finfo.fdate >> 5) & 15,												
									(finfo.fdate >> 9) + 1980, 									
										finfo.fname);
							fs_write(index,tmp,strlen(tmp));
            } 
      }
    }
		else {
		}
	return i;
}
//*****************************************************************************
//
// Write file Index.htm
//
//*****************************************************************************

int fs_write_file_index(){
		FIL index;
		char *header = "<html><head><title>File Sharing System</title></head><body><h3 align=left>File Sharing System</h3><table align=left border=0 width=1000><tr bgcolor=#aacc00><th width=5%>No.</th><th width=45%>File Name</th><th width=15%>File Size</th><th width=27%>Date Modified</th><th width=8%>Tools</th></tr>";
		char *footer = "</td></table><form action=/upload.cgi method=POST enctype=\"multipart/form-data\" name=fileupload><p><input name=\"File\" type=\"file\" size=\"50\" maxlength=\"1000\"><input type=button value=\"Upload\" OnClick=\"this.form.submit()\"></p></body></html>";//<meta http-equiv=\"refresh\" content=\"5; url:/index.htm\"/>
		
		fs_open_write("index.htm",&index);		
		fs_write(&index,header,strlen(header));
		scan_dir(&index);		
		fs_write(&index,footer,strlen(footer));		
		f_close(&index);
		return 1;
		
}
//*****************************************************************************
//
// Delete file
//
//*****************************************************************************
int fs_delete_file(char *name){
	fs_enable(400000);
	if (f_unlink(name) == FR_OK)	{
		//fs_write_file_index();
		return 1;
	}
	return 0;
}
//*****************************************************************************
//
// Initialize the file system.
//
//*****************************************************************************
void
fs_init(void)
{
    FRESULT fresult;
    DIR g_sDirObject;

    //
    // Initialze the flag to indicate that we are disabled.
    //
    g_bFatFsEnabled = false;

    //
    // Initialize and mount the Fat File System.
    //
    fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK)
    {
        return;
    }

    //
    // Open the root directory for access.
    //
    fresult = f_opendir(&g_sDirObject, "/");

    //
    // Flag and display which file system we are using.
    //
//    RIT128x96x4Enable(1000000);
 //   RIT128x96x4StringDraw("Web Server Using", 18, 48, 15);
    if(fresult == FR_OK)
    {
        //
        // Indicate and display that we are using the SD file system.
        //
        g_bFatFsEnabled = true;
 //       RIT128x96x4StringDraw("SD File System", 24, 56, 15);
    }
    else
    {
        //
        // Indicate and display that we are using the internal file system.
        //
        g_bFatFsEnabled = false;
 //       RIT128x96x4StringDraw("Internal File System", 6, 56, 15);
    }
  //  RIT128x96x4Disable();	
		fs_write_file_index();
}

//*****************************************************************************
// File System tick handler.
//*****************************************************************************
void
fs_tick(unsigned long ulTickMS)
{
    static unsigned long ulTickCounter = 0;

    //
    // Check if the file system has been enabled yet.
    //
    if(!g_bFatFsEnabled)
    {
        return;
    }
		
    //
    // Increment the tick counter.
    //
    ulTickCounter += ulTickMS;

    //
    // Check to see if the FAT FS tick needs to run.
    //
    if(ulTickCounter >= 10)
    {
        ulTickCounter = 0;
        disk_timerproc();
    }
}