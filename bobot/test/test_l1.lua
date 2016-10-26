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
local ret = send("OPEN ledA")
print("el handler del ledA es " .. ret)
while true do
	ret = send("CALL ledA prender")
	while(ret == "fail") do
	  ret = send("CALL ledA prender")
	  print("reintentando...")
	end
	socket.sleep(1)
	send("CALL ledA apagar")
	socket.sleep(1)
end
