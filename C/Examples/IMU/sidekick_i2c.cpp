
#include <stdio.h>
#include <stdint.h>

#include "MPU6050.h"

int main(void)
{
  printf("hello world!\n");

  MPU6050 sensor;

  sensor.initialize();

  // verify connection
  bool success = sensor.testConnection();

  printf("test connection: %s\n", success ? "success" : "fail");

  return 0;
}
