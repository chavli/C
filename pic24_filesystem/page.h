/******************************************************************************                                                                                                                
* page.h
*
* constants and functions used by the page system. descriptions of each 
* function can be found in page.c
*
* Name: Cha Li
* Date: 5 January 2012
*
* Development
* OS: Ubuntu 11.04 x64
* Compiler: gcc 4.5.2 (default settings)
* Kernel: 2.6.38-13-generic
******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "eeprom.h"

#ifndef PAGESYS_H
#define PAGESYS_H

#define MAX_OPEN_PAGES 5  
#define NUM_PAGES 512

//number of bytes the data portion of a page is offset from the start 
//of a page. -> avoid overwriting headers with data
#define FILE_HEADER_LENGTH  22
#define HEADER_LENGTH 2

//number of bytes used by various things
#define FLAG_LENGTH 1
#define FILESIZE_LEN 4
#define PAGE_ID_LEN 2

//byte index with a page that contains the flags
#define PAGE_FLAGS 0

//NOTE: PAGE FLAGS LAYOUT, used for all pages
//  MSB| 7 6 5 4 3 2 1 0 |
//  7-5:  page type
//  4:    page is open
//  3:    page is full
//  2:    write-lock
//  1-0:  unused  

//page flags
#define PAGE_OPEN 0x10  //page is open
#define PAGE_FULL 0x08  //page is full
#define PAGE_WRLK 0x04  //no writes allowed
#define PAGE_NULL2 0x02 //unused
#define PAGE_NULL3 0x01

#define PAGE_TYPE_MASK  0xE0

//the number of bytes a single data page has to store data (254B)
#define DIRECT_SIZE (PAGE_EDGE - HEADER_LENGTH) 
//max number of bytes a file can store in direct pages before having to use
//it's indirect page. (~28.5 KB)
#define MAX_SIZE_DIRECT 29210 


//the number of bytes an indirect page has to store data
#define INDIRECT_SIZE 32258
//max number of bytes a file can store before having to use it's double 
//indirect page. ( ~60.5 KB )
#define MAX_SIZE_INDIRECT (INDIRECT_SIZE + MAX_SIZE_DIRECT)


//the number of bytes a dbl ind page has to store data
#define DBL_IND_SIZE  4096766
//max number of bytes a single file can store ( ~3.97 MB )
#define MAX_FILE_SIZE (DBL_IND_SIZE + MAX_SIZE_INDIRECT)

//Page 0:
//This page is used to manage other pages. not accessible to users
//  MSB| { 1B flags } { 191B unused } { 64B freemap (bitmap) } | 
//
//  [byte]    [description]
//  255:      page flags
//  254-64:   unused
//  63-0:     bitmap representing used(0) and free(1) pages
//

//NOTE: Page headers by type. page size = 256 bytes(B).
//
//file header page (first page of a file)
//  MSB| { 22B header } { 238B data } |
//
//  [byte]    [description]
//  255:      page flags
//  254-239:  16B filename including null terminator  
//  238:      unused
//  237-234   4B file length
//  233-232:  ID of dbl indirect page
//  231-230:  ID of indirect page
//  229-0:    IDs of data pages (2 bytes each)
//
//double indirect and indirect pages
//  MSB|  { 2B header } { 254B data } |
//
//  [byte]    [description]
//  255:      page flags
//  254:      unused
//  253-0:    IDs of indirect and data pages (respectively)
//
//data page
//  MSB|  { 2B header } { 254B data } |
//
//  [byte]    [description]
//  255:      page flags
//  254:      offset of end of data (data is written left to right)
//  253-0:    binary data
//

//A note about data page ID layout:
//  data page ids are added right to left in the file header page. this means
//  the id of the first page allocated to a file is stored in the two(2) LSB
//  of the page. Subsequent page ids are added in "front" of previously 
//  added ids. When the location of the indirect page id is reached, an 
//  indirect page is allocated and its ID recorded. Additional data page ids
//  are written to the indirect page in the same manner (left to right).
//  The same procedure is used when working with the dbl indirect page. 
//  Additionally, the IDS themselves are written backwards.
//
//  Example:
//  allocated page 4 (0x400)
//  allocated page 5 (0x500)
//  allocated page 6 (0x600)                                                                                                                                                                                                                                                      
//  allocated page 7 (0x700)
//  allocated page 8 (0x800)
//
//
//  file header (hex):
//  ... 08 00 07 00 06 00 05 00 04 00|
//

//microchip 25LC1024 eeprom chip
typedef struct mc25lc_page{
  uint32_t address; //start address of this page
  uint16_t id;      //id * 0x100 = address
  uint8_t type;     //page type
} page_t;           //7 bytes

enum PAGE_TYPE{
  NONE_P = 0x00,    //no type, uninitialized
  FILE_P = 0x20,    //a file header
  DATA_P = 0x40,    //contains raw data
  IND_P  = 0x60,    //page pointer (page of page ids)
  DBL_IND_P = 0x80  //double page pointer (page of pages of page ids)
};

//functions that operate on the whole page system
void pagesys_init(void);
uint8_t pagesys_qwipe(void);
uint8_t pagesys_wipe(void);
uint16_t pagesys_exists(uint8_t *name);

#ifdef FILESYS_DEBUG
void pagesys_print(void);
#endif

//functions that operate on individual pages
page_t *page_create(uint8_t type);
page_t *page_open(uint16_t id);
void page_close(page_t *page_entry);
uint16_t page_read(page_t *page_entry, uint16_t offset, uint8_t *data, uint16_t length);
uint16_t page_write(page_t *page_entry, uint16_t offset, uint8_t *data, uint16_t length);
uint16_t page_erase(page_t *page_entry, uint16_t offset, uint16_t length);

#endif

