
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
local item_Delete = internal.DlgItemDelete
local callVMT = internal.CallDialogVMT
local dialogs_ScrollBar = P.ScrollBar

----------- No globals from this point ------------

local _NOGLOBALS

--------------------------------------------------------------------------------

local function GetPrivate(t)
	return table_hidden(t)
end

local function item_SetBounds(t, x, y, w, h)
	if x then  t.X = x  end
	if y then  t.Y = y  end
	if w then  t.Width = w  end
	if h then  t.Height = h  end
end

local function BoolToInt(v)
	if v == true then
		return 1
	else
		return v or 0
	end
end

local function ControlOrName(t, c)
	if type(c) == "string" then
		return t.Parent and t.Parent.Items[c]
	end
	return c
end

--------------------------------------------------------------------------------
--
--  Composite Items
--
--------------------------------------------------------------------------------

local set

-- Object Model: Object -index- Fields Table -index- Class
-- Class Private contains setters

local function Composite_DoAddItem(t, it, name)
	local items = t.Items
	name = name or it.Name
	if name and items[name] then
		error('Item named "'..name..'" already exists', 3)
	elseif name then
		items[name] = it
	end
	items[#items + 1] = it
	it.Owner = t
	if t.Parent and not it.Parent then
		t.Parent:Add(it)
	end
end

local function Composite_AddItem(t, it, name)  -- for proper error level
	Composite_DoAddItem(t, it, name)
end

local function Composite_RemoveItem(t, it)
	local items = t.Items
	for i = #items, 1, -1 do
		if items[i] == it then
			table_remove(items, i)
		end
	end
	for k, v in pairs(items) do
		if v == it then
			items[k] = nil
		end
	end
	if it.Owner == t then
		it.Owner = nil
	end
end

local function Composite_Attach(self, d)
	local t = self.Items
	for i = 1, #t do
		d:Add(t[i])
	end
end

local function Composite_Delete(self)
	local t = self.Items
	self.Items = {}
	for i = 1, #t do
		t[i]:Delete()
	end
	item_Delete(self)
end

local function Composite_Redraw(self)
	local d = self.Parent
	if not d or d.NoRedraw then
		return
	end
	call(0x4EDB20, 0)
	local need = false
	local t = self.Items
	for i = 1, #t do
		local v = t[i]
		if v.?ptr then
			if v.Visible then
				v:SendCmd(2, 0)
				need = true
			end
		else
			local f = v.Redraw
			if f then
				f(v)
			end
		end
	end
	if need --[[and d]] then
		call(0x603190, 1, u4[0x6992D0], d.X + self.X, d.Y + self.Y, self.Width, self.Height)
	end
end

local function Composite_RedrawDlg(self)
	local d = self.Parent
	local f = d and d.Redraw
	if f then
		f(d)
	end
end

local function Composite_SetX(self, v, old)
	if old and v then
		v = v - old
		local t = self.Items
		for i = 1, #t do
			local it = t[i]
			it.X = it.X + v
		end
	end
end

local function Composite_SetY(self, v, old)
	if old and v then
		v = v - old
		local t = self.Items
		for i = 1, #t do
			local it = t[i]
			it.Y = it.Y + v
		end
	end
end

local function Composite_SetCommon(self, v, old, a)
	local t = self.Items
	for i = 1, #t do
		t[i][a] = v
	end
end

local function Composite_InitClass(class, name)
	class.ControlKind = name
	local priv = GetPrivate(class)
	priv.setters = set
	local setters = set
	function priv.newindex(t, a, v)
		local setter = setters[a] or setters._
		if setter then
			local fields = t.RawFields
			local old = fields[a]
			fields[a] = v
			setter(t, v, old, a)
		else
			rawset(t, a, v)
		end
	end
end

local function Composite_NewClass(name, inherit)
	set = table_copy(inherit and GetPrivate(inherit).setters or {
		X = Composite_SetX,
		Y = Composite_SetY,
		Active = Composite_SetCommon,
		Visible = Composite_SetCommon,
		Disabled = Composite_SetCommon,
		Shaded = Composite_SetCommon,
	})
	local class = table_copy(inherit or {
		Attach = Composite_Attach,
		Delete = Composite_Delete,
		AddItem = Composite_AddItem,
		RemoveItem = Composite_RemoveItem,
		Redraw = Composite_RedrawDlg,
		NeedSize = nullsub,
	})
	Composite_InitClass(class, name)
	return class, set
end

local function Composite_InitObject(t, fields, class)
	setmetatable(fields, {__index = class, __persist = nullpersist})
	t.RawFields = fields
	fields.Items = nil
	t.Items = t.Items or {}
	local priv = GetPrivate(class)
	setmetatable(t, {__index = fields, __newindex = priv.newindex, __persist = nullpersist})
	local setters = priv.setters
	local defset = setters._
	for k,v in pairs(fields) do
		local setter = setters[k] or defset
		if setter then
			setter(t, v, nil, k)
		else
			t[k] = v
			fields[k] = nil
		end
	end
	return t
end

P.CompositeControls = {
	NewClass = Composite_NewClass,
	InitObject = Composite_InitObject,
	RedrawSolid = Composite_Redraw,
	RedrawDlg = Composite_RedrawDlg,
}

--------------------------------------------------------------------------------

local Aligner, AlignerSet = Composite_NewClass("Align")

local function AlignerInitScroll(t, area, LineH)
	local sb = ControlOrName(t, t.ScrollBar)
	if not sb then  return  end
	
	local Y, H
	if not t.Multiline == not t.Horizontal then
		Y, H = "Y", "Height"
	else
		Y, H = "X", "Width"
	end
	
	-- list of line coordinates
	local list, listH = {}, {}
	if t.Multiline then
		local y, space = 0, t["Space"..Y] or 0
		for i = 1, #LineH do
			if LineH[i] > 0 then
				list[#list + 1] = y
				listH[#listH + 1] = LineH[i]
			end
			y = y + LineH[i] + space
		end
	else
		local top = area[Y]
		for _, v in ipairs(t) do
			if type(v) ~= "number" and not v.StaticAlign and v[H] > 0 then
				list[#list + 1] = v[Y] - top
				listH[#listH + 1] = v[H]
			end
		end
	end
	list[1] = list[1] or 0
	listH[1] = listH[1] or 0

	local BaseTop, height = area[Y] - t[Y], area[H]
	
	-- TicksCount
	local lastY = list[#list] + listH[#listH] - height
	local count = 1
	for i = #list, 1, -1 do
		if list[i] < lastY then
			count = i + 1
			break
		end
	end
	sb.TicksCount = count
	
	-- Scroll functions
	sb.Tick = max(0, min(count, sb.Tick or 0))
	local showPart = t.ShowPartialRow
	local curY = 0
	
	local function GetStopY(tick)
		local stopY = height
		if not showPart then
			local curY = list[tick]
			for i = #list, tick, -1 do
				stopY = list[i] + listH[i] - curY
				if stopY <= height then
					break
				end
			end
		end
		return stopY
	end
	
	local function CalcBigStep(tick, stopY)
		local step = 1
		local startY = curY + (showPart and 1 or listH[tick]) - height
		stopY = curY + stopY
		for i = 1, #list do
			if tick - i > 0 and list[tick - i] < startY then
				break
			elseif list[tick + i] and list[tick + i] >= stopY then
				break
			end
			if t.FullPageScroll then
				step = i + 1
			else
				step = i
			end
		end
		sb.BigStepSize = max(step, 1)
	end
	
	local function DoScroll(tick)
		local top = BaseTop + t[Y]
		tick = tick + 1
		local dy, stopY = list[tick] - curY, GetStopY(tick)
		curY = list[tick]
		for _, v in ipairs(t) do
			if type(v) ~= "number" then
				v[Y] = v[Y] - dy
				local y = v[Y] - top
				local vis = (y >= 0 and y < stopY)
				v.Visible = vis and t.RawFields.Visible ~= false
				v.Active = vis and t.RawFields.Active ~= false
			end
		end
		CalcBigStep(tick, stopY)
	end
	DoScroll(sb.Tick)
	
	function sb:OnScroll(tick)
		DoScroll(tick)
		if t.OnScroll then
			t:OnScroll(tick)
		end
		local d = t.Parent
		if d and d.Redraw then
			d:Redraw()
		end
	end
	
	function t.UpdateScroll()
		DoScroll(sb.Tick)
	end
end


local function GetXYWH(t)
	if t.Horizontal then
		return "X", "Y", "Width", "Height"
	else
		return "Y", "X", "Height", "Width"
	end
end

local function ApplyMargins(a, b, t, mul)
	local bl = t.MarginLeft or t.Margin or 0
	local br = t.MarginRight or t.Margin or 0
	local bt = t.MarginTop or t.Margin or 0
	local bb = t.MarginBottom or t.Margin or 0
	a.X = b.X + bl*mul
	a.Y = b.Y + bt*mul
	a.Width = b.Width - (bl + br)*mul
	a.Height = b.Height - (bt + bb)*mul
	return a
end

function Aligner:Realign()
	local fields = self.RawFields
	local X, Y, W, H = GetXYWH(self)
	self.AutoWidth = self.AutoWidth or self.AutoWidth == nil and not self.Width and not self.ExpandWidth
	self.AutoHeight = self.AutoHeight or self.AutoHeight == nil and not self.Height and not self.ExpandHeight
	self.AutoMinWidth = self.AutoMinWidth or self.MinWidth == nil
	self.AutoMinHeight = self.AutoMinHeight or self.MinHeight == nil
	local mult = self.Multiline
	local needW = self["Auto"..W] or not self[W]
	local needH = self["Auto"..H] or not self[H]
	fields.Width = fields.Width or 0
	fields.Height = fields.Height or 0
	local t = ApplyMargins({}, fields, self, 1)
	local noW = self[W] == 0 and self["Expand"..W] and mult
	local noH = self[H] == 0 and self["Expand"..H] and not mult
	local layers = self.AlignLayers
	local SpaceX = self["Space"..X] or 0
	local SpaceY = self["Space"..Y] or 0
	local AlignX = self["Align"..X] or 0  -- 0 to 1, 0.5 = center
	local AlignY = self["Align"..Y] or 0
	local MinW = self["Min"..W] and not self["AutoMin"..W] and (self["Min"..W] + t[W] - self[W]) or 0
	local MinH = self["Min"..H] and not self["AutoMin"..H] and (self["Min"..H] + t[H] - self[H]) or 0
	local wlim = not self["Auto"..W] and t[W]
	for AlignRepeat = 1, 2 do
		local NeedRealign
		local lineW, lineH, lineExpands = {}, {}, {}
		local SetPos
		local w, h = 0, layers and (not self["Auto"..H] and not self["Expand"..H] and t[H] or MinH) or -1/0
		for _ = 1, 2 do
			local x, y = 0, 0
			local needSpace = false
			local ln = 1
			local lnSpace, lnExpands, lnStarted

			local function InitLine()
				lnStarted = false
				if SetPos and not layers then
					h = lineH[ln] or 0
					lnSpace = t[W] - lineW[ln]
					lnExpands = lineExpands[ln]
					if not lnExpands then
						x = round(lnSpace*AlignX)
					elseif lnSpace < 0 then
						lnSpace = 0
					end
				end
			end
			
			InitLine()
			for i, v in ipairs(self) do
				if type(v) == "number" then
					x = x + v
					needSpace = false
					lnStarted = lnStarted or v > 0
				elseif not v.StaticAlign then
					if needSpace then
						x = x + SpaceX
					end
					if not SetPos then
						v:NeedSize()
					end
					local vw = not v["Expand"..W] and v[W] or v["Min"..W] or 0
					if not layers and mult and wlim and lnStarted and x + vw > wlim then
						x, y, h, ln = 0, y + h + SpaceY, -1/0, ln + 1
						InitLine()
					end
					if SetPos then
						local vh = v[H]
						if layers then
							lnSpace = t[W] - vw - x
							if v["Expand"..W] then
								vw = vw + lnSpace
								v[W] = vw
							else
								x = lnSpace*AlignX
							end
						elseif v["Expand"..W] then
							vw = round(lnSpace*v["Expand"..W]/lnExpands)
							lnSpace = lnSpace - vw
							lnExpands = lnExpands - v["Expand"..W]
							vw = vw + (v["Min"..W] or 0)
							v[W] = vw
						end
						if v["Expand"..H] then
							vh = h
							v[H] = h
						end
						if AlignRepeat == 1 then
							v:NeedSize()
							NeedRealign = NeedRealign or v[W] ~= vw or v[H] ~= vh
						end
						v[X] = t[X] + x
						v[Y] = t[Y] + y + round((h - vh)*AlignY)
					end
					x = x + vw
					if not SetPos then
						w = max(x, w)
						h = max(h, not v["Expand"..H] and v[H] or v["Min"..H] or 0)
						if v["Expand"..W] then
							lineExpands[ln] = (lineExpands[ln] or 0) + v["Expand"..W]
						end
						lineW[ln] = w
						lineH[ln] = h
					end
					if layers then
						x = 0
					end
					needSpace = true
					lnStarted = lnStarted or vw > 0
				end
			end
			if not mult and not SetPos then
				lineH[ln] = max(h, (self["Auto"..H] and MinH or t[H]) - y)
			end
			lineW[1] = max(0, lineW[1] or 0)
			
			if not SetPos then
				if not noH and (not self["Min"..W] or self["AutoMin"..W]) then
					self["Min"..W] = w + self[W] - t[W]
				end
				if not noH and needW then
					t[W] = max(w, MinW)
				end
				if not noW and (not self["Min"..H] or self["AutoMin"..H]) then
					self["Min"..H] = max(h, 0) + self[H] - t[H]
				end
				if not noW and needH then
					t[H] = max(y + max(h, 0), MinH)
				end
				ApplyMargins(fields, t, self, -1)
			end
			SetPos = true
		end
		
		if not noW and not noH then
			self.NeedSize = nullsub
		end
		if not NeedRealign then
			return AlignerInitScroll(self, t, lineH)
		end
	end
end

function set:Width(v, old)
	if v ~= old and rawget(self, "NeedSize") then
		self:Realign()
	end
end
set.Height = set.Width

function set:Visible(v, old, a)
	Composite_SetCommon(self, v, old, a)
	self:UpdateScroll()
end
set.Active = set.Visible

function Aligner:AddItem(it, name)
	self[#self + 1] = it
	if type(it) ~= "number" then
		Composite_DoAddItem(self, it, name)
	end
end
function Aligner:RemoveItem(it)
	for i = #self, 1, -1 do
		if self[i] == it then
			table_remove(self, i)
		end
	end
	if ControlOrName(self.ScrollBar) == it then
		self.ScrollBar = nil
	end
	Composite_RemoveItem(self, it)
end

Aligner.UpdateScroll = nullsub
Aligner.NeedSize = Aligner.Realign

function Aligner:Attach(d)
	Composite_Attach(self, d)
	if type(self.ScrollBar) == "string" and rawget(self, "NeedSize") then
		self:Realign()
	end
end


local function dialogs_Aligner(fields)
	item_SetBounds(fields, fields.X or 0, fields.Y or 0)
	
	local t = {Items = {}}
	for _, v in ipairs(fields) do
		if type(v) ~= "number" then
			Composite_DoAddItem(t, v)
		end
	end
	if fields.ScrollBar and type(fields.ScrollBar) ~= "string" and not fields.ScrollBar.Owner then
		Composite_DoAddItem(t, fields.ScrollBar)
	end
	
	Composite_InitObject(t, fields, Aligner)
	return t
end

function P.AlignH(t)
	setmetatable(t, nil)
	t.Horizontal = true
	return dialogs_Aligner(t)
end

function P.AlignV(t)
	setmetatable(t, nil)
	t.Horizontal = false
	return dialogs_Aligner(t)
end

function P.AlignLayers(t)
	setmetatable(t, nil)
	t.AlignLayers = true
	return dialogs_Aligner(t)
end

--------------------------------------------------------------------------------

local CheckBox = Composite_NewClass("CheckBox")

local function CheckBox_SetBounds(t)
	local it = t.Items
	local x, y, w, h = t.X, t.Y, t.Width, t.Height
	if not w or not h then
		return
	end
	local text, btn = it.Text, it.Button
	text:SetBounds(x + 23, y, w - 25, h)
	btn.DrawY = floor((h - (text.?ptr and btn:GetHeight() or 16))/2)
	btn:SetBounds(x, y, w, h)
	local bh = text.?ptr and text:GetHeight(text.Width) + 4 or 20
	local by = y + floor((h - bh)/2)
	it.Fill:SetBounds(x + 1, by, w - 2, bh)
	it.BorderS:SetBounds(x + 1, by, w - 1, bh + 1)
	it.BorderL:SetBounds(x, by - 1, w - 1, bh + 1)
end

set.Width = CheckBox_SetBounds
set.Height = CheckBox_SetBounds

function set:Text(v, old, a)
	self.Items.Text[a] = v
end
set.AutoHeight = set.Text
set.AutoWidth = set.Text
set.Font = set.Text
set.MinHeight = set.Text
function set:MinWidth(v)
	self.Items.Text.MinWidth = v - 30
end

function set:Hint(v, old, a)
	self.Items.Button[a] = v
end
set.LongHint = set.Hint
set.HotKey = set.Hint
set.CloseDialog = set.Hint
set.Id = set.Hint

function CheckBox:Attach(d)
	if self.Border then
		Composite_Attach(self, d)
	else
		d:Add(self.Items.Text)
		d:Add(self.Items.Button)
	end
end

function CheckBox:Redraw()
	local d = self.Parent
	local f = d and d.Redraw
	if f then
		if self.Items.Fill.Visible then
			f(d)
		else
			Composite_Redraw(self)
		end
	end
end

function set:Group(v, old)
	if old then
		old.Checks[self] = nil
	end
	if v then
		v.Checks[self] = true
	end
end

function CheckBox:Delete()
	if self.Group then
		self.Group.Checks[self] = nil
		self.Group = nil
	end
	Composite_Delete(self)
end

function CheckBox:RedrawButton()
	local t = self.Items.Button
	if t.?ptr and t.Redraw then
		t:Redraw()
	end
end

function set:On(v, old)
	if v ~= old then
		if self.Radio and v then
			for t,_ in pairs(ControlOrName(self, self.Group).Checks) do
				if t ~= self and t.Radio then
					t.On = false
				end
			end
		end
		if self.Radio then
			self.Items.Button.File = self.RadioDefs[BoolToInt(v) + 1]
		else
			self.Items.Button.File = self.CheckDefs[BoolToInt(v) + 1]
		end
		self:RedrawButton()
		self:OnChange()
	end
end

function set:TextColor(v)
	self.Items.Text.Color = v
end

function set:FillColor(v)
	self.Items.Fill.Color = v
end

function set:FillVisible(v)
	self.Items.Fill.Visible = v
end

function set:BorderLColor(v)
	self.Items.BorderL.Color = v
end

function set:BorderSColor(v)
	self.Items.BorderS.Color = v
end

function set:Visible(v, old, a)
	Composite_SetCommon(self, v, old, a)
	if self.FillVisible == false then
		self.Items.Fill.Visible = false
	end
end


CheckBox.OnChange = nullsub
CheckBox.OnClick = nullsub

local function CheckBox_OnClick(t, cmd)
	local self = t.Owner
	if self.Radio then
		self.On = true
	else
		if self.States then
			local i = BoolToInt(self.On) + 1
			self.On = (i < self.States) and i
		else
			self.On = not self.On
		end
	end
	self:OnClick(cmd)
end

function CheckBox.NeedSize(t)
	local text = t.Items.Text
	repeat
		local w, h = t.Width, t.Height
		-- if w then
			-- text.Width = w - 30
		-- end
		text:NeedSize()
		t.AutoWidth = t.AutoWidth or t.AutoWidth == nil and not w and not t.ExpandWidth
		t.AutoHeight = t.AutoHeight or t.AutoHeight == nil and not h and not t.ExpandHeight
		t.MinWidth = t.MinWidth or text.Width + 30
		local first = not w or not h
		if not w or t.AutoWidth then
			w = max(text.Width + 30, t.MinWidth or 0)
		end
		t.MinHeight = t.MinHeight or text.MinHeight
		if not h or t.AutoHeight then
			h = max(text.Height + (t.BorderHeight or t.Border and 6 or 4), t.MinHeight or 0)
		end
		t.Width, t.Height = w, h
	until not first
end

local function dialogs_CheckBox(fields)
	setmetatable(fields, nil)
	item_SetBounds(fields, fields.X or 0, fields.Y or 0)--, nil, fields.Height or 18)
	fields.On = fields.On or false
	local t = {}
	t.RawFields = fields
	t.Items = {}
	--Composite_DoAddItem(t, P.Frame{Color = 20899, Fill = true}, "Fill")
	Composite_DoAddItem(t, P.Frame{Color = 25158, Fill = true}, "Fill")
	Composite_DoAddItem(t, P.Text{Align = 4, Color = 1 --[[19]], Text = fields.Text}, "Text")
	Composite_DoAddItem(t, P.Frame{Color = 10434}, "BorderS")
	Composite_DoAddItem(t, P.Frame{Color = 33574}, "BorderL")
	Composite_DoAddItem(t, P.Button{DrawX = 3, OnClick = CheckBox_OnClick}, "Button")
	t.RadioDefs = {"ZRadOff.def", "ZRadOn.def"}
	t.CheckDefs = {"ZChkClr.def", "ZChkOn.def", "ZChkOff.def"}
	return Composite_InitObject(t, fields, CheckBox)
end
P.CheckBox = dialogs_CheckBox

--------------------------------------------------------------------------------

local CheckGroup = Composite_NewClass("CheckGroup", Aligner)

local CheckPropogate = {
	Width = true,
	MinWidth = true,
	AutoWidth = true,
	ExpandWidth = true,
	OnClick = true,
	OnChange = true,
	On = true,
	CloseDialog = true,
	Hint = true,
	LongHint = true,
	States = true,
	Radio = true,
	Border = true,
	Active = true,
	Visible = true,
	Disabled = true,
	Shaded = true,
	TextColor = true,
	FillColor = true,
	FillVisible = true,
	BorderLColor = true,
	BorderSColor = true,
	BorderHeight = true,
	Font = true,
}

for a in pairs(CheckPropogate) do
	set[a] = set[a] or Composite_SetCommon
end

function set:ItemsAutoHeight(v, old)
	for i, it in ipairs(self) do
		it.AutoHeight = v
	end
	if v ~= old and rawget(self, "NeedSize") then
		self:Realign()
	end
end

function set:Texts(v)
	-- for i = 1, min(#v, self.Count or 0) do
		-- self[i].Text = v[i]
	-- end
	self.Count = 0
	self.Count = #v
end

function set:Hints(v)
	for i, it in ipairs(self) do
		it.Hint = v[i]
	end
end

function set:LongHints(v)
	for i, it in ipairs(self) do
		it.LongHint = v[i]
	end
end

function set:RadioDefs(v, old, a)
	for i, it in ipairs(self) do
		it[a] = table_copy(v)
	end
end
set.CheckDefs = set.RadioDefs

local function CheckGroup_NewItem(self, i)
	local t = {Owner = self, Group = self, Index = i}
	for a in pairs(CheckPropogate) do
		t[a] = self[a]
	end
	t.AutoHeight = self.ItemsAutoHeight
	t.Text = self.Texts and self.Texts[i]
	if self.Hints then
		t.Hint = self.Hints[i]
	end
	if self.LongHints then
		t.LongHint = self.LongHints[i]
	end
	return dialogs_CheckBox(t)
end

function set:Count(v, old)
	local d = self.Parent
	v = v or 0
	old = old or 0
	if v < old then
		for i = old, v + 1, -1 do
			self[i]:Delete()
		end
	elseif v > old then
		for i = old + 1, v do
			self:AddItem(CheckGroup_NewItem(self, i))
		end
	else
		return
	end
	if rawget(self, "NeedSize") then
		self:Realign()
	end
end

function P.CheckGroup(fields)
	setmetatable(fields, nil)
	fields.Horizontal = false
	fields.Checks = nil
	item_SetBounds(fields, fields.X or 0, fields.Y or 0)
	local t = {Items = {}, Checks = {}}
	local sb = fields.ScrollBar
	sb = sb and type(sb) ~= "string" and not sb.Owner and sb
	if sb then
		t.Items[0] = sb
	end
	local n = fields.Count
	Composite_InitObject(t, fields, CheckGroup)
	if n then
		t.Count = n
	end
	if sb then
		t.Items[0] = nil
		Composite_DoAddItem(t, sb)
	end
	return t
end
