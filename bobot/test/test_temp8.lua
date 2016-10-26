#!/usr/bin/lua

local socket = require("socket")

--local host, port = "192.168.10.1", 2009
local host, port = "localhost", 2009

local client = assert(socket.connect(host, port))
client:settimeout(nil) --blocking
--client:settimeout(1)

local function send(s)
	--print("sending", s)
	client:send(s.."\n")
	local ret = client:receive()
	--print("ret:", ret)
	return ret
end
send("LIST")
socket.sleep(1)
send("OPEN temp")
socket.sleep(1)
while true do
    io.write(send("CALL temp getTemp"))
    for i=1,7 do
	    io.write("   ", send("CALL temp"..i.." getTemp"))
    end
	socket.sleep(0.01)
    io.write("\n")
end
