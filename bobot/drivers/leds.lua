local device = _G
local RD_VERSION = string.char(0x00)
local PRENDER    = string.char(0x01)
local APAGAR     = string.char(0x02)
local BLINK      = string.char(0x03)

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
api.prender.parameters = {[1]={rname="numLed", rtype="number"}} --recibe the led number
api.prender.returns = {} --no return
api.prender.call = function (ledNumber)
    local write_res, err = device:send(PRENDER .. string.char(ledNumber))
    if write_res then return 1 else return 0 end
end

api.apagar = {}
api.apagar.parameters = {[1]={rname="numLed", rtype="number"}} --recibe the led number
api.apagar.returns = {} --no return
api.apagar.call = function (ledNumber)
    local write_res, err = device:send(APAGAR .. string.char(ledNumber))
    if write_res then return 1 else return 0 end
end

api.blink = {}
api.blink.parameters = {[1]={rname="time", rtype="number"}, [2]={rname="blinks", rtype="number"}, [3]={rname="numLed", rtype="number"}} 
api.blink.returns = {} --no return
api.blink.call = function (time, blinks, ledNumber)
    local msg = BLINK .. string.char(time) .. string.char(blinks) .. string.char(ledNumber)
    local write_res, err = device:send(msg)
    if write_res then return 1 else return 0 end
end
