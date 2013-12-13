
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

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local CommanderId = 150
local GodBonId = 151
local GodBlessId = 152

local function UpdateBlesses(dlg, h)
	local g, c = internal.GetHeroGod(h), internal.HeroHasBlessCurse(h)
	local spec = not (g or c)
	local it = dlg.Items[107]
	it.Visible = spec
	it.Active = spec
	local it = dlg.Items[139]
	it.Visible = spec
	it.Active = spec
	local it = dlg.Items[GodBonId]
	it.Visible = not not g
	it.Active = not not g
	it.X = c and 67 or 88
	it.Frame = g or 6
	local it = dlg.Items[GodBlessId]
	it.Visible = c
	it.Active = c
	it.X = g and 110 or 88
end

mem_autohook(0x4E2476, function(d)
	local dlg = d_ByHandle(d.ebx)
	if not dlg.Items[GodBonId] then
		dlg.Items.HintControl = dlg.Items[115]
		if i4[WogOptionsPtr + 3*4] == 0 then
			dlg:Add(d_Pcx{Id = CommanderId, File = "ZCmndBtn.pcx", X = 330, Y = 17, Hint = TEXT.CommanderBtnHint})
		end
		dlg:Add(d_Def{Id = GodBonId, File = "ZGodBon.DEF", Y = 180, Hint = TEXT.GodBonusBtnHint})
		dlg:Add(d_Def{Id = GodBlessId, File = "ZGodBon.DEF", Y = 180, Frame = 5, Hint = TEXT.BlessCurseBtnHint})
	end
	local h = u4[0x698B70]
	
	UpdateBlesses(dlg, h)
	internal.event("HeroScreenUpdate", dlg)
end)

