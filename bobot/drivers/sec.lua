local device = _G
local RD_VERSION    = string.char(0x00)
local GET_SEC       = string.char(0X01)
local INTRUSION     = string.char(0x02)
local RESET_FLAG    = string.char(0x03)
local CONT_INT      = string.char(0x04)
local MESS          = string.char(0x05)
local RESET         = string.char(0xFF)

api={}
api.read_version = {}
api.read_version.parameters = {} --no parameters
api.read_version.returns = {[1]={rname="version", rtype="number"}} --one return
api.read_version.call = function ()
        local get_read_version = RD_VERSION
        device:send(get_read_version)
        local version_response = device:read(4) 
        local raw_val = string.byte(version_response, 3) 
        --print("rawval, deg_temp: ", raw_val, deg_temp)
        return raw_val
end

api.get_sec = {}
api.get_sec.parameters = {} --no parameters
--api.get_sec.returns = {[1]={rname="error", rtype="string"},[2]={rname="SecSensorValue_1", rtype="number"},[3]={rname="SecSensorValue_2", rtype="number"}} 
api.get_sec.returns = {[1]={rname="SecSensorValue_1", rtype="number"},[2]={rname="SecSensorValue_2", rtype="number"}} 
api.get_sec.call = function ()
    local send_res, err
    send_res, err = device:send(GET_SEC)
    local ret = device:read(3)
    local SecSensorValue_1 = (string.byte(ret,2) or 0)
    local SecSensorValue_2 = (string.byte(ret,3) or 0)
--  local SecSensorValue_1 = (string.byte(ret,2))
--  local SecSensorValue_2 = (string.byte(ret,3))
        if ((SecSensorValue_1 == 0) and (SecSensorValue_2 == 0)) then
            alarma = "00"
        elseif ((SecSensorValue_1 == 0) and (SecSensorValue_2 == 1)) then
            alarma = "01"
        elseif ((SecSensorValue_1 == 1) and (SecSensorValue_2 == 0)) then
            alarma = "10"
        else
            alarma = "11"
        end
    return alarma
    --if ret then return true, SecSensorValue_1, SecSensorValue_2 else return false end
end

api.intrusion = {}
api.intrusion.parameters = {} --no parameters
api.intrusion.returns = {[1]={rname="alarma", rtype="number"}} --one return
api.intrusion.call = function ()
        local intrusion = INTRUSION
        device:send(intrusion)
        local alarma = device:read(1)
        if (alarma == string.char(0x00)) then
            alarma = 0
        else
            alarma = 1
        end
        return alarma
end

api.reset_flag = {}
api.reset_flag.parameters = {} --no parameters
api.reset_flag.returns = {[1]={rname="borrado", rtype="number"}} --one return
api.reset_flag.call = function ()
        local reset_flag = RESET_FLAG
        device:send(reset_flag)
        local borrado = device:read(1) 
        if (borrado == string.char(0x00)) then
            borrado = 0
        else
            borrado = 1
        end

        return borrado
end

api.cont_int = {}
api.cont_int.parameters = {} --no parameters
api.cont_int.returns = {[1]={rname="cantidad", rtype="number"}} --one return
api.cont_int.call = function ()
        local cont_int = CONT_INT
        device:send(cont_int)
        local cantidad = device:read(1) 
        return string.byte(cantidad)
end
