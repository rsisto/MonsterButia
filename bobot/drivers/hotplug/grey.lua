local device = _G

local RD_VERSION=string.char(0x00)
local GET_VALUE=string.char(0x01)
local string_byte=string.byte

-- description: lets us know grey module's version
api={}
api.getVersion = {}
api.getVersion.parameters = {} -- no input parameters
api.getVersion.returns = {[1]={rname="version", rtype="int"}}
api.getVersion.call = function ()
	device:send(RD_VERSION) -- operation code 0 = get version
    local version_response = device:read(3) -- 3 bytes to read (opcode, data)
    if not version_response or #version_response~=3 then return -1 end
    local raw_val = (string_byte(version_response,2) or 0) + (string_byte(version_response,3) or 0)* 256
    return raw_val
end

-- description: lets us know grey sensor's current value
-- input: empty
-- output: grey sensor's current value.
api.getValue = {}
api.getValue.parameters = {} -- no input parameters
api.getValue.returns = {[1]={rname="par1", rtype="int"}} 
api.getValue.call = function ()
	device:send(GET_VALUE) -- operation code 1 = get grey level
	local sen_anl_response = device:read(3) -- operation code and data
	if not sen_anl_response or #sen_anl_response~=3	then return -1 end
    local raw_val = (string_byte(sen_anl_response, 2) or 0) + (string_byte(sen_anl_response, 3) or 0)* 256
	return raw_val
end
