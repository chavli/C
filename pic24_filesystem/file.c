/******************************************************************************                                                                                                                
* file.c
* 
* The filesystem for an EEPROM chip. This file provides similar file 
* functionality as the standard C library, as well as some functions to deal
* with the filesystem as a whole.
*
* The maximum supported filesize for a single file is about 3.97MB which is
* much greater than the EEPROM chip this system is written for (see eeprom.c).
*
* An upper limit on the amount of files that can be opened at once is defined
* in file.c. This is so the amount of memory used for overhead to represent 
* files is bounded. At the time of this comment, the max is two(2) files. This
* can be changed in file.h.
* 
* Filenames are limited to 15 characters (15 bytes). no naming convention is 
* enforced. names that exceed 15 characters will have ther trailing 
* characters truncated:
* this_is_a_very_long_name -> this_is_a_very_
*
* Supported modes are write(w),append(a), and read(r)  
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
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "headers/util.h"

//keep track of number of opened files
static uint8_t files_opened = 0;

//private interface
static uint16_t file_get_pageid(file_t *, uint32_t offset);
static uint8_t file_add_pageid(file_t *, uint16_t id);

/*
  filesys_init

  called once to initialize the filesystem (by initializing the paging system)
  additional calls to this function once the paging system is started will
  do nothing. see pagesys_init() in page.c for more details.

  arguments:
    none

  returns:
    nothing
*/
void filesys_init(){
  pagesys_init(); 
}


/*
  filesys_quick_format

  makes changes in the page system so all pages are marked as free.
  does not zero out any pages. see pagesys_qwipe in page.c for more details.
  the filesystem cannot be formatted if there are any open files.

  arguments:
    none

  returns:
    nothing
*/
uint8_t filesys_quick_format(){
  printf("Quick Formattting...\n");
  if( files_opened == 0 )
    return pagesys_qwipe();
  return 0;

}


/*
  filesys_format

  marks all pages as free and zeroes them. see pagesys_wipe in page.c for more
  details. the filesystem cannot be formatted if there are open files.

  arguments:
    none

  returns:
    nothing
*/
uint8_t filesys_format(){
  printf("Formattting...\n");
  if( files_opened == 0 )
    return pagesys_wipe();
  return 0;
}


/*
  file_print

  prints the properties of a file. used for debugging

  arguments:
    handle  - a handler to an open file

  returns:
    nothing
*/
void file_print(file_t *handle){
  if(handle){
    printf("\nfilename: %s\n", handle->name);
    printf("page: %d\n", handle->page->id);
    printf("address: 0x%X\n", handle->page->address);
    printf("flags: 0x%X\n", handle->flags);
    printf("size: %d\n", handle->size);
    printf("offset: %d\n\n", handle->offset);
  }
}


/*
  file_open

  opens a file with the given name in the given mode. if no files exists
  with name, a new file is created and set to the given mode. if an illegal
  mode is given or the open file limit is reached, nothing will be done.

  arguments:
    name  - a filename
    mode  - a character representing the mode (w, r, a)

  returns:
    a file handle
*/
file_t *file_open(char *name, char mode){
  file_t *handle; 
  page_t *h_page, *d_page;
  uint16_t pid;
  
  //check if more files can be opened and if mode is valid
  if( (mode == 'a' || mode == 'w' || mode == 'r') &&
    files_opened < MAX_FILES ){
   
    //see if file already exists
    pid = file_exists(name);
    if(!pid) {
      printf("creating %s\n", name);
      //create file header page
      h_page = page_create(FILE_P); 
      page_write(h_page, 1, name, (strlen(name) > NAME_LEN) ? 
        NAME_LEN : strlen(name) ); 
      
      //create file handle
      handle = malloc(sizeof(file_t));
      handle->page = h_page;
      handle->size = 0;
      handle->offset = 0;
      strncpy(handle->name, name, NAME_LEN);
      switch(mode){
        case 'w':
          handle->flags |= FILE_WRITE;
          break;
        case 'r':
          handle->flags |= FILE_READ;
          break;
        case 'a':
          handle->flags |= FILE_APPEND;
          break;
      }
      
      //create the first data page for this file
      d_page = page_create(DATA_P);
      page_write(h_page, PAGE_EDGE - PAGE_ID_LEN, (uint8_t *)&(d_page->id),
        PAGE_ID_LEN);
      page_close(d_page);

      ++files_opened;
    }
    //file alreay exists, open it. pages that are already open cant be reopened
    else{
      printf("loading %s\n", name);
      h_page = page_open(pid);
      if(h_page){
        handle = malloc(sizeof(file_t));
        handle->page = h_page;
        strncpy(handle->name, name, NAME_LEN);
        //do a page read to get the filesize
        uint8_t filesize[FILESIZE_LEN];
        page_read(h_page, FILESIZE_OFFSET, filesize, FILESIZE_LEN);
        
        handle->size = *(uint32_t *)filesize;
        handle->offset = 0;
        handle->flags = 0;

        switch(mode){
          case 'w':
            handle->flags |= FILE_WRITE;
            break;
          case 'r':
            handle->flags |= FILE_READ;
            break;
          case 'a':
            handle->flags |= FILE_APPEND;
            handle->offset = handle->size;
            break;
        }
        ++files_opened;
      }
    }
  }

  return handle;
}


/*
  file_close

  closes a file that was opened earlier and frees any memory allocated to 
  represent the file and related pages. before the file is closed, 
  relevant metadata is written back to disk. on a successful close, the 
  open file counter is decremented by 1. this function should be called
  as often as possible if max open files is small.

  arguments:
    handle  - an active file handler

  returns:
    nothing
*/
void file_close(file_t *handle){
  if(handle){
    //write back the file size
    page_write(handle->page, FILESIZE_OFFSET, (uint8_t *)&handle->size, FILESIZE_LEN);
    page_close(handle->page);
    --files_opened;
    free(handle);
  }
}


/*
  file_exists

  checks if a file exists with the name name.

  arguments:
    name  - a filename

  returns:
    non-zero(true) if the file exists, 0 (false) if it doesn't
*/
uint8_t file_exists(char *name){
  return pagesys_exists(name);
}

/* not implemented
uint8_t file_remove(char *name){}
uint8_t file_rename(char *old_name, char *new_name){}
*/


/*
  file_write

  writes length data from buf to the file referenced by handle. if the
  length of new data exceeds the available space in the available pages 
  of the target file, additional pages are allocated to the file until
  all length bytes are written.

  the behavior of this function depends on the mode used.
  in write(w) mode, all data is written from the start of the file
  and overwrites any existing data.
  in append(a) mode, all data is added to the end of the file
  
  this function does nothing if the file is opened in read(r) mode.
  
  the start of writing can also be changed by modifying the file offset with
  file_rewind and file_seek.

  arguments:
    handle  - an active file handler for the target file
    buf     - the data to be written
    length  - the amount of bytes from buf to write to the file

  returns:
    the number of bytes written to the file. bytes can be less than
    length if no more pages can be allocated to accomodate length.
*/
uint32_t file_write(file_t *handle, char *buf, uint32_t length){
  printf("writing %s\n", buf);
  uint32_t total_write = 0, write = 0, pid;
  if(handle && (handle->flags & MODE_MASK) != FILE_READ ){

    //figure out where to start writing given write or append mode 
    //int16_t start = ( (handle->flags & MODE_MASK) == FILE_WRITE ) ? 0 : handle->size;
    
    //find out where the file left off in the last page of the last write
    pid = file_get_pageid(handle ,handle->offset);
    uint16_t start = handle->offset % DIRECT_SIZE;
    
    page_t *target;
    while( length > 0 ){
      if( !pid ){
        target = page_create(DATA_P);
        file_add_pageid(handle, target->id);
        start = 0;
      }
      else  
        target = page_open(pid);
      
      //write the data
      write = page_write(target, HEADER_LENGTH + start, buf, length);

      //update values
      buf += write;
      total_write += write;
      start += write;
      handle->offset += write;
      if( handle->offset > handle->size ){
        handle->size = handle->offset;
      }
      page_close(target);
      
      //update amount of bytes remaining. either fetch the next page or create
      //a new one
      length -= write;
      pid = file_get_pageid(handle, handle->size + length);
    }
  }
  return total_write;
}


/*
  file_writeline

  a convenience function that simply adds a '\n' character to the end of 
  the given data. file_write handles the rest.

  arguments:
    handle  - an active file handler for the target file
    buf     - the data to be written
    length  - the amount of bytes from buf to write to the file
  
  returns:
    the number of bytes written to the file. bytes can be less than
    length if no more pages can be allocated to accomodate length.
*/
uint32_t file_writeline(file_t *handle, char *buf, uint32_t length){
  char line[length +  1];
  strncpy(line, buf, length);
  line[length] = '\n';
  return file_write(handle, line, length + 1);

  return 0;
}


/*
  file_read

  reads length bytes from the file referenced to by handle and stores the 
  results in buf. if length exceeds the size of the file, length is changed
  to match the remaining number of bytes left in the file. (end - offset)

  this function only works if the target file is opened in read(r) mode.
  if opened in write(w) or append(a) mode this function does nothing.

  this function reads from where previous reads left off. the first read
  of a just opened file always starts from the beginning. this can be changed
  if the offset of the file is changed with file_rewind and file_seek

  arguments:
    handle  - an active file handler for the target file
    buf     - a buffer to store the read data
    length  - the amount of bytes to read from the file
  
  returns:
    the number of bytes read from the file. bytes can be less than
    length if the original value of length exceeded the size of the
    file.
*/
uint32_t file_read(file_t *handle, char *buf, uint32_t length){
  uint32_t total_read = 0, read = 0, pid;
  if(handle && (handle->flags & MODE_MASK) == FILE_READ ){
  
    //check if the read goes beyond the end of the file. if so, reduce the 
    //amount of bytes read to amount of remaining bytes
    if(handle->offset + length > handle->size)
      length = handle->size - handle->offset;

    //reads always start from where the previous left off.
    pid = file_get_pageid(handle, handle->offset);
    uint16_t start = handle->offset % DIRECT_SIZE;
    page_t *target;

    while(handle->offset < handle->size && length > 0){
      target = page_open(pid);

      //read the data
      read = page_read(target, HEADER_LENGTH + start, buf, length);

      //update values based on read
      buf += read;
      handle->offset += read;
      length -= read;
      page_close(target);
      pid = file_get_pageid(handle, handle->offset + length); 
    }
  }
  return total_read;
}

/*
  file_readline
  
  reads up to length bytes from the file referenced by handle. the read
  stops once a newline (\n) character is found. any additional characters
  read by this function that appear after the newline are removed.
  the new line character is included in string returned through buf.

  arguments:
    handle  - an active file handler for the target file
    buf     - a buffer to store the read line
    length  - the max number of bytes to store a single line of the file
  
  returns:
    the amount of bytes read that apprear before the newline. in all cases
    the number of bytes read should be <= length.
*/
uint32_t file_readline(file_t *handle, char *buf, uint32_t length){
  if(!handle || !buf || length <= 0)
    return 0;

  uint32_t read = 0;
  char *nl;
  do{
    read += file_read(handle, buf + read, length - read);
  }while( !(nl = strchr(buf, '\n')) && (length - read) > 0);
  

  if(nl){
    buf[(nl - buf) + 1] = '\0';
    return (nl - buf);
  }
  
  buf[0] = '\0';
  return 0;
}


/*
  file_seek

  moves the internal pointer of the file (the offset) to a new location.
  manually moving the internal pointer will effect how file_write and
  file_read function. origin + offset is the absolute offset of the file.
  offset alone is a relative offset with respect to origin. the offset
  cannot be moved past the end of the file.

  arguments:
    handle  - an active file handler for the target file
    origin  - an additional offset value used to calculate the final location
              of the offset specified by the offset argument. set this to
              0 if offset should be relative to the start of the file.
    offset  - the number of bytes to move the internal file pointer with 
              respect to origin.

  returns:
    non-zero(true) if the offset was succefully changed. zero(false) if it
    wasn't changed.

*/
uint8_t file_seek(file_t *handle, uint32_t origin, uint32_t offset){
  //cant seek past end of file
  if ( handle && origin + offset <= handle->size){
    handle->offset = origin + offset;
    return 1;
  }
  return 0;
}


/*
  file_rewind

  moves the internal pointer of the file (the offset) back to the start of
  the file. manually moving the offset will effect subsequent calls to
  file_read and file_write.

  arguments:
    handle  - an active file handler for the target file

  returns:
    nothing
*/
void file_rewind(file_t *handle){
  if(handle)
    handle->offset = 0;   
}


/*
  file_tell

  returns the location of the internal file pointer relative to the start
  of the file.

  arguments:
    handle  - an active file handler for the target file

  returns:
    the offset of the internal pointer or -1 on failure
*/
int32_t file_tell(file_t *handle){
  if( handle )
    return handle->offset;
  return -1;
}


/*
  file_get_pageid

  this function returns the id of the data page where the end of offset falls
  in. the offset given must be relative to the start of the file, relative 
  offsets will return the wrong id. 0 (error) will be returned if the given
  offset is greater than the max filesize limit or greater than the size of
  the target file.
  
  the returned id can then be used with page_open to fetch the actual page.
  
  arguments:
    handle  - an active file handler for the target file
    offset  - an absolute offset value used to look for the page that it
              ends in.

  returns:
    the id of the data page (DATA_P) that the offset ends in. 0 is returned
    if offset is too great or if handle is null.

*/
static uint16_t file_get_pageid(file_t *handle, uint32_t offset){
  page_t *cur_page = handle->page;
  uint8_t pid[PAGE_ID_LEN];
  uint32_t pages;


  if(!handle || offset >= MAX_FILE_SIZE || offset > handle->size)
    return 0;

  //offset ends in the double indirect page
  if( offset >= MAX_SIZE_INDIRECT ){
    //read in the double indirect page
    page_read(cur_page, FILE_HEADER_LENGTH, pid, PAGE_ID_LEN);
    cur_page = page_open(*(uint16_t *)pid);

    //offset is set to the amount of offset within the double indirect page
    offset -= MAX_SIZE_INDIRECT;
    
    //calculate the location of the indirect page id within the dbl ind page
    pages = (offset / INDIRECT_SIZE) + 1;
    page_read(cur_page, PAGE_EDGE - (pages * PAGE_ID_LEN), pid, PAGE_ID_LEN);
    page_close(cur_page); 
    cur_page = page_open(*(uint16_t *)pid); //read indirect page
    
    //offset is set to the amount of offset within the ind page
    offset %= INDIRECT_SIZE;
  }
  //offset ends in the indirect page
  else if( offset >= MAX_SIZE_DIRECT ){
    page_read(cur_page, FILE_HEADER_LENGTH + PAGE_ID_LEN, pid, PAGE_ID_LEN);
    cur_page = page_open(*(uint16_t *)pid);
    offset -= MAX_SIZE_DIRECT;
  }
  //else offset ends in a direct page
  
  //calculate location of direct page id in cur_page and read it
  pages = (offset / DIRECT_SIZE) + 1;
  page_read(cur_page, PAGE_EDGE - (pages * PAGE_ID_LEN), pid, PAGE_ID_LEN);
  
  //close any indirect pages
  if(cur_page != handle->page)
    page_close(cur_page);

  return *(uint16_t *)pid;
}


/*
  file_add_pageid

  looks for the first available location to write the id of a new data page.
  data page ids are either stored in a file's direct pages, indirect page, or
  dbl indirect page. see page.c for more details about how files handle page
  ids. this function will return 0 (error) if the file handler is null or if
  no more pages can be added (full disk).

  page ids are written backwards to disk. for example page id "20" sits in a 
  contiguous two(2) bytes as "0" and then "2".

  this function should only be called when a file needs to be expanded to hold
  more data. page ids can be obtained by calling page_create.

  if the dbl indirect or indirect pages haven't been initialized for the target
  file, this function will initialize them when they are needed to store the 
  new id.

  arguments:
    handle  - an active handle for the target file
    id      - the id of the data page (DATA_P) to be added

  returns:
    non-zero (true) if the page id was added successfully, 0 (false) otherwise
*/
static uint8_t file_add_pageid(file_t *handle, uint16_t id){
  char pagebuf[PAGE_EDGE];
  uint8_t buf[PAGE_ID_LEN];
  uint16_t cur_id;
  page_t *cur_page = handle->page;

  //figure out where the new page belongs
  if( !handle || handle->size >= MAX_FILE_SIZE )
    return 0;

  else if( handle->size >= MAX_SIZE_INDIRECT ){ //fetch dbl indirect page id
    page_read(cur_page, FILE_HEADER_LENGTH, buf, PAGE_ID_LEN);
    //dbl indirect has to be allocated
    if( *(uint16_t *)buf == 0){
      cur_page = page_create(DBL_IND_P);
      page_write(handle->page, FILE_HEADER_LENGTH, (uint8_t *)&cur_page->id,
        PAGE_ID_LEN);
    }
    else
      cur_page = page_open( *(uint16_t *)buf);
  }
  else if( handle->size >= MAX_SIZE_DIRECT ){ //fetch indirect page id
    page_read(cur_page, FILE_HEADER_LENGTH + PAGE_ID_LEN, buf, PAGE_ID_LEN);
    //indirect has to be allocated
    if( *(uint16_t *)buf == 0){
      cur_page = page_create(DBL_IND_P);
      page_write(handle->page, FILE_HEADER_LENGTH + PAGE_ID_LEN, 
        (uint8_t *)&cur_page->id, PAGE_ID_LEN);

    }
    else
      cur_page = page_open( *(uint16_t *)buf);
  }
  //else new id can fit in direct pages
  
  //look for the first available slot for the new page id
  page_read(cur_page, 0, pagebuf, PAGE_EDGE); 
  
  uint16_t page_offset = 1;
  do{
    //grab a page id one at a time starting from the end of the page. each page
    //id uses 2 bytes so we move backwards through the page 2 bytes at a time
    cur_id = *(uint16_t *)((pagebuf+(PAGE_EDGE - 1)) - (page_offset * PAGE_ID_LEN));
  }while( cur_id != 0 && page_offset++);
  
  //WRITE THE ID NOW 
  page_write(cur_page, PAGE_EDGE - (page_offset * PAGE_ID_LEN), (uint8_t *)&id, PAGE_ID_LEN);

  //close indirect pages
  if( cur_page != handle->page)
    page_close(cur_page);
  
  return 1;
}

