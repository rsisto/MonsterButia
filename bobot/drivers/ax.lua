local device = _G
local WRITE_INFO = 0x01
local READ_INFO  = 0x02
local GET_RAW_POS = 0x03
local char000    = string.char(0,0,0)
local mode='wheel'

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

--- Set wheel mode.
--Set the motor to continuous rotation mode.
api.wheel_mode = {}
api.wheel_mode.parameters = {[1]={rname="id", rtype="number", min=0, max=255}}
api.wheel_mode.returns = {}
api.wheel_mode.call = function (id )
        id = tonumber(id)
		local ret = device:send(string.char(WRITE_INFO,id,0x06,0x00,0x00))
        local write_info_response = device:read(1) or string.char(0,0)
		local ret = device:send(string.char(WRITE_INFO,id,0x08,0x00,0x00))
        local write_info_response = device:read(1) or string.char(0,0)
        mode='wheel'
    end

--- Set joint mode.
-- Set the motor to joint mode. Angles are provided in degrees,
-- in the full servo coverage (0 - 300 degrees arc)
-- @param min the minimum joint angle (defaults to 0)
-- @param max the maximum joint angle (defaults to 300)
api.joint_mode = {}
api.joint_mode.parameters = {[1]={rname="id", rtype="number", min=0, max=255},[2]={rname="minimo", rtype="number", min=0, max=1023},[3]={rname="maximo", rtype="number", min=0, max=1023}}
api.joint_mode.returns = {}
api.joint_mode.call = function (id ,minimo, maximo)
        id = tonumber(id)
        minimo=tonumber(minimo)
        maximo=tonumber(maximo)
		local ret = device:send(string.char(WRITE_INFO,id,0x06,math.floor(minimo / 256),minimo % 256))
        local write_info_response = device:read(1) or string.char(0,0)
		local ret = device:send(string.char(WRITE_INFO,id,0x08,math.floor(maximo / 256),maximo % 256))
        local write_info_response = device:read(1) or string.char(0,0)
        mode='joint'
    end


--- Set motor position.
-- Set the target position for the motor's axle. Only works in
-- joint mode.
-- @param value Angle in degrees, in the 0 .. 300deg range.
api.set_position = {}
api.set_position.parameters = {[1]={rname="id", rtype="number", min=0, max=255},[2]={rname="pos", rtype="number", min=0, max=1023}}
api.set_position.returns = {}
api.set_position.call = function (id, pos )
        id = tonumber(id)
        pos = tonumber(pos)
        local ret = device:send(string.char(WRITE_INFO,id,0x1E,math.floor(pos / 256),pos % 256))
        local write_info_response = device:read(1) or string.char(0,0)
    end


--- Get motor position.
-- Read the axle position from the motor.
-- @return The angle in deg. The reading is only valid in the 
-- 0 .. 300deg range
api.get_position = {}
api.get_position.parameters = {[1]={rname="id", rtype="number", min=0, max=255},[2]={rname="pos", rtype="number", min=0, max=1023}}
api.get_position.returns = {[1]={rname="motor_position", rtype="number"}} --one return
api.get_position.call = function(id)
        local send_response = device:send(string.char(GET_RAW_POS,id))
		local value = device:read(3) or string.char(0,0,0)
        local h_value = string.byte(value, 2)
        local l_value = string.byte(value, 3)
        local raw_angle = 256 * h_value + l_value
		local ang=0.29*(raw_angle) -- deg ?
		return raw_angle 

	end


--- Set motor speed.
-- @param value If motor in joint mode, speed in deg/sec in the 1 .. 684 range 
-- (0 means max available speed). 
-- If in wheel mode, as a fraction of max torque (in the -1 .. 1 range).
api.set_speed = {}
api.set_speed.parameters = {[1]={rname="id", rtype="number", min=0, max=255},[2]={rname="speed", rtype="number", min=-1, max=684}}
api.set_speed.returns = {} --no return
api.set_speed.call = function(id, speed)
	--if mode=='joint' then
		-- 0 .. 684 deg/sec
		local vel=math.floor(speed * 1.496)
		local lowb = math.floor(vel/ 256)
		local highb = vel % 256
	
		
		--local lowb, highb = get2bytes_unsigned(vel)
		local ret = device:send(id,0x20,string.char(lowb,highb))
		print ("ret= ",ret, " lowb= ", lowb, " highb= ", highb)
		if ret then return ret:byte() end
	--else --mode=='wheel'
		-- -1 ..  +1 max torque
		--local vel=math.floor(speed * 1023)
		--local lowb, highb = get2bytes_signed(vel)
		--local ret = device:send(id,0x20,string.char(lowb,highb))
		--if ret then return ret:byte() end
	--end
end


