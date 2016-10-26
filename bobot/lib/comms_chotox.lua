local bobot_device = require("bobot_device")

local comms_chotox = {}

function comms_chotox.send(endpoint, data, timeout)
end

function comms_chotox.read(endpoint, len, timeout)
end


function comms_chotox.init(baseboards)
	--parameters sanity check
	assert(type(baseboards)=="table")

	local n_boards=1
	--local bb = bobot_baseboard.BaseBoard:new({idBoard=iSerial, comms=comms_usb})
	local bb = {idBoard=1, comms=comms_chotox}
	local devices={}
	local is_hotplug= {button=true, led=true, grey=true, distanc=true}
	for i, name in ipairs({"button", "grey", "distanc","butia"}) do
		local dd={name=name, module=name, baseboard=bb, handler=i}
		dd.open = function() return true end
		dd.close = function() end
		dd.read = function() return "" end
		dd.send = function() return true end
		if is_hotplug[name] then dd.hotplug=true end

		local d = bobot_device:new(dd) -- in_endpoint=0x01, out_endpoint=0x01})

		devices[name]=true
		devices[#devices+1]=d
	end
	bb.devices=devices
	baseboards[1]=bb
	return n_boards
end

return comms_chotox
