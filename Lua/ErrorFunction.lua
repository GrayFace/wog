local internal = debug.getregistry()
local getinfo = debug.getinfo
local getlocal = debug.getlocal
local getupvalue = debug.getupvalue
local insert = table.insert
local concat = table.concat
local type = type
local tostring = tostring
local error = error
local assert = assert
local format = string.format
local sub = string.sub
local traceback = debug.traceback
local rawget = rawget
local getmetatable = debug.getmetatable
local xpcall = xpcall
local co_create = coroutine.create
local co_yield = coroutine.yield
local co_resume = coroutine.resume
local co_status = coroutine.status
local co_running = coroutine.running
local co_main = coroutine.main
local _G = _G

----------- No globals from this point ------------

local _NOGLOBALS

-- ErrorLevel

local ErrorLevel = 1

function internal.SetErrorLevel(level)
	ErrorLevel = level
end

local function GetErrorLevel(co)
	local lev = ErrorLevel
	ErrorLevel = 1
	if lev <= 0 then
		return 0
	end
	if not co then  -- not a 'remote' error
		lev = lev + 1
		co = co_running() or co_main
	end
	local d = getinfo(co, 3, "f")
	if d ~= nil then
		d = d.func
		if d == error or d == assert then
			lev = lev + 1
		end
	end
	return lev, co
end

-- errorinfo (e.g. pass a file name to it before processing a file and file name would be appended in case of an error)

local errorExtraInfo = ""

function _G.errorinfo(s)
	local old = errorExtraInfo
	if s ~= nil then
		errorExtraInfo = tostring(s)
	end
	return old
end
_G.ErrorInfo = _G.errorinfo

-- tostring2

local function ShortenStr(s, lim)
	if not lim or #s <= lim then
		return format("%q", s)
	end
	local L = (lim - lim % 2)/2
	return format("%q..[%d symbols cut]..%q", sub(s, 1, L), #s - lim, sub(s, L - lim))
end

local function tostring2(v, lim)
	local t = type(v)
	if v == nil or t == "number" or t == "boolean" then
		return tostring(v)
	elseif t == "string" then
		return ShortenStr(v, lim)
	else
		local t = getmetatable(v)
		if t and rawget(t, "__tostring") then
			return '('..ShortenStr(tostring(v), lim)..')'
		else
			return '('..tostring(v)..')'
		end
	end
end
_G.tostring2 = tostring2

-- DefErrorFunction

local function OutputVars(text, co, lev)
	local d, func, funcname
	local i = lev
	repeat
		d = getinfo(co, i, "fnSlu")
		i = i + 1
	until d == nil or d.func and d.what ~= "C"
	if d then
		func = d.func
		funcname = d.name
		if not funcname or funcname == "" then
			funcname = d.short_src
			if d.currentline >= 0 then
				funcname = funcname..":"..d.currentline
			end
		end
		lev = i - 1
	end

	if not func then
		return
	end

	if d.nparams > 0 or getlocal(co, lev, -1) then
		text[#text + 1] = funcname and format("\n\narguments of '%s':", funcname) or '\n\narguments:'
		for i = 1, d.nparams do
			local a, v = getlocal(co, lev, i)
			text[#text + 1] = "\n\t"..a.." = "..tostring2(v, 1000)
		end
		for i = 1, 1000 do
			local a, v = getlocal(co, lev, -i)
			if not a then
				break
			end
			text[#text + 1] = "\n\t... = "..tostring2(v, 1000)
		end
	end
	i = d.nparams + 1
	local a, v = getlocal(co, lev, i)
	if a then
		text[#text + 1] = funcname and format("\n\nlocal variables of '%s':", funcname) or '\n\nlocal variables:'
		repeat
			text[#text + 1] = "\n\t"..a.." = "..tostring2(v, 1000)
			i = i + 1
			a, v = getlocal(co, lev, i)
		until a == nil
	end

	i = 1
	local a, v = getupvalue(func, i)
	if a then
		text[#text + 1] = funcname and format("\n\nupvalues of '%s':", funcname) or '\n\nupvalues:'
		repeat
			text[#text + 1] = "\n\t"..a.." = "..tostring2(v, 1000)
			i = i + 1
			a, v = getupvalue(func, i)
		until a == nil
	end
end

local function DefErrorFunction(s, co1)
	_G.print(s)
	if errorExtraInfo ~= "" then
		s = s.." ("..errorExtraInfo..")"
		errorExtraInfo = ""
	end
	local lev, co = GetErrorLevel(co1)
	if lev == 0 then
		return s
	end
	local text = {traceback(co, tostring(s).."\n", co1 and 0 or 2)}
	OutputVars(text, co, (co1 and lev - 1 or lev + 1))
	return concat(text)
end

-------------------------------------------------------------------------------
-- Call default error function if no error function is supplied
-------------------------------------------------------------------------------

local function handler(f)
	return function(s)
		if errorExtraInfo ~= "" then
			s = s.." ("..errorExtraInfo..")"
			errorExtraInfo = ""
		end
		return f(s, (GetErrorLevel()))
	end
end

_G.pcall = function(f, ...)
	return xpcall(f, DefErrorFunction, ...)
end

_G.xpcall = function(f, errf, ...)
	return xpcall(f, errf and handler(errf) or DefErrorFunction, ...)
end

local function resume_ret(co, ok, ...)
	if ok then
		return ok, ...
	end
	return ok, DefErrorFunction(..., co)
end

function _G.coroutine.resume(...)
	return resume_ret(..., co_resume(...))
end

local function wrap_ret(co, ok, ...)
	if ok then
		return ...
	end
	local msg = DefErrorFunction(..., co)
	ErrorLevel = 1
	return error(msg, 2)
end

function _G.coroutine.wrap(f)
	local co = co_create(f)
	return function(...)
		return wrap_ret(co, co_resume(co, ...))
	end
end

-------------------------------------------------------------------------------

return DefErrorFunction
