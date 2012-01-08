/******************************************************************************                                                                                                                
* eeprom.c
*
* simulates a byte addressable EEPROM flash chip and provides functions to 
* interact with the chip.
*
* the simulated chip in this file is a Microchip 25LC1024 EEPROM chip.
* data sheet can be found here: 
* ww1.microchip.com/downloads/en/devicedoc/21836a.pdf
*
* Name: Cha Li
* Date: 5 January 2012
*
* Development
* OS: Ubuntu 11.04 x64
* Compiler: gcc 4.5.2 (default settings)
* Kernel: 2.6.38-13-generic
******************************************************************************/



#include "eeprom.h"
#include "headers/util.h"

//simulated array of addressable memory
static char memory[DISK_SIZE];

/*
  eeprom_page_boundry

  determines if an address is the start of a page (a page boundry)

  arguments:
    address - a 3 byte address

  returns:
    true if the address is ona boundry, false if it doesn't
*/
inline uint8_t eeprom_page_boundry(uint32_t address){
  return (address % PAGE_EDGE) == 0;
}


/*
  eeprom_get_pageaddr

  finds the start address of the page the given address falls into

  arguments:
    address - a 3 byte address

  returns:
    the start address of the page address falls into
*/
inline uint32_t eeprom_get_pageaddr(uint32_t address){
  return (address >> 8) << 8;
}


/*
  eeprom_read

  reads length bytes from EEPROM starting from address and stores the data
  in data. this function will not read across page boundries.

  arguments:
    address - the 3-byte address to start reading from
    data    - a buffer to store the data read from EEPROM
    length  - amount of bytes to read starting from address (in bytes)
  
  returns:
    the number of bytes read. bytes can be less than length if length
    crosses over a page boundry.
*/
uint16_t eeprom_read(uint32_t address, uint8_t *data, uint16_t length){
  uint16_t read = 0;
  while( (length-- > 0) && (eeprom_get_pageaddr(address) <= (address - read)) ){
    *data++ = memory[address++];
    ++read;
  }
  return read;
}


/*
  eeprom_read_page

  reads length bytes from EEPROM starting from the beginning of the page which 
  contains address. this function will not read across page boundries.

  arguments:
    address - a 3 byte address used to find the target page to read from
    data    - a buffer to store the data read from EEPROM
    length  - amount of bytes to read from the start of a page (in bytes)

  returns:
    the number of bytes read. bytes can be less than length if length
    crosses over a page boundry.
*/
uint16_t eeprom_read_page(uint32_t address, uint8_t *data, uint16_t length){
  address = eeprom_get_pageaddr(address);
  return eeprom_read(address, data, length);
}


/*
  eeprom_write

  writes length bytes from data to EEPROM starting from address. this function
  will not write across page boundries.

  arguments:
    address - a 3 byte address marking where to start writing
    data    - the data to be written
    length  - the amount of data to write (in bytes)

  returns:
    the number of bytes written, bytes can be less than length if length
    crosses a page boundry.
*/
uint16_t eeprom_write(uint32_t address, uint8_t *data, uint16_t length){
  uint16_t write = 0;
  while( (length-- > 0) && (eeprom_get_pageaddr(address) <= (address - write))){
    memory[address++] = *data++;
    ++write;
  }
  eeprom_backup();
  return write;
}


/*
  eeprom_write_page

  writes length bytes to EEPROM starting from the beginning of the page
  which contains address. this function will not write across page boundries.

  arguments:
    address - a 3 byte address used to find the target page to write to
    data    - the data to be written
    length  - the amount of data to write (in bytes)

  returns:
    the number of bytes written. bytes can be less than length if length
    crosses over a page boundry.
*/
uint16_t eeprom_write_page(uint32_t address, uint8_t *data, uint16_t length){
  address = eeprom_get_pageaddr(address);
  return eeprom_write(address, data, length);
}


/*
  eeprom_erase

  zeroes out length bytes of EEPROM starting from address. this function
  will not cross page boundries.

  arguments:
    address - a 3 byte address to start erasing from
    length  - the number of bytes to zero out

  returns:
    the number of bytes erased. bytes can be less than length if length
    crosses page boundries.
*/
uint16_t eeprom_erase(uint32_t address, uint16_t length){
  uint16_t erase = 0;
  while( (length-- > 0) && (eeprom_get_pageaddr(address) <= (address - erase))){
    memory[address++] = 0;
    ++erase; 
  }
  eeprom_backup();
  return erase;
}


/*
  eeprom_erase_page

  zeroes out the entire page that contains address.

  arguments: 
    address - a 3 byte address used to find the target page

  returns:
    nothing
*/
void eeprom_erase_page(uint32_t address){
  address = eeprom_get_pageaddr(address);
  eeprom_erase(address, PAGE_EDGE);
}


/*
  eeprom_wipe

  zeroes out the entire EEPROM chip

  arguments:
    none

  returns:
    nothing
*/
void eeprom_wipe(){
  uint32_t addr;
  for(addr = 0; addr < DISK_SIZE; ++addr)
    memory[addr] = 0;
}


/*
  eeprom_print

  dumps out the contents (in table format) of the simulated EEPROM chip.
  used for debugging

  arguments:
    none

  returns:
    nothing
*/
void eeprom_print(){
  memview(memory, DISK_SIZE);
}


/*
  eeprom_backup

  writes all the contents of the EEPROM chip to a file as binary data.
  used for debugging

  arguments:
    none

  returns:
    nothing
*/
void eeprom_backup(){
  FILE *handler = fopen(DISK_FILE, "w");
  fwrite(memory, DISK_SIZE, 1, handler);
  fclose(handler);
}


/*
  eeprom_recover

  reads binary data representing the contents of an EEPROM chip from a file.
  used for debugging

  arguments:
    none

  returns:
    nothing
*/
void eeprom_recover(){
  FILE *handler = fopen(DISK_FILE, "r");
  fread(memory, DISK_SIZE, 1, handler);
  fclose(handler);
}
