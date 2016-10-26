--Generates a proxy for a table (intended for _G), which disables all creations and updating of 
--entries, and errors on any acces to non-existing entries.

local M = {}

function M.make_fixed_proxy (T)
	local proxy={}
	proxy._G=T
	local mt={}

	---[[
	mt.__index = function (t, n)
		--print ("xxx__index", t, n)
		local v=rawget(T, n)
	  	if not v then
			error("Variable must be declared with 'local "..tostring(n).."' before use", 2)
		end
		return v
	end
	--]]
	--mt.__index=T

	mt.__newindex = function (t, n, v)
		--print ("xxx__newindex")
		local v=rawget(T, n)
	  	if v then
			error("Attempt to assign to unmodifiable variable '"..tostring(n).."'", 2)
		else
			error("Variable must be declared with 'local "..tostring(n).."' before use", 2)
		end
	end

	setmetatable(proxy, mt)
--print ("&&&", T, proxy)
	return proxy
end

return M
  

