local device = _G

-- descripción: permite conocer el estado el botón en un momento dado.
-- entrada: no tiene.
-- salida: estado del botón. Posibles estados: 1 presionado, 0 libre.
api={}
api.getCampo = {}
api.getCampo.parameters = {} -- no tiene parámetros de entrada
api.getCampo.returns = {[1]={rname="par1", rtype="int"}} -- 1 = presionado, 0 = libre
api.getCampo.call = function ()
	device:send(string.char(0x00)) 			-- codigo de operacion = 0 
	local sen_dig_response = device:read(3) -- leo 2 bytes (opcode, data)
	local raw_val
	if not sen_dig_response or string.byte(sen_dig_response or "00000000", 2) == nil or string.byte(sen_dig_response or "00000000", 3) == nil 
	then 
		raw_val = "nil value"
	else
		raw_val =  1 - (string.byte(sen_dig_response, 3) % 2)
	end	
	return raw_val 
end
