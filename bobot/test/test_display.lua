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
local ret = send("OPEN display")
print("el handler del display es " .. ret)
send("CALL display iniciar")
while true do
	send("CALL display iniciar")
	send("CALL display borrar")
	send("CALL display prender_bkl")
	send("CALL display escribir JoJoJo_____._._.________________")
	socket.sleep(2)
	send("CALL display apagar_bkl")
	send("CALL display escribir Feliz_Navidad_!_!_!_____________")
	socket.sleep(2)
end
