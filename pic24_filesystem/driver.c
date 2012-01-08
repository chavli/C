#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "headers/util.h"
#include "file.h"

int main(int argc, char *argv){
  eeprom_recover();  
  filesys_init();
  

  /* general file manipulation calls */
  file_t * foo = file_open("output.csv", 'a');
  file_write(foo, "hello world",11); 
  file_write(foo, "heres some more stuff", 21); 
  file_close(foo);


  foo =  file_open("vim", 'w');
  char buf[2048] = "Vim has some intelligence about what to do if the swap file is corrupt in  some way.  If Vim has doubt about what it found, it will give an error  message and insert lines with ??? in the text.  If you see an error message  while recovering, search in the file for ??? to see what is wrong.  You may  want to cut and paste to get the text you need.    The most common remark is ???LINES MISSING.  This means that Vim cannot read  the text from the original file.  This can happen if the system crashed and  parts of the original file were not written to disk.    Be sure that the recovery was successful before overwriting the original  file or deleting the swap file.  It is good practice to write the recovered  file elsewhere and run diff to find out if the changes you want are in the  recovered file.  Or use |:DiffOrig|.    Once you are sure the recovery is ok delete the swap file.  Otherwise, you  will continue to get warning messages that the .swp file already exists.    {Vi: recovers in another way and sends mail if there is something to recover}\0";
  file_write(foo, buf, strlen(buf));
  file_close(foo);


  
  printf("--------------------------------------\n");

  foo = file_open("output.csv", 'w');
  file_write(foo, "***************************************|", 40);
  file_close(foo);

  foo = file_open("another.test", 'w');
  file_close(foo);

  foo = file_open("vim", 'a');
  char wiki[1024] = "Using universal hashing (in a randomized algorithm or data structure) refers to selecting a hash function at random from a family of hash functions with a certain mathematical property (see definition below). This guarantees a low number of collisions in expectation, even if the data is chosen by an adversary. Many universal families are known (for hashing integers, vectors, strings), and their evaluation is often very efficient. Universal hashing has numerous uses in computer science, for example in implementations of hash tables, randomized algorithms, and cryptography.";
  file_write(foo, wiki, strlen(wiki));
  file_close(foo);
  

  foo = file_open("lines.txt", 'a');
  file_writeline(foo, "this is the first line", 22);
  file_write(foo, "some more stuff", 15);
  file_close(foo);

  foo = file_open("lines.txt", 'r');
  char in[256];
  file_readline(foo, in, 256);
  printf("|%s|", in);

  file_rewind(foo);

  memset(in, 0, 256);

  file_read(foo, in, 10);
  printf("|%s|", in);
  file_close(foo);

  eeprom_print();
  //char buf[256] = "**********************************************************************************************************************************************************************************************************************************************************ABC|";
  //file_write(foo, buf, strlen(buf));
  


  
  /*
  foo = file_open("test.foo",'a');
  file_print(foo); 

  file_write(foo, "$$$",3); 
  */
  //eeprom_print();

  /*
  foo = file_open("test.foo", 'r');
  file_print(foo);
  
  char read[128];
  int b = file_read(foo, read, 11);
  printf("%d %s\n", b, read);
  
  file_print(foo);
  eeprom_print();
  
  memset(read, 0, 128);
  b = file_read(foo, read, 29);
  printf("%d %s\n", b, read);
  file_close(foo);
*/
  /* write test:
  foo = file_open("test.foo", 'w');
  file_print(foo); 
  file_write(foo, buf, 256);
  file_print(foo); 
  eeprom_print();
  */

  /*=== PAGE tests ===
  page_t *page_1 = page_create(DATA_P);
  page_t *page_2 = page_create(DATA_P);
  page_t *page_3 = page_create(DATA_P);
  page_t *page_4 = page_create(DATA_P);

  
  char text[256];
  memset(text, 0, 256);

  int16_t a;

  sprintf(text, "%s %d", "hello world", 1);
  a = page_write(page_1, 2, text, 100);
  printf("bytes written: %d\n", a);
  
  sprintf(text, "%s %d", "hello world", 2);
  a = page_write(page_2, 2, text, 100);
  printf("bytes written: %d\n", a);
  
  sprintf(text, "%s %d", "hello world", 3);
  a = page_write(page_3, 2, text, 100);
  printf("bytes written: %d\n", a);
  

  int i;
  for(i = 0; i < 256; ++i)
    text[i] = i % 0x80;

  a = page_write(page_4, 2, text, 256);
  printf("bytes written: %d\n", a);
  
  //eeprom_print();
  memset(text, 0, 256);
  page_read(page_4, 5, text, 5);
  
  page_erase(page_4, 64, 32);
  page_write(page_4, 64, "this is erased data", 19);
 
  //will fail due to open pages
  pagesys_qwipe();
  
  //eeprom_print();

  printf("\n"); 
  page_close(page_1);
  page_close(page_2);
  page_close(page_3);
  page_close(page_4);

  pagesys_qwipe();

  //eeprom_print();

  
  memset(text, 0, 256);
  page_1 = page_create(DATA_P);
  sprintf(text, "%s %d", "goodbye", 5);
  page_write(page_1, 2, text, 9);

  //eeprom_print();
  
  page_close(page_1);
  
  //full wipe
  pagesys_wipe();
  
  
  eeprom_print();
  
  //printf("%s\n", text);
  */


  /* === EEPROM tests === 
  uint8_t x = 0xFF;
  printf("0x%X\n", x);
  
  int i = 0;
  for(; i < 8; ++i){
    x &= (0xFF ^ (1 << i)) ;
    printf("0x%X\n", x);
    x = 0xFF;
  }
  */


  /*
  char buf[256] = "hello world";
  eeprom_write_page(0x0, buf, sizeof(buf));

  sprintf(buf, "%s", "this is a test sentence. here are some numbers 100 20123 1i12 DFASDFADSf");
  eeprom_write_page(0x100, buf, sizeof(buf));



  //full aligned page write
  int i;
  for(i = 0; i < 256; ++i)
    buf[i] = i % 0x80;
  
  eeprom_write_page(0x300, buf, sizeof(buf));
  //eeprom_erase_page(0x3f1);


  eeprom_write(0x290, buf, sizeof(buf));
  
  
  eeprom_print();
  eeprom_backup();
  */
/*  
  eeprom_read_page(0x0, buf, sizeof(buf));
  memview(buf, sizeof(buf));


  eeprom_read_page(0x290, buf, sizeof(buf));
  memview(buf, sizeof(buf));


  eeprom_read_page(0x300, buf, sizeof(buf));
  memview(buf, sizeof(buf));
  
  //eeprom_erase(0x300, 129);
  eeprom_wipe();

  eeprom_write(0x000, "A", 1);
  eeprom_write(0x100, "B", 1);
  eeprom_write(0x200, "C", 1);
  eeprom_write(0x300, "D", 1);
  eeprom_print();
  //eeprom_wipe();
*/
    
}

