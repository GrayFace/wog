do return end  -- Lua hooks are too slow for the task

local _G = _G
local internal = debug.getregistry()
local pairs = pairs
local assert = assert

local i4, i2, i1, u4, u2, u1, call, mem_string = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.call, mem.string
local string_lower = string.lower
local min = math.min

----------- No globals from this point ------------

local _NOGLOBALS

----------- Manage replacements ------------

local FrameMul = 32*4
local GroupMul = 4
local PermBit = 2
local FrameBit = 1

local DefRep = {}

local function Deref(rep, n)
	local p = rep[n]
	if p then
		call(u4[u4[p] + 4], 1, p)
		rep[n] = nil
		rep[n + FrameBit] = nil
	end
end

local function UpdateDefCounts(def)
	if def ~= u4[0x69E564] then
		
	end
end

local function AddReplace(def, group, frame, NewDef, NewFrame, temp)
	assert(group >= 0 and group < 32)
	local rep = DefRep[def] or {}
	DefRep[def] = rep
	local n = group*GroupMul + frame*FrameMul
	Deref(rep, n)
	if not temp then
		n = n + PermBit
		Deref(rep, n)
	end
	NewDef = NewDef and call(0x55C9C0, 1, NewDef)
	NewDef = (NewDef ~= 0 and NewDef or nil)
	rep[n] = NewDef
	rep[n + FrameBit] = NewDef and NewFrame
	-- !!! UpdateDefCounts
end

function _G.DefReplace(def, group, frame, NewDef, NewFrame)
	AddReplace(string_lower(def), group, frame, NewDef, NewFrame, true)
end

function _G.DefReplaceGlobal(def, group, frame, NewDef, NewFrame)
	AddReplace(string_lower(def), group, frame, NewDef, NewFrame)
end

function internal.events.LeaveContext()
	for _, rep in pairs(DefRep) do
		for i, frame in pairs(rep) do
			if i % GroupMul == 0 then
				Deref(rep, i)
			end
		end
	end
end

----------- Hooks ------------

-- !!!
-- _G.mem.hook(0x55D0EB, function(d)
	-- UpdateDefCounts(d.ebx)
-- end)


-- a convenient way to know the defs in dialogs
local AllDefs, DefsDrawn = {}, {}
_G.DefsDrawn = DefsDrawn

local function checkExist(def, group, frame)
	if group < u4[def + 0x28] then
		local a = u4[u4[def + 0x1C] + 4*group]
		return a ~= 0 and frame < u4[a + 4] and u4[u4[a + 8] + 4*frame] ~= 0
	end
end

local function check(def, p, HasGroup, Interface)
	p = p + (HasGroup and 4 or 0)
	local group, frame = HasGroup and i4[p - 4] or 0, i4[p]
	-- find replacement def
	local name = string_lower(mem_string(def + 4, 12))
	if not AllDefs[name] then
		AllDefs[name] = true
		DefsDrawn[#DefsDrawn + 1] = name
	end
	local rep = DefRep[name]
	local n = group*GroupMul + frame*FrameMul
	local NewDef = rep and (rep[n] or rep[n + PermBit])
	if NewDef then
		def, group, frame = NewDef, 0, rep[n + FrameBit] or rep[n + PermBit + FrameBit]
		i4[p] = frame
		if HasGroup then
			i4[p - 4] = group
		end
	end
	return checkExist(def, group, frame) and def
end

local function Do(p, retn, HasGroup, Interface)
	_G.mem.autohook(p, function(d)
		local def = check(d.ecx, d.esp + 4, HasGroup, Interface)
		if not def then
			return d:ret(retn) or true
		end
		d.ecx = def
	end)
end

Do(0x47B610, 0x38, true)
Do(0x47B7D0, 0x20, false)
Do(0x47B820, 0x30, false)
Do(0x47B680, 0x38, true)
Do(0x47B6E0, 0x30, false)
Do(0x47B730, 0x34, false)
Do(0x47B9A0, 0x34, true)
Do(0x47BA40, 0x34, true)
Do(0x47B780, 0x30, false)
Do(0x47B9F0, 0x34, true)
Do(0x47B870, 0x34, false)
Do(0x47B910, 0x34, false)
Do(0x47B8C0, 0x34, false)
Do(0x47BA90, 0x38, true)

_G.mem.autohook(0x495AD0, function(d)
	local def = check(u4[d.esp + 4], d.esp + 8, true)
	if not def then
		return d:ret(0x20) or true
	end	
	u4[d.esp + 4] = def
end)
