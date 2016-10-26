# Makefile para instalar todas las dependencias de lua y bobot

all:  dependency
	@echo bobot listo para usar en XO !

dependency:
	$(MAKE) -C ../serialcomm/lua_bindings
	$(MAKE) -C ../lualibusb
	$(MAKE) -C ../libs/lua-5.1.4 linux
	$(MAKE) -C ../libs/luasocket

clean:
	@rm -f *~ *.o drivers/*~
	$(MAKE) -C ../serialcomm/lua_bindings clean
	$(MAKE) -C ../lualibusb clean
	$(MAKE) -C ../libs/lua-5.1.4 clean
	$(MAKE) -C ../libs/luasocket clean
