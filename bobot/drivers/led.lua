local device = _G

local string_char=string.char

api={}
api.setLight = {}
api.setLight.parameters = {[1]={rname="message", rtype="string"}} 
api.setLight.returns = {} 
api.setLight.call = function (intensidad)
	intensidad=tonumber(intensidad)
	if (not intensidad) or intensidad<0 then intensidad=0
	elseif intensidad>255 then intensidad=255 end

	local msg = string_char(0x00) .. string_char(math.floor(intensidad)) -- entre 0 y 255
	device:send(msg)	
	local version_response = device:read() 	
end
