#include "scara.h"

#define MAX_BLOCK_LENGTH 80

static char block[MAX_BLOCK_LENGTH];

#define TICKS_PER_MICROSECOND (F_CPU/1000000)

void printString(const char* s) {
    while(*s) {
        write(*s++);
    }
    write('\r\n');
}

ISR(TIMER1_COMPA_vect)
{  
  // flip digial pin
    // write('.');
}

void initTimer() {

  // initialise timer 
  // set frequency
  // use oscillioscope to check frequency
  // what algorithm should I use?

  TCCR1B &= ~(1<<WGM13); // waveform generation = 0100 = CTC
  TCCR1B |=  (1<<WGM12);
  TCCR1A &= ~((1<<WGM11) | (1<<WGM10)); 
  TCCR1A &= ~((1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0));
	
  // Enable Timer1 interrupt
	TIMSK1 |= (1 << OCIE1A);

	TCNT1 = 0;
	
  uint32_t cycles = ceil( (TICKS_PER_MICROSECOND*1000000*60)*inv_rate ); // (cycles/step)    

  // cycles per tick
	OCR1A = 15625;
}

int main()
{
	  initSerial();
    initTimer();

    // Enable global interrupts
    sei();

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