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
local traceback = debug.traceback
local co_create = coroutine.create
local co_yield = coroutine.yield
local co_resume = coroutine.resume
local co_status = coroutine.status
local co_running = coroutine.running

local ErrorLevel = 1

function internal.SetErrorLevel(level)
	ErrorLevel = level
end

local function GetErrorLevel(handlerLevel)
	local lev = ErrorLevel
	ErrorLevel = 1
	if lev <= 0 then
		return 0
	end
	lev = lev + 2
	local d = getinfo(3, "f")
	if d ~= nil then
		d = d.func
		if d == error or d == assert then
			lev = lev + 1
		end
	end
	return lev
end

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

local function DefErrorFunction(s, lev)
	if lev == 0 then
		return s
	end
	local text = {traceback(tostring(s).."\n", 2)}
	
	local d, func, funcname
	local i = lev
	repeat
		d = getinfo(i, "fnSl")
		i = i + 1
	until d == nil or d.func
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

	if func then
		i = 1
		local a, v = getlocal(lev, i)
		if a then
			text[#text + 1] = funcname and format("\n\nlocal variables of '%s':", funcname) or '\n\nlocal variables:'
			repeat
				text[#text + 1] = "\n\t"..a.." = "..tostring2(v)
				i = i + 1
				a, v = getlocal(lev, i)
			until a == nil
		end

		i = 1
		local a, v = getupvalue(func, i)
		if a then
			text[#text + 1] = funcname and format("\n\nupvalues of '%s':", funcname) or '\n\nupvalues:'
			repeat
				text[#text + 1] = "\n\t"..a.." = "..tostring2(v)
				i = i + 1
				a, v = getupvalue(func, i)
			until a == nil
		end
	end

	return concat(text)
end

-------------------------------------------------------------------------------
-- My implementation of copcall. Partially based on www.keplerproject.org
-------------------------------------------------------------------------------

local pcallErr, pcallCo

local function co_resumeEx_ret(old1, old2, ...)
	pcallErr, pcallCo = old1, old2
	return ...
end

local function co_resumeEx(err, baseco, ...)
	local old1, old2 = pcallErr, pcallCo
	if old1 == err and old2 == baseco then
		return co_resume(...)
	end
	pcallErr, pcallCo = err, baseco
	return co_resumeEx_ret(old1, old2, co_resume(...))
end

function coroutine.resume(...)
	local old1, old2 = pcallErr, pcallCo
	if old1 == nil and old2 == nil then
		return co_resume(...)
	end
	pcallErr, pcallCo = nil, nil
	return co_resumeEx_ret(old1, old2, co_resume(...))
end

function coroutine.running()
	return pcallCo or co_running()
end

-------------------------------------------------------------------------------
-- Implements ypcall - pcall with custom yield function
-------------------------------------------------------------------------------

local oldpcall, oldxpcall = pcall, xpcall

local function handleReturnValue(yf, err, pco, co, status, ...)
	if status and co_status(co) == 'suspended' then
		return handleReturnValue(yf, err, pco, co, co_resumeEx(err, pco, co, yf(...)))
	end
	return status, ...
end

function ypcall(f, err, yf, ...)
	local res, co = oldpcall(co_create, f)
	if not res then  -- C function
		assert(type(f) == "function")
		co = co_create(function(...)  return f(...)  end)
	end
	local pco = pcallCo or co_running()
	
	return handleReturnValue(yf or co_yield, err, pco, co, co_resumeEx(err, pco, co, ...))
end
local ypcall = ypcall

-------------------------------------------------------------------------------
-- Implements pcall and xpcall with coroutines
-------------------------------------------------------------------------------

function pcall(f, ...)
	if co_running() then
		return ypcall(f, nil, nil, ...)
	else
		return oldpcall(f, ...)
	end
end

function xpcall(f, err, ...)
	if co_running() then
		return ypcall(f, err, nil, ...)
	elseif err ~= nil then
		local function callErr(s)
			return err(s, GetErrorLevel())
		end
		return oldxpcall(f, callErr, ...)
	else
		return oldxpcall(f, nil, ...)
	end
end

-------------------------------------------------------------------------------
-- Extra information for errors
-------------------------------------------------------------------------------

local errorExtraInfo = ""

function errorinfo(s)
	local old = errorExtraInfo
	if s ~= nil then
		errorExtraInfo = tostring(s)
	end
	return old
end
ErrorInfo = errorinfo

-------------------------------------------------------------------------------

local function ErrorFunction(s)
	if errorExtraInfo ~= "" then
		s = s.." ("..errorExtraInfo..")"
		errorExtraInfo = ""
	end
	local f = pcallErr or DefErrorFunction
	return f(s, GetErrorLevel())
end

return ErrorFunction
