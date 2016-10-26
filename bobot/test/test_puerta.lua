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
local ret = send("OPEN puerta")
print("el handler del puerta es " .. ret)
while true do
	ret = send("CALL puerta prender")
	while(ret == "fail") do
	  ret = send("CALL puerta prender")
	  print("reintentando...")
	end
	socket.sleep(0.1)
	send("CALL puerta apagar")
	socket.sleep(0.1)
end