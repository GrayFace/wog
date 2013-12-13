
local _G = _G
local internal = debug.getregistry()

local msg = debug.Message
local floor = math.floor
local RGB = color.RGB
local print = print
local Message = Message
local type = type
local unpack = unpack
local error = error
local pairs = pairs
local sortpairs = sortpairs
local next = next
local ipairs = ipairs
local tonumber = tonumber
local tostring = tostring
local tostring2 = tostring2
local rawget = rawget
local rawset = rawset
local getmetatable = getmetatable
local setmetatable = setmetatable
local setmetatableW = internal.setmetatableW
local getfenv = getfenv
local setfenv = setfenv
local pcall = pcall
local assert = assert
local format = string.format
local string_lower = string.lower
local string_find = string.find
local table_copy = table.copy
local table_concat = table.concat
local ScreenWidth = ScreenWidth
local ScreenHeight = ScreenHeight
local GetKeyState = internal.GetKeyState

local d_ByHandle = dialogs.ByHandle
local d_Text = dialogs.Text
local d_Def = dialogs.Def
local d_Button = dialogs.Button
local d_Pcx = dialogs.Pcx
local d_ScrollBar = dialogs.ScrollBar
local d_AlignH = dialogs.AlignH
local d_AlignV = dialogs.AlignV
local d_AlignLayers = dialogs.AlignLayers
local d_CheckGroup = dialogs.CheckGroup
local d_CheckBox = dialogs.CheckBox
local d_Frame = dialogs.Frame

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
local mem_hookjmp = mem.hookjmp
local mem_autohook = mem.autohook
local mem_autohook2 = mem.autohook2
local getdefine = mem.structs.getdefine
local structs_enum = structs.enum

local WogOptionsPtr = internal.WogOptionsPtr
local TEXT = internal.TEXT
local CO_E = ERM.CO(-1).E
local ?v = ?v

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local CommanderId = 150
local GodBonId = 151
local GodBlessId = 152

local function SetVisible(dlg, id, v, x)
	local it = dlg.Items[id]
	it.Visible = v
	it.X = (x or it.X % 20000) + (v and 0 or 20000)
	return it
end

local function UpdateCommander(dlg, h)
	SetVisible(dlg, CommanderId, (CO_E(?v) >= 0))
end

local function UpdateBlesses(dlg, h)
	local g, c = internal.GetHeroGod(h), internal.HeroHasBlessCurse(h)
	local spec = not (g or c)
	SetVisible(dlg, 107, spec)
	SetVisible(dlg, 139, spec)
	SetVisible(dlg, GodBonId, not not g, (c and 67 or 88)).Frame = g or 6
	SetVisible(dlg, GodBlessId, c, (g and 111 or 88))
end

mem_autohook(0x4E2476, function(d)
	local dlg = d_ByHandle(d.ebx)
	if not dlg.Items[GodBonId] then
		dlg.Items.HintControl = dlg.Items[115]
		dlg:Add(d_Pcx{Id = CommanderId, File = "ZCmndBtn.pcx", X = 330, Y = 17, Hint = TEXT.CommanderBtnHint})
		dlg:Add(d_Def{Id = GodBonId, File = "ZGodBon.DEF", Y = 180, Hint = TEXT.GodBonusBtnHint})
		dlg:Add(d_Def{Id = GodBlessId, File = "ZGodBon.DEF", Y = 180, Frame = 5, Hint = TEXT.BlessCurseBtnHint})
	end
	local h = u4[0x698B70]
	u4[internal.ERM_HeroStr] = h
	UpdateCommander(dlg, h)
	UpdateBlesses(dlg, h)
	internal.event("HeroScreenUpdate", dlg)
end)

