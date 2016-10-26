CC = gcc
LD = ld
OPTS=-c -fPIC -fno-stack-protector
OBJS=serialcomm.o
OBJS_SO = serialcomm.so
CFLAGS=$(OPTS)  
LDFLAGS=-shared

#gcc -shared -Wl,-soname,libmean.so.1 -o libmean.so.1.0.1  calc_mean.o
#gcc *.o -shared -o file.so [-l<libraries>]

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
		$(LD) $(LDFLAGS) $(OBJS_SO) -L/usr/lib/gcc/arm-linux-gnueabi/4.3.2/libgcc_s.so $(OBJS)
else
		$(CC) $(OBJS) -o $(OBJS_SO) $(LDFLAGS) 
endif

clean:
	rm -f $(OBJS) $(OBJS_SO)
