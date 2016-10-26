#!/usr/bin/lua
assert(package.loadlib("./lua_serialcomm.so","luaopen_serialcomm"))()
package.loaded.util                = nil; local util              = require "util"
local socket = require("socket")
local handler, err, ret
local mensaje = "aloha\R"
handler, err = serialcomm.init("/dev/ttyUSB0", 57600)
if not err then 
    util.debug("inicializado handler: " .. handler)
else 
	util.debug("init err: " .. err)
end
util.debug("se manda: ".. mensaje)
ret, err = serialcomm.send_msg(handler, mensaje)
if not err then
    util.debug("send: " .. ret)
else
    util.debug("send err: " .. err)
end
ret, err = serialcomm.read_msg(handler, 200)
if not err then
    util.debug("read: " .. ret)
else
    util.debug("read err: " .. err)
end
ret, err = serialcomm.close(handler)
if not err then
    util.debug("close : " .. ret)
else
    util.debug("close err: " .. err)
end

