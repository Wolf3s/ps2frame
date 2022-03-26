

LIBDIR      = $(PS2LIB)/ee/lib
INCLUDES    = -I$(PS2LIB)/common/include -I$(PS2LIB)/ee/include -I$(PS2LIB)/ee/lib/gcc-lib/mips64r5900-sce-elf/2.9-ee-991111b/include

OBJS        = main.o hw.o hardware.o pad.o loadmodule.o

LCFILE      = linkfile

LIBS        = -lc -lm -lkernel

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
CPP         = cpp
#LD          = ld
LD			= $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RM          = /bin/rm -f

# -Wa,-al  switches on assembly listing
#CFLAGS      = -O2 -EL -pipe -Wall -Wa,-al -fno-common -Wall -mips3 -mcpu=r5900 -ffreestanding -fnonnull-objects -fno-builtin -fshort-double -nostartfiles -mlong64 -mhard-float -mno-abicalls -c
CFLAGS       = -D_EE -O2 -EL -Wall  -fno-common -Wall -mips3 -mcpu=r5900 -ffreestanding -fnonnull-objects -fno-builtin -fshort-double -nostartfiles -mlong64 -mhard-float -mno-abicalls

#CFLAGS      = -O2 -EL  -Wall  -fno-common -mips3 -mcpu=r5900 -mlong64 -mhard-float -fno-builtin -fnonnull-objects -ffreestanding 
CXXFLAGS    = -O2 -Wall -Wa,-al -fno-exceptions -fno-common
#LDFLAGS     = -Map,$(TARGET).map -L $(LIBDIR)
LDFLAGS     = -Wl,-Map,$(TARGET).map -L$(LIBDIR) -LC:\ps2dev\gcc\ee\lib

.SUFFIXES: .c .s .cc .dsm

all: frame.elf

frame.elf: $(OBJS) crt0.o
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LDFLAGS) $(LIBS)

crt0.o:
	$(AS) $(CFLAGS) -c crt0.S -o $@ > $*.lst

hw.o:
	$(AS) $(CFLAGS) -c hw.s -o $@ > $*.lst

.s.o:
	$(AS) $(CFLAGS) $(INCLUDES) $< -o $@ > $*.lst

.S.o:
	$(AS) $(CFLAGS) $(INCLUDES) $< -o $@ > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $*.o 
	#>$*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf
