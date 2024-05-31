#ifndef SPIAVR_H
#define SPIAVR_H

#include "common.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//B5 should always be SCK(spi clock) and B3 should always be MOSI. If you are using an
//SPI peripheral that sends data back to the arduino, you will need to use B4 as the MISO pin.
//The SS pin can be any digital pin on the arduino. Right before sending an 8 bit value with
//the SPI_SEND() funtion, you will need to set your SS pin to low. If you have multiple SPI
//devices, they will share the SCK, MOSI and MISO pins but should have different SS pins.
//To send a value to a specific device, set it's SS pin to low and all other SS pins to high.

// Outputs, pin definitions
#define PIN_SCK                   PORTB5//SHOULD ALWAYS BE B5 ON THE ARDUINO
#define PIN_MOSI                  PORTB3//SHOULD ALWAYS BE B3 ON THE ARDUINO
#define PIN_SS                    PORTB2
#define PIN_DC                    PORTD7
#define PIN_RST                   PORTD6

#define DISPON 0x29
#define SWRESET 0x01
#define SLPOUT 0x11
#define COLMOD 0x3A

#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C
#define RAMRD 0x2E

void SPI_SEND(char data);
void spic(uchar x);
void spid(uchar x, uchar di = 0);
void spic1(uchar x, uchar a);
void spic2(uchar x, uchar a, uchar b);
void spic4(uchar x, uchar a, uchar b, uchar c, uchar d);
void SPI_INIT();

#endif /* SPIAVR_H */
