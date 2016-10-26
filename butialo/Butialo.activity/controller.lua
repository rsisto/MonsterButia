local socket = require "socket"
local gettime = socket.gettime

--disable for production
--require "strict"

local MIN_DT = 0.001

local M={}

local eval_pid = function ( self, actual, setpoint )
	if setpoint then self.setpoint=setpoint end
	local t = gettime()
	local err, now, dt, dpv_dt, out
	local sum_err = 0
	local actual_ant = actual
	while true do
		err = self.setpoint - actual
		now = gettime()
		dt = now - t
		t = now

		sum_err = sum_err + (err*dt/2)

		if dt~=0 then 
			dpv_dt = (actual-actual_ant)/dt
		else
			dpv_dt = 0
		end

		out = self.Kp * ( err + sum_err/self.Ti + self.Td*dpv_dt )

		if out>self.out_max then out=self.out_max
		elseif out<self.out_min then out=self.out_min end

		actual_ant = actual
		self, actual, setpoint = coroutine.yield(out)
		if setpoint then self.setpoint=setpoint end
	end
end

local eval_pi = function ( self, actual, setpoint )
	if setpoint then self.setpoint=setpoint end
	local t = gettime()
	local err, now, dt, out
	local sum_err = 0
	while true do
		err = self.setpoint - actual
		now = gettime()
		dt = now - t
		t = now

		sum_err = sum_err + (err*dt/2)

		out = self.Kp * ( err + sum_err/self.Ti )

		if out>self.out_max then out=self.out_max
		elseif out<self.out_min then out=self.out_min end

		self, actual, setpoint = coroutine.yield(out)
		if setpoint then self.setpoint=setpoint end
	end
end

local eval_p = function ( self, actual, setpoint )
	if setpoint then self.setpoint=setpoint end
	local t = gettime()
	local err, now, dt, out
	while true do
		err = self.setpoint - actual
		now = gettime()
		dt = now - t
		t = now

		out = self.Kp * err

		if out>self.out_max then out=self.out_max
		elseif out<self.out_min then out=self.out_min end

		self, actual, setpoint = coroutine.yield(out)
		if setpoint then self.setpoint=setpoint end
	end
end

M.get_pid = function ( controller )
	controller=controller or {}
	controller.Kp = controller.Kp or 1
	controller.Ti = controller.Ti or 1
	controller.Td = controller.Td or 0
	controller.out_min = controller.out_min or -math.huge
	controller.out_max = controller.out_max or math.huge
	controller.setpoint = controller.setpoint or 0
	local mt = {__call=coroutine.wrap(eval_pid)}
	setmetatable(controller, mt)
	return controller
end

M.get_pi = function ( controller )
	controller=controller or {}
	controller.Kp = controller.Kp or 1
	controller.Ti = controller.Ti or 1
	controller.out_min = controller.out_min or -math.huge
	controller.out_max = controller.out_max or math.huge
	controller.setpoint = controller.setpoint or 0
	local mt = {__call=coroutine.wrap(eval_pi)}
	setmetatable(controller, mt)
	return controller
end

M.get_p = function ( controller )
	controller=controller or {}
	controller.Kp = controller.Kp or 1
	controller.out_min = controller.out_min or -math.huge
	controller.out_max = controller.out_max or math.huge
	controller.setpoint = controller.setpoint or 0
	local mt = {__call=coroutine.wrap(eval_p)}
	setmetatable(controller, mt)
	return controller
end

return M


--[[
M.get_controller_pid = function ( Kp, Ki, Kd, out_min, out_max )
	local co = coroutine.create( 
		function( actual, setpoint )
			local t = gettime()
			local err = setpoint - actual
			local err_prev, sum_err = err, 0
			local derr_dt, now, dt, out
			while true do
				err = setpoint - actual
				now = gettime()
				dt = now - t
				if dt > MIN_DT then
					t = now
					sum_err = sum_err + (err*dt/2)
					derr_dt = (err - err_prev) / dt
					out = (Kp*err) + (Ki*sum_err) + (Kd*derr_dt)
					if out>out_max then out=out_max
					elseif out<out_min then out=out_min end
					err_prev = err
				else
					out = 0
				end
				actual, setpoint = coroutine.yield(out)
			end
		end
	)
	return co
end
--]]

--[[
local controller = M.get_controller({Kp=0.01, Ti=100, Td=0, out_min=-1, out_max=1, setpoint=arg[1]})
local v=0
local next_reft = gettime()+4
local init = gettime()
while true do
	local out = controller(v) --, math.sin(gettime()))
	--if not ok then error(out) end
	v=v+out
	print (controller.setpoint,v, string.format("%.3f", controller.setpoint-v ))
	--print(gettime()-init, v)
	if gettime()>next_reft then
		--os.exit()
		next_reft = gettime()+4
		controller.setpoint = math.random(5)
	end
	socket.sleep(math.random(0.00001,0.01))
	--socket.sleep(1)
end
--]]


