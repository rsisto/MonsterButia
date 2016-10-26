local device = _G
local RD_VERSION = string.char(0x00)
local RD_DEBUG = string.char(0x01)
local MESSAGE = string.char(0x02)


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
        --return raw_val
	return version_response
end

api.rd_debug = {}
api.rd_debug.parameters = {} --no parameters
api.rd_debug.returns = {[1]={rname="data", rtype="string"}} --debug message
api.rd_debug.call = function ()
    local write_res, err = device:send(RD_DEBUG)
    local len = 4
	local ret = device:read(len) or ""
	print("====",ret, string.len(ret))
	return ret
end

api.message = {}
api.message.parameters = {} --no parameters
api.message.returns = {} 
api.message.call = function ()
    local write_res, err = device:send(MESSAGE)
        return write_res
end

