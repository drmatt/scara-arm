#include "scara.h"


uint8_t receiveBuffer[RECEIVE_BUFFER_SIZE];
uint8_t receiveBufferHead = 0;
volatile uint8_t receiveBufferTail = 0;

uint8_t transmitBuffer[TRANSMIT_BUFFER_SIZE];
uint8_t transmitBufferHead = 0;
volatile uint8_t transmitBufferTail = 0;


// Returns the number of bytes used in the RX serial buffer.
uint8_t getReceiveBufferCount()
{
  uint8_t tail = receiveBufferTail;
  if (receiveBufferHead >= tail) { return(receiveBufferHead-tail); }
  return (RECEIVE_BUFFER_SIZE - (tail-receiveBufferHead));
}

void initSerial()
{
  // baud rate must be >57600
  uint16_t UBRR0_value = ((F_CPU / (4L * BAUD_RATE)) - 1)/2;
  UCSR0A |= (1 << U2X0);  

  UBRR0H = UBRR0_value >> 8;
  UBRR0L = UBRR0_value;
            
  // enable rx and tx
  UCSR0B |= 1<<RXEN0;
  UCSR0B |= 1<<TXEN0;
	
  // enable interrupt on complete reception of a byte
  UCSR0B |= 1<<RXCIE0;
}


void write(uint8_t data) {
  uint8_t nextHead = (transmitBufferHead + 1)%TRANSMIT_BUFFER_SIZE;

  while (nextHead == transmitBufferTail) {}

  transmitBuffer[transmitBufferHead] = data;
  transmitBufferHead = nextHead;
  
  UCSR0B |=  (1 << UDRIE0); 
}

ISR(USART_UDRE_vect)
{
  uint8_t tail = transmitBufferTail;
  UDR0 = transmitBuffer[tail];
  
  tail = (tail+1)%TRANSMIT_BUFFER_SIZE;

  transmitBufferTail = tail;
 
  // Turn off Data Register Empty Interrupt to stop tx-streaming if this concludes the transfer
  if (tail == transmitBufferHead) { UCSR0B &= ~(1 << UDRIE0); }
}

uint8_t read()
{
  uint8_t tail = receiveBufferTail;
  if (receiveBufferHead == tail) {
    return SERIAL_NO_DATA;
  } else {
    uint8_t data = receiveBuffer[tail];
    receiveBufferTail = (tail+1)%RECEIVE_BUFFER_SIZE;
    return data;
  }
}


ISR(USART_RX_vect)
{
  uint8_t data = UDR0;
  uint8_t nextHead = (receiveBufferHead + 1)%RECEIVE_BUFFER_SIZE;

  // Write data to buffer unless it is full.
  if (nextHead != receiveBufferTail) {
    receiveBuffer[receiveBufferHead] = data;
    receiveBufferHead = nextHead;    
  }
}

