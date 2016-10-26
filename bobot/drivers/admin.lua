local device = _G
local RESET = string.char(0xFF)

api={}
api.reset = {}
api.reset.parameters = {}
api.reset.returns = {}
api.reset.call = function (data)
	device:send(RESET)
end

