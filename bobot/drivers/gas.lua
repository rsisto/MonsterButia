local device = _G
local RD_GAS = string.char(0x01)
local char000 = string.char(0,0,0)

api={}
api.get_gas = {}
api.get_gas.parameters = {} --no parameter
api.get_gas.returns = {[1]={rname="gas level", rtype="number"}} --one return
api.get_gas.call = function ()
	local get_payload = RD_GAS 
	device:send(get_payload)
	local response = device:read(3) 
	if not response then 
		print ('WARN: api.get_gas.call failure on device:read(3)')
		response=char000
	end
	local raw_val = string.byte(response, 2) + 255*string.byte(response, 3)
	return raw_val
end

