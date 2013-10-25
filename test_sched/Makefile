# This sample Makefile, explains how you can compile plain AVR C file.
#
# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

ARDMK_DIR = /home/vincent/Devel/Arduino-Makefile
ARDUINO_DIR = /usr/share/arduino
AVR_TOOLS_DIR = /usr

# NO_CORE = Yes
# VARIANT = standard
ARDUINO_CORE_PATH = /home/vincent/Devel/arduino-projects/arduino_core

BOARD_TAG = uno
# MCU = atmega328p
# F_CPU = 16000000L

# ISP_PROG   = stk500v1
# AVRDUDE_ISP_BAUDRATE = 19200
ISP_PORT = /dev/ttyACM*

include $(ARDMK_DIR)/arduino-mk/Arduino.mk

# !!! Important. You have to use make ispload to upload when using ISP programmer
