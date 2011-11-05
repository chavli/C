#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

/******************************************************************************
  memview - prints the contents of memory one byte at a time in hex and ascii
 
  arguments
    void *start:    start address
    bytes_t length: number of bytes to read

  return:
    -1:   if pointer or length is 0
    0:    succesfully printed memory
******************************************************************************/

int memview(void *start, bytes_t length){
  if(start == NULL || length == 0)
    return -1;
  printf("\nptr addr: %p, stored addr: %p, data length: %ld, ", 
          &start,
          start,
          length);
  
  //round up to nearest MAX_BYTES bytes
  bytes_t i = 1, j;
  while(length > i * MAX_BYTES && i++);
  length = i * MAX_BYTES;
  printf("padded to: %ld\n", length);
  unsigned char *buf, *byte; 
  for(j = 0; j < length / MAX_BYTES; ++j){
    buf = malloc(MAX_BYTES * sizeof(char) + 1);
    printf("0x%016lX\t|",
          (bytes_t)(start + j * MAX_BYTES),
          (bytes_t)(start + (j + 1) * MAX_BYTES - 1 ));

    for(i = 0; i < MAX_BYTES; ++i){
      byte = (char *)(start + j * MAX_BYTES + i);
      printf(" %02X", *byte);

      //print the ascii representation of the hex value if it's not a
      //blank, newline, or tab
      if(*byte != 0x0 && *byte != 0x0A && *byte != 0x09) 
        memcpy(buf + i, byte, 1);
      else 
        strcpy(buf + i, ".");
    }
    printf("|\t%s\n", buf);
    free(buf);
  }
  return 0;
}

