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
send("OPEN luz")
socket.sleep(1)
local luz
while true do
	luz = send("CALL luz getLuz")
    print ( " la luz vale " .. luz)
    -- send("CALL led setLight " .. 320 -  (math.log10(1 + luz) * 100))
    send("CALL led setLight " .. (luz / 4)) --tejera
	socket.sleep(0.1)
end
