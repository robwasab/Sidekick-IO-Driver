
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include "MPU6050.h"
#include "sidekickio.h"

#define SK SidekickIO



void poll_data(MPU6050 &imu)
{
  int16_t ax, ay, az, gx, gy, gz;

  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  printf("accl: [%04hd, %04hd, %04hd] gyro [%04hd, %04hd, %04hd]\r",
    ax, ay, az, gx, gy, gz);
	fflush(stdout);
}



void gpio_callback(SidekickIO * self, uint8_t gpio_index, void * obj)
{
  assert(NULL != obj);
  MPU6050 * imu = (MPU6050 *) obj;

  //while(false == sidekick.gpio_read(0))
  {
    uint8_t status = imu->getIntStatus();

    //printf("mpu6050 status: %02x\n", status);
    if(status & (1 << MPU6050_INTERRUPT_DATA_RDY_BIT))
    {
      //printf("data ready!\n");
      //self_test_gyro(imu);
      poll_data(*imu);
    }
  }
}


int main(void)
{
  #define INT_PIN 4
  sidekick.send_config_layout_i2cm(
    SK::I2CM_CLK_SEL_100KHZ);

  printf("accelgyro...\n");
  MPU6050 accelgyro;

  printf("resetting...\n");
  accelgyro.reset();

  printf("done resetting... waiting...\n");
  usleep(100 * 1000);

  accelgyro.resetGyroscopePath();
  accelgyro.resetAccelerometerPath();
  accelgyro.resetTemperaturePath();
  accelgyro.resetFIFO();
  accelgyro.resetSensors();

  printf("initializing...\n");
  accelgyro.initialize();

  // verify connection
  printf("test connection...\n");
  bool success = accelgyro.testConnection();
  printf("test connection: %s\n", success ? "success" : "fail");

  // configure the imu interrupt
  printf("configuring imu interrupt...\n");

  // configure the interrupt level to be active low
  accelgyro.setInterruptMode(true);

  // configure interrupt to be open drain
  accelgyro.setInterruptDrive(true);

  // configure the interrupt to be latched until all status bits are cleared
  //accelgyro.setInterruptLatch(true);

  // configure the interrupt to assert for only 50us and then deassert
  accelgyro.setInterruptLatch(false);

  // enable data ready interrupts
  accelgyro.setIntDataReadyEnabled(true);

  // configure the sidekick interrupt pin on p0
  printf("configuring sidekick interrupt...\n");

  sidekick.gpio_config(
      INT_PIN,  // pin number
      SK::GPIO_CONFIG_DIR_IN, // input
      SK::GPIO_CONFIG_PULL_UP); // pullup

  sidekick.gpio_enable_pin_intrpt(
      INT_PIN, // pin number
      gpio_callback,  // callback handler
      &accelgyro); // void * object passed to handler

  while(true) {
    sidekick.main_loop_task();
    usleep(10000);
  }

  return 0;
}
