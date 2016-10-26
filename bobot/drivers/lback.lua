local device = _G

api={}
api.send = {}
api.send.parameters = {[1]={rname="data", rtype="string"}}
api.send.returns = {}
api.send.call = function (data)
	--print("####", data, string.len(data))
	device:send(data)
end

api.read = {}
api.read.parameters = {[1]={rname="len", rtype="int", default=64, min=0, max=255}}
api.read.returns = {[1]={rname="data", rtype="string"}}
api.read.call = function (len)
	local len = len or 64
	local ret = device:read(len)
--	print("====",ret, string.len(ret))
	return ret
end

