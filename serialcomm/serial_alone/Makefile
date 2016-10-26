CC = gcc
LD = ld
OPTS=-c -fPIC -fno-stack-protector
OBJS=serialcomm.o
OBJS_SO = serialcomm.so
CFLAGS=$(OPTS)  
LDFLAGS=-shared -soname serialcomm.so.1 -o

ifdef MIPS
    CC = ../mips_tools/toolchain/mipsel-linux-uclibc-gcc
    LD = ../mips_tools/toolchain/mipsel-linux-uclibc-ld
    OPTS=-c -fPIC
endif

ifdef ARM
    CC = arm-linux-gnueabi-gcc
    LD = arm-linux-gnueabi-ld
    #OPTS=-c -fPIC
endif

all: serialcomm so

serialcomm: $(OBJS)
	$(CC) $(CFLAGS) $(OPTS) -o $(OBJS) $@.c  

so: $(OBJS)
ifdef ARM
		$(LD) -shared -soname serialcomm.so.1 -o $(OBJS_SO) -L/usr/lib/gcc/arm-linux-gnueabi/4.3.2/libgcc_s.so $(OBJS)
else
		$(LD) -shared -soname serialcomm.so.1 -o $(OBJS_SO) $(OBJS)
endif

clean:
	-rm $(OBJS) $(OBJS_SO) *.*~
