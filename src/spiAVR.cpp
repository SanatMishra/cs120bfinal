#include "spiAVR.h"

void SPI_SEND(char data)
{
    SPDR = data;//set data that you want to transmit
    while (!(SPSR & (1 << SPIF)));// wait until done transmitting
}

void spic(uchar x) {
  PORTD = SetBit(PORTD, PIN_DC, 0);
  SPI_SEND(x);
}

void spid(uchar x, uchar di = 0) {
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(x);
}

void spic1(uchar x, uchar a) {
  PORTD = SetBit(PORTD, PIN_DC, 0);
  SPI_SEND(x);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(a);
}

void spic2(uchar x, uchar a, uchar b) {
  PORTD = SetBit(PORTD, PIN_DC, 0);
  SPI_SEND(x);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(a);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(b);
}

void spic4(uchar x, uchar a, uchar b, uchar c, uchar d) {
  PORTD = SetBit(PORTD, PIN_DC, 0);
  SPI_SEND(x);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(a);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(b);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(c);
  PORTD = SetBit(PORTD, PIN_DC, 1);
  SPI_SEND(d);
}

//If SS is on a different port, make sure to change the init to take that into account.
void SPI_INIT(){
    DDRB |= (1 << PIN_SCK) | (1 << PIN_MOSI) | (1 << PIN_SS);//initialize your pins. 
    SPCR |= (1 << SPE) | (1 << MSTR); //initialize SPI coomunication

    PORTB = SetBit(PORTB, PIN_SS, 0);
    PORTD = SetBit(PORTD, PIN_RST, 0);
    _delay_ms(200);
    PORTD = SetBit(PORTD, PIN_RST, 1);
    _delay_ms(200);

    spic(SWRESET);
    _delay_ms(150);
    spic(SLPOUT);
    _delay_ms(200);
    spic1(COLMOD, 0x06);
    _delay_ms(10);
    spic(DISPON);
    _delay_ms(200);
}
