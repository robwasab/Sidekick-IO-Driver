
#include <stdio.h>
#include <stdint.h>

#include "MPU6050.h"
#include "sidekickio.h"

#define SK SidekickIO

int main(void)
{
  printf("hello world!\n");

  sidekick.send_config_layout_i2cm(
    SK::I2CM_CLK_SEL_100KHZ);

  MPU6050 sensor;


  sensor.initialize();

  // verify connection
  bool success = sensor.testConnection();

  printf("test connection: %s\n", success ? "success" : "fail");

  return 0;
}
