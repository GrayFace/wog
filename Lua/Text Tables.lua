local type = type
local unpack = unpack
local assert = assert
local format = string.format
local string_byte = string.byte
local string_sub = string.sub
local string_match = string.match
local string_split = string.split
local io_LoadString = io.LoadString

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
local getmetatable = debug.getmetatable
local d_setmetatable = debug.setmetatable
local os_time = os.time
local debug_getinfo = debug.getinfo
local loadfile = loadfile
local loadstring = loadstring
local table_insert = table.insert
local table_remove = table.remove
local table_concat = table.concat
local table_copy = table.copy
local min = math.min
local max = math.max
local abs = math.abs
local dofile = dofile

local _G = _G

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local function ParseAny(s, t)
	local f = assert(loadstring(s))
	setfenv(f, t or {})
	return f()
end

local function ParseTextTable(s)
	local t1 = string_split(s, "\r\n", true)
	local ht = string_split(t1[1], "\t", true)
	local r = {}
	for i = 2, #t1 do
		local t = string_split(t1[i], "\t", true)
		local rt = {}
		for j = 1, min(#ht, #t) do
			local k, v = ht[j], t[j]
			local c = string_sub(k, -1)
			if c == "$" then
				rt[string_sub(k, 1, -2)] = v
			elseif c == "*" then
				ParseAny(v, rt)
			elseif c ~= "-" and k ~= "" then
				rt[k] = tonumber(v) or ParseAny("return "..t[j])
			end
		end
		r[i-1] = rt
	end
	return r
end
_G.ParseTextTable = ParseTextTable

function _G.LoadTextTable(s)
	return ParseTextTable(io_LoadString(s))
end


local function ParseBasicTextTable(s, StartingLinesCount)
	local t = string_split(s, "\r\n", true)
	for i = StartingLinesCount + 1, #t do
		t[i] = string_split(t[i], "\t", true)
	end
	return t
end
_G.ParseBasicTextTable = ParseBasicTextTable

function _G.LoadBasicTextTable(s, StartingLinesCount)
	return ParseBasicTextTable(io_LoadString(s), StartingLinesCount)
end
