-- shared by WoG, MMExtension and GameExtension

local type = type
local unpack = unpack
local assert = assert
local format = string.format

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
local pcall2
local getmetatable = debug.getmetatable
local d_setmetatable = debug.setmetatable
local d_getinfo = debug.getinfo
local os_time = os.time
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
local coroutine_main = coroutine.main
local bit_Or = mem.bit_Or
local bit_And = mem.bit_And
local bit_Xor = mem.bit_Xor
local bit_AndNot = mem.bit_AndNot
local dofile = dofile

local _G = _G
local internal = debug.getregistry() -- internals
local CoreScriptsPath = internal.CoreScriptsPath
local nullpersist = internal.nullpersist

local function setmetatable(t, m)
	d_setmetatable(t, m)
	return t
end

----------- Error function ------------

local errorSt = _G.error
local SetErrorLevel = internal.SetErrorLevel

local function error(msg, level) -- defined before
	if level ~= 0 then
		level = (level or 1) + 1
	end
	SetErrorLevel(level)
	return errorSt(msg, level)
end
_G.error = error

-----------

local function roError(a, lev)  error('attempt to modify a read-only field "'..a..'".', lev + 1)  end
local function readonly(t, a)  roError(a, 2)  end

local function nullsub()
end
local offsets = offsets or {}

-- function _G.print(s)  -- report mobdebug errors
	-- error(s)
-- end
package.path = CoreScriptsPath.."?.lua"
debug.mobdebug = require("mobdebug")
local mobstart = debug.mobdebug.start
local mobon = debug.mobdebug.on
local moboff = debug.mobdebug.off
function debug.start()
	mobstart()
	if coroutine_running() then
		debug.sethook(coroutine_main, debug.gethook())
	end
end
function debug.on()
	mobon()
	if coroutine_running() then
		debug.sethook(coroutine_main, debug.gethook())
	end
end
function debug.off()
	moboff()
	if coroutine_running() then
		debug.sethook(coroutine_main)
	end
end

dofile(CoreScriptsPath.."RSFunctions.lua")
local table_swap = table.swap
local table_move = table.move

----------- No globals from this point ------------

local _NOGLOBALS

----------- General functions ------------

local function assertnum(v, level, msg)
	local v1 = tonumber(v)
	if v1 then
		return v1
	end
	error(msg or "number expected, got "..type(v), (level or 1) + 1)
end
_G.assertnum = assertnum

local function NilOrNum(i)
	if i ~= nil then
		return assertnum(i, 3)
	end
end

local function pcall2_ret(ok, ...)
	if not ok then
		local err = ...
		internal.ErrorMessage(err)
	end
	return ok, ...
end

local function pcall2(...)
	return pcall2_ret(pcall(...))
end
_G.pcall2 = pcall2

function _G.xpcall2(...)
	return pcall2_ret(xpcall(...))
end

function _G.dofile2(path, ...)
	local chunk, err = loadfile(path)
	if chunk == nil then
		internal.ErrorMessage(err)
		return false, err
	else
		return pcall2_ret(pcall(chunk, ...))
	end
end

local function coroutine_resume2(...)
	return pcall2_ret(coroutine_resume(...))
end
_G.coroutine.resume2 = coroutine_resume2

local function tostring2(v)
	local t = type(v)
	if v == nil or t == "number" or t == "boolean" then
		return tostring(v)
	elseif t == "string" then
		return format("%q", v)
	else
		return '('..tostring(v)..')'
	end
end
_G.tostring2 = tostring2

----------- mem ------------

local mem_internal = _G.mem
internal.RSMem = mem_internal
local mem = mem_internal
mem.pcall2 = pcall2
mem.free = offsets.free or mem.free
mem.StaticAlloc = mem.malloc
mem.malloc = offsets.malloc or mem.malloc
mem.realloc = offsets.realloc or mem.realloc
mem.new = offsets.new or mem.new

function mem.struct_callback(t, class, fields, offs, rofields)
	local meta = getmetatable(t)
	meta.members = fields
	meta.offsets = offs
	meta.class = class
	return t
end

dofile(CoreScriptsPath.."RSMem.lua")

local mem = _G.mem
_G.table.destructor = mem.general.table_destructor
mem.general.table_destructor = nil
_G.table.copy(mem.general, _G)
mem.general = nil
local call = mem.call
local malloc = mem.malloc
local StaticAlloc = mem.StaticAlloc
local free = mem.free
local i4, i2, i1, u8, u4, u2, u1, pchar = mem.i4, mem.i2, mem.i1, mem.u8, mem.u4, mem.u2, mem.u1, mem.pchar
local mem_string = mem.string
local mem_copy = mem.copy
local mem_fill = mem.fill
local IgnoreProtection = mem.IgnoreProtection

dofile(CoreScriptsPath.."dump.lua")

--------- bit

local bit = {}
_G.bit = bit
bit.Or = bit_Or
bit.And = bit_And
bit.Xor = bit_Xor
bit.AndNot = bit_AndNot

function bit.Not(a)
	if a <= 2147483647 then
		return -a-1
	end
	return 4294967295 - a
end

local numIndex = _G.table.copy(bit)

do
	local n = 1
	for i = 0,63 do
		bit[i] = n
		n = n*2
	end
end

function bit.ToTable(x, t)
	t = t or {}
	if x < 0 then  x = x + 4294967296  end
	local i = 1
	local b = 1                              -- current bit
	local d = 2                              -- current divider
	while x >= b do
		if x % d >= b then
			t[i] = true
		end
		b = d
		d = d*2
		i = i + 1
	end
	return t
end

function bit.FromTable(t)
	local x = 0
	local b = 1
	for i = 1,32 do
		if t[i] then
			x = x + b
		end
		b = b*2
	end
	if x < 2147483648 then
		return x
	end
	return x - 4294967296
end

--------- math

function _G.math.round(val)
	if val >= 0 then
		return math_floor(val + 0.5)
	else
		return math_ceil(val - 0.5)
	end
end

--------- numIndex

function numIndex.ToHex(v)
	return format("%X", v)
end
numIndex.tohex = numIndex.ToHex

function numIndex.Div(v1, v2)
	return math_floor(v1/v2)
--	if v1 > v2 then
--		math_floor
--	else
--	end
end

numIndex.div = numIndex.Div

numIndex.round = _G.math.round
numIndex.floor = _G.math.floor
numIndex.ceil = _G.math.ceil

setmetatable(0, {__index = numIndex})

--------- color

--color = {}
--
--function color.ToRGB(c)
--	local r = math_floor(c:And(63488)*33/8192)
--	local g = math_floor(c:And(2016)*65/512)
--	local b = math_floor(c:And(31)*33/4)
--	return r, g, b
--end
--
--function color.RGB(r, g, b)
--	return r:And(248)*256 + g:And(252)*8 + math_floor(b/8)
--end
--
----local function Color16To24(c)
----	local r = floor(c:And(63488)*33/8192)
----	local g = (c:And(2016)*65/2):And(65280)
----	local b = (c:And(31)*540672):And(16711680)
----	return r + g + b
----end

--------- os

if offsets.exit then
	function _G.os.exit(code)
		call(offsets.exit, 0, code or 0)
	end
end

local FindStruct = mem.struct(function(define)
	define
	.u4  'FileAttributes'
	.u4  'CreationTimeLow'
	.u4  'CreationTimeHigh'
	.u4  'LastAccessTimeLow'
	.u4  'LastAccessTimeHigh'
	.u4  'LastWriteTimeLow'
	.u4  'LastWriteTimeHigh'
	.i8  'FileSize'
	.skip(8)
	.string(260)  'FileName'
	.string(14)  'AlternateFileName'
end)

local function findNext(data)
	if data then
		local ret = internal.FindNext(data)
		if ret then
			local m = getmetatable(data)
			return m.path..ret, m.struct
		end
	end
end

function _G.os.find(filter, dir)
	local data, path, p = internal.FindStart(filter, not not dir)
	if data == nil then
		return nullsub
	end
	local m = getmetatable(data)
	m.path = path
	m.__gc = internal.FindClose
	m.__persist = nullpersist
	m.struct = FindStruct:new(p)
	return findNext, data
end
_G.os.Find = _G.os.find

_G.path.find = _G.os.find
_G.path.Find = _G.os.find

function _G.path.FindFirst(filter, dir)  -- instead of FileExists/DirectoryExists
	local data, path, p = internal.FindStart(filter, not not dir)
	if data then
		local r = internal.FindNext(data)
		internal.FindClose(data)
		return r and path..r
	end
end

function _G.path.GetCurrentDirectory()
	local len = call(internal.GetCurrentDirectory, 0, 0, 0)
	local buf = malloc(len)
	u1[buf] = 0
	call(internal.GetCurrentDirectory, 0, len, buf)
	local ret = mem_string(buf)
	free(buf)
	return ret
end

function _G.path.SetCurrentDirectory(dir)
	return call(internal.SetCurrentDirectory, 0, dir) ~= 0
end

function _G.path.CreateDirectory(dir)
	return call(internal.CreateDirectory, 0, dir, 0) ~= 0
end

local AppPath = _G.AppPath or _G.path.addslash(_G.path.GetCurrentDirectory())
_G.AppPath = AppPath

--------- debug, events

local function FunctionFile(f)
	if type(f) == "number" then
		f = f + 1
	end
	local s = d_getinfo(f, "S").source
	return s:sub(1, 1) == '@' and s:sub(2) or ""
end
_G.debug.FunctionFile = FunctionFile

dofile(CoreScriptsPath.."EventsList.lua")
