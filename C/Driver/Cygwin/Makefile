
ROOT_PATH = ..

# Sources that have have main definined in them
MAIN_SRCS = \
	$(ROOT_PATH)/test_config.cpp
	#$(ROOT_PATH)/main.cpp

SRCS = \
	$(ROOT_PATH)/sidekickio.cpp \
	$(ROOT_PATH)/sidekickio_i2cm.cpp

INCLUDES = \
	$(ROOT_PATH)

#CFLAGS := -lusb-1.0
#LDFLAGS := -Wl,-lusb-1.0

LIBS = libusb-1.0.dll

LDRS = .

PROJECT := SidekickIO

include ../../common-standalone.mk
