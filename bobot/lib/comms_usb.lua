--module(..., package.seeall);

local bobot_baseboard = require("bobot_baseboard")
local bobot = require("bobot")

local my_path = debug.getinfo(1, "S").source:match[[^@?(.*[\/])[^\/]-$]]
assert(package.loadlib(my_path .. "libluausb.so","luaopen_libusb"))()
local libusb=_G.libusb; _G.libusb=nil

local usb_bulk_write = libusb.bulk_write
local usb_bulk_read = libusb.bulk_read

local USB4ALL_VENDOR        = 0x04d8
local USB4ALL_PRODUCT       = 0x000c
local USB4ALL_CONFIGURATION = 1
local USB4ALL_INTERFACE     = 0

local READ_HEADER_SIZE      = 3

local libusb_handler 


local comms_usb = {}

function comms_usb.send(endpoint, data, timeout)
	--parameters sanity check
	assert(type(libusb_handler)=="userdata")
	assert(type(endpoint)=="number")
	assert(type(data)=="string")
	assert(type(timeout)=="number")

	return usb_bulk_write(libusb_handler, endpoint, data, timeout)
end

function comms_usb.read(endpoint, len, timeout)
	--parameters sanity check
	assert(type(libusb_handler)=="userdata")
	assert(type(endpoint)=="number")
	assert(type(len)=="number")
	assert(type(timeout)=="number")

	return usb_bulk_read(libusb_handler, endpoint, len+READ_HEADER_SIZE, timeout)
end


function comms_usb.init(baseboards)
	--parameters sanity check
	assert(type(baseboards)=="table")


	--refresh devices and buses
	libusb.find_busses()
	libusb.find_devices()
	local n_boards = 0

	local buses=libusb.get_busses()
	for dirname, bus in pairs(buses) do 			--iterate buses
		local devices=libusb.get_devices(bus)
		for filename, device in pairs(devices) do	--iterate devices
			local descriptor = libusb.device_descriptor(device)

			--if device is baseboard...
			if ((descriptor.idVendor == USB4ALL_VENDOR) and (descriptor.idProduct == USB4ALL_PRODUCT)) then
				--try to intialize baseboard
				bobot.debugprint("Initializing Baseboard:", descriptor.idVendor, descriptor.idProduct)
				libusb_handler = libusb.open(device)

				if not libusb_handler then
					bobot.debugprint("Error opening device")
					break
				end				
				if not libusb.set_configuration(libusb_handler, USB4ALL_CONFIGURATION) then
					bobot.debugprint("Error configuring device, retrying after a reset")
					libusb.reset(libusb_handler)				
					if not libusb.set_configuration(libusb_handler, USB4ALL_CONFIGURATION) then
						bobot.debugprint("Error configuring device.")
						break
					end
				end
				if not libusb.claim_interface(libusb_handler, USB4ALL_INTERFACE) then
					bobot.debugprint("Error seting device interface")
					break
				end

				--success initializing, instantiate BaseBoard object and register
				local iSerial=descriptor.iSerialNumber
				local bb = bobot_baseboard:new({idBoard=iSerial, comms=comms_usb})
				--bb:force_close_all()
				--bobot.debugprint("Baseboard:", iSerial)

				baseboards[#baseboards+1]=bb
				n_boards = n_boards + 1
			end
		end
	end
	return n_boards 
end

return comms_usb
