local device = _G

api={}
api.getTilt = {}
api.getTilt.parameters = {} -- -- no input parameters
api.getTilt.returns = {[1]={rname="par1", rtype="int"}} 
api.getTilt.call = function ()
	device:send(string.char(0x00)) -- codigo de operacion 0
	local sen_dig_response = device:read(3) 
	local raw_val	
	if not sen_dig_response or string.byte(sen_dig_response or "00000000", 2) == nil or string.byte(sen_dig_response or "00000000", 3) == nil
	then 
		raw_val = "nil value"
	else
		raw_val = string.byte(sen_dig_response, 3) % 2
	end	
	return raw_val 
	
end
