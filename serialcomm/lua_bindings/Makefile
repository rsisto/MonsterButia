LUA_SRC = ../../libs/lua-5.1.4/src/    #change with your lua sources folder
CC = gcc
LD = gcc
OPTS= -Wall -fPIC -D_GNU_SOURCE
LOPTS=-c -fPIC -fno-stack-protector
OBJS=lua_serialcomm.o serialcomm.o 
OBJS_SO = lua_serialcomm.so
OPENWRT_LIBS = ../mips_tools/libs/openwrt_lib 
CFLAGS=$(OPTS) -I$(LUA_SRC)  
LDFLAGS=-shared

ifdef MIPS
    CC = ../mips_tools/toolchain/mipsel-linux-uclibc-gcc
    LD = ../mips_tools/toolchain/mipsel-linux-uclibc-ld
    OPTS=-c -fPIC 
endif

ifdef ARM
    CC = arm-linux-gnueabi-gcc
    LD = arm-linux-gnueabi-ld
    CFLAGS=$(OPTS) -I$(LUA_SRC) -I/usr/lib/gcc/arm-linux-gnueabi/4.3.2/include
endif

all: lua_serialcomm so

lua_serialcomm: $(OBJS)
	$(MAKE) -C ../
	$(CC) $(CFLAGS) $(LOPTS) -o $@.o $@.c  

so: $(OBJS)
ifdef ARM
		$(LD) $(LDFLAGS) $(OBJS_SO) -L./ -L/usr/lib/gcc/arm-linux-gnueabi/4.3.2/libgcc_s.so $(OBJS)
else
ifdef MIPS
		$(LD) $(LDFLAGS) $(OBJS_SO) -L $(OPENWRT_LIBS)  ./ $(OBJS)
else
		$(LD) $(OBJS) -o $(OBJS_SO) $(LDFLAGS) 
endif
endif

clean:
	rm -f $(OBJS) $(OBJS_SO) *.c~ *.h~
	$(MAKE) clean -C ../
