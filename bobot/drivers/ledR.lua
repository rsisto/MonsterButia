local device = _G
local RD_VERSION = string.char(0x00)
local PRENDER = string.char(0x01)
local APAGAR = string.char(0x02)

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
end

api.apagar = {}
api.apagar.parameters = {} --no parameters
api.apagar.returns = {} --no return
api.apagar.call = function ()
	device:send(APAGAR)
end
