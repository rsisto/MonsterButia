#!/usr/bin/lua
assert(package.loadlib("./lua_serialcomm.so","luaopen_serialcomm"))()
package.loaded.util                = nil; local util              = require "util"
local socket = require("socket")
local handler, err, ret
local mensaje = "aloha\R"
handler, err = serialcomm.init("/dev/ttyUSB0", 57600)
if not err then 
    print("inicializado handler: " .. handler)
else 
	print("init err: " .. err)
end
--socket.sleep(5)
-- ret, err = serialcomm.read_msg(handler)
--ret, err = serialcomm.send_msg(handler, basofia, 7)
while 1 do
        --socket.sleep(1)
        ret, err = serialcomm.send_msg(handler, mensaje)
        if not err then
            util.debug("se manda: ".. mensaje)
            ret, err = serialcomm.read_msg(handler, 200)
            if not err then
                util.debug("me llega: " .. ret)
            else
                util.debug("read err: " .. err)
            end
        else
            util.debug("send err: " .. err)
        end
end
