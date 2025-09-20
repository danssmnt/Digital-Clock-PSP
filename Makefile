TARGET = DigitalClock
OBJS = callbacks.o lib/glib2d/glib2d.o main.o

LIBS = -lpng -ljpeg -lz -lpspgu -lm -lpspvram -lpsprtc -lpspctrl

WARNING_FLAGS = -Werror -Wall -Wextra -Wno-sign-compare

CFLAGS = -O2 -G0 -fno-pic $(WARNING_FLAGS)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP

PSP_EBOOT_TITLE = Digital Clock v2.0
PSP_EBOOT_ICON = assets/xmb/ICON0.PNG
PSP_EBOOT_PIC1 = assets/xmb/PIC1.PNG
PSP_EBOOT_SFO = assets/xmb/CUST_PARAM.SFO

# TODO: Add vvv
# PSP_EBOOT_ICON1 = assets/xmb/ICON1.PMF

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak