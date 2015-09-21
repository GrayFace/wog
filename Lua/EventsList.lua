local _G = _G
local table_insert = table.insert
local table_remove = table.remove
local abs = math.abs
local type = type
local ipairs = ipairs
local next = next
local assert = assert
local setmetatable = setmetatable
local d_getinfo = debug.getinfo
local FunctionFile = debug.FunctionFile
local pcall2 = pcall2
local coroutine_create = coroutine.create
local coroutine_resume2 = coroutine.resume2

----------- No globals from this point ------------

local _NOGLOBALS

--------- events

-- Logic:
-- 'remove' should prevent currently running events from executing the removed function, but keep processing going
-- newly added functions shouldn't be executed by currently running events
-- So:
-- no table.remove/table.insert, instead build a new event table when there are too many gaps
-- invalidate the old event table (with false's), redirect(reloc) handlers stuck with it to the newer table
-- (the old table otherwise stays in the state from which the newer one was created, which is important for reloc)

local function fcocall(f, ...)
	return coroutine_resume2(coroutine_create(f), ...)
end

local function remake_list(f)
	local n = 0
	local t = {from = 1}
	for i = f.from, f.to do
		local v = f[i]
		if v then
			n = n + 1
			t[n] = v
			f[i] = false
		end
	end
	t.to = n
	f.Next = t
	return t
end

local function ProcessReloc(f, index, to)
	local from1, to1 = 1, 0
	for i = f.from, to do
		if f[i] == false then
			to1 = to1 + 1
			if i == index then
				from1 = to1
			end
		end
	end
	return f.Next, from1, to1
end

local function docall(f, from, to, ...)
	local ret
	for i = from, to do
		local v = f[i]
		if v then
			local tmp = v(...)
			if tmp ~= nil then
				ret = tmp
			end
		elseif v == false then
			f, from, to = ProcessReloc(f, i, to)
			local tmp = docall(f, from, to, ...)
			if tmp ~= nil then
				return tmp
			end
			return ret
		end
	end
	return ret
end

local function speccall(call, f, from, to, ...)
	local ret
	for i = from, to do
		local v = f[i]
		if v then
			local ok, tmp = call(v, ...)
			if tmp ~= nil and ok then
				ret = tmp
			end
		elseif v == false then
			f, from, to = ProcessReloc(f, i, to)
			local tmp = speccall(call, f, from, to, ...)
			if tmp ~= nil then
				return tmp
			end
			return ret
		end
	end
	return ret
end

local function make_events(evt)
	evt = evt or {}
	local t = {}
	
	local function call(a, ...)
		local f = t[a]
		return f and docall(f, f.from, f.to, ...)
	end
	evt.call, evt.Call = call, call

	local function pcalls(a, ...)
		local f = t[a]
		return f and speccall(pcall2, f, f.from, f.to, ...)
	end
	evt.pcalls = pcalls

	local function cocalls(a, ...)
		local f = t[a]
		return f and speccall(fcocall, f, f.from, f.to, ...)
	end
	evt.cocalls = cocalls
	
	local function _pcall(a, ...)
		local f = t[a]
		if f then
			local ok, tmp = pcall2(docall, f, f.from, f.to, ...)
			if ok then
				return tmp
			end
		end
	end
	evt.pcall = _pcall
	
	local function cocall(a, ...)
		local f = t[a]
		if f then
			local ok, tmp = coroutine_resume2(coroutine_create(docall), f, f.from, f.to, ...)
			if ok then
				return tmp
			end
		end
	end
	evt.cocall = cocall
	
	local function newindex(_, a, v)
		if v then
			local f = t[a]
			if f then
				f.to = f.to + 1
				f[f.to] = v
			else
				t[a] = {v, from = 1, to = 1}
			end
		end
	end

	local function add(a, v)
		newindex(nil, a, v)
	end
	evt.add, evt.Add = add, add

	local function addfirst(a, v)
		if v then
			local f = t[a]
			if f then
				t.from = t.from - 1
				t[t.from] = v
			else
				t[a] = {v, from = 1, to = 1}
			end
		end
	end
	evt.addfirst, evt.AddFirst = addfirst, addfirst

	local function check_gaps(a, f)
		local d = f.to - f.from
		if d < 0 then
			t[a] = nil
		elseif (f.gaps or 0)*2 + abs(f.from - 1) > d then  -- allow 1/2 to be filled with gaps
			t[a] = remake_list(f)
		end
	end
	
	local function removed(f, i)
		if i == f.from then
			f.from = f.from + 1
			while not f[f.from] and f.from <= f.to do
				f.from = f.from + 1
				f.gaps = f.gaps - 1
			end
		elseif i == f.to then
			f.to = f.to - 1
			while not f[f.to] and f.from <= f.to do
				f.to = f.to - 1
				f.gaps = f.gaps - 1
			end
		else
			f.gaps = (f.gaps or 0) + 1
		end
	end
	
	local function replace(a, func, f2)
		if type(func) == "number" then
			func = assert(d_getinfo(func + 1, 'f').func)
		end
		if type(f2) == "number" then
			f2 = assert(d_getinfo(f2 + 1, 'f').func)
		end
		local f = t[a]
		if f and func then
			for i = f.from, f.to do
				if f[i] == func then
					f[i] = f2
					if not f2 then
						removed(f, i)
						check_gaps(a, f)
					end
					return func
				end
			end
		end
	end
	evt.replace, evt.Replace, evt.remove, evt.Remove = replace, replace, replace, replace

	local function clear(a)
		-- stop currently running handlers
		local f = t[a]
		if f then
			for i = f.from, f.to do
				f[i] = nil
			end
		end
		t[a] = nil
	end
	evt.clear, evt.Clear = clear, clear

	local function exists(a, func)
		local f = t[a]
		if f then
			if not func then
				return true
			end
			for i = f.from, f.to do
				if f[i] == func then
					return true
				end
			end
		end
		return false
	end
	evt.exists, evt.Exists = exists, exists

	local function RemoveFiles(files)
		for a, f in next, t do
			for i = f.from, f.to do
				if f[i] and files[FunctionFile(f[i])] then
					f[i] = nil
					removed(f, i)
				end
			end
			check_gaps(a, f)
		end
	end
	evt.RemoveFiles = RemoveFiles
	
	function evt.RemoveFile(f)
		f = f or FunctionFile(2)
		RemoveFiles{[f] = true}
	end
	
	function evt.RemoveAll()
		-- stop currently running handlers
		for a, f in next, t do
			for i = f.from, f.to do
				f[i] = nil
			end
			t[a] = nil
		end
	end
	
	local function index(_, a)
		local function forward(f)
			return function(...)
				return f(a, ...)
			end
		end
		local call = function(_, ...)
			return call(a, ...)
		end
		local add = function(func)
			newindex(nil, a, func)
		end
		local replace = function(func, f2)
			if type(func) == "number" then
				func = assert(d_getinfo(func + 1, 'f').func)
			end
			if type(f2) == "number" then
				f2 = assert(d_getinfo(f2 + 1, 'f').func)
			end
			return replace(a, func, f2)
		end
		local remove = replace
		local pcalls = forward(pcalls)
		local _pcall = forward(_pcall)
		local cocalls = forward(cocalls)
		local cocall = forward(cocall)
		local exists = forward(exists)
		local clear = forward(clear)

		local t1 = {add = add, Add = add, remove = remove, Remove = remove,
			replace = replace, Replace = replace, exists = exists, Exists = exists,
			clear = clear, Clear = clear,
			__call = call, pcalls = pcalls, pcall = _pcall, cocalls = cocalls, cocall = cocall}
		return setmetatable(t1, t1)
	end

	return setmetatable(evt, {__index = index, __newindex = newindex}), t
end

_G.events = make_events{new = make_events}
