
#ifndef __ARDUINO_H__
#define __ARDUINO_H__


#define BUFFER_LENGTH 32
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


uint32_t micros(void);

uint32_t millis(void);

long map(long x, long in_min, long in_max, long out_min, long out_max);

int min(int a, int b);

int max(int a, int b);

char * dtostrf(float val, int width, int precision, char buf[]);

void delay(int val);

#define F(x)  (x)


class SerialClass
{
public:
  void write(const char * arg) {
    printf(arg);
  }

  void write(char arg) {
    putchar(arg);
  }

  void print(const char * arg) {
    printf(arg);
  }
};

extern SerialClass Serial;


#endif
