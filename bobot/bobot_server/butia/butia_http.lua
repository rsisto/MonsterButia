module(..., package.seeall);

--local devices=devices
local process = require("bobot-server-process").process
local util = require("http-util")

local parse_params = util.parse_params
local load_template = util.load_file

local butia_template=load_template('butia/butia.htm') or "Error loading template butia.htm"
local header_template=load_template('butia/header.htm') or "Error loading template header.htm"

function init (get_page)
	get_page["/butia.htm"] = function (p)
		local params=parse_params(p)
		local rep = {
			['DATA1'] = params['campo'],
			['DATA2'] = tostring(params['unboton'])..', '..tostring(params['otroboton']),
		}
		local page=string.gsub(butia_template, '<!%-%-(%w+)%-%->', rep)
		return "HTTP/1.1 200/OK\r\nContent-Type:text/html\r\nContent-Length: "..#page.."\r\n\r\n"..page
	end

	get_page["/header.htm"] = function (p)
		local params=parse_params(p)
		local rep = {
		}
		local page=string.gsub(header_template, '<!%-%-(%w+)%-%->', rep)
		return "HTTP/1.1 200/OK\r\nContent-Type:text/html\r\nContent-Length: "..#page.."\r\n\r\n"..page
	end

end
