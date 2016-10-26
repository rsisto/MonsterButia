local device = _G
local RD_VERSION = string.char(0x00)
local PRENDER 	 = string.char(0x01)
local APAGAR 	 = string.char(0x02)
local BLINK 	 = string.char(0x03)

api={}
api.read_version = {}
api.read_version.parameters = {} --no parameters
api.read_version.returns = {[1]={rname="version", rtype="number"}} --one return
api.read_version.call = function ()
	local get_read_version = RD_VERSION 
	device:send(get_read_version)
	local version_response = device:read(2) 
	local raw_val = string.byte(version_response, 2) 
	--print("rawval, deg_temp: ", raw_val, deg_temp)
	return raw_val
end

api.prender = {}
api.prender.parameters = {} --no parameters
api.prender.returns = {} --no return
api.prender.call = function ()
	device:send(PRENDER)
--	local response = device:read(2)
--	print("[",string.byte(response, 1),",", string.byte(response, 2),"]")
--	print("-----------------------------------------------")
--	if (string.byte(response, 1) ~= nil or string.byte(response, 2) ~= nil) then
--	    print("vino data")
--	end
end

api.apagar = {}
api.apagar.parameters = {} --no parameters
api.apagar.returns = {} --no return
api.apagar.call = function ()
	device:send(APAGAR)
end

api.blink = {}
api.blink.parameters = {[1]={rname="time", rtype="number"}, [2]={rname="blinks", rtype="number"}} 
api.blink.returns = {} --no return
api.blink.call = function (time, blinks)
    local msg = BLINK .. string.char(time) .. string.char(blinks)
	device:send(msg)
end
