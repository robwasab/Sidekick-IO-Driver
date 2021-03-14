
#include <stdio.h>
#include <stdint.h>

#include "MPU6050.h"
#include "sidekickio.h"

#define SK SidekickIO

int main(void)
{
  printf("hello world!!!!\n");

  exit(0);

  sidekick.send_config_layout_i2cm(
    SK::I2CM_CLK_SEL_100KHZ);


  printf("sensor...\n");
  MPU6050 sensor;


  printf("initializing...\n");
  sensor.initialize();

  // verify connection
  bool success = sensor.testConnection();

  printf("test connection: %s\n", success ? "success" : "fail");

  return 0;
}
