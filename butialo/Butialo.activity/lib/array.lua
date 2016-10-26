--array type, based on a table. adds the following restrictions
-- indices must be number.
-- indices must be in the range 1..lenght+1 (can only add a next element)
-- object of a single type can be stored (fixed when first element is stored)
-- method available:
--	add() 		automatically adds at the end
--	remove_last()	automatically removes the last element, and returns it
--	len()		returns the length of thearray


local M = {}


function M.new_array ()
	local mt = {}
	local t = {}

	local data = {}

	t.add = function(v)
		data[#data+1] = v
	end

	t.remove_last = function()
		if #data>0 then 
			local v = data[#data]
			rawset(data, #data, nil)
			return v
		end
	end

	t.containing = function() return data.containing end

	t.len = function()
		return #data
	end

	mt.__newindex = function (t, n, v)
		--print ("xxx__newindex")
		--print("#######",t,n,v,type(v))
		local addingtype = type(v)
		if addingtype == 'nil' then
			--print("######!!!!!!!",n, #data, n == #data)
			if n == #data then
				rawset(data, n, v)
				return
			else
				if #data==0 then
					error("Trying remove an an entry from an empty array", 2)
				else
					error("Trying remove an entry at index "..n
						..", can be done at index "..#data, 2)
				end
			end
			return
		end
		rawset(data,'containing', rawget(data,'containing') or addingtype)
		if type(n)=='number' then
			local nexti = #data+1
			if n < 1 then
				error("Trying set an index less than 1", 2)
			elseif n > nexti then
				error("Trying to add an element beyond the position "..nexti
					.." (the length of the array + 1)", 2)
			elseif rawget(data,'containing') == addingtype then
				rawset(data, n, v)
			else
				error("Trying to store a '"..addingtype
					.."' in a array initialized with '"..tostring(data.containing).."'", 2)
			end
		else
			error("Trying to use a '"..type(n)
				.."' as a index, must be a number", 2)
		end
	end
	mt.__index = function (t, n)
		--print ("xxx__index", t, n)
		local v=rawget(data, n)
	  	if not v then
			error("Trying to access a non existing index "..n
				..", not in 1-"..#data.." range", 2)
		end
		return v
	end

	setmetatable(t, mt)
	return t
end

return M

