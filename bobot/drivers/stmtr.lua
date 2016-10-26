local device = _G

api={}
api.rawspeed = {}
api.rawspeed.parameters = {[1]={rname="speed1", rtype="int", min=-5, max=5}, [2]={rname="speed2", rtype="int", min=-5, max=5}}
api.rawspeed.returns = {}
api.rawspeed.call = function (speed1, speed2)
	local msg = string.char(0x02,speed1,speed2)
	device:send(msg)
	device:read()
end

api.t0cfg = {}
api.t0cfg.parameters = {[1]={rname="t0_low", rtype="int", min=0, max=255}, [2]={rname="t0_high", rtype="int", min=0, max=255}}
api.t0cfg.returns = {}
api.t0cfg.call = function (t0_low, t0_high)
	local msg = string.char(0x01,t0_low,t0_high)
	device:send(msg)
	device:read()
end

api={}
api.speed = {}
api.speed.parameters = {[1]={rname="vel1", rtype="float", min=-127, max=127}, 
			[2]={rname="vel2", rtype="float", min=-127, max=127}}
api.speed.returns = {}
api.speed.call = function (vel1, vel2)
	vel1=tonumber(vel1)
	vel2=tonumber(vel2)
	
	local speed1, speed2
	if vel1>=0 then
		speed1=vel1
	else
		speed1=0xFF + vel1 + 1
	end
	if vel2>=0 then
		speed2=vel2
	else
		speed2=0xFF + vel2 + 1
	end

	local msg = string.char(0x02,speed1,speed2)
	device:send(msg)
	device:read()
end
