local co_running = coroutine.running
local co_wrap = coroutine.wrap
local co_yield = coroutine.yield
local type = type
local assert = assert
local select = select
local u4 = mem.u4

local internal = debug.getregistry()

local _KNOWNGLOBALS

local function DebugRet(ok, ...)
	return {ok and select('#', ...), ...}
end

local ConsoleVisible
local function HideConsole()
	if ConsoleVisible then
		ConsoleVisible = false
		internal.DebugConsole(nil, false)
	end
end

local function DoDebug(str)
	str = internal.DebugConsole(str, false) or ""
	ConsoleVisible = (str ~= "")
	if str == "" then
		return
	end
	local f, err = loadstring("return "..str, "")
	if f == nil then
		f, err = loadstring(str, "")
	end
	if f == nil then
		return DoDebug(err.."\n")
	end
	assert(type(f)=="function")
	local ret = DebugRet(pcall(f))
	if ret[1] then
		str = ""
		for i = 2, ret[1] + 1 do
			str = str..'  '..tostring2(ret[i])..'\n'
		end
	else
		str = ret[2].."\n"
	end
	return DoDebug(str)
end

function internal.events.ShowDialog()
	HideConsole()
end


local DebugModeWritten

function debug.debug(str)
	if str or not DebugModeWritten then
		DebugModeWritten = not str
		str = "————————————————————————————————————————————————————————————————————————————————\n"
	      ..(str or "Debug Mode Started")
	      .."\n--------------------------------------------------------------------------------\n"
	      --.."\n________________________________________________________________________________"
	      --.."\n================================================================================"
	else
		str = ""
	end

	if co_running() then
		DoDebug(str)
	else
		co_wrap(DoDebug)(str)
	end
end

