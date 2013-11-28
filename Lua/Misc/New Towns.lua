
-- add compiler directive NEWTOWNS

--[[
Diakon's:
все Diakon в коде
MemmoryProc.cpp: // настройка генератора карт (генерируем новых и ВоГовских монстров)
]]


local _G = _G
local internal = debug.getregistry()

local format = string.format

local table_copy = table.copy

local i4, i2, i1, u4, u2, u1, i8, u8 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.i8, mem.u8
local call = mem.call
local pchar = mem.pchar
local mem_struct = mem.struct
local mem_structs = mem.structs
local mem_string = mem.string
local mem_copy = mem.copy
local mem_fill = mem.fill
local mem_free = mem.free
local mem_realloc = mem.realloc
local malloc = mem.malloc
local IgnoreProtection = mem.IgnoreProtection
local mem_autohook = mem.autohook
local mem_autohook2 = mem.autohook2
local getdefine = mem.structs.getdefine
local structs_enum = structs.enum
local bit_And = bit.And

----------- No globals from this point ------------

--local _NOGLOBALS

----------- Defining manager struct ------------

local function And(n1, n2)
	local m1, m2 = n1 % 0x100000000, n2 % 0x100000000
	return bit_And(m1, m2) + 0x100000000*bit_And((n1 - m1)/0x100000000, (n2 - m2)/0x100000000)
end

local pc = mem_structs.types.EditConstPChar

local CurRelocInfos
local CountRef
local RelocInfo
local DefineRelocInfo
local ArrayBase

local old_member_callback
local function member_callback(...)
	local r = old_member_callback(...)
	RelocInfo = DefineRelocInfo or {}
	DefineRelocInfo = nil
	RelocInfo.Ref = RelocInfo.Ref or {}
	RelocInfo.EndRef = RelocInfo.EndRef or {}
	RelocInfo.CountBefore = RelocInfo.CountBefore or 0
end

local function Array(t)
	DefineRelocInfo = t
	if t and t[1] ~= 0 then
		t.Const, t[1] = t[1], nil
	end
	t.Const = t.Const + ArrayBase
	if t and t.CountBefore then --or t.CountAfter) then
		local before = t.CountBefore or 0
		--local after = t.CountAfter or 0
		local function GetLen(_, p)
			return i4[p] + before --+ after
		end
		return getdefine().array{-before, lenA = GetLen, lenP = 0}
	end
	return getdefine().array{lenA = i4, lenP = 0}
end

local function SetCount(std)
	assert(mem_structs.LastStructMember == "Count")
	RelocInfo.Const = std
end

local function DoAdd(t, items)
	for _, v in ipairs(items) do
		t[#t+1] = v
	end
end

local function Ref(t)
	DoAdd(RelocInfo.Ref, t)
end

local function EndRef(t)
	DoAdd(RelocInfo.EndRef, t)
end

local function Alias(s)
	RelocInfo.Alias = s
end

local function Num(t)
	DoAdd(CountRef, t)
end

----------- Manager functions ------------

local function Relocate(t, p, info, DeltaEnd)
	local p1 = t["?ptr"]
	if p1 == p then
		return
	end
	t["?ptr"] = p1
	IgnoreProtection(true)
	for _, ref in ipairs(info.Ref) do
		u4[ref] = p - p1 + u4[ref]
	end
	for _, ref in ipairs(info.EndRef) do
		u4[ref] = p - p1 + DeltaEnd + u4[ref]
	end
	IgnoreProtection(false)
end

local function _ManagerAddCount(CountRef, n)
	for k, t in ipairs(m.CountRef) do
		if type(t) == "table" then
			local a, p = t[1], t[2]
			a[p] = a[p] + (t[3] or 1)*n
		else
			local a = (u4[t] >= 0x10000 and u1 or u4)
			a[t] = a[t] + n
		end
	end
end

local function MakeManager(f)
	local Relocs = {}
	CountRef = {}
	CurRelocInfos = Relocs
	
	if internal.RSMem.member_callback ~= member_callback then
		old_member_callback = internal.RSMem.member_callback
		internal.RSMem.member_callback = member_callback
	end
	local Class = mem_struct(function(define, ...)
		define.i4  'Count'
		return f(define, ...)
	end)
	internal.RSMem.member_callback = old_member_callback

	local Struct = mem_struct(malloc(Class["?size"]))

	Struct.Count = Relocs.Count.Const
	for k, info in pairs(Relocs) do
		-- allocate a copy and keep original arrays as backup (won't work if there are dynamic fields, e.g. mem.DynStr!)
		-- !!! in release version gotta make a separate backup of original and not realocate unless needed, for full stability when there are no new towns
		local t = Struct[k]
		if type(t) == "table" then
			Relocate(t, malloc(t.Size), info, 0)
			mem_copy(t["?ptr"], info.Const, t.Size)
		-- else
			-- Struct[k] = info.Const
		end
	end
	return {Relocs = Relocs, CountRef = CountRef, Class = Class, Struct = Struct,
		AllocCount = Struct.Count, Elements = {}, NameID = {}}
end

local function ManagerExpand(m)  -- add space for 1 new element
	local Struct = m.Struct
	local n = Struct.Count
	local NeedRealloc = (n >= m.AllocCount)
	Struct.Count = n + 1
	for k, info in pairs(m.Relocs) do
		local t = Struct[k]
		if type(t) == "table" then
			if NeedRealloc then
				Relocate(t, mem_realloc(t["?ptr"], t.Size), info, t.ItemSize)
			end
			-- use the original 1st element as prototype (won't work if there are dynamic fields, e.g. mem.DynStr!)
			if info.new then
				info.new(t["?ptr"] + (n - t.low)*t.ItemSize, t, n)
			elseif info.InitZero then
				mem_fill(t["?ptr"] + (n - t.low)*t.ItemSize, t.ItemSize, 0)
			else
				mem_copy(t["?ptr"] + (n - t.low)*t.ItemSize, info.Const - t.low*t.ItemSize, t.ItemSize)
			end
		end
	end
	_ManagerAddCount(m.CountRef, 1)
end

local function StructAssign(a, t, noshift)
	if type(t) ~= "table" then
		error("must be a table") -- !!!
	end
	local f, param, shift
	local members = getmetatable(a).members
	if members then
		f, param = next, members  -- enum struct members
	else
		f = a  -- enum array
		if not noshift and a.Low == 0 and t[0] == nil and t[1] ~= nil then
			shift = true
		end
	end
	
	for k, v in f, param do
		local t1 = t[shift and k + 1 or k]
		if t1 ~= nil then
			if type(v) == "table" then
				StructAssign(v, t1)
			else
				a[k] = t1
			end
		end
	end
end

local function ManagerAssignElement(m, el, id)
	local Struct = m.Struct
	for k, info in pairs(m.Relocs) do
		local a, t = Struct[k], el[info.Alias or k]
		if type(a) == "table" and t ~= nil then
			StructAssign(a, t, info.NoShift)
		end
	end
end

local function ManagerAttachElement(m, name, id)
	if not m.Elements[name] then
		error("name not found") -- !!!
	end
	local NameID = m.NameID
	if NameID[name] then  -- already attached
		if NameID[name] == id then
			return
		end
		error("already attached with different ID") -- !!!
	end
	if NameID[id] then  -- already taken
		if NameID[id] == name then
			return
		end
		error("ID already taken") -- !!!
	end
	for i = m.Struct.Count, id do
		ManagerExpand(m)
	end
	if m.Template then
		ManagerAssignElement(m, m.Template, id)
	end
	ManagerAssignElement(m, m.Elements[name], id)
end

local function ManagerNewElement(m, el, id, allowChange)
	m.Elements[el.NAME] = el
	if id then
		ManagerAttachElement(m, el.NAME, id, allowChange)
	end
end


----------- TownTypes manager ------------

local HordeBuildingData = mem_struct(function(define)
	define
	.i4  'Monster'
	.i2  'Count'
	.i2  'Level'
	.i4  'UpgMonster'
	.i2  'UpgCount'
	.i2  'UpgLevel'  -- = Level + 7
end)

local SpecialBuildingsNames = mem_struct(function(define)
	pc(17)
	pc(18)
	pc(19)
	pc(20)
	pc(21)
	pc(22)
	pc(23)
	pc(24)
	pc(25)
	pc(26)
	pc(15)
end)

local ResourcesData = mem.struct(function(define)
	define
	.i4  'Wood'
	.i4  'Mercury'
	.i4  'Ore'
	.i4  'Sulfur'
	.i4  'Crystal'
	.i4  'Gems'
	.i4  'Gold'
	[0].array(7).i4  'ByIndex'
	.indexmember  'ByIndex'
	.newindexmember  'ByIndex'	
end)

local TownSetupMapping = mem.struct(function(define)
	define
	.array(10).u1  'Std'
	.u1  'ArtifactMerchant'
	.array(7).u1  'Std2'
	.array(4).u1  'Special'
	.u1  'Dwelling1'
	.u1  'Dwelling1Upg'
	.u1  'Dwelling1Horde'
	.u1  'Dwelling2'
	.u1  'Dwelling2Upg'
	.u1  'Dwelling2Horde'
	.u1  'Dwelling3'
	.u1  'Dwelling3Upg'
	.u1  'Dwelling3Horde'
	.u1  'Dwelling4'
	.u1  'Dwelling4Upg'
	.u1  'Dwelling4Horde'
	.u1  'Dwelling5'
	.u1  'Dwelling5Upg'
	.u1  'Dwelling5Horde'
	.u1  'Dwelling6'
	.u1  'Dwelling6Upg'
	.u1  'Dwelling7'
	.u1  'Dwelling7Upg'
	assert(define.size == 41)
end)

local WallSection = mem.struct(function(define)
	define
	.i2  'X'
	.i2  'Y'
	.i2  'unk1'  -- (-1)
	.i2  'unk2'  -- (0)
	.array(5) pc  'PcxName'
	pc  'Name'
	.i2  'HP'
	.i2  'unk3'  -- (0)
	assert(define.size == 0x24)
end)

local CombatHeroDef = mem.struct(function(define)
	pc  'Def'
	.i4  'CastX'
	.i4  'CastY'
	.i4  'CastFrame'
end)

local TownTowers = mem.struct(function(define)
	define
	.i4  'Monster'
	.array(3).array(2).i4  'MonPositions'
	pc  'ShotDef'
	assert(define.size == 0x20)
end)


local function f_TownTypes(define)
	SetCount(9)
	
	Array{0x6436A4, CountBefore = 1}.i4  'NativeLand'
	Ref{0x43D4F5, 0x44C2B1, 0x5C1847}

	Array{0x6408D8}.i4  'NativeLandRMG'
	Ref{0x532FA1}
	
	Array{0x6436CC} pc  'MP3'
	Ref{0x5C70F5}
	
	Array{0x6A755C} pc  'Name'
	Ref{0x40C9AB, 0x5697FA, 0x57635D, 0x58D95D, 0x58DD27, 0x5C1857}
	
	Array{0x68277C} pc  'NameLowerCase'
	Ref{0x54A4AC}

	Array{0x6A60B0}.array(16) pc  'TownNames'
	Ref{0x4CA9D3}
	
	Array{0x6971F0}.array(0x18).u1  'internal_TownNamesIndex'
	Ref{0x4CA757, 0x4CA990, 0x4CA9B2}
	EndRef{0x4CA78B}
	function RelocInfo.new(p)
		mem.call(0x4CA6D0, 1, p)
	end
	function RelocInfo.free(p)
		mem.call(0x4CA700, 1, p)
	end
	
	-- puzzle
	
	Array{0x6818D0} pc  'PuzzleNamePart'
	Ref{0x52C9CD, 0x52CF5C}
	
	Array{0x6818F4}.array(2).array(48).i2  'PuzzlePieces'
	Ref{0x52CC05, 0x52CC0C, 0x52CF97, 0x52CF9E}
	
	Array{0x681FB4}.array(48).i2  'PuzzlePiecesOrder'
	Ref{0x52CBF6, 0x52CF6C}
	
	-- on map

	Array{0x677A0C} pc  'MapTownDef'
	Ref{0x4C9823}

	Array{0x677A30} pc  'MapCastleDef'
	Ref{0x4C9810}

	Array{0x677A54} pc  'MapCapitolDef'
	Ref{0x4C97C4}

	-- monsters
	
	Array{0x6747B4}.array(14).i4  'Monsters'
	Ref{0x428605, 0x428967, 0x429BB4, 0x429DEF, 0x429F35, 0x42A029, 0x42B53B, 0x42B5DC, 0x42B5F6, 0x42B727, 0x42BE45, 0x42CF0A, 0x42D244, 0x432E97, 0x432F62, 0x43363E, 0x47AA82, 0x47AA93, 0x47AB03, 0x47AB14, 0x47AB83, 0x47AB94, 0x4BF30A, 0x4C8D30, 0x503293, 0x51CFDB, 0x525AB0, 0x52A31E, 0x5519AA, 0x551B6B, 0x576457, 0x5BE386, 0x5BE3AE, 0x5BEFA1, 0x5BFC69, 0x5BFFE2, 0x5C009B, 0x5C0206, 0x5C0267, 0x5C0582, 0x5C0B37, 0x5C0BEF, 0x5C6026, 0x5C7199, 0x5C7CE8, 0x5C7D21, 0x5D9DE7, 0x5D9E60, 0x5D9ED6, 0x5D9F4F, 0x5D9FC8, 0x5DA041, 0x5DA0C5, 0x5DA1BD, 0x5DD09C, 0x5DD96E, 0x5DDAD9}
	Ref{0x47AB36, 0x4BF304, 0x525A8E, 0x5765AC, 0x5C052B}
	Ref{0x42BCCC, 0x47ABB8, 0x4C69B2, 0x5218F7, 0x521910, 0x521954, 0x5219B5, 0x551A17, 0x5C803A}

	Array{0x68295C, CountBefore = 1} pc  'MonstersBackground'
	Ref{0x449650, 0x550012, 0x551E38, 0x5F5453}
	
	Array{0x68A344, CountBefore = 1} pc  'MonstersBackgroundInCastle'
	Ref{0x5D90A7, 0x5D9359}

	Array{0x642EA0}.i4  'BlacksmithMonster'
	Ref{0x5D17B2, 0x5D18C4, 0x5D1B05, 0x5D1FA4, 0x5D1FD6, 0x5D224F, 0x5D228C}
	
	Array{0x6AAA60}.array(2).i4  'BlacksmithArtifact'
	Ref{0x525DE3, 0x5D1D8C, 0x5D1EC1, 0x5D1ECE, 0x5D2242}
	
	-- picture screen
	
	Array{0x643050} pc  'BackgroundPrefix'
	Ref{0x5C6E43}
	
	Array{0x642EC4}.array(44).i4  'BuildingsDrawOrder'
	Ref{0x5C6EB4, 0x5C6FE8}

	Array{0x643074}.array(44) pc  'BuildingsDefs'
	Ref{0x5C6EEC}
	
	Array{0x68A3DC}.array(44) pc  'BuildingsSelection'  -- the same names are also used for TZ* contours
	Ref{0x5C3396}
	
	-- town hall
	
	Array{0x68A36C} pc  'MageGuildWindow'
	Ref{0x5CCEDA}
	
	Array{0x67F5D4} pc  'HallBuildingsDef'
	Ref{0x5D5B21, 0x5D9A6D}

	Array{0x65F4C4, CountBefore = (0x65F53C-0x65F4C4)/4} pc  'HallBuildingsDef2'
	Ref{0x407126, 0x40716A, 0x407193}
	Alias 'HallBuildingsDef'
	
	Array{0x6755B8}.array(44) pc  'HallBildingsIcons'
	Ref{0x484162}
	
	-- Array{0x66CF98}.array(18).i4  'HallBuildingsNumbers'  -- overwritten by hook at 0x461049 instead
	-- Ref{0x460DE5}
	
	Array{0x66D03C}.i4  'HallBuildingSlotsCount'  -- generated from Buildings
	Ref{0x460DD3, 0x46103A, 0x46131F, 0x4613B0, 0x4613CE, 0x461411, 0x46142D, 0x4614C4, 0x4614E0, 0x46162F}

	-- buildings data
	
	Array{0x6887F0}.array(2).struct(HordeBuildingData)  'HordeBuildings'
	Ref{0x5BE36A, 0x5BEDA7, 0x5BFB2E, 0x5BFBE4, 0x5BFBEF, 0x5C015F, 0x5C0186, 0x5C166E}
	
	Array{0x68A3B4}.array(2).u1  'Horde1MonLevel'
	Ref{0x5C7CD5, 0x5D38F6, 0x5D3931}

	Array{0x68A3C8}.array(2).u1  'Horde2MonLevel'
	Ref{0x5C7D0E, 0x5D4205, 0x5D423E}

	Array{0x688F04}.struct(ResourcesData)  'ResourceSilo'
	Ref{0x5BFA94, 0x5C1690}
	
	Array{0x688910}.struct(TownSetupMapping)  'SetupMapping'
	Ref{0x484327, 0x5C0323, 0x5C0E23, 0x5C0EFD}
	
	-- also used: Buildings, BuildingDecorations
	
	-- dependancies
	
	Array{0x6977E8}.array(44).u8  'BuildingsDependMasks'  -- generated from Buildings
	Ref{0x42B284, 0x42B293, 0x5C11DA, 0x5C11E1, 0x5C12E8, 0x5C12EF, 0x5C137A, 0x5C1381, 0x5D6018, 0x5D601F}
	Ref{0x4EB852, 0x4EB885, 0x4EB88E, 0x4EB8F5, 0x4EB8FC, 0x4EB917, 0x4EB91E, 0x4EB92A, 0x4EB931, 0x4EB938, 0x4EB93F, 0x4EB948, 0x4EB94F, 0x4EB958, 0x4EB9B3, 0x4EB9BA, 0x4EB9D5, 0x4EB9DC, 0x4EB9E8, 0x4EB9EF, 0x4EB9F6, 0x4EB9FD, 0x4EBA06, 0x4EBA0D, 0x4EBA16, 0x4EBA34, 0x4EBA43, 0x4EBA57, 0x4EBA6B, 0x4EBA7F, 0x4EBA93}
	
	Array{0x697740}.u8  'BuildingsMask'  -- generated from Buildings
	Ref{0x5C035C, 0x5C0363, 0x5C0D75, 0x5C0D84, 0x5C0E9F, 0x5C0EA6, 0x5C0FEB, 0x5C0FF4, 0x5C3839, 0x5C3842, 0x5D374C, 0x5D3756, 0x5D378D, 0x5D3796}
	Ref{0x4EB81C, 0x4EB822, 0x4EB82A, 0x4EB845, 0x4EB84B, 0x4EB85C, 0x4EB8B9, 0x4EB8BF, 0x4EB8C7, 0x4EB8E0, 0x4EB8E6, 0x4EB8EE, 0x4EB977, 0x4EB97D, 0x4EB985, 0x4EB99E, 0x4EB9A4, 0x4EB9AC, 0x4EBA2F, 0x4EBA4D, 0x4EBA61, 0x4EBA75, 0x4EBA89, 0x4EBA9D}
	
	Array{0x6A8C20}.array(44).u8  'BuildingsReplaceMasks'  -- generated from Buildings
	Ref{0x5BED82, 0x5BED8B, 0x5BF2E6, 0x5BF2ED, 0x5BF785, 0x5BF78C, 0x5C03A7, 0x5C03AE, 0x5C0C67, 0x5C0C6E, 0x5C0D11, 0x5C0D18, 0x5C0F0A, 0x5C0F1B,  0x5D606D}
	Ref{0x4EBAAC, 0x4EBAB1, 0x4EBAB7, 0x4EBABD, 0x4EBAC2, 0x4EBAD3, 0x4EBAE7, 0x4EBAEC, 0x4EBAF2, 0x4EBAF8, 0x4EBAFD, 0x4EBB0E, 0x4EBB22, 0x4EBB27, 0x4EBB2D, 0x4EBB33, 0x4EBB38, 0x4EBB49, 0x4EBB5D, 0x4EBB62, 0x4EBB68, 0x4EBB6E, 0x4EBB73, 0x4EBB84, 0x4EBB93, 0x4EBBA2, 0x4EBBB1, 0x4EBBC0, 0x4EBBCF}
	
	-- Resources
	
	Array{0x6A8344}.array(17, 25).struct(ResourcesData)  'BuildingsCost'
	Ref{0x5BF829, 0x5C14BB, 0x5C153C, 0x5C15BE}

	Array{0x6A9880}.array(30, 43).struct(ResourcesData)  'DwellingsCost'
	Ref{0x5BF848, 0x5C14DC, 0x5C1569, 0x5C15DD}
	Alias 'BuildingsCost'

	-- buildings texts
	
	Array{0x6A543C}.struct(SpecialBuildingsNames)  'BuildingsNames'
	Ref{0x460CD3, 0x460CF0, 0x46146C, 0x46148B, 0x4617F3, 0x461824, 0x461960, 0x461991, 0x4619E8, 0x461A07, 0x461A54, 0x461A7F}
	Ref{0x5C7D42, 0x5EA141, 0x5EA2C2}
	Ref{0x460CD3, 0x46146C, 0x4617F3, 0x461960, 0x4619E8, 0x461A54, 0x5EA2C2}

	Array{0x6A7874}.struct(SpecialBuildingsNames)  'BuildingsDescriptions'
	Ref{0x5D2EB1, 0x5D2F28}
	
	Array{0x6A6310}.array(30, 43) pc  'DwellingsNames'
	Ref{0x460D04, 0x4614A0, 0x461842, 0x4619AF, 0x461A1C, 0x461A9A}
	Alias 'BuildingsNames'
	
	Array{0x6A6A2C}.array(30, 43) pc  'DwellingsDescriptions'
	Ref{0x5D3143}
	Alias 'BuildingsDescriptions'
	
	Array{0x6A7ED8}.array(16, 16) pc  'BuildingsDescriptionsBlacksmith'
	Ref{0x5D2E67}
	Alias 'BuildingsDescriptions'
	
	-- combat
	
	Array{0x63BD18}.i4  'MoatDamage'
	Ref{0x421760, 0x4217C3, 0x465FDF, 0x469A88, 0x4B31D0, 0x4B3204}

	-- also used: SiegePicturesPrefix
	
	Array{0x63D2A0} pc  'SiegeBackground'  -- generated from SiegePicturesPrefix
	Ref{0x4642CB}

	Array{0x66D848}.array(18).struct(WallSection)  'SiegeWalls'  -- generated from SiegePicturesPrefix
	Ref{0x462FBA, 0x465EE9, 0x479C39, 0x479C86, 0x4929F3, 0x493923, 0x493967, 0x494259, 0x494260, 0x49437C, 0x494384, 0x4945C9, 0x4945D1, 0x4947C3, 0x4955C2}
	--[[ Differences in towns:
	(Rampart)
	66d8e0 66db68  SgCsTpWl.pcx SgRmTpW1.pcx
	66d898 66dda8  SgCsMoat.pcx 
	66d8bc 66ddcc  SgCsMlip.pcx 
	(Conflux)
	66d8e0 66e300  SgCsTpWl.pcx SgNcTpW1.pcx
	66d8e0 66e588  SgCsTpWl.pcx SgDnTpW1.pcx
	66d8e0 66e810  SgCsTpWl.pcx SgStTpW1.pcx
	66d910 66ed50  SgCsTw21.pcx SgElTw22.pcx
	66d914 66ed54  SgCsTw21.pcx 
	66d938 66ed78  SgCsWa61.pcx 
	66d980 66edc0  SgCsWa41.pcx 
	66d9a4 66ede4  SgCsArch.pcx 
	66d9c8 66ee08  SgCsWa31.pcx 
	66da10 66ee50  SgCsWa11.pcx 
	66da34 66ee74  SgCsTw11.pcx 
	66da58 66ee98  SgCsMan1.pcx 
	(others match Castle)
	]]
	
	Array{0x63CF88}.struct(TownTowers)  'SiegeTowers'
	Ref{0x466868}
	
	Array{0x63BD40}.array(2).struct(CombatHeroDef)  'CombatHeroDef'  -- Male, Female
	Ref{0x463079, 0x5A040F, 0x5A04BF, 0x5A04CB, 0x5A051D, 0x5A057F, 0x5A241E}
end

local TownTemplate = {
	ResourceSilo = {
		Wood = 0,
		Ore = 0,
	},
	SetupMapping = {
		ArtifactMerchant = 44,
		Special = {44, 44, 44, 44},
		Dwelling1Horde = 44,
		Dwelling2Horde = 44,
		Dwelling3Horde = 44,
		Dwelling4Horde = 44,
		Dwelling5Horde = 44,
	},
	-- HordeDwellings...
}

--[[
Example:

Buildings = {
	-- creatures
	{
		{30, Gold = 500, Wood = 10, Depend = {0}},
		{31, Gold = 500, Depend = {1}}
		{}
	},
	{
		{32},
		{33},
	},
	...
	-- then all in lexicographical order, typical:
	nil, nil, nil, nil,
	nil, {Levels = 4},
	{  -- Horde building
		{18, Replace = {32}},
		{19, Replace = {18, 33}},
	},
	...
}

]]

local ReorderBuildings16 = {0, 1, 2, 3, 4, 5, 6;  12, 11, 8, 13;   10, 7, 9;     15, 14}
local ReorderBuildings17 = {0, 1, 2, 3, 4, 5, 6;  11, 10, 8, 12;   9, 7, 16;   15, 14, 13}
local ReorderBuildings18 = {0, 1, 2, 3, 4, 5, 6;  12, 11, 8, 13; 10, 7, 9, 16; 17, 15, 14}

local StdBuildings = {
	nil, nil, nil, nil, nil, nil, nil,
	{  -- Village Hall
		{10},
		{11, Depend = {5}},
		{12, Depend = {16, 0, 14}},
		{13, Depend = {9}},
	},
	{{7},{8},{9}},  -- Fort
	{{5}},  -- Tavern
	{{16}},  -- Blacksmith
	{{14},{15}},  -- Marketplace
	{{0},{1},{2},{3},{4}},  -- Guild
}

local function MakeDepBits(t, t1)
	local r = 0
	for i = 1, #t do
		r = r + 2^t[i]
	end
	return r + (t1 and MakeDepBits(t1) or 0)
end

local function ProcessTownBuildings(m, town)
	local bld = m.Elements[town].Buildings or {}
	local BuildBits = 0
	local Deps = m.Struct.BuildingsDependMasks[town]
	local Reset = m.Struct.BuildingsReplaceMasks[town]
	local Slots = {}
	m.BuildingSlots[town] = Slots
	local count = 0
	for i = 1, 18 do
		local LastSlot = {}
		local function process(t)
			count = i
			local n = t[1]
			assert(And(BuildBits, 2^n) == 0, "duplicate building: "..n)
			assert(n >= 0 and n <= 25 or n >= 30 and n <= 43, "wrong building number: "..n)
			BuildBits = BuildBits + 2^n
			local rep = t.Replace or {LastSlot[1]}
			local deps = MakeDepBits(t.Depend or {}, rep)
			Deps[n] = deps - And(deps, 2^10)  -- no Village Hall dependance
			Reset[n] = MakeDepBits(rep)
			local t = {n, Replace = rep}
			LastSlot.Next = t
			Slots[i] = Slots[i] or t
			LastSlot = t
			-- set resources
			if n >= 17 then
				local a = m.Struct[n >= 30 and "DwellingsCost" or "BuildingsCost"][town][n]
				mem_fill(a["?ptr"], 7*4, 0)
				StructAssign(a, t)
			end
		end
		
		local slotStd = StdBuildings[i]
		local slot = bld[i] or slotStd or {}
		for i = 1, slot.Level or 0 do
			process(slotStd[i])
		end
		for i = 1, #slot do
			process(slot[i])
		end
	end
	m.Struct.BuildingsMask[town] = BuildBits
	m.Struct.HallBuildingSlotsCount[town] = count
end

local function ProcessSiegePictures(m, town)
	local prefix = m.Elements[town].SiegePicturesPrefix
	if not prefix then
		return
	end
	m.Struct.SiegeBackground[town] = prefix.."Back.pcx"
	for _, wall in m.Struct.SiegeWalls[town] do
		for i, s in wall.PcxName do
			if s ~= "" then
				wall.PcxName[i] = prefix..string_sub(s, 5)
			end
		end
	end
end

-- t = {number, Replace = {...}, Next = {...}}
local function ChooseBuilding(t, town)
	if not t then
		return
	end
	local n = t[1]
	-- if built
	if And(u8[town + 0x158], 2^n) ~= 0 then
		return ChooseBuilding(t.Next, town) or n
	end
	-- for horde buildings
	for _, i in ipairs(t.Replace) do
		-- built, but not shown => assume it's been upgraded and so this structure offer should be skipped
		if And(u8[town + 0x158], 2^n) ~= 0 and And(u8[town + 0x150], 2^n) == 0 then
			return ChooseBuilding(t.Next, town) or n
		end
	end
	return n
end

local function DecorateBuilding(town, decor)
	for _, n in ipairs(decor or {}) do
		call(0x5BED30, 1, d.ecx, n)
	end
end

local function hook_TownTypes(m)
	IgnoreProtection(true)
	
	-- town hall layout: 16, 17, 18 slots
	local TownHall = malloc(16)
	u4[0x5CAA82] = TownHall
	u4[0x5CACC9] = TownHall
	u4[0x5CAF11] = TownHall
	mem_autohook2(0x5CA8FF, function(d)
		local town = d.eax
		local hall = m.Struct.HallBuildingsDef[town]
		assert(#hall < 16)
		mem_copy(TownHall, hall, #hall + 1)
		if m.Elements[town] then
			local n = m.Struct.HallBuildingSlotsCount[town]
			if n <= 16 then
				d:push(0x5CA912)
			elseif n == 17 then
				d:push(0x5CAB59)
			else
				d:push(0x5CADA0)
			end
			return true
		end
	end, 6)
	
	-- town hall fill slots
	mem_autohook2(0x461049, function(d)
		local town = d.ecx
		if town >= 9 then
			for i = 1, 18 do
				u1[0x694EC0-1 + i] = ChooseBuilding(m.BuildingSlots[i], d.eax) or 0
			end
			d:push(0x4612DF)
			return true
		end
	end, 7)

	-- decor for buildings
	mem_autohook2(0x5BEFEA, function(d)
		local town = d.al
		local decor = m.Elements[town] and m.Elements[town].BuildingDecorations
		DecorateBuilding(d.ecx, decor and decor[d.edx])
	end)
	
	-- starting decor
	mem_autohook2(0x5C3875, function(d)
		local town = d.eax
		local decor = m.Elements[town] and m.Elements[town].BuildingDecorations
		DecorateBuilding(d.ecx, decor and decor[-1])
	end, 7)
	
	-- click on a building
	mem_autohook2(0x5D38A5, function(d)
		i4[0x6AAA50] = -1
		local town = u4[d.ebx + 0x38]
		local bld = d.edi
		local param = {Building = bld, LeftClick = u4[d.esi + 4] == 12}
		local BuildProc = m.Elements[town] >= 9 and m.Elements[town].OnBuildingClick
		if BuildProc[-1] then
			BuildProc[-1](param)
		end
		if BuildProc[bld] then
			BuildProc[bld](param)
		end
		d.edi = param.Building
	end)
	
	-- hook 4BB0C0
	-- hook 5BDB00
	-- hook 583C57 - hero type by town
	-- hook SpTraits - chance to gain spell in town
	-- hook 532F17 (RMG)
	-- hook 53C475 (RMG)
	
	IgnoreProtection(false)
end


----------- HeroTypes manager ------------

local HeroType = mem.struct(function(define)
	define
	.i4  'TownType'
	pc  'Name'
	.i4  'Agression'
	.array(4).u1  'PSkillStart'
	.array(4).u1  'PSkillProbabilityToLvl9'
	.array(4).u1  'PSkillProbabilityFromLvl10'
	.array(28).u1  'SSkillProbability'
	.array(9).u1  'TownProbability'  -- !!! problem
	.skip(3)
	assert(define.size == 0x40)
end)

local function f_HeroTypes(define)
	SetCount(18)
	-- a lot of count in 4BB2A0 and in param, some may be Speciality
	-- 583B60 is also useful for choose game screen players infos (for RMG and heroes list)

	-- !!! set TownType
	Array{0x67D868}.struct(HeroType)  'Data'
	Ref{0x67DCEC}

	Array{0x65F5A8} pc  'MapDef'
	Ref{0x4072EB}
	Num{{i4, 0x4077B6+3}, {i1, 0x407308}}
end

-- !!! hook AdvManager(0x406E63, 407740):
local function f_AM_HeroTypes(define)
	SetCount(18)
	ArrayBase()
	
	Array{0x10C}.i4  'internal_MapDef'
	Ref{0x4077B2, 0x410186, 0x410202, 0x410626, 0x4106A2, 0x47F446, 0x47F58D, 0x47F879, 0x5F7C1F, 0x5F7C95, 0x5F801F, 0x5F8095}
end

----------- Heroes manager ------------

local HeroSpec = mem.struct(function(define)
	define
	.array(7).i4  'Data'  -- !!! unk
	pc  'ShortName'
	pc  'Name'
	pc  'Description'
	assert(define.size == 0x28)
end)

local HeroInfo = mem.struct(function(define)
	define
	.b4  'Female'
	.array(8).i4  'unk1'  -- !!! unk  
	.array(3).i4  'MonsterTypes'
	pc  'SmallPortrait'
	pc  'Portrait'
	.array(8).i1  'unk2'  -- !!! unk  
	pc  'Name'
	.array(3).array(2).i4  'MonsterCounts'  -- !!! unk
	assert(define.size == 0x5C)
end)

local function f_Heroes(define)
	SetCount(156)

	-- count refs: 4B8B1E, 4BB1EC, 4BB252, 4BB36F, 4BB500, 4BD088, (4BD098), 4BD139, 4BD144, (4BD16B, 4BD176), 4BD1CB, 4BE597, 4BF61E, 4BF8D8, 4BFBA1, 4C7919, 4C7D95, 4C81DE, 4C825F, 4C8222, 4C89E5, 4CDE2F, 4CDE53, 4CE070, 4CE054, 580A74, 583610, 5838C2, 583A1E, 58C0AB, 4BE565, 4BE570, 4CAADB, 4BEF9A, 4C2CCF, 4CAADB, 4BEF62, 4BEF49, 4C4B3E, 5381D1, 538225, 583DAE

	-- check count: 0x9C, 0x9C*4, @after Heroes, 0x9C*sizeof(_HERO_), near refs to f_Heroes stuff

	Array{0x678420}.struct(HeroSpec)  'Speciality'
	Ref{0x679C80}

	Array{0x679DD0}.struct(HeroInfo)  'Data'
	Ref{0x67DCE8}

	--Array{0x698F48} pc  'Name'  -- unused
	Array{0x6A6740} pc  'Bio'
	Ref{0x4D92BA, 0x4DD978}
end

local function f_MainStructHeroes(define)
	SetCount(156)

	Array{0xA4}.array(0x334).u1  'HeroSetup'
	Ref{0x485C58, 0x485CB0, 0x4CDC31, 0x4CE581, 0x5029D9, 0x62C921, 0x62CAE1, 0x4CAAE2+2}
	
	Array{0x21620}.array(0x492).u1  'Heroes'
	Ref{0x402480, 0x40315E, 0x4035E2, 0x40625E, 0x406287, 0x4062A9, 0x4062BE, 0x407B1C, 0x407FFD, 0x4080EF, 0x408BFD, 0x408E2E, 0x408EA5, 0x40909A, 0x409264, 0x409A8D, 0x409B7F, 0x40A7F7, 0x40A9CC, 0x40AF7F, 0x40AFF8, 0x40B129, 0x40BBD7, 0x40DF89, 0x40E0F7, 0x40E4B0, 0x40E586, 0x40E5FF, 0x40E60A, 0x40E9A7, 0x40EC34, 0x40F10E, 0x40FCAB, 0x40FDBD, 0x41024D, 0x412CE3, 0x413787, 0x414558, 0x416530, 0x4165BD, 0x4167E2, 0x4168D3, 0x416FAE, 0x417120, 0x417641, 0x417B77, 0x417B7F, 0x418DE4, 0x418E4F, 0x419185, 0x4192D7, 0x419440, 0x4199F1, 0x4199F8, 0x41C4F1, 0x41C63F, 0x41C6B5, 0x41C7A7, 0x41C865, 0x41C946, 0x41CA59, 0x41D037, 0x41D25C, 0x41D547, 0x428178, 0x42980A, 0x42986F, 0x42B142, 0x42BB03, 0x42DEEB, 0x42EFB9, 0x42F6FA, 0x43145D, 0x4315CD, 0x431636, 0x4316ED, 0x4317EE, 0x431EEC, 0x43204B, 0x4331A6, 0x433B05, 0x4518D4, 0x453196, 0x4534D9, 0x453555, 0x47F120, 0x47F1DE, 0x47F4BC, 0x47F69D, 0x47FB8A, 0x47FF5B, 0x4801BD, 0x4803CD, 0x480647, 0x4806D5, 0x480B0D, 0x480FFD, 0x48124D, 0x4812E8, 0x4814CF, 0x4814D6, 0x4817CD, 0x48181F, 0x481911, 0x481981, 0x481AAF, 0x483C42, 0x483C49, 0x483C8C, 0x483CD1, 0x483F8E, 0x4860EB, 0x486BB3, 0x487655, 0x4896AF, 0x48971E, 0x49A2ED, 0x49B0E0, 0x49D449, 0x49D4AF, 0x49E042, 0x49E08B, 0x4A24A2, 0x4A3CAF, 0x4AA738, 0x4AAD36, 0x4AAE0F, 0x4AAF70, 0x4AAFFE, 0x4B262E, 0x4B8AC6, 0x4B9CCE, 0x4BA76A, 0x4BA7BE, 0x4BA8C9, 0x4BA93C, 0x4BA9A2, 0x4BAA08, 0x4BB185, 0x4BB307, 0x4BB49E, 0x4BC925, 0x4BD0AD, 0x4BE503, 0x4BF5A8, 0x4BF63C, 0x4BF862, 0x4BF94A, 0x4BFB49, 0x4BFDAA, 0x4BFF38, 0x4C0047, 0x4C6772, 0x4C6D12, 0x4C6D19, 0x4C709F, 0x4C70F2, 0x4C7163, 0x4C7922, 0x4C7D91, 0x4C7EF9, 0x4C8039, 0x4C80A1, 0x4C817B, 0x4C81C5, 0x4C820D, 0x4C824A, 0x4C82A8, 0x4C83A2, 0x4C83DC, 0x4C83E2, 0x4C8425, 0x4C8833, 0x4C89E1, 0x4C941F, 0x4C94BF, 0x4C94E5, 0x4C9EA8, 0x4CD8EA, 0x4CD9D6, 0x4CDE36, 0x4CE4FB, 0x4DA469, 0x4DE57F, 0x4DE61D, 0x4E18C8, 0x4E1ADA, 0x4E7718, 0x4E7FB8, 0x4E86DB, 0x4E87BF, 0x4E87D9, 0x4E8AC2, 0x4E8ADC, 0x4F1A4E, 0x4F26A9, 0x4F313D, 0x4F3192, 0x4F4A14, 0x4F4EDB, 0x4F53B6, 0x4F540F, 0x4F5484, 0x4FD246, 0x4FD2A8, 0x4FD2F6, 0x4FD381, 0x4FD4A4, 0x50609E, 0x5060EF, 0x51C910, 0x51CBB6, 0x51D5B9, 0x51F3C3, 0x51F68A, 0x51FA13, 0x51FAB0, 0x51FB2C, 0x52137E, 0x521641, 0x521744, 0x5217AC, 0x52189F, 0x5218D7, 0x52210E, 0x5222C0, 0x522982, 0x522ADA, 0x525744, 0x5257FE, 0x525953, 0x525E8A, 0x526529, 0x5267F8, 0x526C03, 0x526F45, 0x526FC3, 0x526FCA, 0x5270EA, 0x527E87, 0x52A1CC, 0x52A8B0, 0x52B1A7, 0x52B372, 0x52B492, 0x52B5DA, 0x557099, 0x5683A9, 0x569DB3, 0x56A899, 0x56E47B, 0x56E57C, 0x56E9A7, 0x5721AB, 0x5722AC, 0x572474, 0x582657, 0x582869, 0x58D57C, 0x58D69A, 0x5AEEFE, 0x5AEF26, 0x5BE412, 0x5BE4AB, 0x5BE685, 0x5BE68C, 0x5BE7C1, 0x5BE7F2, 0x5BE87E, 0x5BE89E, 0x5BE9A9, 0x5BF5AA, 0x5BF5F1, 0x5BFD45, 0x5BFD6C, 0x5BFE12, 0x5C1740, 0x5C17D5, 0x5C188E, 0x5C69E8, 0x5C6A15, 0x5C7256, 0x5C7294, 0x5C7376, 0x5C76A4, 0x5C76C4, 0x5C7E0D, 0x5C7EF0, 0x5C9BF0, 0x5CE9A0, 0x5CE9C4, 0x5D1D65, 0x5D223B, 0x5D31E9, 0x5D320C, 0x5D323B, 0x5D33E0, 0x5D34F2, 0x5D3516, 0x5D449C, 0x5D4519, 0x5D7DAC, 0x5D7E87, 0x5D80C6, 0x5D8442, 0x5D8483, 0x5D84F6, 0x5D8541, 0x5D8776, 0x5D8865, 0x5D89D9, 0x5D8AAE, 0x5DE4AE, 0x5DE510, 0x5DEF20, 0x5DF3E2, 0x5DF463, 0x5EA234, 0x5EA3B1, 0x5EA4AB, 0x5F1BA0, 0x5F1D0E, 0x5F1DE4, 0x5F1F6B, 0x5F7838, 0x5F78DD, 0x5F7CDD, 0x62C9C4, 0x62CB60}
	Ref{0x4BEF70, 0x4C76D9, 0x4C776A, 0x4CABA1, 0x4CAC57, 0x513ACC, 0x513CEA}
	Ref{0x4BFE10+2, 0x4BFE16+2, 0x4C9E87+3, 0x4D90A0+3, 0x4DA4FE+2, 0x4DA50F+3}
	
	Array{0x4DF18}.u1  'HeroOwner'
	Ref{0x412F1C, 0x485F40, 0x485F47, 0x485FE7, 0x4868E4, 0x486D34, 0x48752E, 0x4A3B38, 0x4A3CD5, 0x4BB192, 0x4BB20F, 0x4BB319, 0x4BB4B1, 0x4BD135, 0x4BD167, 0x4BD199, 0x4BE561, 0x4BFC46, 0x4BFDBA, 0x4C81D2, 0x4C821A, 0x4C8257, 0x4C8291, 0x4C83CC, 0x4C8714, 0x4C9E54, 0x4CE042, 0x4D7BBF, 0x4DA414, 0x4DA48C, 0x4DA495, 0x4DA4C2, 0x4DA500, 0x4DA512, 0x4F1A7C, 0x4F1A86, 0x502FC6, 0x502FDB, 0x56AF42, 0x577A51, 0x577A5F, 0x580A7E, 0x58360C, 0x5838C9, 0x583A25, 0x583CEB, 0x58C0A7}
	Ref{0x4BEF45, 0x4C2190, 0x4C4B2E, 0x4C4B38, 0x4CAB6A, 0x4CABBC}
	EndRef{0x4BD19F, 0x577A57}
	
	Array{0x4DFB4}.i4  'HeroMayBeHiredBy'
	Ref{0x4868FB, 0x486D4B, 0x4A3CE6, 0x4BB18B, 0x4BB208, 0x4BB30F, 0x4BB4A4, 0x4BD1C7, 0x4BE593, 0x4CDE4F, 0x4CE07C, 0x4D7BCF}
	Ref{0x4BEF5E, 0x4C4B87}
end

--[[
RMGStruct:  F88 DenyHeroes
]]

----------- Monsters manager ------------

local MonInfo = mem.struct(function(define)
	define
	.i4  'TownType'
	.i4  'Level'
	pc  'SoundPrefix'
	pc  'DefName'
	.u4  'Flags'
	pc  'Name'
	pc  'PluralName'
	pc  'Special'  -- e.g. Fearsome
	.struct(ResourcesData)  'Cost'
	.i4  'FightValue'
	.i4  'AIValue'
	.i4  'Growth'
	.i4  'HordeGrowth'
	.i4  'HitPoints'
	.i4  'Speed'
	.i4  'Attack'
	.i4  'Defence'
	.i4  'DamageLow'
	.i4  'DamageHigh'
	.i4  'Shots'
	.i4  'Spells'
	.i4  'AdvLow'
	.i4  'AdvHigh'
	assert(define.size == 0x74)
end)



local SodExe
local FuncsList

-- FindRef

function _G.FR(p, p1, start, stop)
	SodExe = SodExe or io.LoadString[[c:\_WoG\wog359\trunk\Build\heroes3.ex_]]
	FuncsList = FuncsList or dofile(AppPath.."Mods/FuncsList.lua")
	p1 = p1 or p + 1
	local t = {}
	local t1 = {}
	local off = mem.topointer(SodExe) - 0x400000
	
	for nfu, fu in ipairs(FuncsList) do
		local i, j = fu + off, (FuncsList[nfu + 1] or 0x639F00) + off
		while i < j do
			local n = mem.GetInstructionSize(i)
			local _, IsJump = mem.GetHookSize(i)
			if not IsJump then
				local op = u1[i]
				for i = i + 1, i + n - 4 do
					local v = u4[i]
					if v >= p and v < p1 then
						local t = (op ~= 0x68 and t or t1)  -- put "push XXX" in t1
						t[#t+1] = format("0x%X", i - off)
					end
				end
			end
			i = i + n
		end
	end
	return "Ref{".._G.table.concat(t, ", ").."}", "Ref{".._G.table.concat(t1, ", ").."}"
end


-- function _G.FR(p, p1, start, stop)
	-- SodExe = SodExe or io.LoadString[[c:\_WoG\wog359\trunk\Build\heroes3.ex_]]
	-- p1 = p1 or p + 1
	-- local t = {}
	-- local t1 = {}
	-- local off = mem.topointer(SodExe) - 0x400000
	-- -- local i, j = (start or 0x401000) + off, (stop or 0x639F00) + off
	-- -- while i < j do
		-- -- local n = mem.GetInstructionSize(i)
		-- -- local _, IsJump = mem.GetHookSize(i)
		-- -- if not IsJump then
			-- -- local op = u1[i]
			-- -- for i = i + 1, i + n - 4 do
				-- -- local v = u4[i]
				-- -- if v >= p and v < p1 then
					-- -- local t = (op ~= 0x68 and t or t1)  -- put "push XXX" in t1
					-- -- t[#t+1] = format("0x%X", i - off)
				-- -- end
			-- -- end
		-- -- end
		-- -- i = i + n
	-- -- end
	-- local last
	-- for i = (start or 0x401000) + off, (stop or 0x639F00) + off do
		-- local n = mem.GetInstructionSize(i)
		-- local _, IsJump = mem.GetHookSize(i)
		-- if not IsJump then
			-- local op = u1[i]
			-- for i = i + 1, i + n - 4 do
				-- local v = u4[i]
				-- if v >= p and v < p1 then
					-- if i ~= last then
						-- local t = (op ~= 0x68 and t or t1)  -- put "push XXX" in t1
						-- t[#t+1], last = format("0x%X", i), i
					-- end
				-- end
			-- end
		-- end
	-- end	
	-- -- for i = start or 0x401000, stop or 0x63A000 do
		-- -- local v = u4[i]
		-- -- if v >= p and v < p1 then
			-- -- t[#t+1] = format("0x%X", i)
		-- -- end
	-- -- end	
	-- return "Ref{".._G.table.concat(t, ", ").."}", "Ref{".._G.table.concat(t1, ", ").."}"
-- end


--[[
Monsters:

hook global 43DA94

calls to 534740 with new monsters
539000 - add new dwellings and other new objects here

67FF74 CrAnim_Loaded

bitset safe: 4C8F80
]]

--[[
Dwellings:

63D570   DwMonster
677938 DwMonster2
]]

--[[
Heroes:
678420 HerosSpecStr
679C80 HeroSpecStrPo
679DD0 HeroInfo
67DCE8 HeroInfoTablePo
698F48 HeroNames
698B78 HcTraits
]]

--[[
Artifacts:
hook 4C9700 to allow arbitrary names

4E2B4 ArtAllowed  and other with count = 144
]]

--[[
Art Combos
]]

--[[
Specialities
]]

--[[
Defs:
FlagPort.def
artifact.def
artibon.def
avwattak.def
]]

--[[
New war machine: 4C94B0
]]