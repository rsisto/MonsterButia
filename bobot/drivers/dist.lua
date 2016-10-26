local device = _G

local GET_VALUE=string.char(0x00)
local string_byte=string.byte

-- description: lets us know dist sensor's current value
-- input: empty
-- output: dist sensor's current value.
api={}
api.getValue = {}
api.getValue.parameters = {} -- no input parameters
api.getValue.returns = {[1]={rname="par1", rtype="int"}} 
api.getValue.call = function ()
	device:send(GET_VALUE) -- operation code 1 = get distance of object
	local sen_anl_response = device:read(3) -- operation code and data
	if not sen_anl_response or #sen_anl_response~=3	then return -1 end
    --local raw_val = (string_byte(sen_anl_response, 2) or 0) + (string_byte(sen_anl_response, 3) or 0)* 256
    raw_val = string.byte(sen_anl_response, 2)* 256 + string.byte(sen_anl_response, 3) 
	raw_val = 1024 - raw_val
	return raw_val
end




