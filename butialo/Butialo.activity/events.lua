local M = {}

local queue = {}
local scheduled_to_queue -- = {}
local stop = false

local function in_out_range(in_range, evval, op, reference, hysteresis_below, hysteresis_above)
print(in_range, evval, op, reference, hysteresis_below, hysteresis_above)
	if in_range then
		if ( op == ">" and evval <= hysteresis_below )
		or ( op == "<" and evval >= hysteresis_above )
		or ( op == "<=" and evval > hysteresis_above )
		or ( op == ">=" and evval < hysteresis_below )
		or ( op == "==" and evval ~= reference ) 
		or ( op == "~=" and evval == reference ) 
		then
			--exiting range, don't return anything
			--print ("saliendo")
			return false, nil
		else
			--stay in range, don't return anything
			--print ("dentro")
			return true, nil
		end			
	else
		if ( op == ">" and evval > reference) 
		or ( op == "<" and evval < reference) 
		or ( op == "<=" and evval <= reference)
		or ( op == ">=" and evval >= reference)
		or ( op == "==" and evval == reference) 
		or ( op == "~=" and evval ~= reference) 
		then
			--entering range, return value
			--print ("entrando")
			return true, evval
		else
			--staying out of range, don't return anything
			--print ("fuera")
			return false, nil
		end
	end
end

local cache_ev = {}

local function value_tracker (evaluate, op, reference, callback, hysteresis)

	hysteresis = tonumber(hysteresis) or 0
	local hysteresis_below, hysteresis_above = reference, reference
print (type (reference))
	if type (reference) == 'number' then
		hysteresis_below, hysteresis_above = reference - hysteresis, reference + hysteresis
print ("---",reference, hysteresis,hysteresis_below, hysteresis_above )
	end

--print("---value", evaluate, op, reference, callback)

	local in_range,ret = false, nil
	local evval, cache_evval
	while true do
		cache_evval = cache_ev[evaluate]
		if cache_evval then
			evval = cache_evval
		else
			evval = evaluate()
			cache_ev[evaluate] = evval
		end

		in_range, ret = in_out_range(in_range, evval, op, reference, hysteresis_below, hysteresis_above)
		if ret then
			callback(ret)
		end
		coroutine.yield()
	end
end

function M.add ( evaluate, op, reference, callback, hysteresis, eventid )
	eventid=eventid or "event"..math.random(2^30)
	
	local co = coroutine.create(function ()
	        value_tracker( evaluate, op, reference, callback, hysteresis )
	end)
	scheduled_to_queue = scheduled_to_queue or {}
	scheduled_to_queue[eventid] = co
	return eventid
--print(co)
end

function M.remove(id)
	queue[id]=nil
end

function M.stop()
	stop = true
end

function M.go ()
--os.exit()
	while true do
		for id, co in pairs(queue) do
--print('co', #queue,i, co)
			local ok, val = coroutine.resume(co)
			if not ok then 
				error("Coroutine for '"..id
				.."' died with '"..val.."'") 
			end
			if stop then return end
		end
		if scheduled_to_queue then
			for id, newco in pairs(scheduled_to_queue) do
				queue[id]=newco
				scheduled_to_queue[id] = nil
			end
			scheduled_to_queue = nil
		end 
		cache_ev = {}
	end
end

return M
