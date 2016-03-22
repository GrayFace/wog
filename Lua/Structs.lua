
local _G = _G
local internal = debug.getregistry()

local getmetatable = debug.getmetatable
local setmetatable = setmetatable
local rawget = rawget
local rawset = rawset
local tostring = tostring
local error = error
local format = string.format

local pairs = pairs
local sortpairs = sortpairs

local call = mem.call
local pchar = mem.pchar
local mem_struct = mem.struct
local i4, i2, i1, u4, u2, u1, i8, u8 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.i8, mem.u8
local mem_structs = mem.structs
local mem_string = mem.string
local mem_copy = mem.copy
local malloc = mem.malloc
local mem_free = mem.free
local IgnoreProtection = mem.IgnoreProtection

----------- No globals from this point ------------

local _NOGLOBALS

--------------------------- Structs Support -------------------------

local structs = _G.structs or {}
_G.structs = structs -- classes
structs.f = structs.f or {}  -- definitions functions
structs.o = structs.o or {}  -- offsets
structs.m = structs.m or {}  -- members

function structs.class(t)
	local mt = getmetatable(t)
	return mt and rawget(mt, "class")
end

function structs.enum(t, unordered)
	local mt = getmetatable(t)
	local a = mt and mt.members
	if a == nil then
		error("not a structure", 2)
	end
	local f, data = (unordered and pairs or sortpairs)(a)
	return function(_, k)
		k = f(data, k)--next(a, k)
		if k ~= nil then
			return k, t[k]
		end
	end
end


local function structs_index(t, a)
	local v = rawget(structs, "f")[a]
	if v then
		local class = {}
		rawset(structs, a, class)
		return mem_struct(function(define, ...)
			structs.o[a] = define.offsets
			structs.m[a] = define.members
			--define.class[structs_name_t] = a
			return v(define, ...)
		end, class)
	end
end

setmetatable(structs, {__index = structs_index})

-- EditablePChar

local EditablePCharText = {}
internal.EditablePCharText = EditablePCharText

local function EditPChar_newindex(_, o, val)
	local p = u4[o]
	local s = mem_string(p)
	val = tostring(val)
	if s == val then
		return
	end
	local o1 = EditablePCharText[p]
	if not o1 or #s < #val then
		if o1 then
			EditablePCharText[p] = nil
			mem_free(p)
		end
		p = malloc(#val + 1)
	end
	EditablePCharText[p] = o
	u4[o] = p
	mem_copy(p, val, #val + 1)
end

local function EditConstPChar_newindex(_, o, v)
	IgnoreProtection(true)
	EditPChar_newindex(_, o, v)
	IgnoreProtection(false)
end

_G.mem.EditPChar = setmetatable({}, {__index = pchar, __newindex = EditPChar_newindex})
_G.mem.EditConstPChar = setmetatable({}, {__index = pchar, __newindex = EditConstPChar_newindex})

function mem_structs.types.EditPChar(name)
	return mem_structs.CustomType(name, 4, function(o, obj, name, val)
		if val == nil then
			return pchar[obj["?ptr"] + o]
		end
		EditPChar_newindex(nil, obj["?ptr"] + o, val)
	end)
end

function mem_structs.types.EditConstPChar(name)
	return mem_structs.CustomType(name, 4, function(o, obj, name, val)
		if val == nil then
			return pchar[obj["?ptr"] + o]
		end
		EditConstPChar_newindex(nil, obj["?ptr"] + o, val)
	end)
end

local function DynStrShort_index(_, a)
	local p, n = u4[a + 0], u4[a + 4]
	if p == 0 and n == 0 then
		return ""
	end
	local ret = mem_string(p, n, true)
	if ret then
		return ret
	end
	error(format('memory at address %X (referenced by %X) cannot be read', p, a), 2)
end

local function DynStrShort_newindex(_, a, v)
	v = tostring(v)
	call(0x404180, 1, a - 4, v, #v)
end

_G.mem.DynStrShort = setmetatable({}, {__index = DynStrShort_index, __newindex = DynStrShort_newindex})

function mem_structs.types.DynStrShort(name)
	return mem_structs.CustomType(name, 12, function(o, obj, name, val)
		if val == nil then
			return DynStrShort_index(nil, obj["?ptr"] + o)
		end
		DynStrShort_newindex(nil, obj["?ptr"] + o, val)
	end)
end


local function DynStr_index(_, a)
	return DynStrShort_index(a + 4)
end

local function DynStr_newindex(_, a, v)
	v = tostring(v)
	call(0x404180, 1, a + 0, v, #v)
end

_G.mem.DynStr = setmetatable({}, {__index = DynStr_index, __newindex = DynStr_newindex})

function mem_structs.types.DynStr(name)
	return mem_structs.CustomType(name, 16, function(o, obj, name, val)
		if val == nil then
			return DynStrShort_index(nil, obj["?ptr"] + o + 4)
		end
		DynStr_newindex(nil, obj["?ptr"] + o, val)
	end)
end

--------------------------- Some Structs -------------------------

function structs.f.MainStruct(define)
	
end


function structs.f.CombatMonster(define)
	define
	.union 'Unsafe'
		[0x34].i4  'Type'
		[0x38].i4  'Pos'
	.union()
	[0x3C].i4  'Animation'
	[0x40].i4  'Frame'
	[0x4C].i4  'Count'
	[0x50].i4  'VisibleCount'
	[0x54].i4  'LostForeverCount'
	[0x58].i4  'HealthLost'
	[0x5C].i4  'HeroSlot'
	[0x60].i4  'BaseCount'
	[0x6C].i4  'FullHealth'
	[0x74].i4  'Town'
	[0x78].i4  'Level'
	[0x84].i4  'Flags'
	[0x88].i4  'Name'
	[0x8C].i4  'NamePlural'
	[0x90].i4  'Ability'
	[0xC0].i4  'BaseFullHealth'
	[0xC4].i4  'Speed'
	[0xC8].i4  'Attack'
	[0xCC].i4  'Defence'
	[0xD0].i4  'DamageMin'
	[0xD4].i4  'DamageMax'
	[0xD8].i4  'ShotsLeft'
	[0xDC].i4  'CastsLeft'
	[0xF4].i4  'Side'
	--[0xF8].i4  '' -- ?? HeroSlot
	--[0xFC].i4  ''
	[0x100].i4  'OffsetX' -- ??
	[0x104].i4  'OffsetX' -- ??
	[0x14C].i4  'CountRectOffset'
	--[0x154].i4  ''
	[0x164].i4  'Def'
	[0x168].i4  'ShotDef'
	[0x170].i4  'MoveWav'
	[0x174].i4  'AttackWav'
	[0x178].i4  'GetHitWav'
	[0x17C].i4  'ShootWav'
	[0x180].i4  'DeathWav'
	[0x184].i4  'DefendWav'
	.union  'Spells'
		[0x194].i4  'Count'
		[0x198].array(81).i4  'Duration'
		[0x2DC].array(81).i4  'Power'
	.union()
	[0x454].i4  'RetaliationsCount'
	[0x458].i4  'BlessEffect'
	[0x45C].i4  'CurseEffect'
	[0x464].i4  'BloodlustEffect'
	[0x468].i4  'PrecisionEffect'
	[0x48C].i4  'SlayerLevel' -- 1, 2 or 3 - skill in magic shcool (?)
	-- +490 dd номер атакера по пор€дку??? уже атаковал??? (сбрасываетс€ после первого удара)
	[0x494].i4  'CounterstrikeEffect'
	-- +4C0 db Blinded - снизить защиту (сбросить после?) при атаке на него (уст. перед ударом)
	-- +4C1 db Paralized - снизить защиту (сбросить после?) при атаке на него (уст. перед ударом)
	[0x4C2].u1  'ForgetfullnessLevel'
	-- +4DC dd = величина бонуса при выборе защиты
	-- +4E0 dd заклинание дл€ сказ дракона
	[0x4E8].i4  'Moral' -- ћораль и удача пересчитываетс€ при передаче хода другому существу. ѕоэтому полезно только дл€ получени€ или проверки. ≈сли очень нужно мен€ть то можно делать это или перед атакой, или каждый ход
	[0x4EC].i4  'Luck'

	-- +514 dd
	-- +518 dd -> dd first \ adjusted stacks pointers
	-- +51C dd -> dd last  /

	-- +524 dd
	-- dd -> dd first \ adjusted to wich stacks pointers
	-- +52C dd -> dd last  /

	.size = 0x548
end

-- set metatable for Unsafe read access

local function DefineSideMonsterPair(name, dist)
	dist = dist or 1
	return mem_structs.CustomType(name, (dist == 1) and 2 or 1,
		function(o, obj, name, val)
			if val then
				u1[o] = (val >= 21) and 1 or 0
				u1[o + dist] = val % 21
			else
				return u1[o]*21 + u1[o + dist]
			end
		end
	)
end
structs.DefineSideMonsterPair = DefineSideMonsterPair

function structs.f.CombatHex(define)
	define
	[0x00].u2  'MiddleX'
	[0x02].u2  'BottomLineY1'
	[0x04].u2  'LeftX'
	[0x06].u2  'TopY'
	[0x08].u2  'RightX'
	[0x0A].u2  'BottomLineY2'
	[0x0C].u2  'BottomEndY'
	[0x0E].skip(2)
	[0x10].u1  'ObstacleKind'
	[0x14].i4  'ObstacleIndex'
	.goto(0x18)
	DefineSideMonsterPair('Monster')
	define
	[0x1C].u1  'DeadMonstersCount'
	[0x20].array(14)
	DefineSideMonsterPair('DeadMonsters', 14)
	define
	[0x4A].u1  'AccessableForLeftSquare'
	[0x4A].u1  'AccessableForRightSquare'
	.size = 0x70
end

function structs.f.CombatArrowTower(define)
	define
	.i4  'ShooterType'
	.i4  'ShooterDef'
	.skip(4) -- ??? загруженный DEF самой башни ????
	.skip(4) -- ??? позици€
	.skip(4) -- ??? позици€
	.skip(4) -- ??? 0
	.skip(4) -- ??? 2
	.skip(4) -- ??? 0
	.skip(4) -- ???
end



function structs.f.CombatMan(define)
	local last
	define
	.union 'Action'
		[0x3C].i4  'Type'
		[0x40].i4  'Number'
		[0x44].i4  'Target'
		[0x48].i4  'Target2'
	.union()
	[0x4C].array(187).u1  'HexesAccessability1'
	[0x107].array(187).u1  'HexesAccessability2'
	[0x1C4].array(187).struct(structs.CombatHex)  'Hexes'
	--[0x53A4].i4  'SiegeTypeUnk'
	[0x53BC].i4  'MapSquare'
	[0x53C0].i4  'GroundModificator'
	.union 'Attacker'
		[0x53CC].u4  'Hero'
		[0x53E4].i4  'HeroAnimation'
		[0x53EC].i4  'HeroFrame'
		[0x5404].u4  'HeroDef'
		[0x540C].u4  'FlagDef'
		[0x5414].i4  'FlagFrame'
		[0x54B4].i4  'UsedSpell'
	.union()
	.union 'Defender'
		[0x53D0].u4  'Hero'
		[0x53E8].i4  'HeroAnimation'
		[0x53F0].i4  'HeroFrame'
		[0x5408].u4  'HeroDef'
		[0x5410].u4  'FlagDef'
		[0x5414].i4  'FlagFrame'
		[0x54B8].i4  'UsedSpell'
		[0x13DE8].i4  'Attack'
		[0x13DEC].i4  'Defence'
		[0x13DF0].i4  'SpellPower'
		[0x13DF4].i4  'SpellPoints' -- ??
	.union()
	[0x54CC].array(42).struct(structs.CombatMonster) 'Monsters'
	--[0x132B4].i4  'PlayersTurn' -- ? ход игрока, сид€щего за компом
	.goto(0x132B8)
	DefineSideMonsterPair('CurrentMonster')
	define
	-- +132C0 dd = текуща€ атакующа€ сторона (0 или 1) (1)
	[0x132C4].b4  'IsAutoCombat' -- ??
	[0x132C8].pstruct(structs.CombatMonster)  'CurrentMonster'
	[0x132CC].b1  'IsMouseOverBattlefield'
	--[0x132D0].i4  'HexUnderMouse'
	--[0x132D4].i4  'HexUnderMouse'
	--[0x132E0].i4  'CursorSubtype'
	[0x132F4].i4  'SiegeType'
	-- +132F8 dd = закончилась битва (1) или нет (0)
	-- +132FC dd ??? мэнэджер вывода строки в строке сост.
	[0x13464].pchar  'BackgroundPcxName'
	[0x13468].array(187).array(6).u2  'AdjacentHexes'
	-- +13D38 dd = X левого кра€ пр€моугольника анимации (rect.Left)
	-- +13D3C dd = Y верха пр€моугольника анимации (rect.Top)
	-- +13D40 dd = X правого кра€ пр€моугольника анимации (rect.Right-1)
	-- +13D44 dd = Y низа пр€моугольника анимации (rect.Bottom-1)
	-- +13D48 dd = ? 3.58 выигравша€ сторона (0,1) или никто -1 (3)
	-- +13D54 dd -> loaded "cmnumwin.pcx"
	[0x13D68].b1  'IsTactics'
	[0x13D6C].i4  'TurnIndex'
	[0x13D70].i4  'TacticsDifference'
	--[0x13D74].u1  'CheatUnlimitedCast'
	--[0x13D75].u1  'CheatShowInvisibleObjects'
	--[0x13D76].u1  'CheatShow'  -- 13D76 db = „»“! показать все зан€тые клетки на поле
	[0x13D78].array(3).struct(structs.CombatArrowTower)  'ArrowTowers'
end

-- event("StructVars")
