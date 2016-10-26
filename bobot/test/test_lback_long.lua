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
send("OPEN lback")
socket.sleep(1)
while true do
	send("CALL lback send Space_The_final_frontier_These_are_the_voyages_of_")
	send("CALL lback read")
	socket.sleep(0.1)
	send("CALL lback send the_Starship,_Enterprise_Its_5_year_mission_To_")
	send("CALL lback read")
	socket.sleep(0.1)
	send("CALL lback send explore_strange_new_worlds_To_seek_out_new_life_")
	send("CALL lback read")
	socket.sleep(0.1)
	send("CALL lback send and_new_civilizations_To_boldly_go_where_no_man_")
	send("CALL lback read")
	socket.sleep(0.1)
	send("CALL lback send has_gone_before" .. socket.gettime())
	send("CALL lback read")
	socket.sleep(0.1)
end
