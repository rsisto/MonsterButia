local device = _G

api={}
api.getTemp = {}
api.getTemp.parameters = {} -- -- no input parameters
api.getTemp.returns = {[1]={rname="par1", rtype="int"}} 
api.getTemp.call = function ()
	device:send(string.char(0x00)) -- codigo de operacion 0
	local sen_anl_response = device:read(3) 
	local raw_val	
	if not sen_anl_response or string.byte(sen_anl_response or "00000000", 2) == nil or string.byte(sen_anl_response or "00000000", 3) == nil
	then 
		raw_val = "nil value"
	else
		raw_val = string.byte(sen_anl_response, 2)* 256 + string.byte(sen_anl_response, 3)

		local aux = (raw_val * 500) / 1024  --- obtengo la temperatura en grados

                if aux > 180 then
                	raw_val = (raw_val * 45) / 1024 
                else 
			raw_val = aux
		end
                
	end	
	return raw_val 
	
end
