#ifndef SerialAtmega
#define SerialAtmega

#include <avr/io.h>
#include <avr/interrupt.h>

void serial_init (int baud );
void serial_char(char ch );
void serial_print(const char *str);
void serial_print(long num, int base = 10, int pad = 0);
void serial_println(const char *str);
void serial_println(long num, int base = 10, int pad = 0);

#endif