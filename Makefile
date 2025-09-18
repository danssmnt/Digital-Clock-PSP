OBJS = main.o Libs/glib2d/glib2d.o
INCDIR =
CFLAGS = -Wall -fno-lto
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpng -ljpeg -lz  -lglut -lGLU -lGL -lpspvfpu -lm -lpspgu -lpspvram

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Digital Clock
PSP_EBOOT_ICON = MainAssets/XMB/ICON0.PNG
PSP_EBOOT_PIC1 = MainAssets/XMB/PIC1.PNG
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
