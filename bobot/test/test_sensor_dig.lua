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
send("OPEN sensor")
socket.sleep(1)
send("OPEN motores")
while true do
	local digIn = send("CALL sensor sendig 5")
	
	if digIn == "1" then
		send("CALL motores setvel2mtr 0 800 0 800")
		socket.sleep(1)
	else
		send("CALL motores setvel2mtr 0 0 0 0")
	end	
	
	send("CALL sensor senanl 4")
	send("CALL sensor senanl 2")
	
	socket.sleep(1)	
	
end
