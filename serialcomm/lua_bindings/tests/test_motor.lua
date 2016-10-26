#!/usr/bin/lua
assert(package.loadlib("./lua_serialcomm.so","luaopen_serialcomm"))()
local socket = require("socket")
local handler, err, ret
local mensaje = string.char(0x10, 0x8, 0x00) .. string.char(0x00,0x02,0x00,0x00,0xFF)
local basofia = "basofia!"
handler, err = serialcomm.init("/dev/ttyUSB0", 57600)
if not err then 
    print("inicializado handler: " .. handler)
else 
	print("init err: " .. err)
end
socket.sleep(6)
ret, err = serialcomm.send_msg(handler, basofia, 8)
ret, err = serialcomm.send_msg(handler, basofia, 8)
--    while 1 do
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
--   end
socket.sleep(6)
