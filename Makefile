PSPSDK = $(shell psp-config --pspsdk-path)
PSPLIBSDIR = $(PSPSDK)/..

TARGET = DigitalClock
OBJS = main.o Libs/glib2d/glib2d.o

LIBS = -lpng -ljpeg -lz -lpspgu -lm -lpspvram -lpspgum

CFLAGS = -O2 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Digital Clock
PSP_EBOOT_ICON = MainAssets/XMB/ICON0.PNG
PSP_EBOOT_PIC1 = MainAssets/XMB/PIC1.PNG

include $(PSPSDK)/lib/build.mak