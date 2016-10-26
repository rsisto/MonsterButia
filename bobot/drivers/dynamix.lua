local device = _G
local WRITE_INFO = 0x01
local SEND_BUS   = 0x01
local READ_INFO  = 0x02
local GET_RAW_POS = 0x03
local char000    = string.char(0,0,0)

--byte id,byte regstart, int value
api={}
api.write_info = {}
api.write_info.parameters = {[1]={rname="id", rtype="number", min=0, max=255},[2]={rname="regstart", rtype="number", min=0, max=255},[3]={rname="value", rtype="number", min=0, max=65536}} ----byte id,byte regstart, int value
api.write_info.returns = {[1]={rname="write_info_return", rtype="number"}} --one return
api.write_info.call = function (id, regstart, value)
    id, regstart, value = tonumber(id), tonumber(regstart), tonumber(value)
    local write_info_payload = string.char(WRITE_INFO, id, regstart, math.floor(value / 256),value % 256) 
    device:send(write_info_payload)
    local write_info_response = device:read(2) or char000
    local raw_val = (string.byte(write_info_response, 2) or 0) 
    return raw_val
end

api.pasarela = {}
api.pasarela.parameters = {[1]={rname="packet", rtype="string", min=0, max=255}}
api.pasarela.returns = {}
api.pasarela.call = function (value)
	local valueN = tonumber(value)
	local checksum = 255 - ((0xfe + 0x04 + 0x03 + 0x19 +valueN)%256)
	print (checksum)
	local paquete = string.char(SEND_BUS, 0xff, 0xff,0xfe, 0x04, 0x03, 0x19, valueN, checksum)
	device:send(paquete)
end



