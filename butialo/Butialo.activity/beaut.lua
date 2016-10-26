local tab="  "

local function readfile (f)
	local lines={}
	local fh=io.open(f,"rt")
	for line in fh:lines() do
		lines[#lines+1] = line
	end
	fh:close()
	return lines
end

local function purge (l)
	local p={}
	for i, line in ipairs(l) do
		line = string.gsub(line, "('.-')", "")
		line = string.gsub(line, '(\".-\")', "")
		line = string.gsub(line, '(%[%[.*%]%])',"")
		p[i] = line
	end

	local i=1
	while i<#l do
		local line=p[i]
		if string.match(line, "[^%-]%-%-%[%[") 
		or string.match(line, "^%-%-%[%[") then
			line = string.gsub(line, '(%-%-%[%[.*)$',"")
			p[i]=line
			repeat
				i=i+1
				line=p[i]
				line=string.gsub(line, '^(.*%]%])',"")
				if line==p[i] then p[i]=""
				else p[i]=line; break end
			until false
		end
		i=i+1
	end

	for i, line in ipairs(p) do
		line = string.gsub(line, '(%-%-.*)$',"") --comentario
		p[i] = line
	end

	return p
end

local opens = {["do"]=true, ["repeat"]=true, ["function"]=true, ["then"]=true}
local closes = {["end"]=true, ["until"]=true}

local function get_shifted(t)
	local s = {}
	local level=0
	--for i=1,#t do s[i]=level end
	for i, line in ipairs(t) do
		s[i]=level

		local shift = 0
		words = {}
		for w in string.gfind(line, "%a+") do
		--print(w)
			words[w]=true
			words[#words+1]=w
			if opens[w] then shift=shift+1 end
			if closes[w] then shift=shift-1 end
		end
		if shift>0 then level=level+1 end
		if shift<0 then level=level-1 end
		if shift<0 or words[1]=='else' then s[i]=s[i]-1 end
	end
	return s
end

local tab_cache={}
local function tabs(i)
	if tab_cache[i] then return tab_cache[i] end
	local t=string.rep(tab,i)
	tab_cache[i]=t
	return t
end

local function printfile (s, l)
	for i, line in ipairs(l) do
		local formatted=string.gsub(line, "^(%s*)", tabs(s[i]))
		print(formatted)
	end
end

local lines = readfile(arg[1])
local lines_purged = purge(lines)
local shift = get_shifted(lines_purged)
printfile(shift, lines)
