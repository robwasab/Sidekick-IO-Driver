#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "sidekickio.h"

#define SK SidekickIO

// Test: send i2c command while configured as something else
// Test: send spi command while configured as something else

void test_invalid_config(void)
{

}


void test_config(void)
{
  SidekickIO sidekick(SK::FW_MODE_APP);

  sidekick.send_config_layout_i2cm(
    SK::I2CM_CLK_SEL_100KHZ);


  uint8_t data[] = {0x75};

  bool success =
  sidekick.send_i2cm_write_data(0x68, data, sizeof(data));
  assert(true == success);



  uint8_t whoami = 0;
  size_t  readamt = sizeof(whoami);
  success = sidekick.send_i2cm_read_data(0x68, &whoami, &readamt);
  assert(true == success);
  assert(sizeof(whoami) == readamt);

  printf("whoami: %02x\n", whoami);


  whoami = 0;
  success = sidekick.send_i2cm_read_register_byte(0x68, 0x75, &whoami);
  assert(true == success);

  printf("whoami: %02x\n", whoami);


  success = sidekick.send_i2cm_write_register_byte(0x68, 0x6A, 0x01);
  assert(true == success);
  
  printf("done testing config!\n");


}


int main(void)
{
  test_config();

  return 0;
}
