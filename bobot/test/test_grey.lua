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

raw_val = send("LIST")
socket.sleep(1)
_, pos = string.find(raw_val, "grey:")
port = string.sub(raw_val,pos+1,pos+1)  -- get port number

if pos then
    while true do
	    print (send("CALL grey:"..port.." getValue"))
	    socket.sleep(0.01)
    end
else
     print("err::No grey connected.")
end
