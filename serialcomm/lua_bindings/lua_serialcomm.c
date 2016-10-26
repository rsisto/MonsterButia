/* 
 * Authors:	Andres Aguirre, Pablo Gindel, Jorge Visca, Gonzalo Tejera, Santiago Margini, Federico Andrade
 * MINA Group, Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 * 
 * License at file end. 
 */

#include "serialcomm.h" /* serialcomm header */
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int l_serialcomm_send_msg(lua_State *L) {
	int handler = lua_tonumber (L, 1);
	unsigned char *bytes  = (unsigned char*)lua_tostring (L, 2);
    	int size = lua_strlen (L, 2);
	int ret = send_msg(handler, bytes, size);
	if( ret < 0 ) {
		lua_pushnil (L); //data
		lua_pushnumber (L, ret); //error
		return 2; /* number of results */   
	} else{
		lua_pushnumber (L, ret);
		return 1; /* number of results */   
	}	

}

static int l_serialcomm_read_msg(lua_State *L) {
	int handler = lua_tonumber (L, 1);
    	//int size = lua_tonumber (L, 2);
    	int timeout = lua_tonumber (L, 3);
	//char *bytes = (char*)malloc(255);
	unsigned char bytes[255];
	int ret = read_msg(handler,bytes, timeout);
	if( ret < 0 ) {
		lua_pushnil (L); //data
		lua_pushnumber (L, ret); //error
		return 2; /* number of results */   
	} else{
		lua_pushlstring (L, (char*)bytes, ret);
		return 1; /* number of results */   
	}	

}


static int l_serialcomm_init(lua_State *L){
    const char* device = lua_tostring(L, 1);
    int baud_rate = lua_tonumber (L, 2);
    int ret = serialport_init(device, baud_rate);
    if( ret < 0 ) {
        lua_pushnil (L); //data
        lua_pushnumber (L, ret); //error
        return 2; /* number of results */   
	} else{
        lua_pushnumber (L, ret);
        return 1; /* number of results */   
    }	
}

static int l_serialcomm_close(lua_State *L) {
    int handler = lua_tonumber (L, 1);
    int ret = serialport_close(handler);
    if( ret < 0 ) {
        lua_pushnil (L); //data
        lua_pushnumber (L, ret); //error
        return 2; /* number of results */   
	} else{
        lua_pushnumber (L, ret);
        return 1; /* number of results */   
    }
}

static const struct luaL_reg serialcomm [] = {
		{ "send_msg", l_serialcomm_send_msg }, 
		{ "read_msg", l_serialcomm_read_msg }, 
		{ "init",     l_serialcomm_init     },
		{ "close",    l_serialcomm_close    },
		{ NULL, NULL } /* sentinel */
};

int luaopen_serialcomm(lua_State *L){
	luaL_openlib(L, "serialcomm", serialcomm, 0);
	return 1;
}


/************************************************************************
*
* Lua bindings for serialcomm.
* Copyright 2008 MINA Group, Facultad de Ingenieria, Universidad de la
* Republica, Uruguay. 
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
************************************************************************/
