

#include "Arduino.h"

#include <sys/time.h>
#include <unistd.h>


extern SerialClass Serial;


uint32_t micros(void)
{
  struct timeval time;

  gettimeofday(&time, NULL);

  return 1000000 * time.tv_sec + time.tv_usec;
}


uint32_t millis(void) {
  return micros() / 1000;
}


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int min(int a, int b)
{
  if(a < b) {
    return a;
  } else {
    return b;
  }
}

int max(int a, int b)
{
  if(a < b) {
    return b;
  } else {
    return a;
  }
}

void delay(int val) {
  usleep(val * 1000);
}

char * dtostrf(float val, int width, int precision, char buf[])
{
  sprintf(buf, "%f", val);
  return buf;
}
