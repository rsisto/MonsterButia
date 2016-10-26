#!/usr/bin/lua

--module(..., package.seeall);

local my_path = debug.getinfo(1, "S").source:match[[^@?(.*[\/])[^\/]-$]]

local bobot_device = require("bobot_device")
local bobot = require("bobot")

local NULL_BYTE = string.char(0x00)
local DEFAULT_PACKET_SIZE = 0x04
local GET_USER_MODULES_SIZE_COMMAND = string.char(0x05)
local GET_USER_MODULE_LINE_COMMAND = string.char(0x06)
local GET_HANDLER_SIZE_COMMAND = string.char(0x0A)
local GET_HANDLER_TYPE_COMMAND = string.char(0x0B)
local GET_LINES_RESPONSE_PACKET_SIZE = 5
local GET_LINE_RESPONSE_PACKET_SIZE = 12
local GET_HANDLER_TYPE_PACKET_SIZE = 5
local GET_HANDLER_RESPONSE_PACKET_SIZE = 5 --
local ADMIN_HANDLER_SEND_COMMAND = string.char(0x00)
local ADMIN_MODULE_IN_ENDPOINT = 0x01
local ADMIN_MODULE_OUT_ENDPOINT = 0x81
local GET_USER_MODULE_LINE_PACKET_SIZE = 0x05
local CLOSEALL_BASE_BOARD_COMMAND = string.char(0x07) 
local CLOSEALL_BASE_BOARD_RESPONSE_PACKET_SIZE = 5
local TIMEOUT = 250 --ms
local MAX_RETRY = 5

local BaseBoard = {}


--executes s on the console and returns the output
local function run_shell (s) 
	local f = io.popen(s) -- runs command
	local l = f:read("*a") -- read output of command
	f:close()
	return l
end

openable = {}
hotplug = {}

local function parse_drivers()
	local driver_files=run_shell("sh -c 'ls "..my_path.."../drivers/*.lua 2> /dev/null'")
	for filename in driver_files:gmatch('drivers%/(%S+)%.lua') do
		--print ("Driver openable", filename)
		openable[filename] = true
	end
	driver_files=run_shell("sh -c 'ls "..my_path.."../drivers/hotplug/*.lua 2> /dev/null'")
	for filename in driver_files:gmatch('drivers%/hotplug%/(%S+)%.lua') do
		--print ("Driver hotplug", filename)
		hotplug[filename] = true
	end
end
parse_drivers()

local function load_modules(bb)
	local retry = 0
	bb.modules = {}

	local n_modules=bb:get_user_modules_size()
	while n_modules == nil and retry < MAX_RETRY do
		n_modules=bb:get_user_modules_size()
		bobot.debugprint("u4b:the module list size returned a nil value, trying to recover...")
		retry = retry+1
	end
	if not n_modules then return nil end
	retry=0
	bobot.debugprint ("Reading modules:", n_modules)
	for i = 1, n_modules do
		local modulename=bb:get_user_module_line(i)
		while modulename == nil and retry < MAX_RETRY do
			bobot.debugprint("u4b:the module  returned a nil value, trying to recover...")
			modulename=bb:get_handler_type(i)
			retry = retry+1
		end
		if not modulename then return nil end
		if openable[modulename] then
			bb.modules[i]=modulename
			local d = bobot_device:new({
				module=modulename,
				--name=module, 
				baseboard=bb,
				hotplug=false,
				in_endpoint=0x01, out_endpoint=0x01,
			}) -- in_endpoint=0x01, out_endpoint=0x01})
			bb.devices[d]=true
			bb.devices[#bb.devices+1]=d
			
			bb.modules[modulename]=d
		elseif hotplug[modulename] then
			bb.modules[i]=modulename
			bb.modules[modulename]=true
			bb.hotplug = true -- bb has a hotplug module
		else
			bobot.debugprint("Loading modules: missing driver for",modulename)
		end
	end
	return true
end

local function load_module_handlers(bb)
	local retry = 0
	
	local n_module_handlers=bb:get_handler_size()
	while n_module_handlers == nil and retry < MAX_RETRY do
		n_module_handlers=bb:get_handler_size()
		bobot.debugprint("u4b:the module handler list size returned a nil value, trying to recover...")
		retry = retry+1
	end
	if (not n_module_handlers) or (n_module_handlers > 32) then return nil end
	retry=0
	bobot.debugprint ("Reading moduleshandlers:", n_module_handlers)
	for i=1, n_module_handlers do
		local t_handler = bb:get_handler_type(i)
		while(t_handler == nil and retry < MAX_RETRY) do
			bobot.debugprint("u4b:the module handler returned a nil value, trying to recover...")
			t_handler = bb:get_handler_type(i)
			retry = retry+1
		end
		if not t_handler then return nil end
		if t_handler<255  then
			local modulename=bb.modules[t_handler+1]
			local moduledev=bb.modules[modulename]
			if type(moduledev)=='table' 
			and not moduledev.handler then
				moduledev.handler=i-1
			elseif moduledev==true then 
				--name=name.."@"..(i-1)
				local d = bobot_device:new({
					handler=i-1,
					module=modulename,
					--name=name, 
					baseboard=bb,
					hotplug=(hotplug[modulename]),
					in_endpoint=0x01, out_endpoint=0x01,
				}) -- in_endpoint=0x01, out_endpoint=0x01})
				if d then 
					bb.devices[d]=true
					bb.devices[#bb.devices+1]=d
				end
			else
				bobot.debugprint ("No opened device!")
			end
		end
	end
	return true
end


function BaseBoard:refresh()
	self.devices = {}

	if not load_modules(self) then
		return nil,"failure reading modules"
	end

	if not load_module_handlers(self) then
		return nil,"failure reading module handlers"
	end
	return true
end

--Instantiates BaseBoard object.
--Loads list of modules installed on baseboard
function BaseBoard:new(bb)
	--parameters sanity check
	assert(type(bb)=="table")
	assert(type(bb.comms)=="table")

	--OO boilerplate
   	setmetatable(bb, self)
	self.__index = self
	
	bb:refresh()
	
--bobot.debugprint ('----------------')
	--bb:force_close_all()
--bobot.debugprint ('================')
	return bb
end

--Closes all modules opened on baseboard
function BaseBoard:close()
	--state sanity check
	assert(type(self.devices)=="table")

	for _,d in ipairs(self.devices) do
		if type(d.handler)=="number" then
			bobot.debugprint ("closing", d.name, d.handler)
			d:close()
		end
	end

	--TODO actually close the baseboard
end

--returns number of modules present on baseboard
function BaseBoard:get_user_modules_size()
	--state sanity check
	assert(type(self.comms)=="table")

	local comms=self.comms

	-- In case of get_user_modules_size command is atended by admin module in handler 0 and send operation is 000

	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. string.char(DEFAULT_PACKET_SIZE) .. NULL_BYTE
	local admin_packet = GET_USER_MODULES_SIZE_COMMAND
	local get_user_modules_size_packet  = handler_packet .. admin_packet    

	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, get_user_modules_size_packet, TIMEOUT)
	if write_res then
         	local data, err = comms.read(ADMIN_MODULE_OUT_ENDPOINT, GET_LINES_RESPONSE_PACKET_SIZE, TIMEOUT)
		if not data then
			bobot.debugprint("u4b:get_user_modules_size:comunication with I/O board read error", err)
			return 0
		else
			local user_modules_size = string.byte(data, 5)
			return user_modules_size
		end
	else	
		bobot.debugprint("u4b:get_user_modules_size:comunication with I/O board write error", write_res)
		return 0
   	end
end

--returns thename of a given (by a 1-based index)module 
function BaseBoard:get_user_module_line(index)
	--state & parameter sanity check
	assert(type(index)=="number")
	assert(index>0)	
	assert(type(self.comms)=="table")


	local comms=self.comms

	-- In case of get_user_module_line command is atended by admin module in handler 0 and send operation is 000
	local get_user_module_line_packet_length = string.char(GET_USER_MODULE_LINE_PACKET_SIZE)
	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. get_user_module_line_packet_length .. NULL_BYTE
	local admin_packet = GET_USER_MODULE_LINE_COMMAND .. string.char(index-1)
	local get_user_module_line_packet  = handler_packet .. admin_packet

	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, get_user_module_line_packet, TIMEOUT)
    	if write_res then
		local data, err = comms.read(ADMIN_MODULE_OUT_ENDPOINT, GET_LINE_RESPONSE_PACKET_SIZE, TIMEOUT)
		if not data then
			bobot.debugprint("u4b:get_user_modules_line:comunication with I/O board read error", err)
			return
		end
		--the name is between a header and a null
		local end_mark = string.find(data, "\000", GET_USER_MODULE_LINE_PACKET_SIZE, true)
		if not end_mark then
			bobot.debugprint ("u4b:get_user_module_line:Error parsing module name")
			return
		end
		local module_name = string.sub(data, GET_USER_MODULE_LINE_PACKET_SIZE, end_mark-1)
		return module_name
	else	
		bobot.debugprint("u4b:get_user_module_line:comunication with I/O board write error", write_res)
	end
end

function BaseBoard:get_handler_size() ------ NEW LISTI ------
	--state sanity check
	assert(type(self.comms)=="table")

	local comms=self.comms

	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. string.char(DEFAULT_PACKET_SIZE) .. NULL_BYTE
	local admin_packet = GET_HANDLER_SIZE_COMMAND
	local get_handler_size_packet  = handler_packet .. admin_packet    

	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, get_handler_size_packet, TIMEOUT)
	if write_res then
        local data, err = comms.read(ADMIN_MODULE_OUT_ENDPOINT, GET_HANDLER_RESPONSE_PACKET_SIZE, TIMEOUT)
		if not data then
			bobot.debugprint("u4b:get_handler_size:comunication with I/O board read error", err)
			return 0
		else
			local handler_size = string.byte(data, 5)	
			return handler_size
		end
	else	
		bobot.debugprint("u4b:get_handler_type:comunication with I/O board write error", write_res)
	end
end
function BaseBoard:get_handler_type(index) ------ NEW LISTI ------
	--state & parameter sanity check
	assert(type(index)=="number")
	assert(index>0)	
	assert(type(self.comms)=="table")

	local comms=self.comms

	-- In case of get_handler_type command is atended by admin module in handler 0 and send operation is 000
	local get_handler_type_packet_length = string.char(GET_HANDLER_TYPE_PACKET_SIZE) --GET_USER_MODULE_LINE_PACKET_SIZE
	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. get_handler_type_packet_length .. NULL_BYTE
	local admin_packet = GET_HANDLER_TYPE_COMMAND .. string.char(index-1)
	local get_handler_type_packet  = handler_packet .. admin_packet

	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, get_handler_type_packet, TIMEOUT)
    if write_res then
		local data, err = comms.read(ADMIN_MODULE_OUT_ENDPOINT, GET_HANDLER_RESPONSE_PACKET_SIZE, TIMEOUT)
		if not data then
			bobot.debugprint("u4b:get_handler_type:comunication with I/O board read error", err)
			return 0
		else
			local handler_type = string.byte(data, 5)	
			return handler_type
		end
	else	
		bobot.debugprint("u4b:get_handler_type:comunication with I/O board write error", write_res)
	end
end

-- resets the baseboard, after this operation the baseboard will claim reenumeration to the operative system
-- this function is deprecated by force_close_all
function BaseBoard:close_all()
	for _,d in ipairs(self.devices) do
		--bobot.debugprint ("===", d.name,d.handler)
		if d.handler then d:close() end
	end
end

-- switch the baseboard to the bootloader program implementend as a usb4all command to the admin module
function BaseBoard:switch_to_bootloader()
	--state & parameter sanity check
	assert(type(self.comms)=="table")
	
	local comms=self.comms
	-- In case of reset_base_board command is atended by admin module in handler 0 and send operation is 000
	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. string.char(DEFAULT_PACKET_SIZE) .. NULL_BYTE
	local admin_packet = string.char(0x09)  --SWITCH_TO_BOOT_BASE_BOARD_COMMAND
	local boot_base_board_packet  = handler_packet .. admin_packet

	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, boot_base_board_packet, TIMEOUT)
    --from this moment the board is reseted, so there is nothing more to do
end

function BaseBoard:reset()
	--state & parameter sanity check
	assert(type(self.comms)=="table")
	
	local comms=self.comms
	-- In case of reset_base_board command is atended by admin module in handler 0 and send operation is 000
	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. string.char(DEFAULT_PACKET_SIZE) .. NULL_BYTE
	local admin_packet = string.char(0xFF)  --RESET_BASE_BOARD_COMMAND
	local reset_base_board_packet  = handler_packet .. admin_packet

	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, reset_base_board_packet, TIMEOUT)
    	if write_res then
		-- no tego que leer respuesta porque se reseteo
		--libusb.close(libusb_handler)
		--self.libusb_handler=nil
		--for d_name,d in pairs(self.devices) do
			--bobot.debugprint ("===", d.name,d.handler)
		--	d.handler=nil
		--end
	else	
		bobot.debugprint("u4b:reset:comunication with I/O board write error", write_res)
	end
end

function BaseBoard:force_close_all()
	--state & parameter sanity check
	assert(type(self.comms)=="table")
	
	local comms=self.comms
	-- In case of reset_base_board command is atended by admin module in handler 0 and send operation is 000
	local handler_packet = ADMIN_HANDLER_SEND_COMMAND .. string.char(DEFAULT_PACKET_SIZE) .. NULL_BYTE
	local admin_packet = CLOSEALL_BASE_BOARD_COMMAND
	local reset_base_board_packet  = handler_packet .. admin_packet

--bobot.debugprint ('%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%')
	local write_res = comms.send(ADMIN_MODULE_IN_ENDPOINT, reset_base_board_packet, TIMEOUT)
--bobot.debugprint ('%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%')
    	if write_res then
		local data, err = comms.read(ADMIN_MODULE_OUT_ENDPOINT,	CLOSEALL_BASE_BOARD_RESPONSE_PACKET_SIZE, TIMEOUT)
		if err then
			bobot.debugprint("u4b:force_close_all:comunication with I/O board read error",err)
		else
			--bobot.debugprint("u4b:force_close_all:libusb read",string.byte(data,1,string.len(data)))
		end
		for _,d in ipairs(self.devices) do
			--bobot.debugprint ("===", d.name,d.handler)
			d.handler=nil
		end
	else	
		bobot.debugprint("u4b:force_close_all:comunication with I/O board write error", write_res)
	end
end

return BaseBoard
