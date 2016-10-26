local device = _G
local GET_MOVE = string.char(0x01)
local char000 = string.char(0,0,0)

api={}
api.get_move = {}
api.get_move.parameters = {} --no parameter
api.get_move.returns = {[1]={rname="digital_value", rtype="number"}} --one return
api.get_move.call = function ()
	local get_move_payload = GET_MOVE 
	device:send(get_move_payload)
	local move_response = device:read(2) or char000
	local raw_val = (string.byte(move_response, 2) or 0)
	return raw_val
end

