
local _G = _G
local internal = debug.getregistry()

local type = type
local unpack = unpack
local assert = assert
local format = string.format
local string_byte = string.byte
local string_sub = string.sub
local string_match = string.match

local next = next
local pairs = pairs
local ipairs = ipairs
local tonumber = tonumber
local tostring = tostring
local rawget = rawget
local rawset = rawset
local getfenv = getfenv
local setfenv = setfenv
local pcall = pcall
local xpcall = xpcall

local loadfile = loadfile
local loadstring = loadstring
local table_insert = table.insert
local table_remove = table.remove
local table_concat = table.concat
local math_min = math.min
local math_floor = math.floor
local math_ceil = math.ceil
local abs = math.abs
local coroutine_create = coroutine.create
local coroutine_resume = coroutine.resume
local coroutine_running = coroutine.running
local dofile = dofile

local error = error
local AppPath = AppPath
local tostring2 = tostring2
local table_copy = table.copy
local path_find = path.find
local path_setext = path.setext

local msg = debug.Message
local print = print
local dump = dump

local setmetatable = setmetatable
local setmetatableW = internal.setmetatableW
local protectMetatable = internal.protectMetatable
local LuaGetLastError = internal.LuaGetLastError
local roError = internal.roError
local readonly = internal.readonly

local byteA = ("A"):byte(1) -- for recievers and commands
local byteZ = ("Z"):byte(1)
local bytef = ("f"):byte(1) -- for quick variables
local bytet = ("t"):byte(1)

----------- No globals from this point ------------

local _NOGLOBALS

----------- ERM ------------

local erm = {}

local function cmd_call(t, cmd, ...)
	if internal.ERM_Reciever(t.name, unpack(t.params)) ~= 0 then
		error(LuaGetLastError() or "ERM reciever error", 2)
	end
	
	return internal.ERM_Call(cmd, ...)
end

local function rec_index(t, a)
	if #a == 1 then
		return function(par1, ...)
			if par1 == t then
				return cmd_call(t, a, ...)
			end
			return cmd_call(t, a, par1, ...)
		end
	end

	local defpar = 0 + string_sub(a, 2)
	return function(par1, ...)
		if par1 == t then
			return cmd_call(t, a, defpar, ...)
		end
		return cmd_call(t, a, defpar, par1, ...)
	end
end

local function rec_newindex(t, a, v)
	local postFlag, ind
	-- test triggers: postFlag, ind = false, a
	if a == "?" then
		postFlag, ind = false, -1
	elseif a == "first?" or a == "First?" then
		postFlag, ind = false, 0
	elseif a == "$" then
		postFlag, ind = true, -1
	elseif a == "first$" or a == "First$" then
		postFlag, ind = true, 0
	else
		error(format('"%s.%s"-unknown trigger type.', t.name, a), 2)
	end
	if type(v) ~= "function" then
		error("Trigger must be a function.", 2)
	end
	local index = internal.ERM_Trigger(t.name, postFlag, #internal.triggers + 1, ind, unpack(t.params))
	if index then
		table_insert(internal.triggers, v)
	else
		error(LuaGetLastError() or "failed to add trigger", 2)
	end
end

local function rec_call(t, ...)
	t.params = {...}
	return t
end


local function vaValue(name, index, val)
	if name == '$' then
		index = "" .. index
		if string_sub(index, -1, -1) == '$' then
			index = string_sub(index, 1, -2)
		end
		if #index > 16 then
			error("Macro is too long (>16 characters).", 2)
		end
	else
		index = 0 + index
	end

	local ret
	if val == nil then
		ret = internal.ERM_Var(name, index)
	else
		ret = internal.ERM_Var(name, index, val)
	end
	if LuaGetLastError() then
		error(LuaGetLastError(), 2)
	end
	return ret
end

local function varArray_index(t, vtype)
	local function va_index(t, a)
		return vaValue(vtype, a)
	end

	local function va_newindex(t, a, v)
		return vaValue(vtype, a, v)
	end

	local function va_call2(a, b)
		if a <= b then
			return vaValue(vtype, a), va_call2(a + 1, b)
		end
	end
	
	local function va_call(t, a, b)
		return va_call2(a or b, b)
	end

	local t1 = setmetatableW({}, {__index = va_index, __newindex = va_newindex, __call = va_call})
	t[vtype] = t1
	return t1
end

local varArray = setmetatable({}, {__index = varArray_index})

local rec_metatable = protectMetatable({__index = rec_index, __newindex = rec_newindex, __call = rec_call})

local function erm_index(t, a)
	local tp = type(a)
	if tp ~= "string" then
		if tp == "number" then
			return vaValue("", a)
		else
			return
		end
	end
	local len = #a
	if len == 0 then
		return nil
	end
	local a1, a2 = string_byte(a, 1, 2)
	if len == 2 then
		if a1 >= byteA and a1 <= byteZ and a2 >= byteA and a2 <= byteZ then
			return setmetatable({ name = a, params = {} }, rec_metatable)
		end
	end

	-- vars and flags
	if len == 1 then
		if a1 >= bytef and a1 <= bytet or a == 'c' then
			return vaValue(a, 0)
		end
		
		return varArray[a]
	end
	if a == "Flags" or a == "flags" then
		return varArray[""]
	end
	return vaValue(string_sub(a, 1, 1), string_sub(a, 2))
end

local function erm_newindex(t, a, v)
	if type(a) == "number" then
		return vaValue("", a, v)
	end
	local s = string_sub(a, 1, 1)
	if a == s then
		return vaValue(s, 0, v)
	else
		return vaValue(s, string_sub(a, 2), v)
	end
end

local function FindReciever(a)
	if type(a) == "string" and #a == 2 then
		local a1,a2 = string_byte(a, 1, 2)
		if a1 >= byteA and a1 <= byteZ and a2 >= byteA and a2 <= byteZ then
			return setmetatable({ name = a, params={} }, rec_metatable)
		end
	end
end
internal.FindReciever = FindReciever

_G.ERM = erm
setmetatableW(erm, {__index = erm_index, __newindex = erm_newindex})

--- Special case: MC ---

local function MC_index(t, a)
	if string_sub(a, 1, 1) ~= 'S' then
		error('"MC:'..string_sub(a, 1, 1)..'"-unknown command.', 2)
	end
	local vtype = (#a > 1) and string_sub(a, 2, 2) or ""
	return function(par1, par2)
		local vind = (par1 == t and (par2 or 0)) or par1 or 0
		local err = internal.ERM_SetMacro(rawget(t, "macro"), vtype, vind)
		if err then
			error(format('"MC:S%s%s"-%s', vtype, vind, err), 2)
		end
		if vtype == "" then
			return vind
		end
	end
end

local MC_metatable = protectMetatable({__index = MC_index, __newindex = readonly})

function _G.MC(a)
	if #a > 16 then
		error("Macro is too long (>16 characters).", 2)
	end
	return setmetatable({ macro = a }, MC_metatable)
end
