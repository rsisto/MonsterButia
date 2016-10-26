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
send("OPEN led")
socket.sleep(1)
local intensidad = 0
while true do        


    io.write(send("CALL led setLight " ..  intensidad))
    intensidad = (intensidad + 80) % 256
    for i=1,7 do
        socket.sleep(0.1)
        intensidad = (intensidad + 10) % 256
	    io.write("   ", send("CALL led"..i.." setLight " ..  intensidad))
    end
    socket.sleep(0.1)
 	print (intensidad)
end
