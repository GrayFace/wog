
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

local d_dialogs = dialogs
local d_new = dialogs.new
local d_Text = dialogs.Text
local d_Button = dialogs.Button
local d_Pcx = dialogs.Pcx
local d_ScrollBar = dialogs.ScrollBar
local d_AlignH = dialogs.AlignH
local d_AlignV = dialogs.AlignV
local d_AlignLayers = dialogs.AlignLayers
local d_CheckGroup = dialogs.CheckGroup
local d_CheckBox = dialogs.CheckBox
local d_Frame = dialogs.Frame
local d_MainMenuButton = dialogs.MainMenuButton

local dofile = dofile

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local OptionMods = internal.OptionMods
local OptionDefs = internal.OptionDefs
-- local ErmDefs = {}
-- local Defaults = {}
--local Options = internal.Options

local BlueStyle = nil
local FillColor = BlueStyle and RGB(20, 20, 54) or RGB(102, 72, 54)
local BorderLColor = BlueStyle and RGB(99, 113, 173) or 33574
local BorderSColor = BlueStyle and RGB(16, 32, 74) or 10434
local ItemFont --= "Small.fnt"
local CaptionFont = "MedFont.fnt"
local DlgF = {mod = "wog", cat = 1}

local function ModTagClick(t, cmd)
	local g = t.Owner
	local all = g[g.AllIndex]
	local upd = {[t] = true}
	if t == g.Cur or cmd.X < t.X + 22 then  -- change check state
		upd[t] = true
		local last = t.IsOn
		t.IsOn = not last
		if t == all then  -- 'All' item
			if last then
				for i = 2, g.Count do
					local it = g[i]
					upd[it] = (it.IsOn ~= last[i - 1] or nil)
					it.IsOn = last[i - 1]
				end
			else
				last = {}
				t.IsOn = last
				for i = 2, g.Count do
					local it = g[i]
					last[i - 1] = it.IsOn
					upd[it] = not it.IsOn or nil
					it.IsOn = true
				end
			end
		elseif all and all.IsOn then  -- normal item
			all.IsOn = false
			upd[all] = true
		end
	else  -- select
		if g.Cur and t ~= g.Cur then
			local btn = g.Cur.Items.Button
			btn.Frame = 0
			btn:Redraw()
		end
		t.Items.Button.Frame = 1
		g.Cur = t
	end
	for it in pairs(upd) do
		local btn = it.Items.Button
		btn.File = it.CheckDefs[it.IsOn and 2 or 1]
		btn:Redraw()
	end
	g:OnStateChanged()
end

local function BuildDlg()
	return d_new(table_copy(DlgF, {ExpandWidth = 1, ExpandHeight = 1,
		Border = true,
		BorderHint = true,
		PlayerColor = BlueStyle and 1,
		ShowLongHints = true,
		DropShadow = false,
		BackgroundPcx = BlueStyle and "DiBoxBlu.pcx",

		OnKeyUp = function(t, cmd)
			cmd.CloseDialog = cmd.Key == 1
		end,
		
		d_AlignV{Name = "Outer",
			Width = ScreenWidth, Height = ScreenHeight, Margin = 25, SpaceY = 10, MarginTop = 28, MarginBottom = 45,
			AlignX = 0.5, AlignY = 0.5,

			-- d_Text{Text = "WoGify Options", Font = "BigFont.fnt", Color = 19},
			-- 12,
			d_AlignH{ExpandWidth = 1, ExpandHeight = 1, SpaceX = 6,
				
				d_AlignV{ExpandHeight = 1, SpaceX = 1, Width = 210, SpaceY = 4, AlignX = 0.5,
					d_Text{Name = "ModsCaption", Font = CaptionFont, Color = 19, ExpandWidth = 1,
						Text = "Mods", Hint = "Mods List", LongHint = "MODS LIST\n\nInfo",
					},  -- !!! localize
					d_AlignH{ExpandWidth = 1,
						d_CheckGroup{Name = "ModsGroup", 
							BorderHeight = 8,
							-- Texts = {"In The Wake Of Gods", "Horn Of The Abyss", "Master Of Puppets"},
							Border = true,
							--CloseDialog = true,
							ExpandWidth = 1, MarginTop = -1, MarginBottom = -1,
							-- MinWidth = 0, ItemsAutoHeight = true,
							--ScrollBar = "Scroll",
							Radio = true,
							FillColor = FillColor,
							BorderLColor = BorderLColor,
							BorderSColor = BorderSColor,
							Font = ItemFont,
							-- States = 1,
							-- OnClick = ModTagClick,
							OnClick = function(it)
								local dlg = it.Parent
								dlg.mod = dlg.mods[it.Index]
								dlg.NoRedraw = true
								dlg:updateCats()
								dlg.NoRedraw = false
								dlg:Redraw()
							end
						},
						d_ScrollBar{Name = "ScrollMods", ExpandHeight = 1, Blue = BlueStyle},
					},
					17,
					d_Text{Name = "CatsCaption", Font = CaptionFont, Color = 19, ExpandWidth = 1, Visible = false,
						Text = "Category", Hint = "Categories list", LongHint = "CATEGORIES LIST\n\nInfo",
					},  -- !!! localize
					d_AlignH{ExpandWidth = 1, ExpandHeight = 1,
						d_CheckGroup{Name = "CatsGroup",
							BorderHeight = 8,
							-- Texts = {"All!", "Combat", "Interface", "Stupid", "Multiplayer"},
							Border = true,
							--CloseDialog = true,
							ExpandWidth = 1, MarginTop = -1, MarginBottom = -1,
							ScrollBar = "ScrollCats",
							Radio = true,
							FillColor = FillColor,
							BorderLColor = BorderLColor,
							BorderSColor = BorderSColor,
							Font = ItemFont,
							OnClick = function(it)
								local dlg = it.Parent
								dlg.cat = it.Index
								dlg.NoRedraw = true
								dlg:updateOptions()
								dlg.NoRedraw = false
								dlg:Redraw()
							end
							-- AllIndex = 1,
						},
						d_ScrollBar{Name = "ScrollCats", ExpandHeight = 1, Visible = false, Blue = BlueStyle},
					},
				},
				d_AlignV{Name = "OptionsOuter", ExpandWidth = 1, ExpandHeight = 1, MinWidth = 0,
					d_AlignH{Name = "Options",
						ExpandWidth = 1, ExpandHeight = 1, SpaceX = 3, SpaceY = 0,
						MinWidth = 0,
						ScrollBar = "Scroll",
						FullPageScroll = true,
					},
					d_ScrollBar{Name = "Scroll", CatchKeys = true, ExpandWidth = 1, Visible = false, Blue = BlueStyle},
				},
			},
			d_AlignH{SpaceX = 16,
				d_AlignLayers{AlignX = 0.5, AlignY = 0.5,
					d_Pcx{File = "box64x30.pcx"},
					d_Button{Name = "OK", File = "iokay.def", CloseDialog = true, Id = 30722, HotKey = 28},
				},
				d_AlignLayers{AlignX = 0.5, AlignY = 0.5,
					d_Pcx{File = "box64x30.pcx"},
					d_Button{File = "icancel.def", CloseDialog = true, Id = 30721, HotKey = 1},
				},
			},
		},
	}, true))
end

function DlgF.OnRightClick(dlg, cmd)
	local it = cmd.Item
	local h = it and (it.LongHint or it.Hint)
	if h ~= nil and h ~= "" then
		cmd.DefaultAction = false
		if GetKeyState(0x10) < 0 or GetKeyState(0x11) < 0 or GetKeyState(0x12) < 0 then
			Message(h)
		else
			Message(h, 4)
		end
	end
	if cmd.Item then
		local h = it.LongHint or it.Hint
	end
end

local function UpdateColor(group)
	for i = 1, group.Count do
		local it = group.Items[i]
		it.TextColor = it.On and 1 or 10
	end
end

local function UpdateCheckColor(it)
	it.TextColor = it.On and 1 or 10
end

function DlgF.clickItem(dlg, it, def, valIndex)
	if not it.Active then  -- safeguard
		return
	end
	local v = it.On
	if valIndex then
		v = def.Values[valIndex].Value
		-- UpdateColor(it.Owner)
	-- else
		-- UpdateCheckColor(it)
	end
	-- dlg:Redraw()
	internal.SetupOptions(internal.NewOptionsTable{[def.Name] = v})
end

function DlgF.checkOptionValue(dlg, option, value)
	return internal.CurOptions.Active[option] == value
end

function DlgF.updateAll(dlg)
	local group = dlg.Items.ModsGroup
	local scroll = dlg.Items.ScrollMods
	local texts = {}
	local hints = {}
	local lhints = {}
	local mods = {}
	local i = 1
	local sel = 1
	for mod, defs in sortpairs(OptionMods) do
		local opt = OptionDefs[mod..".Enabled"]
		texts[i] = opt.Text
		hints[i] = opt.Hint or texts[i]
		lhints[i] = opt.LongHint
		mods[i] = mod
		if mod == dlg.mod then
			sel = i
		end
		i = i + 1
	end
	local h = scroll.Height + dlg.Items.ScrollCats.Height
	group.Texts = texts
	group.Hints = hints
	group.LongHints = lhints
	if texts[1] then
		group.Items[sel].On = true
		dlg.mod = mods[sel]
	end
	dlg.mods = mods
	local need = group.Height > 1/2*h
	if need then
		group.AutoHeight = false
		local h = (scroll.Height + dlg.Items.ScrollCats)*2/5
		local it = group[1]
		group.Height = h - h % (it.Items.Text:GetHeight(0x7FFF) + it.BorderHeight)
		-- !!! scroll to selected item
	end
	scroll.Visible = need
	scroll.Active = need
	group.Owner:Realign()
	group.Owner.Owner:Realign()
	dlg:updateCats()
end

function DlgF.updateCats(dlg)
	UpdateColor(dlg.Items.ModsGroup)
	dlg.cat = 1
	local CatList = OptionMods[dlg.mod]
	if not CatList then
		return
	end
	local group = dlg.Items.CatsGroup
	local scroll = dlg.Items.ScrollCats
	local texts = {}
	local hints = {}
	local lhints = {}
	for i, cat in ipairs(CatList) do
		local cat = cat[0] or {}
		texts[i] = cat.Text
		hints[i] = cat.Hint or texts[i]
		lhints[i] = cat.LongHint
	end
	group.Count = 0
	group.AutoHeight = true
	group.Texts = texts
	group.Hints = hints
	group.LongHints = lhints
	if not texts[2] then
		group.Count = 0
	else
		group.Items[1].On = true
	end
	local need = group.Height > scroll.Height
	if need then
		group.AutoHeight = false
		group.Height = scroll.Height
	end
	scroll.Visible = need
	scroll.Active = need
	dlg.Items.CatsCaption.Visible = group.Count > 0
	dlg:updateOptions()
end

function DlgF.updateOptions(dlg)
	UpdateColor(dlg.Items.CatsGroup)
	local options = OptionMods[dlg.mod][dlg.cat]
	local BaseCaption = OptionDefs[dlg.mod..".Enabled"].Text

	local Aligner = dlg.Items.Options
	for i = #Aligner, 1, -1 do
		Aligner[i]:Delete()
	end

	local FullItemWidth = 265
	local GroupMargin = 2
	local ItemBorderHeight = 6
	local ItemWidth = FullItemWidth - GroupMargin*2
	local ItemHeight
	local ColWidth = FullItemWidth + Aligner.SpaceX
	local CaptionSpace1 = 10 + GroupMargin
	local CaptionSpace2 = 5 + GroupMargin
	local CaptionHeight
	do
		local title = d_Text{Font = CaptionFont}
		title:NeedSize()
		CaptionHeight = title.Height + CaptionSpace2
		local item = d_CheckBox{BorderHeight = ItemBorderHeight}
		item:NeedSize()
		ItemHeight = item.Height
	end
	local MinGroupOneLineCount = 3
	local MinGroupCount = 2
	local MinGroupHeight = ItemHeight*2.5
	local ColHeight = Aligner.Height - GroupMargin
	local MinColHeight = floor(Aligner.Height/2) + CaptionSpace1

	local function GetColCount(list, colH, y)
		local n = 1
		for i, it in ipairs(list) do
			y = y + it.H
			if y > colH then
				y, n = 0, n + 1
			end
		end
		return n
	end

	local function PlaceItem(x, y, it)
		it.X, it.Y = x + Aligner.X, y + Aligner.Y
		it.StaticAlign = true
		Aligner:AddItem(it)
	end

	local function PlaceGroup(x, y, group)
		for i, it in ipairs(group) do
			PlaceItem(it.X + x, it.Y + y, it)
		end
	end

	local function PlaceBorder(x, y)
		y = y - GroupMargin
		local t = {
			d_Frame{Width = FullItemWidth, Color = FillColor, Fill = true},
			d_Frame{Width = FullItemWidth - 1, Color = BorderSColor},
			d_Frame{Width = FullItemWidth - 1, Color = BorderLColor},
			y
		}
		PlaceItem(x, y, t[1])
		PlaceItem(x + 1, y + 1, t[2])
		PlaceItem(x, y, t[3])
		return t
	end

	local function SetBorderEnd(t, y)
		if t then
			local h = y + GroupMargin - t[4]
			t[1].Height = h
			t[2].Height = h - 1
			t[3].Height = h - 1
		end
	end

	local function PlaceItems(x, y, titleBase, list)
		local title = d_Text(table_copy(titleBase))
		title:NeedSize()
		local titleH = title.Height + CaptionSpace2
		local colH = ColHeight - titleH
		if y >= MinColHeight and GetColCount(list, colH, y) > GetColCount(list, colH, 0) then
			x, y = x + ColWidth, 0
		end
		local border
		local NeedTitle = true
		for i, it in ipairs(list) do
			if y + it.H > colH then
				SetBorderEnd(border, y + titleH)
				x, y = x + ColWidth, 0
				NeedTitle = true
			end
			if NeedTitle then
				PlaceItem(x, y, title or d_Text(table_copy(titleBase)))
				title, NeedTitle = nil, false
				border = PlaceBorder(x, y + titleH)
			end
			PlaceGroup(x + GroupMargin, y + titleH, it)
			y = y + it.H
		end
		SetBorderEnd(border, y + titleH)
		return x, y + CaptionSpace1 + titleH
	end

	local function AddItem(g, it)
		it.Y = g.H + it.Y
		g.H = it.Y + it.Height
		g[#g + 1] = it
	end

	local x, y = 0, 0
	for ia, a in ipairs(options or {}) do
		local list = {}
		local group
		local NeedHeight, NeedCount = MinGroupHeight, MinGroupCount
		for _, b in ipairs(a) do
			local item
			if b.Text or not b.Values then
				item = d_CheckBox{X = 0, Y = 0, Width = ItemWidth, BorderHeight = ItemBorderHeight,
					Text = b.Text, Hint = b.Hint or b.Text, LongHint = b.LongHint, Font = ItemFont,
					On = not dlg:checkOptionValue(b.Name, false),
					OnClick = function(it, cmd)
						return dlg:clickItem(it, b)
					end,
				}
				-- UpdateCheckColor(item)
				item:NeedSize()
			end
			if group and (NeedHeight > 0 or NeedCount > 0) then
				NeedCount = NeedCount - 1
				if item then
					NeedHeight = NeedHeight - item.Height
				end
			else
				group = {H = 0}
				list[#list + 1] = group
			end
			if item then
				AddItem(group, item)
			end
			if b.Values then
				NeedHeight, NeedCount = 0, 0
				local texts, hints, lhints = {}, {}, {}
				for i, c in ipairs(b.Values) do
					texts[i] = c.Text
					hints[i] = c.Hint or c.Text
					lhints[i] = c.LongHint
				end
				local cg = d_CheckGroup{BorderHeight = ItemBorderHeight, Radio = true,
					Texts = texts, Hints = hints, LongHints = lhints, Width = ItemWidth, Font = ItemFont,
					OnClick = function(it, cmd)
						return dlg:clickItem(it, b, it.Index)
					end,
				}
				for i = 1, #texts do
					cg.Items[i].On = dlg:checkOptionValue(b.Name, b.Values[i].Value)
				end
				-- UpdateColor(cg)
				cg:NeedSize()
				AddItem(group, cg)
			end
		end
		if #list > 0 then
			local a0 = a[0] or {}
			x, y = PlaceItems(x, y, {Text = a0.Text or BaseCaption, Hint = a0.Hint, LongHint = a0.LongHint,
				Font = CaptionFont, Color = 19, Width = FullItemWidth}, list)
		end
	end

	local PageW = Aligner.Width - (Aligner.Width + Aligner.SpaceX) % ColWidth
	local n = x / (PageW + Aligner.SpaceX) --ColWidth
	for i = 0, n do
		Aligner:AddItem(d_AlignV{Width = PageW})--FullItemWidth})
	end

	local scroll = dlg.Items.Scroll
	scroll.Tick = 0
	Aligner:Realign()
	scroll.Active = scroll.TicksCount > 1
	scroll.Visible = scroll.TicksCount > 1
end

function DlgF.OnShow(dlg)
	if internal.context ~= "map" then
		dlg:Redraw()
		Message("{Some Info.}\nE.g. Ctrl + Right Click opens permanent info window.\nAlt + Right Click and Shift + Right Click do the same.", 4)
	end
end

local function d_WogOptions(readonly)
	local dlg = BuildDlg()
	dlg.Items.Outer:NeedSize()
	if readonly then
		dlg.Items.Options.Active = false
		dlg.IgnoreFlags = true
	end
	dlg:updateAll()
	local r = dlg:Show() == 30722
	return r
end
d_dialogs.WogOptions = d_WogOptions

local function BtnClick(it, cmd)
	if internal.context == "map" then
		internal.CopyFromWogOptions()
		return d_WogOptions(true)
	end
	local last = internal.CurOptions
	internal.CurOptions = internal.CopyOptionsTable(internal.BaseOptions)
	if d_WogOptions() then
		internal.BaseOptions = internal.CopyOptionsTable(internal.CurOptions)
		internal.CopyToWogOptions()
		internal.SaveOptions()
		internal.event("OptionsChanged")
	else
		internal.CurOptions = last
	end
end

function internal.events.ShowDialog(t)
	local info = t.ClassPtr == 0x641720
	if info or t.ClassPtr == 0x641CBC then
		t:Add(d_MainMenuButton{X = (info and 619 or 622), Y = (info and 99 or 105), File = "ZWogOpt.def", OnClick = BtnClick})
	end
end


-- function _G.Q(...)
	-- dofile[[c:\_WoG\wog359\trunk\Lua\options dialog.lua]]
	-- return d_WogOptions(...)
-- end




--[[

mem.hookfunction(0x4B4F00, 1, 5, function(d, def, this, c, buf, x, y, color)
	if x >= 0 and x <= ScreenWidth and y >= 0 and y <= ScreenHeight then
		return def(this, c, buf, x, y, color)
	end
end)

]]
