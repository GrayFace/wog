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
local TransBit = 1

local DefRep = {}

local function Deref(rep, n)
	local p = rep[n]
	if p then
		call(u4[u4[p] + 4], 1, p)
		rep[n] = nil
		rep[n + TransBit] = nil
	end
end

local function UpdateDefCounts(def)
	if def ~= u4[0x69E564] then
		
	end
end

local function AddReplace(def, group, frame, pcx, trans, temp)
	assert(group >= 0 and group < 32)
	local rep = DefRep[def] or {}
	DefRep[def] = rep
	local n = group*GroupMul + frame*FrameMul
	Deref(rep, n)
	if not temp then
		n = n + PermBit
		Deref(rep, n)
	end
	pcx = pcx and call(0x55B1E0, 1, pcx)
	if pcx and pcx ~= 0 then
		if trans == -1 and u4[pcx + 0x30] then
			trans = u2[u4[pcx + 0x30]]
		end
		rep[n] = pcx
		rep[n + TransBit] = pcx and (trans or 0)
	end
	-- !!! UpdateDefCounts
end

function _G.DefReplace(def, group, frame, pcx, trans)
	AddReplace(string_lower(def), group, frame, pcx, trans, true)
end

function _G.DefReplaceGlobal(def, group, frame, pcx, trans)
	AddReplace(string_lower(def), group, frame, pcx, trans)
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

local function check(def, group, frame, Interface, p)
	if Interface and group >= 0 and group < 32 then
		local rep = DefRep[string_lower(mem_string(def + 4, 12))]
		local n = group*GroupMul + frame*FrameMul
		local pcx = rep and (rep[n] or rep[n + PermBit])
		if pcx then
			local trans = rep[n + TransBit] or rep[n + PermBit + TransBit]
			local srcX, srcY, width, height = 0, 0, i4[pcx + 0x24], i4[pcx + 0x28]
			if Interface == 2 then
				local x, y, w, h = i4[p], i4[p+4], i4[p+8], i4[p+12]
				if x >= 0 then
					srcX = x
				else
					w = w + x
				end
				if y >= 0 then
					srcY = y
				else
					h = h + y
				end
				if x >= width or y >= height or w <= 0 or h <= 0 then
					return false
				end
				width = min(width, w)
				height = min(height, w)
				p = p + 16
			end
			-- !!! mirror: u4[p + 24] ~= 0
			call(0x44DF80, 1, pcx, srcX, srcY, width, height, i4[p], i4[p+4], i4[p+8], i4[p+12], i4[p+16], i4[p+20], trans)
			return false
		end
	end
	if group < u4[def + 0x28] then
		local a = u4[u4[def + 0x1C] + 4*group]
		return a ~= 0 and frame < u4[a + 4] and u4[u4[a + 8] + 4*frame] ~= 0
	end
end

local function Do(p, retn, HasGroup, Interface)
	_G.mem.autohook(p, function(d)
		local p = d.esp + (HasGroup and 8 or 4)
		local group, frame = HasGroup and i4[p - 4] or 0, i4[p]
		return not check(d.ecx, group, frame, Interface, p + 4) and (d:ret(retn) or true)
	end)
end

Do(0x47B610, 0x38, true, 2)
Do(0x47B7D0, 0x20, false, 1)
Do(0x47B820, 0x30, false, 2)
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
	return not check(d:getparams(0, 3)) and (d:ret(0x20) or true)
end)
