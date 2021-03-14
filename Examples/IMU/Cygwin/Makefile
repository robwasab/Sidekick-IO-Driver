
ROOT_PATH = ..

MAIN_SRCS = \
	$(ROOT_PATH)/imu_self_test.cpp \
	$(ROOT_PATH)/imu_interrupt_read.cpp
	#$(ROOT_PATH)/sidekick_i2c.cpp \

SRCS = \
	$(ROOT_PATH)/i2cdevlib/Arduino/MPU6050/MPU6050.cpp \
	$(ROOT_PATH)/i2cdevlib/Arduino/I2Cdev/I2Cdev.cpp \
	$(ROOT_PATH)/Arduino.cpp


INCLUDES = \
	$(ROOT_PATH)/i2cdevlib/Arduino/MPU6050 \
	$(ROOT_PATH)/i2cdevlib/Arduino/I2Cdev \
	$(ROOT_PATH)/../../Driver \
	$(ROOT_PATH)

# Passes these arguments to the linker with the -WL,-l:(name of library)
LIBS = \
	SidekickIO.a \
	libusb-1.0.dll

# Passes these arguments to the linker. Tells it where the libraries are.

LDRS = \
	../../../Driver/build \
	../../../Driver/Cygwin

#LDFLAGS :=

PROJECT := IMUExample

DEFINES := ARDUINO=100

SidekickIO.a :
	make -C ../../../Driver/Cygwin lib

include ../../../common-standalone.mk
