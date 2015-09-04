--[[----------------------------------------------------------------------------

RSMem
Copyright(c) Sergey Rozhenko aka GrayFace (sergroj@mail.ru)

Input:
mem                  | from RSMemRegister of RSMem.cpp
mem.pcall2           | same as pcall, but displays error message
mem.persist          | __persist is set to this
mem.struct_callback  | function(t, class, fields, offs, rofields) called when a struct object is created, returns the object
mem.union_callback   | function(t, fields, offs, rofields) called when a union is created, returns the union
mem.array_callback   | function(t) called when an array is created, returns the array
mem.member_callback  | function(name, define[, newfields, newoffs, newrofields]) called before a member is added to structure (additonal parameters indicate a union)

To control in calling script:
mem.free
mem.malloc
mem.realloc
mem.new
WoG: internal.LastStructMember in member_callback

Output:
mem - table with all functions
mem.general - some general purpose functions declared in RSMem.lua

CustomType:
define.CustomType('Name', size, function(offset, obj, name, val))

!!! error in mem.string if access violation

--]]----------------------------------------------------------------------------

local internal = mem

local print = print

local type = type
local unpack = unpack
local error = error
local assert = assert
local format = string.format
local string_byte = string.byte
local string_sub = string.sub
local string_lower = string.lower
local string_match = string.match
local string_gsub = string.gsub

local next = next
local pairs = pairs
local tonumber = tonumber
local tostring = tostring
local rawget = rawget
local rawset = rawset
local pcall2 = mem.pcall2 or function(f, ...)  return true, f(...)  end
local getmetatable = debug.getmetatable
local d_setmetatable = debug.setmetatable
local table_insert = table.insert
local math_floor = math.floor
local math_ceil = math.ceil
local abs = math.abs
local max = math.max
local min = math.min
local bit_Or = internal.bit_Or
local bit_And = internal.bit_And
local bit_AndNot = internal.bit_AndNot
local nullpersist = mem.persist
local rawcall = internal.call
local IsBadCodePtr = internal.IsBadCodePtr
local IsBadReadPtr = internal.IsBadReadPtr
local IsBadWritePtr = internal.IsBadWritePtr
local GetHookSize = internal.GetHookSize or function()  return 5  end
local GetInstructionSize = internal.GetInstructionSize

local function nocallback(a)
	return a
end

local function nullsub()
end

local struct_callback
local union_callback
local array_callback
local member_callback
local mem_ptr_callback

function internal.UpdateCallbacks()
	struct_callback = internal.struct_callback or nocallback
	union_callback = internal.union_callback or nocallback
	array_callback = internal.array_callback or nocallback
	member_callback = internal.member_callback or nullsub
	mem_ptr_callback = internal.mem_ptr_callback or nullsub
end
internal.UpdateCallbacks()

mem = {}
local pkl = (package or {}).loaded or {}
local _mem = mem
pkl.mem = mem
mem.general = {}
local general = mem.general  -- general scripts


----------- No globals from this point ------------

local _NOGLOBALS

-----------------------------------------------------
-- General functions
-----------------------------------------------------

local function roError(a, lev)  error('attempt to modify a read-only field "'..a..'".', lev + 1)  end

local function setmetatable(t, m)
	d_setmetatable(t, m)
	return t
end

local function assertnum(v, level, msg)
	return tonumber(v) or (error(msg or "number expected, got "..type(v), (level or 1) + 1))
end

local function tostring2(v)
	if type(v) == "string" then
		return '"'..v..'"'
	else
		return tostring(v)
	end
end

local function CallMetatableIndex(t, a, f, IsFunction)
	if IsFunction == nil then
		IsFunction = type(f) == "function"
	end
	if IsFunction then
		return f(t, a)
	elseif f ~= nil then
		return f[a]
	end
end
general.CallMetatableIndex = CallMetatableIndex

local function CallMetatableNewIndex(t, a, v, f, IsFunction)
	if IsFunction == nil then
		IsFunction = type(f) == "function"
	end
	if IsFunction then
		return f(t, a, v)
	elseif f ~= nil then
		f[a] = v
	else
		return rawset(t, a, v)
	end
end
general.CallMetatableNewIndex = CallMetatableNewIndex

local function table_copy(src, dest, overwrite)
	if not dest or overwrite then
		dest = dest or {}
		for k,v in pairs(src) do
			dest[k] = v
		end
	else
		for k,v in pairs(src) do
			if dest[k] == nil then
				dest[k] = v
			end
		end
	end
	return dest
end

local function destructor_obj(t, f)
	local obj = internal.NewObj()
	local m = {}
	setmetatable(obj, m)

	function m.__gc(udata, ...)
		f(t, ...)
	end
	m.__persist = nullpersist

	return obj
end

local function table_destructor(t, f)
	local obj = destructor_obj(t, f)
	return function(t, ...)
		local m = getmetatable(obj)
		m.__gc(obj, ...)
		m.__gc = nil
		m.__persist = nil
		obj = nil
	end
end
general.table_destructor = table_destructor


-----------------------------------------------------
-- Actual mem functions
-----------------------------------------------------

-- mem.string(p)  - read null-terminated string
-- mem.string(p, size)  - read null-terminated string not more than 'size' bytes
-- mem.string(p, size, true)  - read 'size' bytes as string
local mem_string = internal.Mem_String
_mem.string = mem_string
_mem.String = mem_string

-- mem.call and mem.func return conversions

local retconv_nums = {
	["nil"] = "nil",
	i1 = -0x100,
	i2 = -0x10000,
	u1 = 0x100,
	u2 = 0x10000,
	u4 = 0x100000000,
}

local function retconv(v, conv)
	if conv == "i4" or conv == nil then
		return v
	end
	local iconv = retconv_nums[conv]
	if iconv then
		if iconv > 0 then
			return v % iconv
		end
		v = v % (-iconv)
		return v*2 < -iconv and v or v + iconv
	end
	
	local t = type(conv)
	if t == "boolean" then
		return v ~= 0
	elseif t == "string" then
		return v == 0 and "" or mem_string(v)
	elseif t == "table" then
		return conv:new(v)
	end
end

local function call(t, ...)
	local ist = type(t) == "table"
	local p = assertnum(ist and t.p or t, 2)
	if rawcall(IsBadCodePtr, 0, p) ~= 0 then
		error(format("attempt to call invalid address %X", p), 2)
	end
	if ist then
		return retconv(rawcall(p, t.cc or 0, unpack(t)), t.ret)
	else
		return rawcall(p, ...)
	end
end
_mem.call = call

local function ucall(...)
	local r = rawcall(...)
	if r < 0 then
		r = r + 0x100000000
	end
	return r
end

local IgnoreCount = 0
local IgnoreInc = {[true] = 1, [false] = -1}

function _mem.IgnoreProtection(on)
	IgnoreCount = IgnoreCount + (IgnoreInc[on] or 0)
	assert(IgnoreCount >= 0)
	return IgnoreCount > 0
end


_mem.topointer = internal.toaddress
local Mem_GetNum = internal.Mem_GetNum
local Mem_SetNum = internal.Mem_SetNum

local function malloc(size)
	return ucall(internal.malloc, 0, assertnum(size, 2))
end
_mem.malloc = malloc
_mem.alloc = malloc

function _mem.free(p)
	rawcall(internal.free, 0, assertnum(p, 2))
end

function _mem.realloc(p, size)
	return ucall(internal.realloc, 0, assertnum(p, 2), assertnum(size, 2))
end

function _mem.new(size, f, ...)
	local ret
	if type(size) == "table" then
		ret = ucall(internal.malloc, 0, assertnum(size["?size"], 2))
		return size:new(ret)
	else
		ret = ucall(internal.new, 0, assertnum(size, 2))
		if f then
			call(f, 1, ret, ...)
		end
		return ret
	end
end

local StaticAlloc = malloc
if internal.StaticAlloc then
	function StaticAlloc(size)
		return ucall(internal.StaticAlloc, 0, assertnum(size, 2))
	end
	_mem.StaticAlloc = StaticAlloc
end


local function read_error(p, size, level)
	error(format('memory at address %X of size %d cannot be read', p, size), level + 1)
end

local function write_error(p, size, level)
	error(format('memory at address %X of size %d cannot be written to', p, size), level + 1)
end

local function code_error(p, level)
	error(format('memory at address %X is not executable', p), level + 1)
end

local VirtualProtect_tmp = StaticAlloc(4)
local VirtualProtect_ptr = internal.VirtualProtect
-- local function VirtualProtect(p, size, prot)
	-- if prot == true then
		-- prot = 0x40
	-- elseif not prot then
		-- prot = Mem_GetNum(-4, VirtualProtect_tmp)
	-- end
	-- return call(VirtualProtect_ptr, 0, assertnum(p, 3), assertnum(size, 3), assertnum(prot, 3), VirtualProtect_tmp) ~= 0
-- end

local function Protect(p, size)
	p = assertnum(p, 3)
	size = assertnum(size, 3)
	local can, need = mem_ptr_callback(p, size)
	if can == false then
		write_error(p, size, 3)
	end
	if (need or (need == nil) and IgnoreCount > 0) and rawcall(VirtualProtect_ptr, 0, p, size, 0x40, VirtualProtect_tmp) ~= 0 then
		if rawcall(IsBadWritePtr, 0, p, size) ~= 0 then
			rawcall(VirtualProtect_ptr, 0, p, size, Mem_GetNum(-4, VirtualProtect_tmp), VirtualProtect_tmp)
			write_error(p, size, 3)
		end		
		return p, size
	end
	if rawcall(IsBadWritePtr, 0, p, size) ~= 0 then
		write_error(p, size, 3)
	end
end

local function Unprotect(p, size, ret)
	if p then
		rawcall(VirtualProtect_ptr, 0, p, size, Mem_GetNum(-4, VirtualProtect_tmp), VirtualProtect_tmp)
	end
	return ret
end

function _mem.copy(dest, src, count)
	local st = type(src)
	if st == "table" then
		count = count or src["?size"]
		src = src["?ptr"]
	elseif st == "string" then
		count = count or #src
	end
	dest, count = assertnum(dest, 2), assertnum(count, 2)
	if rawcall(IsBadReadPtr, 0, src, count) ~= 0 then
		read_error(src, count, 2)
	end		
	local a1, a2 = Protect(dest, count)
	return Unprotect(a1, a2, rawcall(internal.memcpy, 0, dest, src, count))
end
local mem_copy = _mem.copy

function _mem.fill(dest, n, c)
	if type(dest) == "table" then
		n = n or dest["?size"]
		dest = dest["?ptr"]
	end
	if type(c) == "string" then
		c = string_byte(c)
	else
		c = (c == true and 1 or c or 0)
	end
	dest, c, n = assertnum(dest, 2), assertnum(c, 2), assertnum(n, 2)
	local a1, a2 = Protect(dest, n)
	return Unprotect(a1, a2, rawcall(internal.memset, 0, dest, c, n))
end
local mem_fill = _mem.fill


local function memarr(x)
	local size = abs(x)
	if x == 5 then
		size = 4
	elseif x == 6 then
		size = 8
	elseif x == 7 then
		size = 10
	end
	
	local function index(t, a)
		local ret = Mem_GetNum(x, assertnum(a, 2))
		return ret or (read_error(a, size, 3))
	end

	local function newindex(t, a, v)
		local ret
		a, v = assertnum(a, 2), assertnum(v, 2)
		local a1, a2 = Protect(a, size)
		Unprotect(a1, a2, Mem_SetNum(x, a, v))
	end
	
	local call
	if x == 4 then
		function call(_, n)
			n = n % 0x100000000
			return n < 0x80000000 and n or n - 0x100000000
		end
	elseif x == -4 then
		function call(_, n)
			return n % 0x100000000
		end
	elseif x == 2 then
		function call(_, n)
			n = n % 0x10000
			return n < 0x8000 and n or n - 0x10000
		end
	elseif x == -2 then
		function call(_, n)
			return n % 0x10000
		end
	elseif x == 1 then
		function call(_, n)
			n = n % 0x100
			return n < 0x80 and n or n - 0x100
		end
	elseif x == -1 then
		function call(_, n)
			return n % 0x100
		end
	end

	return setmetatable({}, {__index = index, __newindex = newindex, __call = call})
end

_mem.i8 = memarr(8)
_mem.i4 = memarr(4)
_mem.i2 = memarr(2)
_mem.i1 = memarr(1)
_mem.u8 = memarr(-8)
_mem.u4 = memarr(-4)
_mem.u2 = memarr(-2)
_mem.u1 = memarr(-1)
_mem.r4 = memarr(5)
_mem.r8 = memarr(6)
_mem.r10 = memarr(7)
local i4, i2, i1, u8, u4, u2, u1 = _mem.i4, _mem.i2, _mem.i1, _mem.u8, _mem.u4, _mem.u2, _mem.u1

local function pchar_index(t, a)
	local p = u4[a + 0]
	local ret = mem_string(p)
	if ret then
		return ret
	end
	error(format('memory at address %X (referenced by %X) cannot be read', p, a), 2)
end
local function pchar_newindex(t, a, v)
	error('cannot assign a value to PChar', 2)
end

_mem.pchar = setmetatable({}, {__index = pchar_index, __newindex = pchar_newindex})
_mem.PChar = _mem.pchar

function _mem.LuaAlloc(size)
	local o, p = internal.NewObj(size)
	local t = {["?ptr"] = p}
	t.__index = t
	t.__newindex = t
	return setmetatable(o, t)
end

local function mem_function(def)
	local p = def.p
	if not p then
		error("function address not specified", 3)
	end
	local pfunc = type(p) == "function"
	local needstr = {}
	local deflen = #def
	for i = 1, deflen do
		local a = type(def[i])
		if a == "string" then
			needstr[i] = true
		elseif a == "function" or a == "thread" then
			error("unknown parameter type", 3)
		end
	end
	local cc = def.cc or 0
	local must = def.must or 0
	if must > deflen then
		deflen = must
	end
	local rettype = def.ret

	return function(...)
		local par = {...}
		local v
		for i = 1, deflen do
			v = par[i]
			if v == nil then
				if i <= must then
					error("not enough actual parameters", 2)
				end
				par[i] = def[i]
			elseif needstr[i] then
				-- if type(v) == "table" then
					-- par[i] = tostring(v["?ptr"])
				-- else
					par[i] = tostring(v)
				-- end
			end
		end
		
		if pfunc then
			return retconv(p(cc, unpack(par)), rettype)
		else
			return retconv(call(p, cc, unpack(par)), rettype)
		end
	end
end

function _mem.func(...)
	return (mem_function(...))
end

-----------------------------------------------------
-- mem.dll
-----------------------------------------------------

local dll_meta = {
	__index = function(t, k)
		local p = rawcall(internal.GetProcAddress, 0, t["?ptr"], k)
		if p ~= 0 then
			local function f(...)
				return rawcall(p, t["?cc"], ...)  -- reference to t is important to prevent GC of it
			end
			rawset(t, k, f)
			return f
		end
	end,
	
	__gc = function(t)
		if t["?ptr"] then
			rawcall(internal.FreeLibrary, 0, t["?ptr"])
			t["?ptr"] = nil
		end
	end
}

-- local DllNameBuf

local function mem_LoadDll(name, cc)
	local p = name
	if type(p) ~= "number" then
		p = ucall(internal.LoadLibrary, 0, string_gsub(name, "/", "\\"))
	end
	if p ~= 0 then
		return setmetatable({["?ptr"] = p, ["?cc"] = tonumber(cc) or 0, ["?path"] = name}, dll_meta)
	end
end
_mem.LoadDll = mem_LoadDll

local mem_dll = setmetatable({}, {__index = function(t, k)
	-- append .dll if no extension is specified
	local v = mem_LoadDll(type(k) ~= "number" and not string_match(k, "%.[^%.\\/:]*$") and k..".dll" or k)
	rawset(t, k, v)
	return v
end})
_mem.dll = mem_dll

function _mem.UnloadDll(t)
	rawcall(internal.FreeLibrary, 0, t["?ptr"])
	t["?ptr"] = nil
	if mem_dll[t["?path"]] == t then
		mem_dll[t["?path"]] = nil
	end
end

function _mem.GetProcAddress(p, proc)
	return call(internal.GetProcAddress, 0, p, proc)
end

-----------------------------------------------------
-- mem.struct
-----------------------------------------------------

do -- mem.struct
	local types = {}
	_mem.structs = {}
	_mem.structs.types = types
	local define
	local unions
	local definemeta = {__index = types}
	
	function _mem.structs.getdefine()
		return define
	end
	function _mem.structs.getunions()
		return unions
	end

	function types.goto(n)
		define.offset = n
		if n > define.size then
			define.size = n
		end
		return define
	end
	local goto = types.goto

	local function types_index(t, a)
		if type(a) == "number" then
			return goto(a)
		elseif a == "alt" then
			define.isalt = true
			return define
		elseif a == "ro" or a == "RO" then
			define.isro = true
			return define
		end
	end

	setmetatable(types, {__index = types_index})

	local function addsize(n)
		if not define.isalt then
			n = define.offset + n
			define.offset = n
			if n > define.size then
				define.size = n
			end
		end
		define.isalt = nil
	end

	local declare_array

	local function member(name, size, f)
		if define.members[name] and name ~= '' then
			error("Field already exists", 2)
		end
		member_callback(name, define)
		if unions and unions.amin then
			size, f = declare_array(size, f)
		end
		define.offsets[name] = define.offset
		addsize(size)
		define.members[name] = f
		if define.isro then
			define.rofields[name] = true
		end
		return define
	end

	types.CustomType = member
	_mem.structs.CustomType = member

	function types.skip(n)
		addsize(n)
		return define
	end


	local function arraydef(n, arr, bool)
		local handler
		if bool then
			function handler(o, obj, _, val)
				if val ~= nil then
					arr[obj["?ptr"] + o] = val and val ~= 0 and 1 or 0
				else
					return arr[obj["?ptr"] + o] ~= 0
				end
			end
		else
			function handler(o, obj, _, val)
				if val ~= nil then
					arr[obj["?ptr"] + o] = val
				else
					return arr[obj["?ptr"] + o]
				end
			end
		end
		return function(a)
			if define.offset % 1 ~= 0 then  -- if bits were involved
				define.offset = math_ceil(define.offset)
			end
			return member(a, n, handler)
		end
	end

	types.i8 = arraydef(8, _mem.i8)
	types.i4 = arraydef(4, _mem.i4)
	types.i2 = arraydef(2, _mem.i2)
	types.i1 = arraydef(1, _mem.i1)
	types.u8 = arraydef(4, _mem.u8)
	types.u4 = arraydef(4, _mem.u4)
	types.u2 = arraydef(2, _mem.u2)
	types.u1 = arraydef(1, _mem.u1)
	types.b4 = arraydef(4, _mem.u4, true)
	types.b2 = arraydef(2, _mem.u2, true)
	types.b1 = arraydef(1, _mem.u1, true)
	types.r4 = arraydef(4, _mem.r4)
	types.r8 = arraydef(8, _mem.r8)
	types.r10 = arraydef(10, _mem.r10)
	types.pchar = arraydef(4, _mem.pchar)
	types.PChar = types.pchar

	function types.string(len)
		len = assertnum(len, 2)
		return function(a)
			if define.offset % 1 ~= 0 then  -- if bits were involved
				define.offset = math_ceil(define.offset)
			end
			return member(a, len, function(o, obj, _, val)
				local p = obj["?ptr"] + o
				if val ~= nil then
					val = tostring(val)
					if #val >= len then
						mem_copy(p, val, len - 1)
						u1[p + len - 1] = 0
					else
						mem_copy(p, val, #val + 1)
					end
				end
				return mem_string(p, len)
			end)
		end
	end

	local bitHandlers = {}
	local bitIndex = {}
	do
		local curbit = 1
		for i = 0, 7 do
			local b = curbit
			bitHandlers[i] = function(o, obj, _, val)
				local p = obj["?ptr"] + math_floor(o)
				if val ~= nil then
					if val then
						u1[p] = bit_Or(u1[p], b)
					else
						u1[p] = bit_AndNot(u1[p], b)
					end
				else
					return bit_And(u1[p], b) ~= 0
				end
			end
			bitIndex[curbit] = i
			curbit = curbit*2
		end
	end

	local function AnyBitHandler(o, ...)
		return bitHandlers[math_floor((o*8)%8)](o, ...)
	end

	local function AnyABitHandler(o, ...)
		return bitHandlers[7 - math_floor((o*8)%8)](o, ...)
	end
	
	local function doBit(name, b, anti)
		if b then
			b = assertnum(b, 2)
			local o = 0
			while b >= 256 do
				b = b / 256
				o = o + 1
			end
			b = bitIndex[b]
			if b == nil then
				error("not a valid bit", 2)
			end
			local o1 = b/8
			b = bitHandlers[b]
			local off = define.offset
			define.offset = math_floor(off) + o + o1
			member(name, 1 - o1, b)
			define.offset = off
			return define
		else
			return member(name, 1/8, anti and AnyABitHandler or AnyBitHandler)
		end
	end
	
	function types.bit(name, b)
		return doBit(name, b, false)
	end

	function types.abit(name, b)
		return doBit(name, b, true)
	end
	
	local function any(int, size)
		size = assertnum(size, 3)
		if size < 1 or size % 1 ~= 0 then
			error("incorrect number size ("..size..")", 3)
		end
		local sizePow = 2^size
		local sizeHalf = sizePow/2
		return function(name)
			return member(name, size/8, function(o, obj, _, val)
				local p = obj["?ptr"] + math_floor(o)
				local start = (o % 1)*8
				local n = size + start
				local v
				if val then
					if int then
						while val < 0 do
							val = val + sizePow
						end
					end
					start = 2^start
					val = math_floor(val)*start
					if start > 1 then
						start = u1[p] % start
					else
						start = 0
					end
					while n > 8 do
						v = val % 256
						u1[p] = v + start
						val = (val - v)/256
						start = 0
						p = p + 1
						n = n - 8
					end
					if n > 0 then
						local pow = 2^n
						u1[p] = val % pow + bit_And(u1[p], 256 - pow) + start
					end
				else
					local ret, pow = 0, 2^(-start)
					while n > 8 do
						v = u1[p]
						ret = ret + v*pow
						p = p + 1
						n = n - 8
						pow = pow*256
					end
					if n > 0 then
						ret = ret + (u1[p] % (2^n))*pow
					end
					ret = math_floor(ret)
					if int and ret >= sizeHalf then
						return ret - sizePow
					end
					return ret
				end
			end)
		end
	end
	
	function types.uany(size)
		return (any(false, size))
	end

	function types.iany(size)
		return (any(true, size))
	end

	local function nonewf(t, a)
		error('field "'..tostring(a)..'" doesn\'t exist', 3)
	end
	
	local function mem_struct(f, class, p)
		local old, def = define, setmetatable({}, definemeta)
		define = def
		def.offset, def.size = 0, 0
		local oldunions = unions
		unions = nil
		class = class or {}
		local fields, offs, rofields = {}, {}, {}
		def.members, def.class, def.offsets, def.rofields = fields, class, offs, rofields
		def.newindex = rawset
		local cindex, cnewindex, ccall
		
		local function MakeClass()
			local function newindex(t, a, v)
				local f = class[a]
				if f ~= nil then
					return rawset(t, a, v)
				end
				f = fields[a]
				if f then
					if rofields[a] then
						roError(a, 2)
					else
						return f(offs[a], t, a, v)
					end
				end
				return cnewindex(t, a, v)
			end
			
			local function class_new(class, p)

				local function index(t, a)
					local f
					if a ~= "new" then
						f = class[a]
						if f ~= nil then
							return f
						end
					end
					f = fields[a]
					if f then
						return f(offs[a], t, a)
					end
					if cindex then
						return cindex(t, a)
					end
				end

				local ret = setmetatable(type(p)=="number" and {["?ptr"] = p} or p, 
				         {__index = index, __newindex = newindex, __persist = nullpersist, __call = ccall})
				return struct_callback(ret, class, fields, offs, rofields)
			end

			rawset(class, "new", class_new)
		end

		local obj
		if p then  -- possability to make structure in-place, to access its fields while defining the structure class
			MakeClass()
			obj = class:new()
		end

		local ok = pcall2(f, def, obj)  -- do it

		define = old
		unions  = oldunions

		if ok then
			local size = def.size
			cindex = def.index
			cnewindex = def.newindex or nonewf
			ccall = def.call
			if not p then
				MakeClass()
			else
				setmetatable(p).__call = ccall
			end

			rawset(class, "?size", size)
			return class, obj
		end
	end
	_mem.struct = mem_struct

	local function GetPtr(obj, off)
		local check = rawget(obj, "?CheckOffset")
		if check then
			check(obj, off, 3)
		end
		return obj["?ptr"] + off
	end
	
	local function declare_struct(f, a, pstruct)
		if type(f) == "function" then
			f = mem_struct(f)
			if f == nil then
				return define
			end
		end
		if f == nil then
			error("struct expected, got nil", 2)
		end

		return member(a, pstruct and 4 or f["?size"], function(o, obj, name, val)
			if val ~= nil then
				roError(name)
			end

			if pstruct then
				val = f:new(u4[obj["?ptr"] + o])
			else
				val = f:new(obj["?ptr"] + o)
			end
			rawset(val, "?ptr", nil)
			local m = getmetatable(val)
			if not m then
				m = {}
				setmetatable(val, m)
			end
			local old_index = m.__index
			local old_newindex = m.__newindex
			local old_index_func = type(old_index) == "function"
			local old_newindex_func = type(old_newindex) == "function"

			local new
			if pstruct then
				function new(t, a)
					if a == "?ptr" then
						return u4[GetPtr(obj, o)]
					else
						return CallMetatableIndex(t, a, old_index, old_index_func)
					end
				end
			else
				function new(t, a)
					if a == "?ptr" then
						return GetPtr(obj, o)
					else
						return CallMetatableIndex(t, a, old_index, old_index_func)
					end
				end
			end
			rawset(m, '__index', new)

			if pstruct then
				function new(t, a, v)
					if a == "?ptr" then
						u4[GetPtr(obj, o)] = assertnum(v, 2)
					else
						return CallMetatableNewIndex(t, a, v, old_newindex, old_newindex_func)
					end
				end
			else
				function new(t, a, v)
					if a == "?ptr" then
						error("attempt to set ?ptr of substructure", 2)
					else
						return CallMetatableNewIndex(t, a, v, old_newindex, old_newindex_func)
					end
				end
				new = old_newindex
			end
			rawset(m, '__newindex', new)

			rawset(obj, name, val)
			return val
		end)
	end

	function types.struct(f)
		return function(a)
			return declare_struct(f, a)
		end
	end

	function types.pstruct(f)
		return function(a)
			return declare_struct(f, a, true)
		end
	end

	function types.union(a)
		if unions and unions.amin then
			error("cannot declare a union inside an array", 2)
		end
		if a then
			unions = {last = unions, members = define.members, offsets = define.offsets, rofields = define.rofields}
			local fields, offs, rofields = {}, {}, {}
			member_callback(a, define, fields, offs, rofields)
			define.rofields = rofields
			define.offsets = offs
			define.members[a] = function(_, obj, name, val)
				if val ~= nil then
					roError(name)
				end

				local function newindex(t, a, v)
					if a == "?ptr" then
						error("attempt to set ?ptr of a union")
					end
					local f = fields[a]
					if f then
						if rofields[a] then
							roError(a)
						else
							return f(offs[a], t, a, v)
						end
					else
						rawset(t, a, v)
					end
				end

				local function index(t, a)
					if a == "?ptr" then
						return obj["?ptr"]
					end
					local f = fields[a]
					return f and f(offs[a], t, a)
				end
				val = setmetatable({}, {__index = index, __newindex = newindex, __persist = nullpersist})
				val = union_callback(val, fields, offs, rofields)
				rawset(obj, name, val)
				return val
			end
			define.members = fields
		else
			define.members = unions.members
			define.offsets = unions.offsets
			define.rofields = unions.rofields
			unions = unions.last
		end
		return define
	end

	local function DoArray(ptr, a, b)
		local lenA, lenP
		local i, j
		if type(a) ~= "table" then
			i, j = a, b
			a = {}
		else
			setmetatable(a, nil)
			i, j = a[1], a[2]
			local asize
			for k, v in pairs(a) do
				if k == "lenA" then
					lenA = v
				elseif k == "lenP" then
					lenP = v
				elseif k == "index" or k == "newindex" or k == "lenSet" or k == "AccessBeyondLength" then
					-- keep
				else
					if k == "ItemSize" then
						asize = v
					end
					a[k] = nil
				end
			end
			if lenA and lenP == nil and type(lenA) ~= "function" then
				error("you must specify offset of length field", 3)
			end
			a.asize = asize
		end
		if not i then
			if not ptr and not lenA then
				error("you must specify array length", 3)
			end
			i, j = 0, 0xFFFFFFFF
		elseif j then
			j = j - i + 1
		else
			j, i = i, 0
		end
		a.last = unions
		a.amin = i
		a.acount = j
		a.ptr = ptr
		unions = a
		return define
	end
	
	-- types.array([length func/mem.*, length offset, ] low, high, sz)  (if high = nil then  low is interpreted as Count)
	function types.array(...)
		return (DoArray(nil, ...))
	end
	local array = types.array

	function types.parray(...)
		return (DoArray(true, ...))
	end

	local sOutOfBounds = "array index (%s) out of bounds [%s, %s]"	
	
	function declare_array(size, f)
		size = unions.asize or size
		local low, count, ptr, lenA, lenP, SetLen = unions.amin, unions.acount, unions.ptr, unions.lenA, unions.lenP, unions.lenSet
		local _index, _newindex, beyondLen = unions.index, unions.newindex, unions.AccessBeyondLength
		local lenFunc = type(lenA) == "function"
		unions = unions.last
		local mySize = ptr and 4 or size*count
		
		if type(_index) == "table" then
			local old = _index
			_index = function(t, a)  return old[a]  end
		end
		if type(_newindex) == "table" then
			local old = _newindex
			_newindex = function(t, a, v)  old[a] = v  end
		end
		
		local function GetLen(t)
			if lenA == nil then
				return count
			elseif lenFunc then
				return lenA(t, lenP and t["?ptr"] + lenP)
			else
				return lenA[t["?ptr"] + lenP]
			end
		end

		if lenA == nil then
			SetLen = nil
		elseif SetLen == nil and not lenFunc then
			function SetLen(_, v, p, lenA)
				if v < 0 or v > count then
					error(format("attempt to set array length to %s, maximum length is %s", v, count), 3)
				end
				lenA[p] = v
			end
		end

		local function myF(o, obj, name, val)
			if val then
				roError(name)
			end
			
			local function CheckOffset(t, o, lev)
				local n = beyondLen and count or (GetLen(obj) + (beyondLen or 0))
				if o < 0 or o >= n*size then
					error(format(sOutOfBounds, o/size + low, low, low + n - 1), (lev or 1) + 1)
				end
			end
			
			local function indexes(t, a, v)
				if a == "?ptr" then
					if ptr then
						if v == nil then
							return u4[GetPtr(obj, o)]
						end
						u4[GetPtr(obj, o)] = assertnum(v, 2)
						return
					else
						if v == nil then
							return GetPtr(obj, o)
						end
						error("attempt to set ?ptr of embedded array", 2)
					end
				elseif type(a) == "string" then
					if v == nil or SetLen then
						if a == "high" or a == "High" then
							if v == nil then
								return low + GetLen(obj) - 1
							else
								return (SetLen(obj, v - low + 1, lenP and obj["?ptr"] + lenP, lenA))
							end
						elseif a == "length" or a == "Length" or a == "count" or a == "Count" then
							if v == nil then
								return GetLen(obj)
							else
								return (SetLen(obj, v, lenP and obj["?ptr"] + lenP, lenA))
							end
						elseif v == nil then
							if a == "low" or a == "Low" then
								return low
							elseif a == "limit" or a == "Limit" then
								return count
							elseif a == "size" or a == "Size" then
								return GetLen(obj)*size
							elseif a == "?size" then
								return count*size
							elseif a == "ItemSize" then
								return size
							end
						end
					end
					local f = (v == nil) and (_index or 1) or _newindex or 1
					if f ~= 1 then
						return f(t, a, v)
					end
				end
				local aorig = a
				a = tonumber(a)
				if a then
					local a1 = a - low
					local n = beyondLen and count or (GetLen(obj) + (beyondLen or 0))
					if a1 >= 0 and a1 < n then
						return f(size*a1, t, a, v)
					end
					error(format(sOutOfBounds, tostring(a), low, low + n - 1), 2)
				else
					error(format("attempt to %s field %s of array", v == nil and "get" or "set", tostring2(aorig)), 2)
				end
			end
			
			local function _call(t, _, i)
				if i == nil then
					i = low
				else
					i = i + 1
				end
				local a1 = i - low
				if a1 >= 0 and a1 < GetLen(obj) then
					return i, f(size*a1, t, i)
				end
			end
			
			local meta = {__index = indexes, __newindex = indexes, __call = _call, __persist = nullpersist}
			val = setmetatable({["?CheckOffset"] = CheckOffset}, meta)
			val = array_callback(val)
			rawset(obj, name, val)
			return val
		end
		if unions and unions.amin then
			return declare_array(mySize, myF)
		end
		return mySize, myF
	end
	
	function types.indexmember(name)
		local old = define.index
		if old then
			define.index = function(t, a)
				local ret = t[name][a]
				if ret == nil then
					return old(t, a)
				end
			end
		else
			define.index = function(t, a)
				return t[name][a]
			end
		end
		if define.call == nil then
			function define.call(t, ...)
				local v = t[name]
				if v then
					return v(...)
				end
			end
		end
		return define
	end

	function types.newindexmember(name, ifexists)
		if ifexists then
			local old = define.newindex or nonewf
			define.newindex = function(t, a, v)
				local fld = t[name]
				if fld[a] == nil then
					return old(t, a, v)
				end
				fld[a] = v
			end
		else
			define.newindex = function(t, a, v)
				t[name][a] = v
			end
		end
		return define
	end

	function types.method(def)
		table_insert(def, 1, 0)
		def.must = assertnum(def.must or 0, 2) + 1
		def.cc = assertnum(def.cc or 1, 2)
		define.class[def.name] = mem_function(def)
		return define
	end

	function types.func(def)
		def.must = assertnum(def.must or 0, 2)
		def.cc = assertnum(def.cc or 0, 2)
		define.class[def.name] = mem_function(def)
		return define
	end
	
	function types.vmethod(def)
		table_insert(def, 1, 0)
		def.must = assertnum(def.must or 0, 2) + 1
		def.cc = assertnum(def.cc or 1, 2)
		local vmt = def.vmt or define.vmt
		if vmt ~= define.vmt then
			define.vmt = vmt
			define.voffset = 0
		end
		local off = def.offset or def.index and def.index*4 or define.voffset
		define.voffset = off + 4
		function def.p(cc, t, ...)
			return call(u4[u4[t["?ptr"] + vmt] + off], cc, t, ...)
		end
		define.class[def.name] = mem_function(def)
		return define
	end
	
end -- mem.struct

-----------------------------------------------------
-- mem.hook
-----------------------------------------------------

local OpCALL, OpJMP = 0xE8, 0xE9
local Mem_HookProc = internal.Mem_HookProc

local mem_hooks = {}
_mem.hooks = mem_hooks

if internal.GetHookSize then

	function _mem.GetHookSize(p)
		if rawcall(IsBadCodePtr, 0, p) ~= 0 then
			code_error(p, 2)
		end
		return GetHookSize(p)
	end
	
end

local GetHookSize = function(p)
	if rawcall(IsBadCodePtr, 0, p) ~= 0 then
		code_error(p, 3)
	end
	return GetHookSize(p)
end

if GetInstructionSize then

	function _mem.GetInstructionSize(p)
		if rawcall(IsBadCodePtr, 0, p) ~= 0 then
			code_error(p, 2)
		end
		return GetInstructionSize(p)
	end
	
end

local GetInstructionSize = GetInstructionSize and function(p)
	if rawcall(IsBadCodePtr, 0, p) ~= 0 then
		code_error(p, 3)
	end
	return GetInstructionSize(p)
end

function _mem.hook(p, f, size)
	assert(size == nil or size >= 5)
	size = size or GetHookSize(p)
	if mem_hooks[p] then
		error(format("hook at address %X is already set", p), 2)
	end
	for i = p - 4, p + size - 1 do
		if mem_hooks[i] then
			error(format("attempt to set hook at address %X, that intercepts with existing hook at %X", p, i), 2)
		end
	end
	mem_hooks[p] = f
	IgnoreCount = IgnoreCount + 1
	u1[p] = OpCALL
	local std = i4[p + 1] + p + 5
	i4[p + 1] = Mem_HookProc - p - 5
	for i = p + 5, p + size - 1 do
		u1[i] = 0x90
	end
	IgnoreCount = IgnoreCount - 1
	return std
end
local mem_hook = _mem.hook

function _mem.hookjmp(p, f, size)
	return mem_hook(p, function(data)
		data.esp = data.esp + 4
		return f(data)
	end, size)
end
local mem_hookjmp = _mem.hookjmp

local HookData = _mem.struct(function(define)
	define
	-- 4 byte registers
	[0x0].i4  'EFLAGS'
	
	[0x4].i4  'EDI'
	[0x8].i4  'ESI'
	
	[0xC].i4  'EBP'
	[0x10].i4  'ESP'
	
	[0x14].i4  'EBX'
	[0x18].i4  'EDX'
	[0x1C].i4  'ECX'
	[0x20].i4  'EAX'

	-- 2 byte registers
	[0x0].u2  'FLAGS'
	
	[0x4].u2  'DI'
	[0x8].u2  'SI'
	
	[0x14].u2  'BX'
	[0x18].u2  'DX'
	[0x1C].u2  'CX'
	[0x20].u2  'AX'

	-- 1 byte registers
	[0x14].u1  'BL'
	[0x18].u1  'DL'
	[0x1C].u1  'CL'
	[0x20].u1  'AL'

	[0x15].u1  'BH'
	[0x19].u1  'DH'
	[0x1D].u1  'CH'
	[0x21].u1  'AH'
	
	-- flags
	[0x0].bit('CF', 0x1)
	[0x0].bit('PF', 0x4)
	[0x0].bit('AF', 0x10)
	[0x0].bit('ZF', 0x40)
	[0x0].bit('SF', 0x80)
	[0x0].bit('TF', 0x100)
	[0x0].bit('IF', 0x200)
	[0x0].bit('DF', 0x400)
	[0x0].bit('OF', 0x800)
	[0x0].bit('NT', 0x4000)
	[0x0].bit('RF', 0x10000)
	[0x0].bit('VM', 0x20000)
	[0x0].bit('AC', 0x40000)
	[0x0].bit('VIF', 0x80000)
	[0x0].bit('VIP', 0x100000)
	[0x0].bit('ID', 0x200000)
	
	for k,v in pairs(table_copy(define.members)) do
		define.members[string_lower(k)] = v
	end
	for k,v in pairs(table_copy(define.offsets)) do
		define.offsets[string_lower(k)] = v
	end
	
	local c = define.class
	
	function c:push(val)
		val = assertnum(val, 2)
		local p = self.esp - 4
		self.esp = p
		if val < 0 then
			i4[p] = val
		else
			u4[p] = val
		end
	end
	
	function c:pop()
		local p = self.esp
		self.esp = p + 4
		return i4[p]
	end
	
	function c:popu()
		local p = self.esp
		self.esp = p + 4
		return u4[p]
	end
	
	function c:ret(n)
		if n then
			local p = u4[self.esp]
			self.esp = self.esp + assertnum(n, 2)
			u4[self.esp] = p
		end
	end
	
	function c.getparams(d, nreg, nstack, startstack)
		assert(nreg <= 2)
		local par = {}
		if nreg >= 1 then
			par[1] = d.ecx
			if nreg >= 2 then
				par[2] = d.edx
			end
		end
		local n = #par + 1
		startstack = startstack or d.esp + 4
		for i = 0, nstack - 1 do
			par[n + i] = i4[startstack + i*4]
		end
		return unpack(par)
	end
	c.GetParams = c.getparams
	
	c.offsets = define.offsets
end)

internal.RegisterHookCallback(function(d)
	local data = HookData:new(d)
	pcall2(mem_hooks[u4[data.esp] - 5], data)
end)



local HookMem, HookMemEnd
local VirtualAlloc = internal.VirtualAlloc
_mem.VirtualAllocPtr = internal.VirtualAlloc
local PageSize = internal.PageSize

local FreeHook = {}
local NextFreeHook = {}
local HookSizes = {}

local block, blocksize = nil, 0

function _mem.hookalloc(size)
	local f = (type(size) == "function" and size)
	size = (not f and size or 5)
	local p = FreeHook[size]
	if p then
		FreeHook[size] = NextFreeHook[p]
		mem_fill(p, size, 0x90)
		return p
	end
	blocksize = blocksize - size
	if blocksize < 0 then
		blocksize = (size + PageSize - 1):AndNot(PageSize - 1)
		block, blocksize = ucall(VirtualAlloc, 0, 0, blocksize, 0x1000, 0x40), blocksize - size
	end
	p = block
	mem_fill(p, size, 0x90)
	if size ~= 5 then
		HookSizes[p] = size
	end
	block = block + size
	if f then
		mem_hookjmp(p, f, 5)
	end
	return p
end
local mem_hookalloc = _mem.hookalloc

function _mem.hookfree(p)
	local size = HookSizes[p] or 5
	NextFreeHook[p] = FreeHook[size]
	FreeHook[size] = p
end
local mem_hookfree = _mem.hookfree


-- absolete

-- local function hookarounddestructor(info)
	-- local p = info.buffer
	-- mem_hooks[p] = nil
	-- mem_hookfree(p)
-- end

-- function _mem.hookaround(p, nreg, nstack, f1, f2, size, keepParams)
	-- assert(nreg <= 2)
	-- local after = mem_hookalloc()
	-- local info = {f1 = f1, f2 = f2, buffer = after}
	-- info.destructor = table_destructor(info, hookarounddestructor)
	-- setmetatable(info, info)
	
	-- function info.__call(_, d)  -- before
		-- local par = {}
		-- local p = d.esp + 4
		-- d.esp = p
		-- if nreg >= 2 then
			-- d:push(d.edx)
		-- end
		-- if nreg >= 1 then
			-- d:push(d.ecx)
		-- end
		-- for i = 1, nreg do
			-- par[i] = i4[d.esp + (i-1)*4]
		-- end
		-- d.esp = d.esp - nstack*4
		-- mem_copy(d.esp, p + 4, nstack*4)
		-- d:push(after)
		-- local n, esp = #par, d.esp
		-- for i = 1, nstack do
			-- par[n + i] = i4[esp + i*4]
		-- end
		-- return info.f1(d, unpack(par))
	-- end
	
	-- local function h2(d)  -- after
		-- d:pop()  -- return address
		-- local par = {}
		-- for i = 1, nreg do
			-- par[i] = d:pop()
		-- end
		-- local n, esp = #par, d.esp
		-- for i = 1, nstack do
			-- par[n + i] = i4[esp + i*4]
		-- end
		-- if not keepParams then
			-- d:ret(nstack*4)
		-- end
		-- return info.f2(d, unpack(par))
	-- end
	
	-- mem_hook(after, h2, size)
	-- mem_hook(p, info)
-- end

local function GetNoJumpSize(p)
	local byte1 = u1[p]
	if byte1 == OpCALL or byte1 == OpJMP then  -- allow standard jump / call
		return 5
	end
	local n, j = GetHookSize(p)
	assert(not j, "call or jump in original code")
	return n
end

-- copies standard code into a memory block that then jumps to the regular function
-- (the copied code must not contain jumps or calls)
function _mem.copycode(ptr, size, MemPtr, NoJumpBack)
	size = size or GetHookSize(ptr)
	local FullSize = size + (NoJumpBack and 0 or 5)
	for i = ptr - 4, ptr + size - 1 do
		if mem_hooks[i] then
			error(format("attempt to copy code containing a hook at address %X", i), 2)
		end
	end
	--assert(u1[ptr] ~= OpCALL and u1[ptr] ~= OpJMP, "call or jump in original code")  -- a little check for already existing hook
	local std = MemPtr or mem_hookalloc(FullSize)
	mem_copy(std, ptr, size)
	if size >= 5 then
		local byte1 = u1[std]
		if byte1 == OpCALL or byte1 == OpJMP then  -- fix standard jump / call at the beginning
			local addr = i4[std + 1] + 5
			if addr < 0 or addr >= size then
				i4[std + 1] = i4[std + 1] + ptr - std
			end
		end
	end
	if not NoJumpBack then
		u1[std + size] = OpJMP
		i4[std + size + 1] = ptr - std - 5
	end
	return std
end
local copycode = _mem.copycode

local function MyCopyCode(p, size, MemPtr, NoJumpBack)
	if size < 5 then
		local size1 = GetHookSize(p)
		for i = p + size, p + size1 - 1 do
			if mem_hooks[i] then
				error(format("attempt to set hook at address %X, which intercepts with existing hook at %X", p, i), 3)
			end
		end
		MemPtr = copycode(p, size, MemPtr, true)
		if not NoJumpBack then
			copycode(p + size1, 0, MemPtr)
		end
		return MemPtr, size1
	else
		return copycode(p, size, MemPtr), size
	end
end

-- hookjmp with automatic calling of overwritten code (see mem.copycode note)
-- if the function returns 'true', the jump to original code isn't performed
function _mem.autohook(p, f, size)
	size = size or GetNoJumpSize(p)
	local code, size1
	local byte1 = (size == 5 and u1[p])
	-- CALL and JMP can be hooked without copying
	if byte1 == OpCALL then
		code = p + 5 + i4[p + 1]
		mem_hook(p, function(d)
			d.esp = d.esp + 4
			if not f(d, code) then
				d:push(p + 5)
				d:push(code)
			end
		end, size)
		return
	elseif byte1 == OpJMP then
		code = p + 5 + i4[p + 1]
		size1 = size
	else
		code, size1 = MyCopyCode(p, size)
	end
	mem_hook(p, function(d)
		d.esp = d.esp + 4
		if not f(d, code) then
			d:push(code)
		end
	end, size1)
	if byte1 ~= OpJMP then
		return code
	end
end

local function PlaceJMP(p, code, size)
	IgnoreCount = IgnoreCount + 1
	u1[p] = OpJMP
	i4[p + 1] = code - p - 5
	for i = p + 5, p + size - 1 do
		u1[i] = 0x90
	end
	IgnoreCount = IgnoreCount - 1
	-- In .text:
	--   call Mem_HookProc
	-- In block:
	--   std_code
	--   jmp .text
end

-- hookjmp with automatic calling of overwritten code (see mem.copycode note)
-- the function is called after the overwritten code
-- if the function returns 'true', the jump to original code isn't performed
function _mem.autohook2(p, f, size)
	size = size or GetNoJumpSize(p)
	local code, size1 = MyCopyCode(p, size)
	local retaddr = p + size1
	mem_hook(code + size, function(d)
		d.esp = d.esp + 4
		if not f(d, retaddr) then
			d:push(retaddr)
		end
	end, 5)
	PlaceJMP(p, code, size1)
	-- In .text:
	--   jmp block
	-- In block:
	--   std_code
	--   call Mem_HookProc
	return code
end

-- like autohook, but takes a compiled Asm code string as parameter
-- 'code' can be a function f(ptr) where ptr is the address of memory allocated for hook code or nil (to calculate size)
function _mem.bytecodehook(p, code, size)
	local codef = (type(code) == "function" and code)
	code = (codef and codef() or code)
	size = size or GetNoJumpSize(p)
	local new = mem_hookalloc(#code + size + 5)
	mem_copy(new, codef and codef(new) or code, #code)
	local _, size1 = MyCopyCode(p, size, new + #code)
	PlaceJMP(p, new, size1)
	-- In .text:
	--   jmp block
	-- In block:
	--   code
	--   std_code
	--   jmp .text
	return new
end
local mem_bytecodehook = _mem.bytecodehook

-- like autohook2, but takes a compiled Asm code string as parameter
-- see note of bytecodehook about 'code' 
function _mem.bytecodehook2(p, code, size)
	local codef = (type(code) == "function" and code)
	code = (codef and codef() or code)
	size = size or GetNoJumpSize(p)
	local new = mem_hookalloc(size + #code + 5)
	local _, size1 = MyCopyCode(p, size, new, true)
	mem_copy(new + size, codef and codef(new + size) or code, #code)
	copycode(p + size1, 0, new + size + #code)  -- put jmp
	PlaceJMP(p, new, size1)
	-- In .text:
	--   jmp block
	-- In block:
	--   std_code
	--   code
	--   jmp .text
	return new
end
local mem_bytecodehook2 = _mem.bytecodehook2

-- replaces original instructions with new ones, jumping out if needed
-- see note of bytecodehook about 'code' 
function _mem.bytecodepatch(p, code, size)
	local codef = (type(code) == "function" and code)
	code = (codef and codef() or code)
	local size1 = size or GetHookSize(p)
	if #code <= size1 then  -- patch in place
		if not size and GetInstructionSize then
			size = 0
			repeat
				size = size + GetInstructionSize(p + size)
			until #code <= size
		elseif not size then
			size = size1
		end
		for i = p - 4, p + size - 1 do
			if mem_hooks[i] then
				error(format("attempt to patch address %X, which intercepts with existing hook at %X", p, i), 2)
			end
		end
		IgnoreCount = IgnoreCount + 1
		mem_copy(p, codef and codef(p) or code, #code)
		for i = p + #code, p + size - 1 do
			u1[i] = 0x90
		end
		IgnoreCount = IgnoreCount - 1
		return
	end
	local new = mem_hookalloc(#code + 5)
	mem_copy(new, codef and codef(new) or code, #code)
	copycode(p + size1, 0, new + #code)  -- put jmp
	PlaceJMP(p, new, size1)
	return new
end
local mem_bytecodepatch = _mem.bytecodepatch

-- asm
if internal.CompileAsm then
	_mem.asm = internal.CompileAsm(1)
	local toasm = internal.CompileAsm(2, 3)
	local toasm2 = internal.CompileAsm(2, 2)
	
	-- like autohook, but takes an Asm code string as parameter
	function _mem.asmhook(p, code, size)
		return mem_bytecodehook(p, toasm(code), size)
	end

	-- like autohook2, but takes an Asm code string as parameter
	function _mem.asmhook2(p, code, size)
		return mem_bytecodehook2(p, toasm(code), size)
	end
	
	-- replaces original instructions with new ones, jumping out if needed
	function _mem.asmpatch(p, code, size)
		return mem_bytecodepatch(p, toasm(code), size)
	end
	
	-- creates an Asm function
	function _mem.asmproc(code)
		code = toasm2(code)
		local codef = (type(code) == "function" and code)
		code = (codef and codef() or code)
		local p = mem_hookalloc(#code)
		mem_copy(p, codef and codef(p) or code, #code)
		return p, #code
	end
end

-- set hook at the beginning of a function, allows calling the original function (see mem.copycode note)
function _mem.hookfunction(p, nreg, nstack, f, size)
	size = size or GetNoJumpSize(p)
	assert(size >= 5)
	local code = copycode(p, size)
	local function def(...)
		return call(code, nreg, ...)
	end
	mem_hook(p, function(d)
		d.esp = d.esp + 4
		d.eax = f(d, def, d:getparams(nreg, nstack))
		d:ret(nstack*4)
	end, size)
end

-- replaces an existing CALL instruction and uses the same protocol as mem.hookfunction
function _mem.hookcall(p, nreg, nstack, f)
	assert(u1[p] == OpCALL)
	local code = i4[p + 1] + p + 5
	local function def(...)
		return call(code, nreg, ...)
	end
	mem_hook(p, function(d)
		d.eax = f(d, def, d:getparams(nreg, nstack))
		d:ret(nstack*4)
	end, 5)
end

-- writes 'n' NOPs
function _mem.nop(p, n)
	IgnoreCount = IgnoreCount + 1
	for i = p, p + n - 1 do
		u1[i] = 0x90
	end
	IgnoreCount = IgnoreCount - 1
end
