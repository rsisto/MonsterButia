module(..., package.seeall);

local bobot = require("bobot")

--local devices=devices
local process = require("bobot-server-process").process
local butia = require("butia/butia_http")
local util = require("http-util")

local parse_params = util.parse_params
local load_template = util.load_file
local find_page = util.find_page

--enable to cache in RAM
--[[
local index_template=load_template('indextemplate.txt')
local dump_template=load_template('dumptemplate.txt')
local dump_template_descr=load_template('dumptemplate_descr.txt')
local dump_template_descr_row=load_template('dumptemplate_descr_row.txt')
--]]

local function check_open_device(d, ep1, ep2)
	if not d then return end
	if d.handler then return true end
        -- if the device is not open, then open the device
	bobot.debugprint ("Opening", d.name, d.handler)
	return d:open(ep1 or 1, ep2 or 1) --TODO asignacion de ep?
end

local html_list_devices = function (params)
	local ret,comma = "", ""
	local dsel=params['dsel']
	for d_name, d in pairs(devices) do
		local broken=""
		if not check_open_device(d, ep1, ep2) then 
			broken=" (failed to open)"
			bobot.debugprint ("bs: WARN! Failure opening", d_name)
		end
		if dsel==d_name then
			ret = ret .. comma .. '<strong>' .. d_name .. broken .. '</strong>'
		else
			ret = ret .. comma .. '<a href="/dump.htm?dsel='..d_name..'">'..d_name..broken..'</a>'
		end
		comma=", "
	end
	return ret
end

local html_describe_device = function (params)
	--borrar
	dump_template_descr_row=load_template('dumptemplate_descr_row.txt')

	local dsel=params['dsel']
	local command=params['command']
	if not dsel then return "" end

	local device=devices[dsel]

	if not device then return "<TR><TD>Missing Device!</TD></TR>" end
	if not device.api then return "<TR><TD>Missing Driver for Device!</TD></TR>" end

	local ret = ""
	for fname, fdef in pairs(device.api) do
		local row=dump_template_descr_row

		local result,ok
		--print ("::",command,fname)
		if command==fname then
			--preparar parametros
			local fparams={}
			for i,param in ipairs(fdef.parameters) do
				local rname=param['rname']
				local rtype=param['rtype']
				if rtype=="int" or rtype=="number" or rtype=="numeric" then
					fparams[i]=tonumber(params[rname])
				else
					fparams[i]=params[rname]
				end
			end
			
			--ejecutar
			ok, result= pcall( fdef.call, unpack(fparams) )
			if not ok then bobot.debugprint ("Error calling", ret) end
			--imprimir
			bobot.debugprint ("::::",result)
		end

		local returns=''
		local comma=''
		for i,rets in ipairs(fdef.returns) do
			returns = returns..comma..rets['rtype']..'&nbsp;'..rets['rname']
			comma=','
		end
		local formfields=''
		local parameters=''
		local comma=''
		for i,rets in ipairs(fdef.parameters) do
			local rname=rets['rname']
			formfields=formfields..rname..': <INPUT TYPE="text" NAME="'..rname..'" /><br>'
			parameters = parameters..comma..rets['rtype']..'&nbsp;'..rname
			comma=','
		end

		local rep = {
			['COMMAND'] = fname,
			['RETURNS'] = returns,
			['PARAMETERS'] = parameters,
			['RESULT'] = result,
			['MODULENAME'] = dsel,
			['FORMFIELDS'] = formfields,
		}
		local generated_row=string.gsub(row, '<!%-%-(%w+)%-%->', rep)

		ret=ret..generated_row

	end

	return ret
end

local get_page={}
setmetatable(get_page, {__index = function(_,page) bobot.debugprint ("======", page);return find_page(page) end})
get_page["/index.htm"] = function (p)
	local index_template=load_template('indextemplate.txt')

	local params=parse_params(p)
	local rep = {
		['DATA1'] = params['campo'],
		['DATA2'] = tostring(params['unboton'])..', '..tostring(params['otroboton']),
	}
	local page=string.gsub(index_template, '<!%-%-(%w+)%-%->', rep)
	return "HTTP/1.1 200/OK\r\nContent-Type:text/html\r\nContent-Length: "..#page.."\r\n\r\n"..page
end
get_page["/"]=get_page["/index.htm"]
get_page["/dump.htm"] = function (p)
	--remove this
	dump_template=load_template('dumptemplate.txt')
	dump_template_descr=load_template('dumptemplate_descr.txt')

	local params=parse_params(p)
	local dsel=params['dsel']

	local usetemplate
	if dsel then
		usetemplate=dump_template_descr
	else
		usetemplate=dump_template
	end

	local rep = {
		['LIST'] = html_list_devices(params),
		['MODULENAME'] = dsel or "(empty dsel)",
		['ROWS'] = html_describe_device(params),
	}
	local page=string.gsub(usetemplate, '<!%-%-(%w+)%-%->', rep)
	return "HTTP/1.1 200/OK\r\nContent-Type:text/html\r\nContent-Length: "..#page.."\r\n\r\n"..page
end
get_page["/favicon.ico"] = function ()
	local served, err = io.open('bobot_server/favicon.ico', "rb")
	if served ~= nil then
		local content = served:read("*all")
		return "HTTP/1.1 200/OK\r\nContent-Type:image/x-icon\r\nContent-Length: "
			..#content.."\r\n\r\n" .. content
	else
		bobot.debugprint("Error opening favicon:", err)
		return default_page()
	end
end
get_page["/bobot.png"] = function ()
	local served, err = io.open('bobot_server/bobot.png', "rb")
	if served ~= nil then
		local content = served:read("*all")
		return "HTTP/1.1 200/OK\r\nContent-Type:image/png\r\nContent-Length: "
			..#content.."\r\n\r\n" .. content
	else
		bobot.debugprint("Error opening logo:", err)
		return default_page()
	end
end
butia.init(get_page)

function serve(skt)
	local line,err = skt:receive('*l') --read first line, must be GET or POST
	if err then return nil, err end

	local f,p=string.match(line, '^GET ([%/%.%d%w%-_]+)[%?]?(.-) HTTP/1.1$')
	if f then
		repeat
			--skip headers we don't care
			line,err=skt:receive()
		until line=='' or line==nil
		if err then return end
		local s=get_page[f](p)
		return(s..'\r\n')
	end

	local f,p=string.match(line, '^POST ([%/%.%d%w%-_]+) HTTP/1.1$')
	if f then
		local length
		repeat
			--skip headers we don't care
			line,err=skt:receive()
			length=length or string.match(line, '^Content%-Length%: (%d+)$')
		until line=='' or line==nil
		if err then return end
		local p=skt:receive(tonumber(length))
		local s=get_page[f](p)
		return(s..'\r\n')
	end
end

