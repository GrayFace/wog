--[[
NoGlobals v0.0
(c) 2015 Sergey Rozhenko, MIT license

What it enforces:
- After "local _NOGLOBALS" declaration no direct global access is allowed. local _G = _G; _G.* is still allowed.
  Checking can be stopped by "local _NOGLOBALS_END" declaration and then can be re-enabled by subsequent _NOGLOBALS.
- If _NOGLOBALS is too restrictive, you can declare "local _KNOWNGLOBALS". In this mode only access to globals not
  present in environment at the time the function was loaded and not accessed by the function before _KNOWNGLOBALS
  declaration would be reported. For example, I typically use it like this:
    local _KNOWNGLOBALS = nil, glob1, glob2, glob3  (globals I need that aren't present when script is loaded)
- Even less restrictive is _KNOWNGLOBALS_F, which allows declaring new global functions.
- _NOGLOBALS_F is a similar option for otherwise strict no-globals mode.
- All checking is done at file load time.
- Note that these declarations abide by normal scoping rules, so _NOGLOBALS defined in a function would only have
  effect inside the function.

How it works:
  A light-weight Lua parser assumes Lua code has been previously validated by Lua.

Typical usage:
  local NoGlobals = require("RSNoGlobals")
  NoGlobals.Activate()  -- replace all loading functions
  NoGlobals.CheckChunkFile(1, 0)  -- check current file as well

Functions:

CheckStr(string, chunk)
  Checks the string and uses 'chunk' as the name for errors. Returns error string or nothing.
  If 'chunk' is a function, uses its name.
  If 'chunk' is nil, returns the error in form "line_num: error"

CheckChunkFile(f, raise)
  Checks the source file of 'f'. 'f' can be a stack level.
  If 'raise' is specified, raises the error at 'raise' level.

Activate()
  Replaces load, loadstring, loadfile, require and dofile functions, making them check the file.

Options = {
  AddOperators(binary, ...) adds given names to binary ('binary' = true) or unary ('binary' = false) operators list.
  SkipUnknown, when set to true, makes the parser try to skip unknown symbolic operators.
  NameCharCodes is a table with valid indentifier char codes as keys and 'true' as values.
  SpaceCharCodes is a similar table for whitespace char codes.
}

--]]
local abs, floor, ceil, max, min = math.abs, math.floor, math.ceil, math.max, math.min
local type = type
local pairs = pairs
local ipairs = ipairs
local setmetatable = setmetatable
local assert = assert
local error = error
local _loadstring = loadstring or load
local unpack = unpack
local concat = table.concat
local table_sort = table.sort
local debug_getinfo = debug and debug.getinfo
local debug_getupvalue = debug and debug.getupvalue
local io_open = io.open
local sub = string.sub
local match = string.match
local byte = string.byte
local format = string.format
local HasGoto = _loadstring("::a::") and true
local Lua52 = HasGoto and (_VERSION ~= "Lua 5.1")
local LuaJIT = _loadstring("#") and true  -- jit.* might be hidden

local getfenv = getfenv or debug_getinfo and debug_getupvalue and function(f)
	f = (type(f) == 'function' and f or assert(f ~= 0) and debug_getinfo(f + 1, 'f').func)
	local name, val
	local up = 0
	repeat
		up = up + 1
		name, val = debug_getupvalue(f, up)
	until name == '_ENV' or name == nil
	return val
end
getfenv = getfenv or function()
	return _ENV
end

----------- No globals from this point ------------

local _NOGLOBALS

----------- Main Vars ------------

local P = {Options = {}}
local str, pos, C, globals  -- C for common state, globals for known globals

local NameCodes = {[0x5F] = true}  -- use LuaJIT permissive default
for i = 0x30, 0x39 do
	NameCodes[i] = true
end
for i = 0x41, 0x5A do
	NameCodes[i] = true
end
for i = 0x61, 0x7A do
	NameCodes[i] = true
end
for i = 0x80, 0xFF do
	NameCodes[i] = true
end
P.Options.NameCharCodes = NameCodes

----------- General functions ------------

local function MatchStr(s)
	if sub(str, pos, pos + #s - 1) == s then
		pos = pos + #s
		return s
	end
end

local function MatchPattern(s)
	local n, n1 = match(str, s.."()", pos)
	C.LastCapture = n
	pos = (n1 or n or pos)
	return n
end

local function Error(s, ...)
	local p, n = pos, 1  -- find line number
	pos = 1
	while MatchPattern("([\r\n])") and pos < p do
		if C.LastCapture == "\r" then
			MatchStr("\n")
		end
		n = n + 1
	end
	C.Error = C.Error or format(s, n, ...)
	pos = #str + 1
end

----------- Space and comments ------------

local SpaceCodes = {[32] = true}
for i = 9, 13 do
	SpaceCodes[i] = true
end
P.Options.SpaceCharCodes = SpaceCodes

local function Space()
	while true do
		local v = byte(str, pos)
		if SpaceCodes[v] then
			pos = pos + 1
		elseif v == 0x2D and MatchStr("--") then
			if not MatchPattern("^%[(=*)%[.-%]%1%]") and not MatchPattern("[\r\n]") then
				pos = #str + 1
			end
		else
			return v
		end
	end
end

local function Name()
	local old = pos
	while NameCodes[byte(str, pos)] do
		pos = pos + 1
	end
	return sub(str, old, pos - 1)
end

----------- Parsers ------------

local Parsers = {}
local E, locals  -- E for current ExpList state

local SLeftSide = 1
local SLocals = 2
local SRightSide = 3
-- negative state means a variable has been read, binary operator or new statement is expected
local SNewLine = -SLeftSide

local function Reg(f, op, ...)
	Parsers[op] = f
	return (...) and Reg(f, ...) or f
end

local function StoreLocals(donot)
	if donot then
		return
	end
	for k, v in pairs(E.Locals) do
		locals[k] = v
	end
	E.Locals = {}
	if E.OnNewStatement then
		E.OnNewStatement()
	end
end

-- ExpList
local function ExpList(BaseState, RetState, IsBlock)
	local Last, LastLocals = E, locals
	E.State = RetState or -abs(E.State)
	E = {State = BaseState or SNewLine, Locals = {}, IsBlock = IsBlock, Token = C.Token}
	locals = IsBlock and setmetatable({}, {__index = locals}) or locals
	function E.End()
		locals = LastLocals
		E = Last
	end
end

local function Block(BaseState, RetState)
	StoreLocals()
	ExpList(BaseState, RetState or SNewLine, true)
end

-- vars
local function VarName(s, newF)
	StoreLocals(E.State > 0)
	if E.Token == "{" and abs(E.State) < SRightSide and Space() and match(str, "^=", pos) then
		-- table field declaration (do nothing)
	elseif s == "" then
		-- pos = #str + 1
		if pos <= #str then
			s = Name()
			Error("%d: NoGlobals parser failure near %q", (s ~= "" and s or sub(str, pos, pos)))
		end
	elseif abs(byte(s) - 0x35 + 0.5) < 5 then  -- number
		MatchPattern("^%.[0-9a-fA-FpP]*")
		s = match(str, "^[eEpP]", pos - 1) and MatchPattern("^[%+%-][0-9]+")  -- general form
	elseif E.State == SLocals then  -- new local
		E.Locals[s] = true
		if s == "_NOGLOBALS_END" then
			E.Locals._NOGLOBALS = false
		elseif s == "_KNOWNGLOBALS" then
			E.Locals._NOGLOBALS = "K"
		elseif s == "_NOGLOBALS_F" then
			E.Locals._NOGLOBALS = "F"
		elseif s == "_KNOWNGLOBALS_F" then
			E.Locals._NOGLOBALS = "KF"
		end
	elseif not locals[s] then  -- variable access
		local NG = locals._NOGLOBALS
		if NG == true or NG == "F" and not newF then
			Error("%d: use of global variable %q after _NOGLOBALS%s declaration", s, NG == "F" and "_F" or "")
		elseif NG and globals[s] == nil and (NG ~= "KF" or not newF) then
			Error("%d: use of unknown global variable %q after _KNOWNGLOBALS%s declaration", s, NG == "KF" and "_F" or "")
		end
		globals[s] = true
	end
	E.State = -max(E.State, SLeftSide)
end

Reg(function()
	E.State = -SRightSide
end, "...")

-- unary operations
local UnOp = Reg(function()
end, "#", "not")

-- binary operations
local BinOp = Reg(function()
	E.State = abs(E.State)
end, '+', '-', '*', '/', '^', '%', '..', '<', '<=', '>', '>=', '==', '~=', 'and', 'or')
-- Lua53 binary operators, also may be in Lua patches ('\' is added for possible patches)
for _, op in ipairs{'//', '&', '~', '|', '>>', '<<', '\\'} do
	if _loadstring("return a"..op.."a") then
		Reg(BinOp, op)
	end
end

-- string
Reg(function(chr)
	while MatchPattern("(["..chr.."\\])") and C.LastCapture == "\\" do
		pos = pos + 1
	end
	E.State = -SRightSide
end, "'", '"')

-- '.', ':'
Reg(function()
	Name(Space())
end, ".", ":")

-- brackets
Reg(function()
	if MatchPattern("^(=*)%[.-%]%1%]") then  -- long string
		E.State = -SRightSide
	else
		ExpList(SRightSide)
	end
end, "[")

Reg(function()
	ExpList(SRightSide)
end, "(")

Reg(function()
	ExpList(SNewLine)
end, "{")

-- assignment
Reg(function()
	E.State = SRightSide
end, "=", "in")

-- local
Reg(function()
	StoreLocals()
	E.State = SLocals
end, "local")

-- if, while, return
Reg(function()
	StoreLocals()
	E.State = SRightSide
end, "if", "while", "return")

-- line ends
local SetNewLine = Reg(function()
	StoreLocals()
	E.State = SNewLine
end, ";", "break")

Reg(function()
	E.State = (E.Token == "{" and SNewLine or abs(E.State))
end, ",")

-- expression list ends
Reg(function()
	StoreLocals()
	E.End()
end, "]", "}", ")", "end")

Reg(function()
	StoreLocals()
	E.End()
	E.State = SRightSide
end, "elseif")

Reg(function()
	E.OnNewStatement = E.End
	E.State = SRightSide
end, "until")

Reg(function()
	StoreLocals()
	E.End()
	Block()
end, "else")

-- blocks
Reg(function()
	Block()
end, "repeat", "then")

Reg(function()
	if E.Token == "for" then
		E.Token = "do"
		return SetNewLine()
	end
	Block()
end, "do")

-- for
Reg(function()
	Block(SLocals)
end, "for")

-- function
Reg(function()
	local self
	if E.State ~= SRightSide then
		local s = Name(Space())
		while Space() and MatchPattern("^([%.:])") do
			self = (C.LastCapture == ":")
			Name(Space())
		end
		VarName(s, self == nil)
	end
	Block(SNewLine, (E.State == SRightSide and -SRightSide or SNewLine))
	locals.self = self or nil
	Space()
	C.Token = MatchStr("(")
	ExpList(SLocals, SNewLine)
end, "function")

-- goto
Parsers["goto"] = HasGoto and function(s)
	if not E.IsBlock or E.State > SNewLine or Name(Space()) == "" then
		return VarName(s)
	end
	E.State = SNewLine
end

Parsers["::"] = HasGoto and function()
	Space(Name(Space()))
	MatchPattern("::")
	E.State = SNewLine
end

----------- TokenLists ------------
-- create a [first char] -> [tokens array] mapping for symbolic tokens

local function NotName(s, i)
	return #s < i or not NameCodes[byte(s, i)] and NotName(s, i + 1)
end

local TokenLists
local function RebuildParsers()
	TokenLists = {}
	for i = 0, 255 do
		TokenLists[i] = {}
	end
	for s, f in pairs(Parsers) do
		if NotName(s, 1) then
			local t = TokenLists[byte(s)]
			t[#t + 1] = s
		end
	end
	for i = 0, 255 do
		local t = TokenLists[i]
		table_sort(t)  -- longer matches would be at the end of the list
		TokenLists[i] = t[1] and t  -- remove empty lists
	end
end
RebuildParsers()

local function FindToken(t)
	if t then
		for i = #t, 1, -1 do
			if MatchStr(t[i]) then
				return t[i]
			end
		end
	end
end

----------- ParseLoop ------------

local function ParseLoop()
	while pos <= #str do
		local s = FindToken(TokenLists[Space()]) or Name()
		if s == "" and P.Options.SkipUnknown then
			pos = pos + 1
		else
			C.Token = s
			-- print(s, E.State, E.Token)--, pos)
			;(Parsers[s] or VarName)(s)
		end
	end
end

----------- Module functions ------------

local function GetChunkName(chunk)
	local d = debug_getinfo and debug_getinfo(chunk, "nSu")
	return d and (d.name ~= "" and d.name or d.short_src)
end

local function CheckStr(fstr, chunk)
	if byte(fstr) == 33 then
		return  -- '!' signals a binary chunk produced by string.dump
	end
	str = fstr
	pos = 1
	C = {}
	globals = getfenv(type(chunk) == "function" and chunk or 1)
	globals = setmetatable({}, {__index = globals})
	locals = {["nil"] = true, ["false"] = true, ["true"] = true}
	E = {Locals = {}}
	if Lua52 then
		locals._ENV = true
	end
	MatchStr("\239\187\191")  -- UTF-8 BOM
	MatchPattern("^#[^\r\n]*")  -- Unix exec. file?
	Block()
	ParseLoop()
	if C.Error and chunk then
		if type(chunk) ~= "string" then
			chunk = GetChunkName(chunk) or "?"
		end
		return chunk..":"..C.Error
	end
	return C.Error
end
P.CheckStr = CheckStr

local function DoCheckChunkFile(f, pattern)
	local d = debug_getinfo and debug_getinfo(f, "fS")
	local fname = d and d.source and match(d.source, "^@(.*%.[lL][uU][aA])$")
	if fname then
		local file = io_open(fname, "rb")
		if file then
			local s = file:read("*a")
			file:close()
			return CheckStr(s, d.func or fname)
		end
	end
end

function P.CheckChunkFile(f, raise)
	if type(f) == "number" then
		f = f + 2
	end
	local s = DoCheckChunkFile(f, "^@(.+)")
	if s and raise then
		error(s, raise)
	end
	return s
end

function P.Options.AddOperators(binary, ...)
	Reg(binary and BinOp or UnOp, ...)
	RebuildParsers()
end

function P.Activate()
	local _NOGLOBALS_END  -- allow globals again
	local _load = load
	local _loadstring = loadstring or load
	local _loadfile = loadfile
	local _package = package
	local _loaded = package.loaded
	local LOADERS = Lua52 and "searchers" or "loaders"
	local reg = debug and debug.getregistry and debug.getregistry()
	local io_open = io.open
	local stdin = io.input()
	local s_find = string.find
	local _KNOWNGLOBALS

	local function myloadstring(ld, ...)
		local f, err = _loadstring(ld, ...)
		err = not f and err or CheckStr(ld, debug_getinfo and f or match((...) or "", "^@?.*"))
		if err then
			return nil, err
		end
		return f
	end
	local function myload(ld, src, ...)
		local t, i = {}, 1
		local s = ld()
		while s do
			t[i], i = s, i + 1
			s = ld()
		end
		return myloadstring(concat(t), src or "=(load)", ...)
	end

	if HasGoto then  -- LuaJIT or Lua 5.2+ - load accepts strings, loadstring is an alias or doesn't exist
		function load(ld, ...)
			return (type(ld) == "string" and myloadstring or myload)(ld, ...)
		end
		loadstring = loadstring and load
	else
		load = myload
		loadstring = myloadstring
	end

	function loadfile(fname, ...)
		if not fname then
			return _loadfile(fname, ...)
		end
		local f, err = io_open(fname, LuaJIT and "rb" or "r")
		if not f then
			return f, err
		end
		local c = f:read(1)
		if c == "#" then  -- # - Unix exec. file?
			repeat  -- skip line
				c = f:read(1)
			until c == "\r" or c == "\n" or not c
			c = c and c..(f:read(1) or "") or ""
			c = (c == "\r\n" and c..(f:read(1) or "") or c)
			f:seek(-#c)  -- keep line ending
			c = sub(c, -1)  -- still may meet binary data after it
		else
			f:seek("set")
		end
		if c == "!" then
			f:close()
			return _loadfile(fname, ...)  -- binary
		end
		local s = f:read("*a")
		f:close()
		return myloadstring(s, "@"..fname, ...)
	end
	local loadfile = loadfile

	function dofile(fname)
		local f, err = loadfile(fname)
		if f then
			return f()
		else
			error(err, 0)
		end
	end

	if not debug_getinfo or not reg then
		return  -- 'require' needs 'debug' module
	end

	local function FindModule(name)
		local errors = ""
		for _, loader in ipairs(_package[LOADERS]) do
			local f, param = loader(name)
			local tp = type(f)
			if tp == "function" then
				return f, param
			elseif tp == "string" or tp == "number" then
				errors = errors..f
			end
		end
		return nil, format("module '%s' not found:%s", name, errors)
	end

	local loading = {}

	function require(name)
		local loaded = reg._LOADED
		local t = loaded[name]
		if t ~= nil then
			return t
		elseif loading[name] then
			error(format("loop or previous error loading module '%s'", name), 2)
		elseif type(_package[LOADERS]) ~= "table" then
			error("'package."..LOADERS.."' must be a table", 2)
		end
		local f, param = FindModule(name)
		if not f then
			error(param, 2)
		end
		loading[name] = true
		-- check module, require .lua extension to ensure only Lua modules are checked
		local s = DoCheckChunkFile(f, "^@(.*%.[lL][uU][aA])$")
		if s then
			error(s, 0)
		end
		-- call it
		if Lua52 then
			t = f(name, param)
		else
			t = f(name)
		end
		loading[name] = nil
		if t ~= nil then
			loaded[name] = t
		else
			t = loaded[name]
			if t == nil then
				t, loaded[name] = true, true
			end
		end
		return t
	end
end

return P