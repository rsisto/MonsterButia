local device = _G

api={}
api.step = {}
api.step.parameters = {[1]={rname="step", rtype="int", min=0, max=65536}}
api.step.returns = {}
api.step.call = function (freq)
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

api.phasetype = {}
api.phasetype.parameters = {[1]={rname="phasetype", rtype="int", min=0, max=2}}
api.phasetype.returns = {}
api.phasetype.call = function (phasetype)
	local msg = string.char(0x04) .. string.char(phasetype)
	device:send(msg)
end

api.power_on = {}
api.power_on.parameters = {[1]={rname="power_on", rtype="int", min=0, max=1}}
api.power_on.returns = {}
api.power_on.call = function (on)
	local msg = string.char(0x06) .. string.char(on)
	device:send(msg)
end

api.direction = {}
api.direction.parameters = {[1]={rname="direction", rtype="int", min=-1, max=1}}
api.direction.returns = {}
api.direction.call = function (dir)
	if dir==-1 then dir=2 end
	local msg = string.char(0x07) .. string.char(dir+1)
	device:send(msg)
end


