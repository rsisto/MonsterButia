LIBUSB_SRC = ../libs/libusb-0.1.12  #change with your libusb sources folder
LUA_SRC = ../libs/lua-5.1.4/src/    #change with your lua sources folder

OPTS       = -Wall -fPIC -fno-stack-protector
OBJS       = lualibusb.o
OBJS_SO    = libluausb.so

CFLAGS     = $(OPTS) -I$(LIBUSB_SRC) -I$(LUA_SRC) 
LDFLAGS    = -shared -soname libluausb.so.1 -o

all: lualibusb so

lualibusb: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -c
so: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS_SO) -lusb $(OBJS)
clean:
	-rm -f $(OBJS) $(OBJS_SO)
