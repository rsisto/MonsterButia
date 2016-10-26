local device = _G

local GET_VALUE=string.char(0x00)
local string_byte=string.byte

-- description: lets us know button's current status
-- input: empty
-- output: button's current status. Possible status: 1 pressed - 0 not pressed
api={}
api.getValue = {}
api.getValue.parameters = {} -- no input parameters
api.getValue.returns = {[1]={rname="state", rtype="int"}}
api.getValue.call = function ()
	device:send(GET_VALUE) -- operation code 1 = get button's status
	local sen_dig_response = device:read(2) -- 2 bytes to read (opcode, data)
    if not sen_dig_response or #sen_dig_response~=2 then return -1 end
	local raw_val = string_byte(sen_dig_response, 2) or 0 -- keep data
	return raw_val 
end
