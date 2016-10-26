local device = _G
local SEN_DIG = string.char(0x03) -- consulta un sensor digital
local SEN_ANL = string.char(0x00) -- consulta un sensor analogico 
local SEN_ I2C = string.char(0x02) -- consulta un sensor i2c 

api={}
api.senanl = {}
api.senanl.parameters = {[1]={rname="idPin", rtype="int"}} -- -- el parámetro indica el pin que queremos leer de la arduino
api.senanl.returns = {[1]={rname="par1", rtype="int"}} --{[1]={rname="par1", rtype="int"}, [2]={rname="par2", rtype="int"}} --two return
api.senanl.call = function (idPin)
	device:send(string.char(0x00, idPin)) -- codigo de operacion 0
	local sen_anl_response = device:read(3) 
	local raw_val
	if not sen_anl_response or string.byte(sen_anl_response or "00000000", 2) == nil or string.byte(sen_anl_response or "00000000", 3) == nil
	then 
		raw_val = "nil value"
	else
		raw_val = string.byte(sen_anl_response or "00000000", 2)* 256 + string.byte(sen_anl_response or "00000000", 3)
	end	
	--local raw_val2 = string.byte(set_anl_response or " ", 3) 
	return raw_val --string.char(raw_val ,raw_val2)
end

api.sendig = {}
api.sendig.parameters = {[1]={rname="idPin", rtype="int"}} -- el parámetro indica el pin que queremos leer de la arduino
api.sendig.returns = {[1]={rname="lecturaSensor", rtype="int"}} 
api.sendig.call = function (idPin)
	device:send(string.char(0x01, idPin))  -- codigo de operacion 1
	local sen_dig_response = device:read(3) 
	local raw_val
	--if not sen_dig_response
	--then
	--	raw_val = "nil value"
	--else	
		raw_val = string.byte(sen_dig_response or "0000", 2)* 256 + string.byte(sen_dig_response or "0000", 3) + 0		
	--end
	return raw_val
		
end


--api.seni2c = {}
--api.seni2c.parameters = {} -- no params
--api.seni2c.returns = {[1]={rname="par1", rtype="int"}, [2]={rname="par2", rtype="int"}} --two return
--api.seni2c.call = function ()
--	local get_sen_i2c =  SET_I2C
--	device:send(get_sen_i2c) 
--	local sen_i2c_response = device:read(2) 
--	local raw_val = string.byte(set_i2c_response, 2) 
--	return raw_val
--end

