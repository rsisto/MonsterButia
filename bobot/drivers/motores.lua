local device = _G
local SET_VEL_MTR = 0x00 -- código de op para mover un motor con vel y sentido
local SET_VEL_2MTR = 0x01 -- código de op para mover dos motores con vel y sentido


api={}
api.setvelmtr = {}
api.setvelmtr.parameters = {[1]={rname="id", rtype="int"},[2]={rname="sentido", rtype="int"},[3]={rname="vel", rtype="int"}} --parametros, id sentido vel
api.setvelmtr.returns = {[1]={rname="dato", rtype="int"}} --codigo de operación
api.setvelmtr.call = function (id, sentido, vel)
	vel=tonumber(vel)
	if vel>1023 then vel=1023 end
	local msg = string.char(SET_VEL_MTR,id, sentido, math.floor(vel / 256),vel % 256)
	device:send(msg)
	local ret = device:read(1)
	local raw_val = string.byte(ret or " ", 1) 
	return raw_val	 
end

api.setvel2mtr = {}
api.setvel2mtr.parameters = {[1]={rname="sentido", rtype="int"},[2]={rname="vel", rtype="int"},[3]={rname="sentido", rtype="int"},[4]={rname="vel", rtype="int"}} 
api.setvel2mtr.returns = {[1]={rname="dato", rtype="int"}} --codigo de operación
api.setvel2mtr.call = function (sentido1, vel1, sentido2, vel2)
	vel1, vel2 = tonumber(vel1), tonumber(vel2)
	if vel1>1023 then vel1=1023 end
	if vel2>1023 then vel2=1023 end
	local msg = string.char(SET_VEL_2MTR,sentido1, math.floor(vel1 / 256),vel1 % 256, sentido2, math.floor(vel2 / 256),vel2 % 256)
	device:send(msg)
	local ret = device:read(1)
	local raw_val = string.byte(ret or " ", 1) 	
	return raw_val	 
end


api.setvelatr2 = {}
api.setvelatr2.parameters = {[1]={rname="id", rtype="int"}, [2]={rname="vel", rtype="int"}} --primer parametro id motor, segundo velocidad
api.setvelatr2.returns = {[1]={rname="dato", rtype="int"}} --codigo de operación
api.setvelatr2.call = function (vel)
	local vdiv, vmod = math.floor(vel / 256),vel % 256
	local msg = string.char(SET_VEL_ATR, 0, vdiv, vmod)
	device:send(msg)
	msg = string.char(SET_VEL_ATR, 1, vdiv, vmod)
	device:send(msg)
	local ret = device:read(1)
	ret = device:read(1)
	local raw_val = string.byte(ret or " ", 1) 	
	return raw_val	 
end
