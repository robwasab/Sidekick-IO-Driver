
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include "MPU6050.h"
#include "sidekickio.h"

#define SK SidekickIO


void configure_interrupt(MPU6050 &imu)
{
  // configure the interrupt level to be active low
  imu.setInterruptMode(true);

  // configure interrupt to be open drain
  imu.setInterruptDrive(true);

  // configure the interrupt to be latched until all status bits are cleared
  //imu.setInterruptLatch(true);

  // configure the interrupt to assert for only 50us and then deassert
  imu.setInterruptLatch(false);

  // enable data ready interrupts
  imu.setIntDataReadyEnabled(true);
}


void wait_for_data(MPU6050 &imu)
{
  while(true) {
    uint8_t status = imu.getIntStatus();

    if(status & (1 << MPU6050_INTERRUPT_DATA_RDY_BIT))
    {
      break;
    }
  }
}


double gyro_calc_factory_trim(double self_test_val)
{
  if(0 != self_test_val) {
    return 25.0 * 131.0 * pow(1.046, self_test_val - 1);
  }
  return 0;
}


void self_test_gyro(MPU6050 &imu)
{
  #define NUM_TRIALS 1

  int16_t gyro_self_test_dis[3];
  int16_t gyro_self_test_ena[3];
  double  gyro_factory_trim[3];


  // convert trim values using formula


  // self test is required use +/- 250 dps range
  imu.setFullScaleGyroRange(0);

  for(size_t k = 0; k < NUM_TRIALS; k++)
  {
  // get the rotation before enabling self test mode
    wait_for_data(imu);
    imu.getRotation(
      &gyro_self_test_dis[0],
      &gyro_self_test_dis[1],
      &gyro_self_test_dis[2]);

    /*
    printf("gyro before self test[%d]: %hd, %hd, %hd\n",
      k,
      gyro_self_test_dis[0],
      gyro_self_test_dis[1],
      gyro_self_test_dis[2]);
    */

    imu.setGyroXSelfTest(true);
    imu.setGyroYSelfTest(true);
    imu.setGyroZSelfTest(true);

    // read factory trim values
    gyro_factory_trim[0] =
      gyro_calc_factory_trim(imu.getGyroXSelfTestFactoryTrim());

    gyro_factory_trim[1] =
      -gyro_calc_factory_trim(imu.getGyroYSelfTestFactoryTrim());

    gyro_factory_trim[2] =
      gyro_calc_factory_trim(imu.getGyroZSelfTestFactoryTrim());



    wait_for_data(imu);

    imu.getRotation(
      &gyro_self_test_ena[0],
      &gyro_self_test_ena[1],
      &gyro_self_test_ena[2]);

    /*
    printf("gyro after self test[%d]: %hd, %hd, %hd\n",
      k,
      gyro_self_test_ena[0],
      gyro_self_test_ena[1],
      gyro_self_test_ena[2]);
    */


    double deltas[3] = {
      (double)(gyro_self_test_ena[0]) - gyro_self_test_dis[0],
      (double)(gyro_self_test_ena[1]) - gyro_self_test_dis[1],
      (double)(gyro_self_test_ena[2]) - gyro_self_test_dis[2],
    };

    /*
    printf("self test resp x: %.3lf, factory: %.3lf\n",deltas[0], gyro_factory_trim[0]);
    printf("self test resp y: %.3lf, factory: %.3lf\n",deltas[1], gyro_factory_trim[1]);
    printf("self test resp z: %.3lf, factory: %.3lf\n",deltas[2], gyro_factory_trim[2]);
    */

    #define PERC_CHANG(x) 100.0 * ((deltas[x] - gyro_factory_trim[x]) / (gyro_factory_trim[x]))

    printf("percent change x: %.3lf\n", PERC_CHANG(0));
    printf("percent change y: %.3lf\n", PERC_CHANG(1));
    printf("percent change z: %.3lf\n", PERC_CHANG(2));

    imu.setGyroXSelfTest(false);
    imu.setGyroYSelfTest(false);
    imu.setGyroZSelfTest(false);


  }
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
      self_test_gyro(*imu);
    }
  }
}


int main(void)
{
  sidekick.send_config_layout_i2cm(
    SK::I2CM_CLK_SEL_100KHZ);

  printf("accelgyro...\n");
  MPU6050 accelgyro;

  printf("resetting...\n");
  accelgyro.reset();
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
  configure_interrupt(accelgyro);

  // configure the sidekick interrupt pin on p0
  sidekick.gpio_config(0, SK::GPIO_CONFIG_DIR_IN, SK::GPIO_CONFIG_PULL_UP);
  sidekick.gpio_enable_pin_intrpt(0, gpio_callback, &accelgyro);

  while(true) {
    sidekick.main_loop_task();
    usleep(100 * 1000);
  }

  return 0;
}
