local device = _G

api={}
api.speed = {}
api.speed.parameters = {[1]={rname="freq", rtype="int", min=0, max=65536}}
api.speed.returns = {}
api.speed.call = function (freq)
	local msg = string.char(0x02) .. string.char(math.floor(freq / 256)) .. string.char(freq % 256)
	device:send(msg)
	device:read(1)
end

api.steps = {}
api.steps.parameters = {[1]={rname="number", rtype="int", min=0, max=65536}}
api.steps.returns = {}
api.steps.call = function (number)
	local msg = string.char(0x03) .. string.char(math.floor(number / 256)) .. string.char(number % 256)
	device:send(msg)
	device:read(1)
end

api.on = {}
api.on.parameters = {[1]={rname="direction", rtype="int", min=-1, max=1}}
api.on.returns = {}
api.on.call = function (dir)
	local msg = string.char(0x01) .. string.char(dir+1)
	device:send(msg)
end
