#!/usr/bin/lua

local usb4all = require("usb4all")
local Device=usb4all.Device


local function dump_api(device)
	print ("=======Dumping API for device:", device.name, device.handler)
	for fname, fdef in pairs(device.api) do
		print("Function:", fname)
		for i,pars in ipairs(fdef.parameters) do
			print("    Parameter #"..i)
			for k, v in pairs(pars) do
				print("       "..k.. ": " .. tostring(v))
			end
		end
		for i,rets in ipairs(fdef.returns) do
			print("    Return #"..i)
			for k, v in pairs(rets) do
				print("       "..k.. ": " .. tostring(v))
			end
		end
	end
	print ("=======End Dumping")
end



local bbs = usb4all.get_baseboards()
local found
for iSerial, bb in pairs(bbs) do
	found = true
end
if not found then
	print ("No Baseboards found. Quitting")
	os.exit()
end

local baseboard
for _,bb in pairs(bbs) do
	print ("\nBaseboard id:", bb.idBoard, " modules:",bb:get_user_modules_size())
	for _,d in pairs(bb.devices) do
		print ("device:", d.name)
	end
	baseboard=baseboard or bb
end


local device = baseboard.devices["temp"]
if not device then
	print("Missing temp module")
else
	print("opening device:", device.name)
	if not device:open(0x01, 0x01) then
		print "Error opening!!!"
	end

	print("------------------------")
	local api_call=device.api.get_temperature
	print(api_call.returns[1].rname .. " of type " .. api_call.returns[1].rtype, api_call.call() )
	print("------------------------")
end


local device = baseboard.devices["lback"]
print("opening device:", device.name)
if not device:open(0x01, 0x01) then
	print "Error opening!!!"
end

dump_api(device)
device.api.send.call("abcd")
print("Sending to lback and reading:", device.api.read.call(10))


baseboard:close()

