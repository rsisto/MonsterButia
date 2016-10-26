#!/usr/bin/lua
assert(package.loadlib("./lua_serialcomm.so","luaopen_serialcomm"))()
local socket = require("socket")
local handler, err, ret
local mensaje = "aloha"
--while 1 do
    handler, err = serialcomm.init("/dev/ttyUSB0", 9600)
    if not err then 
        print("inicializado handler: " .. handler)
        ret, err = serialcomm.send_msg(handler, mensaje, 5)
        if not err then
            print("se manda: ".. mensaje)
            ret, err = serialcomm.read_msg(handler)
            if not err then
                print("me llega: " .. ret)
            else
                print("read err: " .. err)
            end
        else
            print("send err: " .. err)
        end
    else 
	print("init err: " .. err)
    end
--end
