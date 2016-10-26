local device = _G
local RD_TEMP = string.char(0x34, 0x02)
local char000 = string.char(0,0,0)


api={}
api.get_temperature = {}
api.get_temperature.parameters = {} --no parameters
api.get_temperature.returns = {[1]={rname="temperature", rtype="number"}} --one return
api.get_temperature.call = function ()
	local get_temp_payload = RD_TEMP 
	device:send(get_temp_payload)
	local temperature_response = device:read(3) or char000
	local raw_val = string.byte(temperature_response, 2) + (string.byte(temperature_response, 3) * 256)
	local raw_temp = raw_val / 8
	local deg_temp = raw_temp * 0.0625	
	--print("rawval, deg_temp: ", raw_val, deg_temp)
	return deg_temp
end

