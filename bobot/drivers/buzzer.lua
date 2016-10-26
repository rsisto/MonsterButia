local device = _G
local RD_VERSION = string.char(0x00)
local PRENDER = string.char(0x01)
local APAGAR = string.char(0x02)
local BUZZER_CORTO = string.char(0x03)
local BUZZER_TRIPLE = string.char(0x04)

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
api.prender.parameters = {} --no parameters
api.prender.returns = {} --no return
api.prender.call = function ()
    local write_res, err = device:send(PRENDER)
    if write_res then return 1 else return 0 end
end

api.apagar = {}
api.apagar.parameters = {} --no parameters
api.apagar.returns = {} --no return
api.apagar.call = function ()
    local write_res, err = device:send(APAGAR)
    if write_res then return 1 else return 0 end
end

api.buzzer_corto = {}
api.buzzer_corto.parameters = {[1]={rname="num", rtype="number"}} 
api.buzzer_corto.returns = {} --no return
api.buzzer_corto.call = function (num)
    local write_res, err = device:send(BUZZER_CORTO .. string.char(num))
    if write_res then return 1 else return 0 end
end

api.buzzer_triple = {}
api.buzzer_triple.parameters = {[1]={rname="tiempo1", rtype="number"}, [2]={rname="tiempo2", rtype="number"}, [3]={rname="tiempo3", rtype="number"}} 
api.buzzer_triple.returns = {} --no return
api.buzzer_triple.call = function (tiempo1, tiempo2, tiempo3)
    local write_res, err = device:send(BUZZER_TRIPLE .. string.char(tiempo1) .. string.char(tiempo2) .. string.char(tiempo3))
    if write_res then return 1 else return 0 end
end
