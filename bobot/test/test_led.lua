#!/usr/bin/lua

local socket = require("socket")

--local host, port = "192.168.10.1", 2009
local host, port = "localhost", 2009

local client = assert(socket.connect(host, port))
client:settimeout(nil) --blocking
--client:settimeout(1)

local function send(s)
	print("sending", s)
	client:send(s.."\n")
	local ret = client:receive()
	print("ret:", ret)
	return ret
end
send("LIST")
socket.sleep(1)
send("OPEN led")
socket.sleep(1)
local intensidad = 0
while true do        
    --send("CALL led setLight " .. (256 - math.pow(2,intensidad)))
    send("CALL led setLight " ..  intensidad)
    intensidad = (intensidad + 1) % 256
	print (intensidad)
    socket.sleep(0.1)
end
