#!/usr/bin/lua
assert(package.loadlib("./lua_serialcomm.so","luaopen_serialcomm"))()
local socket = require("socket")
local handler, err, ret
--local mensaje = string.char(0x28, 0x8, 0x00) .. "aloha"
local mensaje = string.char(0x28, 0x08, 0x00) .. "aloha"
--local mensaje = "basofia!"
handler, err = serialcomm.init("/dev/ttyUSB0", 115200)
if not err then 
    print("inicializado handler: " .. handler)
else 
	print("init err: " .. err)
end
ret, err = serialcomm.send_msg(handler, mensaje, 8)
if not err then
	print("se manda: ".. mensaje)
end
ret, err = serialcomm.send_msg(handler, mensaje, 8)
if not err then
	print("se manda: ".. mensaje)
end
while 1 do
    socket.sleep(1)
    ret, err = serialcomm.send_msg(handler, mensaje, 8)
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
end
