#!/usr/bin/lua

local socket = require("socket")

--local host, port = "192.168.10.1", 2009
local host, port = "localhost", 2009

local client = assert(socket.connect(host, port))
client:settimeout(nil) --blocking

local function send(s)
--	print("sending", s)
	client:send(s.."\n")
	local ret = client:receive()
--	print("ret:", ret)
	return ret
end
sensor = {"button","distanc"}
for id=1,2 do -- #sensor
    for i=1,6 do
        print("Connect a "..sensor[id].." sensor in port "..i)
        socket.sleep(10)
        j = 255
        while j>0 do
            j=j-1
            print(send("CALL "..sensor[id]..":"..i.." getValue"))
            socket.sleep(0.01)
        end
    end
end
