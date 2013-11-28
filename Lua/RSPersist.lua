--[[----------------------------------------------------------------------------

RSPersist v0.3
Copyright(c) 2009 - 2011 Sergey Rozhenko aka GrayFace (sergroj@mail.ru)
http://sites.google.com/site/sergroj/
Distributed AS IS under the same license as Lua 5.1.

Pure Lua persistence library. Stores Lua data in a compact binary string. Exceeds functionality of Pluto library
when used with provided C files. Alternatively, persistence can be limited to only safe 

- Only pure Lua is required, with standard libraries. Lua 5.1 and 5.2 are supported, not sure about 5.0.
- May work without debug library, but won't persist protected metatables and functions.
- Precisely stores any number. Correctly handles infinities and NAN.
  (however, stores -0 as 0 and doesn't distinguish between kinds of NAN)
- Supports unusual lua_Number types, like (long double) and (long long).
- Can persist Lua functions without upvalues or with independent upvalues.
- Can persist any Lua function if debug.upvalueid and debug.upvaluejoin are present.
  (they are present in 5.2 and can be added to 5.1 with a patch)
- Powerful and flexible custom persistence. By default it acts like pluto, with addition of custom initializers.
- Threads and C closures are persistable via RSPersistMore.c that uses custom persistence mechanism.
- Userdata is only persisted by user.

!!! loadstring is removed/depricated in 5.2
!!! persisters?

Persist:
Stores the given object in a string.
Usage: string = persist(object, [inverted permanents table])
"inverted permanents table" is a table with permanents as keys and their mappings as values

Unpersist:
Loads stored object from string.
object = unpersist(string, [permanents table], [offset])
"permanents table" is a table with mapped values as keys and permanents as values
offset defines the offset from the beginning of string at which the data is stored (1 by default)


Example:

PermanentsLoad = {_G, pairs, ipairs, ["string.format"] = string.format}
for k, v in pairs(PermanentsLoad) do
	PermanentsSave[v] = k
end

local data = persist({example = "table"}, PermanentsSave)

data = unpersist(data, PermanentsLoad)
assert(data.example == "table")


__persist of metatable can be one of the following:
nil or true = persist by default
false = don't persist, show error
function or callable object = use custom persistence

Let PermanentsLoad, PermanentsSave be already filled with needed values.



Advices:

- If you want to persist several objects, create a table for them and persist it in a single call.
- Add all standard functions to permanents table.
- Even if you think a function can only be accessed internally, it won't hurt to add it to permanents table.
- Be careful not to change order of numerical indexes in permanents table when you update it.
- When you need to create keys consisting of two or more strings, an easy way to do this is by concatenating
  format("%q") representations of them. This will lead to 1-to-1 correspondence between the key and the strings.

	
	
	
Custom persistence:

The  Custom persistence function must not reference the object directly or indirectly.

1) Wrong code:

local function mypersist()
	return function()
		return nil
	end
end

SomeTable = setmetatable({}, {__persist = mypersist})
return persist(SomeTable)

==> "custom persistence loop"

2) Proper way to persist as nil:

local function mypersist()
end

3) In other cases, you can set function environment to a proxy table and add it to permanents tables:

local _G = getfenv(0)
local persistenv = setmetatable({}, {__index = _G, __newindex = _G})
local PermLoad, PermSave = {persistenv}, {persistenv = 1}

setfenv(mypersist, persistenv)

4) In most cases you should use initialization function:

local _G = getfenv(0)
local persistenv = setmetatable({}, {__index = _G, __newindex = _G})
local PermLoad, PermSave = {persistenv}, {persistenv = 1}

-- persistence function for userdata created with newproxy
local function persistproxy(obj)
	local env = debug.getfenv(obj)
	local meta = debug.getmetatable(obj)
	local function create()
		return newproxy(false)
	end
	local function init(obj)
		debug.setfenv(obj, env)
		debug.setmetatable(obj, meta)
	end
	return create, init
end
setfenv(persistproxy, persistenv)

-- just an example proxy
local proxy = newproxy(true)
debug.getmetatable(proxy).__persist = persistproxy

return persist(proxy, PermSave)

The first function will be called to create the object as soon as all objects referenced by it are fully 
unpersisted. The second function would be called to initialize the object as soon as all objects referenced by it
are created.
Objects referenced by your initialization function aren't guaranteed to be fully initialized in case of reference
loops. Say, you have two objects that reference each other. Which one's initializer would run first depends on
which is referenced first by other objects. However, objects created by your persistence function or otherwise
only referenced by your initialization function would be initialized. For example, if your persistence function
creates a table, the initialization function can freely access the table as it will be initialized, it doesn't
matter .


 E.g. if you creSo, feel free to create a table to keep some
When you have 2 functions the object will be created as soon as all objects referenced by the first one are
created. 



--]]----------------------------------------------------------------------------

local concat = table.concat
local error = error
local assert = assert
local type = type
local getmetatable = getmetatable
local setmetatable = setmetatable
local next = next
local rawget = rawget
local rawequal = rawequal
local tostring = tostring
local ldexp, frexp = math.ldexp, math.frexp
local char, byte = string.char, string.byte
local string_sub = string.sub
local format = string.format
local f_dump, loadstring = string.dump, loadstring
local setfenv = setfenv
local getfenv = getfenv
local setupvalue = debug and debug.setupvalue
local upvaluejoin = debug and debug.upvaluejoin
local getupvalue = debug and debug.getupvalue
local upvalueid = debug and debug.upvalueid
local HUGE = math.huge
local NAN = HUGE - HUGE
do
	local ver51 = {["Lua 5.1"] = true, ["Lua 5.1."] = true, ["Lua 5.0"] = true, ["Lua 5.0."] = true}
	if not ver51[string.sub(_VERSION, 1, 8)] then
		getfenv, setfenv = nil, nil  -- no function environments in Lua 5.2
	end
end
local nofunc = RSPersist_NoFunctions
local nometa = RSPersist_NoMetatables
local asnil = RSPersist_Nil or {}

-- prepare types

local Types = {}
local TypeNames = {}
local function MakeType(name, size)
	size = size or 256 - #TypeNames
	Types[name] = #TypeNames
	--print(format("%x:", #TypeNames), name)
	for i = #TypeNames + 1, #TypeNames + size do
		TypeNames[i] = name
	end
end
local SimpleTypes = {0, false, true, "", HUGE, -HUGE, NAN}
MakeType("simple", #SimpleTypes)
MakeType("nil", 1)
MakeType("custom", 1)
MakeType("number", 56)
MakeType("table", 8)
MakeType("function", 4)
MakeType("init", 2)
MakeType("string")

--[[  Types in hex:
0:	0
1:	false
2:	true
3:	""
4:	HUGE
5:	-HUGE
6:	NAN
7:	nil
8:	custom
9:	number
41:	table
49:	function
4d:	init
4f:	string
--]]

-- error functions
local function errorf(text)
	return function()
		error(text)
	end
end

setupvalue = setupvalue or errorf("cannot set upvalues without debug.setupvalue")

do
	local _getmetatable = getmetatable
	getmetatable = debug and debug.getmetatable or function(t)
		t = getmetatable(t)
		return type(t) == "table" and t or nil
	end
end


------------ ErrorFunction ------------

local ErrorInfo  -- function that provides extended error information

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

-- Error function that reports object being persisted, traceback, locals and upvalues
local ErrorFunction
if debug then
	local getinfo = debug.getinfo
	local getlocal = debug.getlocal
	local traceback = debug.traceback

	function ErrorFunction(s)
		local lev = 2
		local d = getinfo(3, "f")
		if d ~= nil then
			d = d.func
			if d == error or d == assert then
				lev = lev + 1
			end
		end
		
		local text = {traceback(s..": "..ErrorInfo().."\n", lev)}
		
		local d, func, funcname
		local i = lev
		repeat
			d = getinfo(i, "fnSL")
			i = i + 1
		until d == nil or d.func
		if d then
			func = d.func
			funcname = d.name
			if funcname == "" then
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
				text[#text + 1] = funcname and format("\n\nlocal Variables of '%s':", funcname) or '\n\nlocal variables:'
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
	
end

local chars

------------ Persist ------------

local function dopersist(object, custom)
	-- Default persistence function works mostly like pluto
	if custom == nil or type(custom) == "table" then
		local simple = {number = true, string = true, boolean = true}
		local perm = custom or {}
		function custom(t)
			local v = perm[t]
			if v == asnil then
				return nil
			elseif v ~= nil then
				return v
			elseif not simple[type(t)] then
				local m = getmetatable(t)
				if type(m) == "table" then
					m = m.__persist
					if m == false then
						error("persistence is denied for this object")
					elseif m ~= nil and m ~= true then
						local v, v1 = m(t)
						-- don't allow simple types, because returning a name of permanent can breach sandboxing
						if v ~= nil and not rawequal(v, t) and simple[type(v)] then
							error("custom unpersisteance function expected, got "..type(v))
						end
						return v, v1
					end
				end
			end
			return t
		end
	end
	
	local getupvalue = getupvalue or errorf("cannot persist a function without debug.getupvalue")

	-- Prepare number -> character table to improve performance
	if not chars then
		chars = {}
		for i = 0, 255 do
			chars[i + 1] = char(i)
		end
	end
	
	local objlist, objid, count = {}, {}, 0  -- mapping from ID to obj, mapping from obj to id
	-- the list is formed in hierarchical manner, the parent obj is placed after children,
	-- one obj may be placed twice (first creation, then initialization)
	local NANt, BaseMarker, EndMarker, SkipMarker = objlist, objid, {}, {}  -- {"NAN"}, {"BASE"}, {"END"}, {"SKIP"}
	local head, oldhead  -- the point where processing goes. oldhead is only for StoreObject and ErrorInfo
	local tostore, tostoreN = {}, 0  -- members of complex objects ready to be persisted
	-- 'tostore' is the interesting part. Format of 1 object in 'tostore' array:
	-- values for creation..., BASE, type, self, values for initialization..., END, type, self, next index
	-- type for creation differs from type for initialization. There are just 2 types for initialization (second with user initialization).
	local stored = {}  -- maps object to index in 'tostore'

	local objtype  -- type of object being persisted
	local pobjtype  -- index of position to store object type
	local curobj

	-- Type codes
	local SimpleTypesWrite = {
		[0] = 0,
		[false] = 1,
		[true] = 2,
		[""] = 3,
		[HUGE] = 4,
		[-HUGE] = 5,
		[NANt] = 6,
	}

	-- store a value
	local function value(v, nonil)
		if v ~= v then
			v = NANt
		end
		if v ~= nil or not nonil then
			tostoreN = tostoreN + 1
			tostore[tostoreN] = v
		end
		return v ~= nil
	end
	
	-- store marker that the object can be created at this point
	local function base()
		value(BaseMarker)
		tostoreN = tostoreN + 1
		pobjtype = tostoreN
		value(curobj)
	end
	
	-- Store objects into intermediate array
	local storers = {
		custom = function(t)
			value(t)
			base()
		end,
	
		table = function(t)
			base()
			-- array part
			local i = 1
			if value(rawget(t, i), true) then
				objtype = objtype + 1
				repeat
					i = i + 1
				until not value(rawget(t, i))
			end
			i = i - 1
			-- <key, value> part
			local pair
			for k, v in next, t do  
				if type(k) ~= "number" or k < 1 or k > i or k % 1 ~= 0 then
					pair = true
					value(k)
					value(v)
				end
			end
			if pair then
				objtype = objtype + 2
				value(nil)
			end
			-- metatable
			if nometa then
				if nometa ~= "ignore" and getmetatable and getmetatable(t) ~= nil then
					error("persistence of metatables is denied")
				end
			elseif value(getmetatable and getmetatable(t), true) then
				objtype = objtype + 4
			end
		end,

		["function"] = not nofunc and function(t)
			-- body
			value(f_dump(t))
			base()
			-- upvalues (only values)
			local i = 1
			local a, v = getupvalue(t, i)
			while a ~= nil do
				value(v)
				i = i + 1
				a, v = getupvalue(t, i)
			end
			-- environment
			if value(getfenv and getfenv(t), true) then
				objtype = objtype + 2
			end
		end or errorf("persistence of functions is denied"),
		
		thread = errorf("cannot persist a thread"),
		userdata = errorf("cannot persist a userdata"),
	}
	
	-- Function that shows meaningfull name for the object that caused error
	local persisting
	function ErrorInfo()
		local base
		local function FindNextObj()
			while tostore[head] ~= EndMarker do
				base = base or tostore[head] == BaseMarker and head
				head = head + 1
			end
			head = tostore[head + 3]
		end
		
		local before, after = "", ""
	
		local obj, start
		
		local finders = {
			custom = function(tp)
				before, after = before.."CustomUnpersister(", ")"..after
			end,
			
			table = function(tp)
				local pos = start + 1
				-- array part
				if tp % 2 >= 1 then
					while pos ~= obj and tostore[pos] ~= nil do
						pos = pos + 1
					end
					if pos ~= obj then
						pos = pos + 1
					else
						after = format("[%d]%s", pos - start, after)
						return
					end
				end
				-- <key, value> part
				if tp % 4 >= 2 then
					start = pos
					while pos + 1 < obj and tostore[pos] ~= nil do
						pos = pos + 2
					end
					if pos + 1 < obj then
						pos = pos + 1
					elseif pos + 1 == obj then
						local k = tostore[pos]
						after = format("[%s]%s", tostring2(k), after)
						return
					else
						local k
						if pos ~= start then
							k = tostore[pos - 2]
						end
						before = before.."next("
						after = format(", %s)%s", tostring2(k), after)
						return
					end
				end
				-- metatable
				assert(tp % 8 >= 4 and pos == obj)
				before, after = before.."getmetatable(", ")"..after
			end,
			
			["function"] = function(tp)
				-- function string
				if obj < base then
					before, after = before.."string.dump(", ")"..after
					return
				end
				-- upvalues
				before = before.."debug.getupvalue("
				after = format(", %d)%s", obj - start, after)
			end,
		}
	
		if not head then
			return "object"
		end
		if persisting then
			FindNextObj()
		elseif stored[curobj] then
			head = oldhead
		end
		-- now head points to the place directly after the value pointing to object
		local Types_CustomInit = Types.init + 1
		while head do
			obj = head - 1
			base = nil
			FindNextObj()
			if tostore[obj + 1] == EndMarker and tostore[obj + 2] == Types_CustomInit then
				before, after = before.."CustomInitializer(", ")"..after
			else
				if not base then
					base = obj - 1
					while tostore[base] ~= BaseMarker do
						base = base - 1
					end
					start = base + 2
				else
					start = stored[tostore[base + 2]]
				end
				local tp = tostore[base + 1]
				local tname = TypeNames[tp + 1]
				local f = finders[tname]
				if f then
					f(tp - Types[tname])
				end
			end
		end
		return before.."object"..after
	end
	
	local Types_init = Types.init
	
	-- Store all values of an object
	local function StoreObject(t)
		if t == nil or stored[t] then
			return
		end
		curobj = t
		local v, v1 = custom(t == NANt and NAN or t)
		if nofunc == "ignore" and type(v) == "function" or
				nometa == "ignore whole" and type(v) == "table" and getmetatable(v) then
			v, v1 = nil, nil
		elseif v == nil and v1 ~= nil then
			error("nil cannot have an initializer")
		elseif v ~= v then
			v = NANt
		end
		oldhead = head
		head = tostoreN + 1
		stored[t] = tostoreN
		local tp
		if not rawequal(v, t) then  -- custom persistence
			tp = "custom"
		else
			tp = SimpleTypesWrite[v] or type(v)
		end
		objtype = Types[tp] or tp;
		(storers[tp] or base)(v)
		tostore[pobjtype] = objtype
		tp = Types_init
		if v1 ~= nil then
			value(v1)
			tp = tp + 1
		end
		value(EndMarker)
		value(tp)         -- type of initializer
		value(t)          -- the object
		value(oldhead)    -- goto
	end
	
	-- Store the given object
	StoreObject(object ~= object and NANt or object)
	
	local IdNil
	local Types_nil = Types["nil"]
	
	-- Iterate over all values and store corresponding objects, fill objlist and objid
	while head do
		local v = tostore[head]
		head = head + 1
		if v == BaseMarker then
			count = count + 1
			v = tostore[head + 1]
			objlist[count] = v
			objid[v] = count
			if tostore[head] == Types_nil then  -- don't store non-nil persisted as nil more than once
				if IdNil then
					objid[v] = IdNil
					count = count - 1
				else
					IdNil = count
				end
			end
			head = head + 2
			if tostore[head] == EndMarker then  -- don't add initializer if it has no values
				head = tostore[head + 3]
			end
		elseif v == EndMarker then
			count = count + 1
			objlist[count] = tostore[head + 1]
			head = tostore[head + 2]
		else
			StoreObject(v)
		end
	end
	
	-- Prepare to persist
	local data, dataN = {}, 0

	local function UintSize(n)
		local b = 1
		local ret = 0
		while n >= b do
			b = b*256
			ret = ret + 1
		end
		return ret
	end
	
	local function WriteUint(i, n)
		if n ~= 0 then
			dataN = dataN + 1
			local b = i % 256
			data[dataN] = chars[b + 1]
			return WriteUint((i - b) / 256, n - 1)
		end
	end
	
	local function idxwriter()
		local idxsize, idxmax = 1, 255
		
		return function(v)
			while v >= idxmax do
				WriteUint(idxmax, idxsize)
				idxsize = idxsize + 1
				idxmax = (idxmax + 1)*256 - 1
			end
			WriteUint(v, idxsize)
		end
	end

	-- Write header
	WriteUint(1, 1)      -- version
	WriteUint(count, 4)  -- objects count
	local ptype = dataN
	dataN = dataN + count
	persisting = true

	-- Persist types
	local writers = {
		number = function(n)
			local m, e
			if frexp then
				m, e = frexp(n)
				while m % 1 ~= 0 do
					m = m*256
					e = e - 8
				end
				while e < 0 and m % 2 == 0 do  -- minimize exp (we'll get integer if it's 0)
					m = m / 2
					e = e + 1
				end
				e = e*2
				if e < 0 then
					e = -e - 1
				end
			else
				-- in case lua_Number is integer
				m, e = n, 0
			end
			local sign = m < 0
			if sign then
				m = -(m + 1)
			else
				m = m - 1
			end
			local mn, en = UintSize(m), UintSize(e)
			if mn == 0 then
				mn = 1
			end
			objtype = objtype + mn - 1 + en*8 + (sign and 32 or 0)
			WriteUint(m, mn)
			WriteUint(e, en)
		end,
		
		string = function(s)
			local len = #s
			local tp = objtype + len
			if tp > 255 then
				WriteUint(len, 4)
			else
				objtype = tp
			end
			dataN = dataN + 1
			data[dataN] = s
		end,
	}

	-- Persist functions
	if upvalueid then
	
		local upvals = {}
		local upvalN = 0
		local upwrite = idxwriter()
		-- write crossing upvalues and don't write corresponding values
		writers["function"] = function(f)
			local iwrite = idxwriter()
			local i = 1
			local was
			while getupvalue(f, i) do
				local v = upvalueid(f, i)
				local n = upvals[v]
				if n then
					iwrite(i)
					upwrite(n - 1)
					tostore[head + i + 1] = SkipMarker  -- skip the value, it's already written
					was = true
				else
					upvalN = upvalN + 1
					upvals[v] = upvalN
				end
				i = i + 1
			end
			if was then
				iwrite(0)
				objtype = objtype + 1
			end
		end
		
	else
	
		local funcs = {}
		local funcsN = 0
		local upback = {}
		-- check for functions with dependant upvalues (only) to issue an error in case debug.upvalueid is absent
		writers["function"] = function(f)
			local n = 1
			local a, v = getupvalue(f, n)
			if a == nil then
				return  -- ignore functions without upvals
			end
			if funcsN > 0 then
				-- temporary set all upvalues to 'upback' which will serve as a marker
				repeat
					upback[n] = v
					setupvalue(f, n, upback)
					n = n + 1
					a, v = getupvalue(f, n)
				until a == nil
				-- check if any other functions got their upvalues changed to 'upback'
				for k = 1, funcsN do
					local f = funcs[k]
					local i = 1
					a, v = getupvalue(f, i)
					while a ~= nil do
						if v == upback then
							error("cannot persist functions with dependant upvalues due to absense of debug.upvalueid")
						end
						i = i + 1
						a, v = getupvalue(f, i)
					end
				end
				-- restore values of upvalues
				for i = 1, n - 1 do
					setupvalue(f, i, upback[i])
				end
			end
			funcsN = funcsN + 1
			funcs[funcsN] = f
		end
		
	end
	
	-- Actual persistence
	local idxwrite = idxwriter()
	
	local function get()
		head = head + 1
		return tostore[head]
	end
	
	for i = 1, count do
		ptype = ptype + 1
		local obj = objlist[i]
		head = stored[obj]
		while true do
			local v = get()
			if v == BaseMarker then
				objtype = get()
				stored[obj] = head
				local f = writers[TypeNames[objtype + 1]]
				if f then
					f(obj)
				end
				data[ptype] = chars[objtype + 1]
				break
			elseif v == EndMarker then
				data[ptype] = chars[get() + 1]
				break
			elseif v ~= SkipMarker then
				local n = objid[v] or 0
				if n > i then
					error("custom persistence loop")  -- !!!
				end
				idxwrite(n)
			end
		end
	end
	
	return concat(data)
end

function persist(object, custom)
	if ErrorFunction then
		local function f()
			return dopersist(object, custom)
		end
		local ok, ret = xpcall(f, ErrorFunction)
		ErrorInfo = nil
		if ok then
			return ret
		else
			error(ret, 0)
		end
	else
		local ok, ret = pcall(dopersist, object, custom)
		local info = ErrorInfo
		ErrorInfo = nil
		if ok then
			return ret
		else
			error(ret..": "..info(), 0)
		end
	end
end



------------ Unpersist (new) ------------



function unpersist(buf, custom, pos)
	-- Default unpersistence function works mostly like pluto
	if custom == nil or type(custom) == "table" then
		local perm = custom or {}
		local simple = {number = true, string = true, boolean = true}
		custom = function(obj, o)
			if o ~= nil then
				return obj(o)
			end
			local v = perm[obj]
			if v ~= nil or obj == nil then
				return v
			elseif simple[type(obj)] then
				error("permanent object wasn't found: "..tostring2(obj))
			else
				return obj()
			end
		end
	end
	
	local function ReadUint(n)
		local b = 1
		local ret = 0
		for i = pos, pos + n - 1 do
			ret = ret + byte(buf, i)*b
			b = b*256
		end
		pos = pos + n
		return ret
	end
	
	-- Start reading
	pos = pos or 1  -- index of current read position
	local version = ReadUint(1)
	if version ~= 1 then
		error("data was saved with a different version of RSPersist")
	end
	local count = ReadUint(4)
	local ptype = pos - 1
	pos = pos + count
	local objlist = {}
	
	-- Read value reference
	local function idxreader()
		local idxsize, idxmax = 1, 255
	
		return function()
			local v = ReadUint(idxsize)
			while v == idxmax do
				idxsize = idxsize + 1
				idxmax = (idxmax + 1)*256 - 1
				v = ReadUint(idxsize)
			end
			return v
		end
	end
	
	local ReadIdx = idxreader()
	local rid, rval
	
	local function ReadValue()
		rid = ReadIdx()
		rval = objlist[rid]
		return rid ~= 0 or nil
	end
	
	-- Readers for type creation
	local readers = {
		simple = function(tp)
			return SimpleTypes[tp + 1]
		end,

		custom = function()
			ReadValue()
			if rval ~= nil then  -- v could have been persisted as nil
				return custom(rval)
			end
		end,
		
		number = function(tp)
			local mn, en
			if frexp then
				mn = tp % 8
				en = (tp % 32 - mn)/8
			else
				-- in case lua_Number is integer
				mn = tp % 32
				en = 0
			end
			local m = ReadUint(mn + 1) + 1
			if tp >= 32 then
				m = -m
			end
			local e = ReadUint(en)
			if e == 0 then
				return m
			end
			if e % 2 == 0 then
				e = e*(1/2)
			else
				e = -(e + 1)*(1/2)
			end
			return ldexp(m, e)
		end,
		
		string = function(n)
			if n == 0 then
				n = ReadUint(4)
			end
			n, pos = pos, pos + n
			return string_sub(buf, n, pos - 1)
		end,
		
		table = function()
			return {}
		end,
	}
	
	-- Readers for type initialization
	local initializers = {
		table = function(tp, t)
			-- read array part
			if tp % 2 >= 1 then
				local n = 1
				while ReadValue() do
					t[n] = rval
					n = n + 1
				end
			end
			-- read <key,value> part
			if tp % 4 >= 2 then
				while ReadValue() do
					if rval ~= nil then
						t[rval] = ReadValue() and rval
					else
						ReadValue()
					end
				end
			end
			-- read metatable
			if tp % 8 >= 4 and ReadValue() and rval ~= nil then
				if not nometa then
					setmetatable(t, rval)
				else --if nometa ~= "ignore" then
					error("unpersistence of metatables is denied")
				end
			end
		end,
	}
	
	readers.init = function(tp)
		local v = ReadValue() and rval
		local tp0 = byte(buf, ptype + rid)
		local tname = TypeNames[tp0 + 1]
		local f = initializers[tname]
		if f then
			f(tp0 - Types[tname], v)
		else
			assert(tp > 0)
		end
		if tp > 0 then  -- custom initializer
			ReadValue()
			if rval ~= nil and v ~= nil then
				custom(rval, v)
			end
		end
		return v
	end
	
	-- Functions reader and initializer
	if not nofunc then

		local upval, upvalN = {}, 0
		local upread = idxreader()
		local upskip, upskipN = {}, 0
		
		readers["function"] = function(tp)
			local f = loadstring(ReadValue() and rval)
			if not getupvalue then
				error("cannot read a function without debug.getupvalue")
			end
			-- join upvalues...
			local skips
			if not upvaluejoin then
				return tp % 2 < 1 and f or error("cannot join upvalues due to absense of debug.upvaluejoin")
			end

			if tp % 2 >= 1 then
				local iread = idxreader()
				skips, upskip[f] = upskipN + 1, upskipN + 1
				while true do
					local i = iread()
					upskipN = upskipN + 1
					upskip[upskipN] = i
					if i == 0 then
						break
					end
					local n = upread()*2
					upvaluejoin(f, i, upval[n + 1], upval[n + 2])
				end
			end
			-- add new upvalues
			local i = 1
			local skipv = skips and upskip[skips]
			while getupvalue(f, i) do
				if skipv == i then
					skips = skips + 1
					skipv = upskip[skips]
				else
					upval[upvalN + 1] = f
					upval[upvalN + 2] = i
					upvalN = upvalN + 2
				end
				i = i + 1
			end
			return f
		end
		
		initializers["function"] = function(tp, f)
			-- read values of upvalues
			local i, skips = 1, upskip[f]
			local skipv = skips and upskip[skips]
			while getupvalue(f, i) do
				if skipv == i then
					skips = skips + 1
					skipv = upskip[skips]
				elseif ReadValue() and rval ~= nil then
					setupvalue(f, i, rval)
				end
				i = i + 1
			end
			-- read environment
			if tp % 4 >= 2 then
				ReadValue()
				if rval ~= nil and setfenv then
					setfenv(f, rval)
				end
			end
		end
	
	else
		readers["function"] = errorf("unpersistence of functions is denied")
	end

	-- Unpersist all
	for i = 1, count do
		local tp = byte(buf, ptype + i)
		local tname = TypeNames[tp + 1]
		local f = readers[tname]
		if f then
			objlist[i] = f(tp - Types[tname])
		end
	end
	
	return objlist[count]
end

--[[

New Format:

byte               Version
4-byte             Count
byte[Count]        Types
byte[*]            type-specific

The last object in the list is the result

(unfinished!!!)

==== simple types ====

byte type:
  0 :         0 - number 0, 1 - false, 2 - true, 3 - empty string, 4 - HUGE, 5 - -HUGE, 6 - NAN, 255 - nil
  1 - 56 :    number (not 0): sign*32 + (exp size)*8 + (mantissa size - 1) + 1
  57 :        variable length string
  58 - 127 :  string of length (type - 57)

---- number (not 0) ----
mantissa, exp. Both are unsigned integers.

converting from real mantissa, exp to stored:
if mantissa < 0 then  mantissa = abs(mantissa) - 1

if exp < 0 then  exp = abs(exp)*2 - 1     = (abs(exp) - 1)*2 + 1 
else             exp = exp*2

takes from 1 to 9 bytes
special numbers:
 infinity: mantissa = 0, exp = 1
 NAN: mantissa = 0, exp = 2

---- string ----
string of fixed length:
  string(Size)  String

length + string:
  u4            Size
  string(Size)  String

==== function ====



==== table ====

byte bits
  0 - 7:  +1 - has array part, +2 - has key/value part, +4 - has metatable

table array:
  for
    idx     Value
  end
  idx       Ending = 0

table key/value:
  for
    idx     Key
    idx     Value
  end
  idx       Ending = 0
	
table metatable:
  idx       Metatable

---- upvalues ----
upvalues are written right before the first table


]]



