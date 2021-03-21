
ROOT_PATH = ..

# Sources that have have main definined in them
MAIN_SRCS = \
	$(ROOT_PATH)/main.cpp \
	$(ROOT_PATH)/test_led.cpp \
	$(ROOT_PATH)/test_dfu.cpp \
	$(ROOT_PATH)/test_echo.cpp \
	$(ROOT_PATH)/test_gpio_read.cpp

SRCS = \
	$(ROOT_PATH)/sidekickio.cpp \
	$(ROOT_PATH)/sidekickio_i2cm.cpp \
	$(ROOT_PATH)/sidekickio_spim.cpp \
	$(ROOT_PATH)/sidekickio_gpio.cpp \
	$(ROOT_PATH)/sidekickio_delay.cpp


INCLUDES = \
	$(ROOT_PATH)

#CFLAGS := -lusb-1.0
#LDFLAGS := -Wl,-lusb-1.0

LIBS = libusb-1.0.dll

LDRS = .

PROJECT := SidekickIO

include ../../common-standalone.mk
