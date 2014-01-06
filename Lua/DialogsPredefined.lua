
local _G = _G
local internal = debug.getregistry()

local msg = debug.Message
local print = print
local dump = dump
local Message = Message
local type = type
local unpack = unpack
local error = error
local pairs = pairs
local ipairs = ipairs
local tonumber = tonumber
local tostring = tostring
local rawget = rawget
local rawset = rawset
local getmetatable = getmetatable
local setmetatable = setmetatable
local getfenv = getfenv
local setfenv = setfenv
local pcall = pcall
local assert = assert
local format = string.format
local string_byte = string.byte
local string_sub = string.sub
local string_find = string.find
local table_insert = table.insert
local table_hidden = table.hidden
local table_copy = table.copy
local table_destructor = table.destructor
local table_remove = table.remove
local table_ifind = table.ifind
local coroutine_create = coroutine.create
local coroutine_resume = coroutine.resume
local coroutine_status = coroutine.status
local mem_new = mem.new
local call = mem.call
local i4 = mem.i4
local u4 = mem.u4
local i2 = mem.i2
local u2 = mem.u2
local i1 = mem.i1
local u1 = mem.u1
local mem_string = mem.string
local mem_struct = mem.struct
local mem_structs = mem.structs
local mem_copy = mem.copy
local mem_fill = mem.fill
local malloc = mem.malloc
local mem_hook = mem.hook
local mem_hookalloc = mem.hookalloc
local abs = math.abs
local min = math.min
local max = math.max
local floor = math.floor
local ceil = math.ceil
local round = math.round
local bit_ToTable = bit.ToTable
local bit_FromTable = bit.FromTable
local event = internal.event

local protectMetatable = internal.protectMetatable
local setmetatableW = internal.setmetatableW
local roError = internal.roError
local topointer = mem.topointer

local function nullsub()
end

local nullpersist = internal.nullpersist

local P = dialogs

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

----------- No globals from this point ------------

local _NOGLOBALS

--------------------------------------------------------------------------------
--
--  Predefined Dialogs
--
--------------------------------------------------------------------------------

function P.CheckBoxesDialog(t)
	local radio = t.Radio

	-- Load items
	if t.On == nil then
		t.On = {}
		if t.OnBits then
			bit_ToTable(t.OnBits, t.On)
		end
	end
	if t.SelectedItem then
		t.On[t.SelectedItem] = true
	end
	t.SelectedItem = nil
	t.Disabled = t.Disabled or {}
	t.Shaded = t.Shaded or {}

	local tcount = 0  -- total count
	for k,_ in pairs(t.Texts) do
		if type(k) == "number" and k > tcount then
			tcount = k
		end
	end
	local vcount  -- visible count
	local map = {}
	do
		local j = 1
		local texts = t.Texts
		for i = 1, tcount do
			if texts[i] and texts[i] ~= "" then
				if t.On[i] then
					t.SelectedItem = i
				end
				map[j] = i
				j = j + 1
			else
				t.On[i] = false  -- behave the same as IF:G
			end
		end
		vcount = j - 1
	end

	-- Dialog
	local dlg = d_new{
		Owner = t, BorderHint = (t.Hints ~= nil), ShowLongHints = (t.LongHints ~= nil),
		d_AlignV{Margin = 14, MarginTop = 30, MarginBottom = 29 + ((t.Hints ~= nil) and 21 or 0),
			Name = "All", Width = (t.Width ~= 0 and t.Width or 560), SpaceY = 16, AlignX = 0.5,
			
			t.Caption and P.Text{ExpandWidth = 1, Text = t.Caption, Font = "MedFont.fnt", Align = 5, Color = 19} or 8,
			d_AlignH{ExpandWidth = 1, MarginLeft = 7, MarginRight = 7,
				d_CheckGroup{Name = "Group", ExpandWidth = 1,
					BorderHeight = 8, MarginTop = -1, MarginBottom = -1,
					Count = vcount, States = t.States, Radio = radio, Border = true,
					ItemsAutoHeight = true, AutoWidth = false, MinWidth = 0,
				},
			},
			d_AlignH{SpaceX = 16,
				d_AlignLayers{AlignX = 0.5, AlignY = 0.5,
					d_Pcx{File = "box64x30.pcx"},
					d_Button{Name = "OK", File = "iokay.def", CloseDialog = true, Id = 30722, HotKey = 28},
				},
				t.CancelButton and d_AlignLayers{AlignX = 0.5, AlignY = 0.5,
					d_Pcx{File = "box64x30.pcx"},
					d_Button{File = "icancel.def", CloseDialog = true, Id = 30721, HotKey = 1},
				} or nil,
			}
		}
	}
	
	local okButton = dlg.Items.OK
	local group = dlg.Items.Group
	
	if radio and not t.SelectedItem then
		okButton.Shaded = true
		function group:OnClick()
			if okButton.Shaded then
				okButton.Shaded = false
				okButton:Redraw()
			end
		end
	end
	
	-- Fill CheckGroup
	local has_enabled_items
	for i = 1, vcount do
		local k = map[i]
		local b = t.On[k]
		local it = group.Items[i]
		it.On = b
		if b then
			okButton.Shaded = false
		end
		it.Text = t.Texts and t.Texts[k]
		it.Hint = t.Hints and t.Hints[k]
		it.LongHint = t.LongHints and t.LongHints[k]
		if i < 10 then
			it.HotKey = i + 1
		end
		it.Disabled = t.Disabled[k]
		it.Shaded = t.Shaded[k]
		has_enabled_items = has_enabled_items or not it.Disabled and not it.Shaded
	end
	okButton.Shaded = okButton.Shaded and has_enabled_items
	
	-- Scroll
	local all = dlg.Items.All
	all:NeedSize()
	-- print(dump(group[1].Items.Text, 2))
	local maxH = t.MaxHeight
	if not maxH then
		local space = 30
		maxH = internal.ScreenHeight - space - (t.Y and t.Y ~= 0 and abs(t.Y) or space)
	end
	if all.Height > maxH then
		local scroll = P.ScrollBar{ExpandHeight = 1, CatchKeys = true}
		local align = group.Owner
		local dh = all.Height - group.Height
		local maxGH = maxH - dh
		align:AddItem(scroll)
		group.ScrollBar = scroll
		align:Realign()
		
		-- calculate new height
		local h = 0
		for i = 1, vcount do
			local it = group.Items[i]
			if h > 0 and h + it.Height > maxGH then
				break
			end
			h = h + it.Height
		end
		if maxGH*2/3 > h then
			h = maxGH
		end
		group.AutoHeight = false
		group.AutoMinHeight = false
		group.AutoMinWidth = false
		group.MinWidth = 0
		group.MinHeight = 0
		group.Height = h + group.MarginTop + group.MarginBottom
		--align.Height = h
		--group:Realign()
		align:Realign()
		
		-- scroll to selected item
		local y = 0
		if radio then
			for i = 1, vcount do
				local it = group.Items[i]
				if t.On[map[i]] then
					if y > h*2/3 then
						y = y - h/2
						for j = i - 1, 1, -1 do
							y = y - group.Items[i].Height
							if y < 0 then
								i = i - j - 1
								if i >= scroll.TicksCount then
									i = scroll.TicksCount - 1
								end
								scroll.Tick = i
								break
							end
						end
					end
					break
				end
				y = y + it.Height
			end
		end
		group:UpdateScroll()
		all:Realign()
	end
	
	-- Dialog position
	t.Dialog = dlg
	if t.X and t.X ~= 0 then
		dlg.X = t.X >= 0 and t.X or internal.ScreenWidth + t.X - all.Width
	end
	if t.Y and t.Y ~= 0 then
		dlg.Y = t.Y >= 0 and t.Y or internal.ScreenHeight + t.Y - all.Height
	end
	
	-- Show
	function t:Show(...)
		local dlg = self.Dialog
		local it = dlg.Items.Group.Items
		local count = dlg.Items.Group.Count
		local r = dlg:Show(...)
		self.Result = (r == 30722)
		if self.Result then -- ok
			local on = self.On
			local sel = nil
			for i = 1, count do
				local k = map[i]
				on[k] = it[i].On
				if on[k] then
					sel = k
				end
			end
			self.SelectedItem = sel
			self.OnBits = bit_FromTable(on)
		else
			if self.OnBits == nil then
				self.OnBits = bit_FromTable(self.On)
			end
		end
		return self
	end

	if t.SuspendShow then
		return t
	else
		return t:Show()
	end
end

-- function _G.Q(n)
	-- P.CheckBoxesDialog{Width = 300, Texts = {
		-- "2222222222222222222 22222 222222 22222 2",
		-- "1", "2222222222222222222 22222 222222 22222 2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
		-- "1", "2", "3", "4",
	-- }, SelectedItem = n or 20, Radio = true, Caption = "Test", CancelButton = true}
-- end

local dialogs_CheckBoxesDialog = P.CheckBoxesDialog

function internal.CheckBoxesDialog(radio, bits, caption, ...)
	return dialogs_CheckBoxesDialog({Radio = radio, OnBits = bits, Caption = caption, Texts = {...}}).OnBits
end

function internal.RadioDialog(x, y, w, sel, caption, ...)
	local t = {Radio = true, X = x, Y = y, Width = w or 500, SelectedItem = sel >= 0 and sel + 1 or nil, Caption = caption, Texts = {...}}
	return (dialogs_CheckBoxesDialog(t).SelectedItem or 0) - 1
end
