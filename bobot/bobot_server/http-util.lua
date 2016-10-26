module(..., package.seeall);

local bobot = require("bobot")

local my_path = debug.getinfo(1, "S").source:match[[^@?(.*[\/])[^\/]-$]]

--local devices=devices
function load_file (filename)
	local served, err = io.open(my_path..filename, "r")
	if served then
		return served:read("*all")
	else
		bobot.debugprint("Error opening", my_path..filename,":", err)
	end
end

function parse_params(s)
	local params={}
	for k,v in string.gmatch(s, '([%w%%%_%-]+)=([%w%%%_%-]+)') do
		bobot.debugprint('param', k, v)
		params[k]=v
	end
	return params
end

local page404="<html><head><title>404 Not Found</title></head><body><h3>404 Not Found!</h3><hr><small>bobot</small></body></html>"
local http404="HTTP/1.1 404 Not Found\r\nContent-Type:text/html\r\nContent-Length: "..#page404.."\r\n\r\n" .. page404
page404=nil
local function error_page()
	return http404
end

function find_page (page)
	local page=string.sub(page,2)
	local file=load_file(page)
	if file then
		return function() return file end
	else
		return error_page
	end
end
