/******************************************************************************                                                                                                                
* eeprom.h
*
* constants and functions used by the simulated EEPROM chip. descriptions of 
* each function can be found in eeprom.c
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


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef EEPROM_H
#define EEPROM_H

//used for debuging (hence the name)
//#define FILESYS_DEBUG

#define PAGE_STOP 0xFF        //the address a page ends on 0xXXXFF
#define PAGE_EDGE 0x0100      //all pages start at a multiple of this value
#define DISK_SIZE 131072      //total space available in bytes
#define DISK_FILE "test.dsk"  //used for debugging
#define DISK_STOP 0x020000    //any address >= than this is invalid

inline uint8_t eeprom_page_boundry(uint32_t address);
inline uint32_t eeprom_get_pageaddr(uint32_t address);

uint16_t eeprom_read(uint32_t address, uint8_t *data, uint16_t length);
uint16_t eeprom_read_page(uint32_t address, uint8_t *data, uint16_t length);

uint16_t eeprom_write(uint32_t address, uint8_t *data, uint16_t length);
uint16_t eeprom_write_page(uint32_t address, uint8_t *data, uint16_t length);


uint16_t eeprom_erase(uint32_t address, uint16_t length);
void eeprom_erase_page(uint32_t address);
void eeprom_wipe(void);

//debugging stuff
#ifdef FILESYS_DEBUG
void eeprom_print(void);
void eeprom_backup(void);
void eeprom_recover(void);
#endif

#endif



