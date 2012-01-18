/******************************************************************************                                                                                                                
* file.h
*
* constants and functions used by the file system. descriptions of each                                                                                                                                                                                                       
* function can be found in file.c
*
* Name: Cha Li
* Date: 5 January 2012
*
* Development
* OS: Ubuntu 11.04 x64
* Compiler: gcc 4.5.2 (default settings)
* Kernel: 2.6.38-13-generic
******************************************************************************/


#ifndef FILESYS_H
#define FILESYS_H

#include <stdlib.h>
#include <stdint.h>
#include "page.h"

#define MAX_FILES 2
#define NAME_LEN  15


//NOTE: file flags layout
//  MSB| 7 6 5 4 3 2 1 0 |
//  7:    read mode
//  6:    write mode
//  5:    append mode
//  4-0:  unused

//file flags
//modes
#define FILE_READ   0x80
#define FILE_WRITE  0x40
#define FILE_APPEND 0x20
//unused
#define FILE_NULL1  0x10
#define FILE_NULL2  0x08
#define FILE_NULL3  0x04
#define FILE_NULL4  0x02
#define FILE_NULL5  0x01

#define MODE_MASK   0xE0


//some offset values for the file header page
#define FILESIZE_OFFSET 19


typedef struct{
  page_t *page;     //file header page
  char name[16];    //file name
  uint8_t flags;    //properties of this file
  uint32_t size;    //size of the file
  uint32_t offset;  //current location of reads

} file_t; //23 bytes

void filesys_init(void);
uint8_t filesys_quick_format(void);
uint8_t filesys_format(void);

#ifdef FILESYS_DEBUG
void file_print(file_t *);
#endif

file_t *file_open(char *name, char mode);
void file_close(file_t *handle);
uint8_t file_exists(char *name);

/* not implemented
uint8_t file_remove(char *name);
uint8_t file_rename(char *old_name, char *new_name);
*/

uint32_t file_write(file_t *handle, char *buf, uint32_t length);
uint32_t file_writeline(file_t *handle, char *buf, uint32_t length);
uint32_t file_read(file_t *handle, char *buf, uint32_t length);
uint32_t file_readline(file_t *handle, char *buf, uint32_t length);

uint8_t file_seek(file_t *handle, uint32_t origin, uint32_t offset);
int32_t file_tell(file_t *handle);
void file_rewind(file_t *handle);
#endif

