#include "scara.h"

#define MAX_BLOCK_LENGTH 80

static char block[MAX_BLOCK_LENGTH];

void printString(const char* s) {
    while(*s) {
        write(*s++);
    }
    write('\r\n');
}

int main()
{
    sei();
	  initSerial();

    uint8_t blockIndex = 0;
    bool isComment = false;
    uint8_t bracketNesting =0;
    uint8_t c;

    while(1) {

    _delay_ms(1);

    while((c = read()) != SERIAL_NO_DATA) {
      if ((c == '\n') || (c == '\r')) { 
         block[blockIndex] = 0;
         printString(block);
         isComment = false;
         bracketNesting = 0;
         blockIndex = 0;
      } else {

        if (isComment) {
          if (c == '(') { bracketNesting++; }
          else if (c == ')' && bracketNesting>0) {
            bracketNesting--;
            if (bracketNesting==0) { isComment = false;}
          }
        } else {
          if (c <= ' ' || c == '/' || blockIndex >= (MAX_BLOCK_LENGTH-1)) {
            // ignore 
          } else if (c == '(') {
            isComment = true;
            bracketNesting ++;
          } else if (c == ';') {
            isComment = true;
          } else if (c >= 'a' && c <= 'z') {
            block[blockIndex++] = c-'a'+'A';
          } else {
            block[blockIndex++] = c;
          }
         }
      }
    }
    
       
   }

 

}