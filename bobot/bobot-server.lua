#!/usr/bin/lua

--[[

Syntax

	# lua bobot-server.lua [DEBUG] [connection]*

Parameters:
	DEBUG		enables debug printing in bobot
	connection	a list of connection services to attempt. Supported
			values in bobot (for now) are usb, serial and chotox

If no connection services are provided, defaults to usb and serial.

Examples:
	Start with debug disabled and the dummy chotox service, only:
	# lua bobot-server.lua chotox

	Start with debug enabled and the serial services only:
	# lua bobot-server.lua DEBUG serial

	Start with debug disabled and the usb and serial services (same as default):
	# lua bobot-server.lua usb serial

--]]

--package.path=package.path..";./bobot_server/?.lua"
local my_path = debug.getinfo(1, "S").source:match[[^@?(.*[\/])[^\/]-$]] or "./"
package.path=package.path..";"..my_path.."bobot_server/?.lua;"
			..my_path.."lib/?.lua;"..my_path.."?.lua"

--tcp listening address
local N_PROTOCOLS = 2
local ADDRESS = "*"
local PORT_B = 2009 --B is for bobot
local PORT_H = 2010 --H is for http

local TIMEOUT_REFRESH = 3

local socket = require("socket")
local process = require("bobot-server-process").process
local http_serve = require("bobot-server-http").serve

local bobot = require("bobot")

local set_debug
for i, v in ipairs(arg) do
	if v=="DEBUG" then
		set_debug=true 
		table.remove(arg, i)
		break
	end
end
if set_debug then 
	bobot.debugprint = print
	bobot.debugprint("Debugging messages enabled")
else
	bobot.debugprint = function() end
end


local server_b = assert(socket.bind(ADDRESS, PORT_B))
local server_h = assert(socket.bind(ADDRESS, PORT_H))

local recvt={[1]=server_b, [2]=server_h}

devices = {}

local function get_device_name(d)

--print("DEVICENAME", d.module, d.hotplug, d.handler)
	local board_id, port_id = '', ''
	if #bobot.baseboards>1 then
		board_id='@'..d.baseboard.idBoard
	end
	if d.hotplug then 
		port_id = ':'..d.handler
	end
	
	local n=d.module..board_id..port_id
	
	if not devices[n] then
		return n
	end

	local i=2
	local nn=n.."#"..i
	while devices[nn] do
		i=i+1
		nn=n.."#"..i
	end

	return nn
end

local function read_devices_list()
	bobot.debugprint("=Listing Devices")
	local bfound
	devices={}
	for _, bb in ipairs(bobot.baseboards) do
		bobot.debugprint("===board ", bb.idBoard)
		for _,d in ipairs(bb.devices) do
			local regname = get_device_name(d)
			devices[regname]=d
			devices[#devices+1]=d
			d.name=regname
			bobot.debugprint("=====module ",d.module," name",regname)
		end
		bfound = true
	end
	if not bfound then bobot.debugprint("ls:WARN: No Baseboard found.") end
end

local function split_words(s)
	local words={}

	for p in string.gmatch(s, "%S+") do
		words[#words+1]=p
	end
	
	return words
end

local socket_handlers = {}
setmetatable(socket_handlers, { __mode = 'k' })
socket_handlers[server_b]=function()
	local client, err=server_b:accept()
	if not client then return end
	bobot.debugprint ("bs:New bobot client", client, client:getpeername())
	table.insert(recvt,client)
	socket_handlers[client] = function ()
		local line,err = client:receive()
		if err=='closed' then
			bobot.debugprint ("bs:Closing bobot client", client)
			for k, v in ipairs(recvt) do 
				if client==v then 
					table.remove(recvt,k) 
					return
				end
			end
		end
		if line then
			local words=split_words(line)
			local command=words[1]
			if not command then
				bobot.debugprint("bs:Error parsing line:", line, command)
			else
				if not process[command] then
					bobot.debugprint("bs:Command not supported:", command)
				else
					if command=="QUIT" and #recvt>N_PROTOCOLS+1 then
						client:send("server in use\n")
						return
					end
					local ret = process[command](words) or ""
					client:send(ret .. "\n")
				end
			end
		end
	end
end

socket_handlers[server_h]=function()
	local client, err=server_h:accept()
	if not client then return end
	bobot.debugprint ("bs:New http client", client, client:getpeername())
	client:setoption ("tcp-nodelay", true)
	--client:settimeout(5)
	table.insert(recvt,client)			
	socket_handlers[client]	= function ()
		local ret,err=http_serve(client)
		if err=='closed' then
			bobot.debugprint ("bs:Closing http client", client)
			for k, v in ipairs(recvt) do 
				if client==v then 
					table.remove(recvt,k) 
					return
				end
			end
		end
		if ret then 
			client:send(ret)
		end
	end
end

function server_refresh ()
	local refreshed
	for i, bb in ipairs(bobot.baseboards) do
		--if bb.refresh and not (bb.comms.type=='serial' and bb.devices) then 
		if bb.refresh and bb.hotplug then 
			if not bb:refresh() then
				bobot.baseboards[i]=nil
			end
			refreshed=true
		end
	end
	if refreshed then read_devices_list() end
end

function server_init ()
	bobot.init(arg)
	read_devices_list()
end


server_init()
bobot.debugprint("Listening...")
-- loop forever waiting for clients

while 1 do
	local recvt_ready, _, err=socket.select(recvt, nil, TIMEOUT_REFRESH)
	if err=='timeout' then
		if #bobot.baseboards==0 then 
			server_init ()
		else
			server_refresh ()
		end
	else
		local skt=recvt_ready[1]
		socket_handlers[skt]()
	end
end



