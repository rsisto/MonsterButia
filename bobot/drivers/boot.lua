local device = _G
local RESET = string.char(0xFF)

api={}
api.reset = {}
api.reset.parameters = {} --no parameters
api.reset.returns = {} --no returns
api.reset.call = function ()
	device:send(RESET)
end

