local device = _G

local string_char=string.char

local RD_VERSION  = string_char(0x00)
local ESCRIBIR    = string_char(0x01)
local PRUEBA      = string_char(0x02)
local BORRAR      = string_char(0x03)
local INICIAR     = string_char(0x04)
local PRENDER_BKL = string_char(0x08)
local APAGAR_BKL  = string_char(0x09)
local AGENDAR_MSG = string_char(0x0A)
local SET_TICKS   = string_char(0x0B)
local ESPACIO	  = string_char(0x20)
 

api={}
api.read_version = {}
api.read_version.parameters = {} --no parameters
api.read_version.returns = {[1]={rname="version", rtype="number"}} --one return
api.read_version.call = function ()
	device:send(RD_VERSION)
	local version_response = device:read(2) 
	local raw_val = string.byte(temperature_response, 2) 
	--print("rawval, deg_temp: ", raw_val, deg_temp)
	return raw_val
end

api.escribir = {}
api.escribir.parameters = {[1]={rname="message", rtype="string"}}
api.escribir.returns = {}
api.escribir.call = function (str)
	local msg = ESCRIBIR .. str
	device:send(msg)
	--device:read()
end

api.prueba = {}
api.prueba.parameters = {} --no parameters
api.prueba.returns = {} --no return
api.prueba.call = function ()
	device:send(PRUEBA)
end

api.borrar = {}
api.borrar.parameters = {} --no parameters
api.borrar.returns = {} --no return
api.borrar.call = function ()
	device:send(BORRAR)
end

api.iniciar = {}
api.iniciar.parameters = {} --no parameters
api.iniciar.returns = {} --no return
api.iniciar.call = function ()
	device:send(INICIAR)
end

api.prender_bkl = {}
api.prender_bkl.parameters = {} --no parameters
api.prender_bkl.returns = {} --no return
api.prender_bkl.call = function ()
	device:send(PRENDER_BKL)
end

api.apagar_bkl = {}
api.apagar_bkl.parameters = {} --no parameters
api.apagar_bkl.returns = {} --no return
api.apagar_bkl.call = function ()
        device:send(APAGAR_BKL)
end

api.set_ticks = {}
api.set_ticks.parameters = {[1]={rname="cantTicks", rtype="numeric"}} --how many timmers ticks have to pass before display agendar_msg screen
api.set_ticks.returns = {} --no return
api.set_ticks.call = function (ticks)
        device:send(SET_TICKS .. string_char(ticks))
end

api.agendar_msg = {}
api.agendar_msg.parameters = {
	[1]={rname="message", rtype="string"}, -- first parameter is the message
	[2]={rname="isCiclic", rtype="numeric"}, -- 1 in case of a ciclic message 0 elsewere
	[3]={rname="isEnd", rtype="numeric"}} -- 1 in case of end of message, so previously calls to agendar_msg could be concatenated
api.agendar_msg.returns = {[1]={rname="isFull", rtype="boolean"}}  --If it is no more space in the event buffer returns true, false elsewere
api.agendar_msg.call = function (str, isCiclic, isEnd)
    	device:send(AGENDAR_MSG .. string_char(32) .. string_char(isEnd) .. string_char(isCiclic) .. str)
        print("despues del send")
	local ret = device:read(3) or char0000 --el tercer byte recibido indica con un 1 si el buffer circuilar que se utiliza para mantener los mensajes esta lleno, 0 en caso controario
        print("despues del read")
        local error = (string.byte(ret,3) or 0)
        print("despues de capturar el 3er byte")
        if(error == 1) then
            print "error, buffer lleno"
        else
            print "ok"
        end
    	return error
end
