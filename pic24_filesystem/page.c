/******************************************************************************                                                                                                                
* page.c
*
* The page system that abstracts and manages the inherent page properties of 
* the EEPROM chip. The functions in this file operate of a single page and 
* do not cross page boundries. Individual pages are accessed by using IDs that
* range from [0, MAX_PAGES). The address of the start of the page is easily 
* calculated by multiplying the ID by PAGE_EDGE (0x100).
*
* In this case the size of a single page, which is set by the hardware, is 
* 256 bytes. The total size of the EEPROM chip is 1Mbit (128KB), this means
* the total number of pages is 512. 64 bytes are required to represent 
* available and used pages as a bitmap.
*
* Page 0:
* Page 0 (id = 0) is a reserved page used by the page system to manage the
* state of avaialble pages. The 64 byte bitmap is stored in the last 64 bytes
* of Page 0. One(1) byte is currently used for flags. Page 0 should never be 
* accessed by the user since corrupting it corrupts the entire system. 
* Additionally, functions in this file prevent access to page 0. Page 0 is 
* only initialized one time.
*
* An artificial limit is placed on the number of pages that can be opened at
* any given time. This is so the memory used to represent pages is bounded.
* Pages should be closed as soon as they are unneeded. The limit is currently
* five(5) but can be changed in page.h
*
* The page system supports four(4) types of pages:
*   FILE_P    - a page that represents a file header
*   DATA_P    - a page that holds raw data
*   IND_P     - a page that contains ids to DATA_P's
*   DBL_IND_P - a page that contains id to IND_P's
* 
* Additional details about the layout of pages can be found in page.h
*
* Properties of the 25LC1024 EEPROM chip:
*   Total size: 1Mb (128KB)
*   Page size: 256 bytes
*   Total pages: 512
*   data sheet: ww1.microchip.com/downloads/en/devicedoc/21836a.pdf 
*
* Name: Cha Li
* Date: 5 January 2012
*
* Development
* OS: Ubuntu 11.04 x64
* Compiler: gcc 4.5.2 (default settings)
* Kernel: 2.6.38-13-generic
******************************************************************************/

#include <stdio.h>
#include "page.h"
#include "headers/util.h"


#define PAGE_0 0x0
#define PS_INIT_FLAG 0x80


static uint8_t buf[PAGE_EDGE];      //used to read in pages
static uint16_t pages_opened = 0;   //keep track of pages currently open

/*
  pagesys_init

  Initializes the page system. This function only does something the first time
  it is called on a fresh EEPROM chip. If no page system exists (page 0 is 
  missing), this function will create Page 0 and write it back to EEPROM.
  The 64 LSB contain the bitmap used to represent free and used pages, a 1
  represents a free page while a 0 represents a used page. 64 bytes are 
  needed to represent 512 pages with page 0 being the least sig bit. This means
  right bit shifting can be used to traverse pages. Page 0 is automatically
  set to used since it should never be used for anything else. The initialized
  flag is also set. Subsequent calls to this function once the page system is
  setup will do nothing.

  arguments:
    none

  returns:
    nothing
*/
void pagesys_init(){
  eeprom_read_page(PAGE_0, buf, PAGE_EDGE);

  //check if page zero has been initialized already
  //NOTE: disk size hard coded at 128KB = 512 * 256B pages
  //512 bit bitmap = 64 bytes
  if(!(buf[PAGE_FLAGS] >> 7)){
    //page zero is never free
    buf[PAGE_EDGE - 1] = 0xFE;
    
    uint8_t i = 2;
    for(; i <= 64 ; ++i)
      buf[PAGE_EDGE - i] = 0xFF;
    buf[PAGE_FLAGS] |= PS_INIT_FLAG;
    eeprom_write_page(PAGE_0, buf, PAGE_EDGE);

    //signature
    eeprom_write(PAGE_0 + 16, "created by Cha Li 2012 -- chavli@gmail.com", 42);
  }
}


/*
  pagesys_qwipe

  performs a quick wipe of the page system. all bits in the bitmap of page 0
  are set to free (except page 0). The contents of the EEPROM chip are not 
  zeroed out. this means a dump of a quick wiped page system will still contain
  old data. This requires fewer writes to EEPROM than pagesys_wipe. The page
  system can't be wiped if there are open pages.

  arguments:
    none

  returns:
    non-zero (true) on success, zero (false) on failure
*/
uint8_t pagesys_qwipe(){
  if( pages_opened == 0 ){
    eeprom_read_page(PAGE_0, buf, PAGE_EDGE);
    //page zero is never free
    buf[PAGE_EDGE - 1] = 0xFE;
    uint8_t i = 2;
    
    //reset all the other bits in the bitmap
    for(; i <= 64 ; ++i)
      buf[PAGE_EDGE - i] = 0xFF;
    eeprom_write_page(PAGE_0, buf, PAGE_EDGE);
    return 1;
  }
  return 0;
}


/*
  pagesys_wipe

  a complete wipe of the EEPROM chip. qwipe is called to reset page 0 and then
  all other pages are zeroed out. The page system can't be wiped if there are
  open pages.

  arguments: 
    none

  returns:
    non-zero (true) on success, zero (false) on failure
*/
uint8_t pagesys_wipe(){
  if( pages_opened == 0 ){
    pagesys_qwipe();
    uint32_t address = 0x100;
    while( address < DISK_STOP ){
      eeprom_erase_page(address);
      address += PAGE_EDGE;
    }
    return 1;
  }
  return 0;
}


#ifdef FILESYS_DEBUG
/*
  pagesys_print

  prints out the state of the bitmap in page 0 and the numnber of open pages.
  used for debugging.

  arguments:
    none

  returns:
    nothing
*/
void pagesys_print(){
  eeprom_read_page(PAGE_0, buf, PAGE_EDGE);
  uint16_t i = 1, j = 1;
  printf("--> pages opened: %d\n", pages_opened);
  for(; i < NUM_PAGES; ++i){
    if( i % 8 == 0)
      ++j;

    if ( (buf[PAGE_EDGE - j] >> (i % 8)) & 0x1 )
      printf("PAGE ID: %d (0x%X)\n", i, i * PAGE_EDGE);
    else
      printf("[used] PAGE ID: %d (0x%X)\n", i, i * PAGE_EDGE);
  }
}
#endif


/*
  pagesys_exists
  
  checks all file header pages for a file with the name name.

  arguments:
    name  - filename

  returns:
    the id of the page representing the file header with the given name or
    0 on failure.
*/
uint16_t pagesys_exists(uint8_t *name){
  eeprom_read_page(PAGE_0, buf, PAGE_EDGE);
  
  //look for available pages and check the name of those that are file headers
  uint16_t i = 1, j = 1;
  for(;i < NUM_PAGES; ++i){
    if( i % 8 == 0 ) ++j;
     
    //available pages are marked with a 0 in the bitmap.
    //shift through the page ids
    if ( !( (buf[PAGE_EDGE - j] >> (i % 8)) & 0x1 ) ){
      uint8_t file[PAGE_EDGE];
      eeprom_read(i * PAGE_EDGE, file, PAGE_EDGE);
      if( (file[PAGE_FLAGS] & PAGE_TYPE_MASK) == FILE_P 
        && strcmp(name, file+FLAG_LENGTH) == 0){
        return i;  
      }
    }
  }
  return 0;
}


/*
  page_create
  
  initializes a new page of the given type and creates a page handler. the
  page handler is used to perform various operations on the page it represents.
  most page functions in this file require an active page handler. this
  function will return null if there are no more pages or if the max open pages
  has been reached.

  arguments:
    type  - the type of page to create. types are defined in page.h

  returns:
    an active page handler representing an open page of the given type. null
    is returned on failure.

*/
page_t *page_create(uint8_t type){
  page_t *page_entry = 0;
  
  //dont malloc more pages if max has been reached
  if(pages_opened <  MAX_OPEN_PAGES){
    //find the address of the first available page 
    uint16_t i = 1, j = 1;
    eeprom_read_page(PAGE_0, buf, PAGE_EDGE);
    for(; i < NUM_PAGES; ++i){
      if( i % 8 == 0)
        ++j;
      
      //malloc a new page entry and mark the page as used
      if ( (buf[PAGE_EDGE - j] >> (i % 8)) & 0x1 ){
        page_entry = malloc(sizeof(page_t));
        page_entry->address = i * PAGE_EDGE;
        page_entry->id = i;
        page_entry->type = type;

        ++pages_opened;

        //mark the page as used in the bitmap
        buf[PAGE_EDGE - j] &= (0xFF ^ (1 << (i % 8)));
        
        //mark the page type in the page's header
        uint8_t page[PAGE_EDGE];
        eeprom_read_page(page_entry->address, page, PAGE_EDGE); 

        page[PAGE_FLAGS] |= type;
        page[PAGE_FLAGS] |= PAGE_OPEN; 
        
        //write back all buffers to disk
        eeprom_write_page(PAGE_0, buf, PAGE_EDGE);
        eeprom_write_page(page_entry->address, page, PAGE_EDGE);

        break;
      }
    }
  }
  
  //returning null means one or both of the following:
  //  1. max number of open pages has  been reached (close some pages)
  //  2. disk is full (remove some files)
  return page_entry;
}


/*
  page_open

  opens a page given the id and returns a page handler. there are a 
  few resrictions when it comes to opening pages:
    -page 0 cannot be opened
    -an open page cannot be re-opened
    -an unitialized page cannot be opened
    -number of open pages cannot exceed max limit

  only open pages can be manipulated by the page system (aside from page 0)

  arguments:
    id  - a valid page id

  returns:
    a page handler for the opened page. null is returned on failure
*/
page_t *page_open(uint16_t id){
  page_t *page_entry = 0;

  //check conditions for opening a page
  if( id < NUM_PAGES && pages_opened < MAX_OPEN_PAGES && id != 0){
    uint8_t page[PAGE_EDGE];
    eeprom_read_page(id * PAGE_EDGE, page, PAGE_EDGE); 
    if ( (page[PAGE_FLAGS] & PAGE_TYPE_MASK) == NONE_P || (page[PAGE_FLAGS] & PAGE_OPEN) )
      return 0;
    
    //page is ok to open, create an entry
    page_entry = malloc(sizeof(page_t));
    page_entry->address = id * PAGE_EDGE; 
    page_entry->id = id;
    page_entry->type = page[PAGE_FLAGS] & PAGE_TYPE_MASK;
    
    //flag the page as opened
    ++pages_opened;
    page[PAGE_FLAGS] |= PAGE_OPEN;

    //write the changes back to eeprom
    eeprom_write_page(page_entry->address, page, PAGE_EDGE);
  }
  return page_entry;
}


/*
  page_close

  closes a page and deallocates the memory used by the handler. any changes
  made to the page while it was open are written back to EEPROM. open pages is
  decremented. this function does nothing if given a null or closed page
  
  arguments:
    page_entry  - an active(open) page handler

  returns:
    nothing
*/
void page_close(page_t *page_entry){
  if(page_entry){
    uint8_t page[PAGE_EDGE];
    eeprom_read_page(page_entry->address, page, PAGE_EDGE);
    
    //mark the page as closed and free entry
    if( page[PAGE_FLAGS] & PAGE_OPEN ){
      page[PAGE_FLAGS] ^= PAGE_OPEN;
      --pages_opened;
    }
    eeprom_write_page(page_entry->address, page, PAGE_EDGE);
    free(page_entry);
  }
}


/*
  page_read

  reads length bytes starting from offset of the given page and stores the data
  in the given buffer (data). this function does not read across page boundries.

  arguments:
    page_entry  - an active page handler
    offset      - an absolute offset marking where to start reading
    data        - the buffer to store read bytes
    length      - the number of bytes to read from the target page starting
                  from offset.

  returns:
    the number of bytes read. this value can be less than length if length 
    crosses a page boundry.
*/
uint16_t page_read(page_t *page_entry, uint16_t offset, uint8_t *data, uint16_t length){ 
  uint16_t read = 0;
  if(page_entry)
    read = eeprom_read(page_entry->address + offset, data, length);
  return read; 

}


/*
  page_write

  writes length bytes from data starting at offset of the given page. this
  function does not write across page boundries.

  arguments:
    page_entry  - an active page handler
    offset      - an absolute offset marking where to start writing
    data        - the buffer containing the data to write
    length      - the number of bytes to write starting from offset

  returns:
    the number of bytes written. this value can be less than length if 
    length crosses a page boundry
*/
uint16_t page_write(page_t *page_entry, uint16_t offset, uint8_t *data, uint16_t length){ 
  uint16_t write = 0;
  if(page_entry)
    write = eeprom_write(page_entry->address + offset, data, length);
  return write; 
}


/*
  page_erase

  zeroes out length bytes of the given page starting at offset. this function 
  does not erase across page boundries. an offset of zero and a length of 
  PAGE_EDGE will erase an entire page.

  arguments:
    page_entry  - an active page handler
    offset      - an absolute offset marking where to start erasing
    length      - the number of bytes to erase starting at offset

  returns:
    the number of bytes erased. this value can be less than length if 
    length crosses a page boundry.
*/
uint16_t page_erase(page_t *page_entry, uint16_t offset, uint16_t length){ 
  uint16_t erase = 0;
  if(page_entry)
    erase = eeprom_erase(page_entry->address + offset, length);
  return erase; 
}

