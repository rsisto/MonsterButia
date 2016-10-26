local device = _G
local SET_VEL_ADL = 0x00 -- código de op para mover el motor hacia adelante
local SET_VEL_ATR = 0x01 -- código de op para mover el motor hacia atrás


api={}
api.setveladl = {}
api.setveladl.parameters = {[1]={rname="id", rtype="int"}, [2]={rname="vel", rtype="int"}} --primer parametro id motor, segundo velocidad
api.setveladl.returns = {[1]={rname="dato", rtype="int"}} --codigo de operación
api.setveladl.call = function (id, vel)
	local msg = string.char(SET_VEL_ADL,id, math.floor(vel / 256),vel % 256)
	device:send(msg)
	local ret = device:read(1)
	local raw_val = string.byte(ret or " ", 1) 
	return raw_val	 
end

api.setvelatr = {}
api.setvelatr.parameters = {[1]={rname="id", rtype="int"}, [2]={rname="vel", rtype="int"}} --primer parametro id motor, segundo velocidad
api.setvelatr.returns = {[1]={rname="dato", rtype="int"}} --codigo de operación
api.setvelatr.call = function (id, vel)
	local msg = string.char(SET_VEL_ATR,id, math.floor(vel / 256),vel % 256)
	device:send(msg)
	local ret = device:read(1)
	local raw_val = string.byte(ret or " ", 1) 	
	return raw_val	 
end


api.setvelatr2 = {}
api.setvelatr2.parameters = {[1]={rname="id", rtype="int"}, [2]={rname="vel", rtype="int"}} --primer parametro id motor, segundo velocidad
api.setvelatr2.returns = {[1]={rname="dato", rtype="int"}} --codigo de operación
api.setvelatr2.call = function (vel)
	local msg = string.char(SET_VEL_ATR,0, math.floor(vel / 256),vel % 256)
	device:send(msg)
	local msg = string.char(SET_VEL_ATR,1, math.floor(vel / 256),vel % 256)
	device:send(msg)
	local ret = device:read(1)
	local ret = device:read(1)
	local raw_val = string.byte(ret or " ", 1) 	
	return raw_val	 
end
