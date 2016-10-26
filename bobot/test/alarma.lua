#!/usr/bin/lua

local socket = require("socket")

--local host, port = "192.168.10.1", 2009
local host, port = "localhost", 2009

local client = assert(socket.connect(host, port))
client:settimeout(nil) --blocking
--client:settimeout(1)

local function send(s)
	print("sending", s)
	local val = client:send(s.."\n")
	--print("val:", val)
	local ret = client:receive()
	print("ret:", ret)
	return ret
end
send("LIST")
socket.sleep(1)
send("OPEN move")
socket.sleep(1)
local alarma
while true do
	alarma = send("CALL move get_move")
    alarma = tostring(alarma)    
    if(alarma=="1") then 
        send("CALL buzzer prender")
        socket.sleep(1)
        send("CALL buzzer apagar")
    end
	socket.sleep(0.2)	
end
