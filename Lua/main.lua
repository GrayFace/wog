--[[

textonpcx autosize, better text autosize
save Lua error log if an error wasn't handled
600530 - bug! туда попадали при другом баге
4BF64C - bug
41C958 - bug - should be CannotCast?
Dialogs - invisible items diallow clicking visible items under them
d:\GAMES\h3\Mods\Wog\Global\misc\Crash Without Log.lua
hook 4C9650 UpdateMapItemLook - ability to change look of objects
write Lua crash log
fix "allow flag color for all objects"
get rid of Bless/Curse slots
choose to show debug.debug or normal message on error
use GetRealObjectTypeOfMapItem and GetRealSetupDwordOfMapItem in ERM instead of hand-made thing
wogify options...
check SOGMAPTYPE refs
PrepareSpecWoG - would need a look after everything else
scripts in Replace folder?
не рисовать стандартные диалоги до вызова event'а показа
up/down arrows in Radio dialog
an option to reload scripts when loading a saved game
protection from malicious scripts.....




Lods:
mylod.lod <- sync -> mylod.lod.files (sync when LOD is loaded)
mymap.h3m events <- sync -> mymap.h3m.files (sync by MapEdit)




Mods:
PreMap
PreWogify
Wogify
PostWogify
General
ERM
Manual
MapManual

mod.lua
Options\*



Mods.Standard.Options.GrowArmy = {Enabled, Text, Description, Incompatible = {"Standard.GrowHair"}, Compatible = {}, Requires = {}, Tags = {"stupid"}, Cosmetic = true}
Mods.Standard = то же самое
Mods.Standard.Options.Wogify = {Values = {NoWogify = {Value = 0, Enabled, Text, Incompatible, ...}}}
Mods.PhoenixMod.LoadAfter = {"Standard"}
require 'Mods.Standard.General.BattlePositions'

ZVSL - обязателен только в картах
Campaign.* - environment скриптов компании
Map.* - environment скриптов карты
scripts.Main - для дебажной консоли и пр. loadstring'ов
scripts.имя - для именованного ZVSL (только в картах?)

Loading:
load options
load table of package.loaded
prepare permanents
load everything

]]--
local internal = debug.getregistry()
local CoreScriptsPath = internal.CoreScriptsPath

-- dofile(CoreScriptsPath.."RSFunctions.lua")
-- PrintToFile("InternalLog.txt")  -- temporary

local NoGlobals = dofile(CoreScriptsPath.."RSNoGlobals.lua")
NoGlobals.Options.NameCharCodes[("?"):byte()] = true
NoGlobals.Activate()
NoGlobals.CheckChunkFile(1, 1)
NoGlobals.CheckChunkFile(NoGlobals.CheckChunkFile, 1)
NoGlobals.CheckChunkFile(pcall, 1)

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
local getmetatable = debug.getmetatable
local d_setmetatable = debug.setmetatable
local os_time = os.time
local os_date = os.date
local debug_getinfo = debug.getinfo
local loadfile = loadfile
local loadstring = loadstring
local table_insert = table.insert
local table_remove = table.remove
local table_concat = table.concat
local floor = math.floor
local abs = math.abs
local coroutine_create = coroutine.create
local coroutine_resume = coroutine.resume
local coroutine_running = coroutine.running
local d_debug
local dofile = dofile
local ffi = require("ffi")

local _G = _G
internal.require = require
local mem_internal = mem

internal.traceback = debug.traceback

offsets = {
	new = 0x617492,
	free = 0x619BB0,
	malloc = 0x61A9D5,
	realloc = 0x619890,
}

dofile(CoreScriptsPath.."Common.lua")
offsets = nil
local mem_structs = mem.structs
internal.events = events.new()

function mem_internal.member_callback(name)
	mem_structs.LastStructMember = name
end
mem_internal.UpdateCallbacks()

local error = error
local AppPath = AppPath
local events = events
local tostring2 = tostring2
local table_copy = table.copy
local path_find = path.find
local path_setext = path.setext
local io_SaveString = io.SaveString

local function roError(a, lev)  error('attempt to modify a read-only field "'..a..'".', lev + 1)  end
internal.roError = roError
function internal.readonly(t, a)  roError(a, 2)  end

local function setmetatable(t, m)
	d_setmetatable(t, m)
	return t
end

internal.getmetatable = getmetatable
internal.setmetatable = setmetatable
internal.dofile = dofile

RSPersist_NoFunctions = "ignore"
--RSPersist_NoMetatables = "ignore"
dofile(CoreScriptsPath.."RSPersist.lua")
RSPersist_NoFunctions = nil
RSPersist_NoMetatables = nil
internal.AutoPersist = {}

internal.persist = persist
persist = nil
internal.unpersist = unpersist
unpersist = nil
local function nullsub()
end

local call = mem.call
local malloc = mem.malloc
local free = mem.free
local i4, i2, i1, u8, u4, u2, u1, pchar = mem.i4, mem.i2, mem.i1, mem.u8, mem.u4, mem.u2, mem.u1, mem.pchar
local mem_string = mem.string
local mem_copy = mem.copy
local mem_fill = mem.fill

local assertnum = assertnum
local function NilOrNum(i)
	if i ~= nil then
		return assertnum(i, 3)
	end
end

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

_G.ScreenWidth, _G.ScreenHeight = 800, 600
internal.ScreenWidth, internal.ScreenHeight = 800, 600

----------- Protected metatables ------------

local function protectMetatable(m)
	m.__metatable = "protected metatable"
	return m
end
internal.protectMetatable = protectMetatable

function internal.setmetatableW(t, m)
	return setmetatable(t, protectMetatable(m))
end

----------- table.hidden ------------

local hiddens = setmetatable({}, {__mode = "k"})

function _G.table.hidden(t)
	local v = hiddens[t]
	if v == nil then
		v = {}
		hiddens[t] = v
	end
	return v
end

----------- General functions ------------

_G.global = _G

_G.PrintToFile(AppPath.."LuaLog.txt")
_G.PrintToFile = nil

local GetKeyStatePtr = internal.GetKeyStatePtr

function internal.GetKeyState(key)
	return i2(call(GetKeyStatePtr, 0, key))
end

do
	local FuncPtr = _G.mem.GetProcAddress(_G.mem.dll.kernel32, "GetPrivateProfileStringA")
	local Buf
	local BufSize = 0

	function _G.ReadIniString(sect, key, def, fname, size)
		size = size or 260
		assert(size > 0)
		if BufSize < size then
			if Buf then
				free(Buf)
			end
			Buf, BufSize = malloc(size), size
			
		end
		local n = call(FuncPtr, 0, tostring(sect), tostring(key), tostring(def or ""), Buf, size, tostring(fname or ".\\WoG.ini"))
		return mem_string(Buf, n, true)
	end
end

--[[
function os.time(...)
	if #{...} > 0 then
		return os_time(...)
	end
	return internal.os_time()
end

function os.difftime(t2, t1)
	if t1 then
		return t2 - t1
	end
	return internal.os_time() - t2
end
]]--

--------- color

_G.color = {}

function _G.color.ToRGB(c)
	local r = floor(c:And(63488)*33/8192)
	local g = floor(c:And(2016)*65/512)
	local b = floor(c:And(31)*33/4)
	return r, g, b
end

function _G.color.RGB(r, g, b)
	return r:And(248)*256 + g:And(252)*8 + floor(b/8)
end

function _G.color.GamePal(n)
	if n < 0 or n > 255 then
		error("color index ("..n.."out of bounds (0..255)", 2)
	end
	return i2[u4[0x6AAD18] + 0x1C + n*2]
end

--local function Color16To24(c)
--	local r = floor(c:And(63488)*33/8192)
--	local g = (c:And(2016)*65/2):And(65280)
--	local b = (c:And(31)*540672):And(16711680)
--	return r + g + b
--end

--------- messages

local function tostringOpt(s)
	if s ~= nil then
		return tostring(s)
	end
end

function _G.Message(par, ...)
	if type(par) ~= "table" then
		par = {par, ...}
	end
	local params = {}
	
	local n = 1
	
	local function s(name, def)
		params[n] = tostringOpt(par[name]) or tostringOpt(par[n]) or def
		n = n + 1
	end
	
	local function p(name, def)
		params[n] = tonumber(par[name]) or tonumber(par[n]) or def or 0
		n = n + 1
	end
	
	local function t(num)
		p("t"..num, -1)
		p("st"..num, 0)
	end

	s("text")
	p("mode", 1)
	t("1")
	t("2")
	t("3")
	p("x", -1)
	p("y", -1)
	p("timeout", 0)
	s("caption", "Heroes of Might and Magic III")
	
	local ret = internal.LuaMessage(unpack(params))
	return ret ~= 0 and ret -- return 'false' in case of '0'
end
local Message = _G.Message

function _G.ManyPicsMessage(par, ...)
	if type(par) == "table" then
		return internal.MultiPicMessage(tostring(par[1]), par.x or -1, par.y or -1, unpack(par, 2))
	else
		return internal.MultiPicMessage(tostring(par), -1, -1, ...)
	end
end

local MessageExParams

function _G.MessageEx(par, ...)
	if type(par) ~= "table" then
		par = {par, ...}
	end
	local i = 1
	
	local function ToPar(tp, totp, def, name1, name2)
		if type(par[i]) == tp then
			i = i + 1
			return par[i - 1]
		else
			return totp(par[name1] or par[name2]) or def
		end
	end
	
	local text = ToPar("string", tostring, "", "text", "Text")
	local mode = ToPar("number", tonumber, 1, "mode", "Mode")
	local x = ToPar("number", tonumber, -1, "x", "X")
	local y = ToPar("number", tonumber, -1, "y", "Y")
	local unk = -1
	local timeout = ToPar("number", tonumber, 0, "timeout", "Timeout")
	
	par[-1] = i
	par[0] = 0
	
	local last = MessageExParams
	MessageExParams = par
	
	call(0x4F6C00, 2, text, mode, x, y, -1, -1, -1, -1, unk, timeout, -1, -1)
	
	MessageExParams = last
	
	local ret = i4[u4[0x6992D0] + 0x38]
	if ret == 0x7805 or ret == 0x7809 then  -- 1 for left pic and Ok
		return 1
	else
		return ret == 0x780A and 2  -- 2 for right pic, false for Cancel
	end
end

local MyMessagePic = 0xFFFF  -- @ const

function internal.MessagePictureInit(data)
	if MessageExParams and MessageExParams[0] < 8 then
		local ind = MessageExParams[0] + MessageExParams[-1]
		MessageExParams[0] = MessageExParams[0] + 1
		local par = MessageExParams[ind]
		if type(par) == "table" then
			local def = par[1] or par.def
			if type(def) == "string" then
				i4[data] = MyMessagePic
				i4[data + 4] = ind
				call(0x404180, 1, data + 8, def, #def)
				i4[data + 0x28] = tonumber(par[2] or par.frame) or 0
				if par[2] then
					local s = tostring(par[3] or par.text)
					call(0x404180, 1, data + 0x18, s, #s)
				end
				return 1
			else
				i4[data] = par[1] or par.type
				i4[data + 4] = par[2] or par.subtype
			end
		end
	end
end

function internal.MessagePictureHint(data)
	if MessageExParams and i4[data + 0x38] == MyMessagePic then
		local par = MessageExParams[i4[data + 0x3C]]
		if type(par) == "table" then
			local hint = par[4] or par.hint
			if type(hint) == "function" then
				hint()
			else
				Message(hint or par[3] or par.text, 4)
			end
		end
	end
end

--------- debug

_G.debug.debug = nil

function _G.debug.Message(...)
	local dbg = debug_getinfo(2,"Sl")
	local msg
	if dbg and dbg.short_src and dbg.currentline then
		msg = format("(%s)\n{debug.Message. Line %s}", dbg.short_src, dbg.currentline)
	else
		msg = "{debug.Message}"
	end
	local par = d_debug and {"Debug Message", "", msg, ...} or {msg, ...}
	for i = 2, #par do
		par[i] = tostring(par[i])
	end
	if d_debug then
		d_debug(table_concat(par, "\n"))
	else
		Message{table_concat(par, "\n"), caption = "Debug Message"}
	end
end
local msg = _G.debug.Message

function _G.debug.ErrorMessage(msg)
	msg = tostring(msg)
	if internal.OnError then
		internal.OnError(msg)
	end
	call(internal.DumpERMVars, 0, msg.."\n", true);
	msg = msg..'\n\nPLEASE SEND "WOGERMLOG.TXT" FILE TO "sergroj@mail.ru"'
	-- io_SaveString(AppPath.."WOGLUALOG.TXT",
	-- 	format("Time Stamp: %s\nWoG Version: %s\n\n%s", os_date(), internal.WogVersion, msg))
	if d_debug then
		d_debug(msg)
	else
		Message{msg, caption = "Heroes of Might and Magic III - Error"}
	end
end
internal.ErrorMessage = _G.debug.ErrorMessage
_G.ErrorMessage = _G.debug.ErrorMessage

--------------------------- Other Files --------------------------

dofile(CoreScriptsPath.."Debug.lua")
d_debug = _G.debug.debug
dofile(CoreScriptsPath.."Localization.lua")
dofile(CoreScriptsPath.."Structs.lua")
dofile(CoreScriptsPath.."Text Tables.lua")
dofile(CoreScriptsPath.."events.lua")
dofile(CoreScriptsPath.."ert.lua")
dofile(CoreScriptsPath.."erm.lua")
dofile(CoreScriptsPath.."options.lua")
dofile(CoreScriptsPath.."scripts.lua")  -- also load/save game, mods
-- dofile(CoreScriptsPath.."DefReplace.lua")
dofile(CoreScriptsPath.."Dialogs.lua")

for f in path_find(CoreScriptsPath.."Misc/*.lua") do
	dofile(f)
end

local require = _G.require
local event = internal.event

--]=]