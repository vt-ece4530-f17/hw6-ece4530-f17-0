TARGET = cyclecount
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -g -Wall -D$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/
LDFLAGS = -g -Wall
CC = $(CROSS_COMPILE)gcc
OBJDUMP= $(CROSS_COMPILE)objdump
ARCH= arm

build: $(TARGET)

$(TARGET): main.o
	$(CC) $(LDFLAGS) $^ -o $@
	$(OBJDUMP) -D $(TARGET) > $(TARGET).lst
	
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(TARGET).lst *.a *.o *~