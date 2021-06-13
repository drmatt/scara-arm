#ifndef serial_h
#define serial_h

#define RECEIVE_BUFFER_SIZE 128
#define TRANSMIT_BUFFER_SIZE 64

#define SERIAL_NO_DATA 0xff

void initSerial();
void write(uint8_t data);
uint8_t read();
uint8_t getReceiveBufferCount();

#endif
