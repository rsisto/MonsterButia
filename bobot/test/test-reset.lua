#!/usr/bin/lua

local usb4all = require("usb4all")
local Device=usb4all.Device


local baseboards = usb4all.baseboards

local baseboard
local devices

local function read_devices_list()
	devices={}
	for d_name, bb in pairs(baseboards) do
		baseboard=bb
		for d_name,d in pairs(bb.devices) do
			devices[d_name]=d
		end
	end
	if not baseboard then print ("tr:WARN: No Baseboard found.") end
end

read_devices_list()
local device_lback = devices["lback"]
local device_temp = devices["temp"]
local device_stmtr = devices["stmtr"]

----------
print("=========1")
device_temp:open(1, 1)
device_lback:open(1, 1)
device_stmtr:open(1, 1)

device_lback.api["send"].call("hola!")
print("ret:", device_lback.api["read"].call())
--print("temp:", device_temp.api["get_temperature"].call())
----------

--baseboard:close_all()
baseboard:force_close_all()


----------
print("=========2")
device_temp:open(1, 1)
device_lback:open(1, 1)
device_stmtr:open(1, 1)

device_lback.api["send"].call("hola!")
print("ret:", device_lback.api["read"].call())
print("temp:", device_temp.api["get_temperature"].call())
----------

--baseboard:close_all()
--baseboard:force_close_all()


