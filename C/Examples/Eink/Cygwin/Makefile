
ROOT_PATH = ..

MAIN_SRCS = \
	$(ROOT_PATH)/USER/main.c

SRCS = \
	$(ROOT_PATH)/HARDWARE/EPD/Display_EPD_W21.c \
	$(ROOT_PATH)/HARDWARE/Fonts/font8.c \
	$(ROOT_PATH)/HARDWARE/Fonts/font12.c \
	$(ROOT_PATH)/HARDWARE/Fonts/font16.c \
	$(ROOT_PATH)/HARDWARE/Fonts/font20.c \
	$(ROOT_PATH)/HARDWARE/Fonts/font24.c \
	$(ROOT_PATH)/HARDWARE/GUI/GUI_Paint.c \
	$(ROOT_PATH)/epd_spi_implementation.cpp
	#$(ROOT_PATH)/HARDWARE/Fonts/font24CN.c
	#$(ROOT_PATH)/HARDWARE/Fonts/font12CN.c


INCLUDES = \
	$(ROOT_PATH)/HARDWARE/EPD \
	$(ROOT_PATH)/HARDWARE/Fonts \
	$(ROOT_PATH)/HARDWARE/GUI \
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
