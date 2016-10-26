local device = _G

local GET_VALUE=string.char(0x00)
local string_byte=string.byte

-- description: lets us know grey sensor's current value
-- input: empty
-- output: grey sensor's current value.
api={}
api.getValue = {}
api.getValue.parameters = {} -- no input parameters
api.getValue.returns = {[1]={rname="par1", rtype="int"}} 
api.getValue.call = function ()
	device:send(GET_VALUE) -- operation code 1 = get grey level
	local sen_anl_response = device:read(3) -- operation code and data
	if not sen_anl_response or #sen_anl_response~=3	then return -1 end
    local raw_val = math.floor(((string_byte(sen_anl_response, 2) or 0) + (string_byte(sen_anl_response, 3) or 0)* 256)/ 100)
	return raw_val
end
