--[[
Plans:

custom draw control
test removal from check group
исправить с KeyPressed в Edit

Spin Buttons: icm08, icm10

]]

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

local P = {}
dialogs = P

----------- No globals from this point ------------

local _NOGLOBALS

------------------------------------

local visibleDialogs = {}
local dialogByHandle = setmetatable({}, {__mode = "kv"})
local dlg_Delete

local function GetItemByHandle(p)
	local t = dialogByHandle[u4[p + 4]]
	if t then
		return t.ItemByHandle[p]
	end
end

local function callVMT(p, i, ...)
	return call(u4[u4[p] + i*4], 1, p, ...)
end
internal.CallDialogVMT = callVMT

local function callDefVMT(p, i, ...)
	return call(u4[u4[u4[p]-4] + i*4], 1, p, ...)
end

local function GetPrivate(t)
	return table_hidden(t)
end

local function BoolToInt(v)
	if v == true then
		return 1
	else
		return v or 0
	end
end

local function destroyedItem(t)
	local function destroyedError(t, a)
		error('attempt to modify field "'..a..'" of a destroyed dialog item', 2)
	end
	rawset(t, "?ptr", nil)
	setmetatable(t, {__newindex = destroyedError, __presist = nullpersist})
end

local function destroyedDialog(t)
	local function destroyedError(t, a)
		error('attempt to modify field "'..a..'" of a destroyed dialog', 2)
	end
	rawset(t, "?ptr", nil)
	setmetatable(t, {__newindex = destroyedError, __presist = nullpersist})
end

--------------------------------------------------------------------------------

local dialog_Pcx, dialog_Def, dialog_Text, dialog_Area

local tileDefVMT, tilePcxVMT, tilePcx16VMT = internal.GetTileVTables()
local buttonVMT, captionButtonVMT = internal.GetDrawPosVTables()
local editVMT = internal.GetEditVTables()
local scrollCallbackPtr = internal.ScrollCallbackPtr
local dialogVMT = 0x63A6A8 -- internal.GetDlgVTable()

local AnyCmd = mem_struct(function(define)
	define
	.i4 'Type'        -- +0
	.i4 'Subtype'     -- +4
	.i4 'ItemId'      -- +8
	.u4 'Flags'       -- +C
	.i4 'X'           -- +10
	.i4 'Y'           -- +14
	.i4 'Param'       -- +18
	.i4 'DialogPtr'   -- +1C
end)

local KeyCmd = mem_struct(function(define)
	define
	.i4 'Type'        -- +0
	.alt.i4 'Subtype' -- +4
	.i4 'Key'         -- +4
	.i4 'ItemId'      -- +8
	.u4 'Flags'       -- +C
	.i4 'X'           -- +10
	.i4 'Y'           -- +14
	.i4 'Param'       -- +18
	.i4 'DialogPtr'   -- +1C
end)

local MouseCmd = mem_struct(function(define)
	define
	.i4 'Type'        -- +0
	.i4 'Subtype'     -- +4
	.i4 'ItemId'      -- +8
	.u4 'Flags'       -- +C
	.i4 'X'           -- +10
	.i4 'Y'           -- +14
	.i4 'Param'       -- +18
	.u4 'DialogPtr'   -- +1C
end)

local cmdByType = {}
cmdByType[512] = MouseCmd
cmdByType[1] = KeyCmd
cmdByType[2] = KeyCmd

--------------------------------------------------------------------------------

local TList = mem_struct(function(define)
	define
	.skip(4)
	.ro.u4  'Start'
	.ro.u4  'End'
	.ro.u4  'MemEnd'
	
	-- function define.class:set(t)
		-- local n = #t
		-- local buf = malloc(n*4)
		-- for i = 0, n - 1 do
			-- u4[buf + i*4] = t[i + 1]
		-- end
		-- call(0x404230, 1, self.?ptr, self.Start, n, buf)
	-- end
end)

local tempmem

local function TList_Add(t, it)
	tempmem = tempmem or malloc(4)
	u4[tempmem] = it
	call(0x5FE2D0, 1, t.?ptr, t.End, 1, tempmem)
end

local function TList_Remove(t, it)
	for i = t.Start, t.End - 1, 4 do
		if u4[i] == it then
			u4[t.?ptr + 8] = t.End - 4  -- t.End = t.End - 4
			return mem_copy(i, i + 4, t.End - i)
		end
	end
end

------------------------------------ Items Classes Support

-- Object Model:
-- Object -index- Class
-- Object Private -index- Class Private
-- Fields and various stuff are in object private

local createParams
local defaultParams
local get
local set
local getters
local setters
local currentPrivate

local function param(name, autoprivate)
	name = name or mem_structs.LastStructMember
	if autoprivate then
		local theSet = set
		function get(t, a)
			return GetPrivate(t)[a]
		end
		function set(t, a, v)
			GetPrivate(t)[a] = v
			return theSet(t, a, v)
		end
	end
	getters[name] = get
	setters[name] = set
	get = nil
	set = nil
end

local function makeParams_index(t, a)
	local i = #createParams
	if type(a) == "string" and a ~= "" then
		createParams[i+1] = a
		defaultParams[i+1] = 0
		if a == "Id" then
			currentPrivate.NeedId = true
		end
	else
		if type(a) == "table" then
			defaultParams[i] = a[1]
		else
			defaultParams[i] = a
		end
	end
	return t
end

local makeParams = setmetatable({}, {__index = makeParams_index, __call = nullsub})

local function private_createHandle(priv, t, level)
	local def = priv.defaultParams
	local structFields = priv.structFields
	local getters = priv.getters
	local par = {}
	for i, name in ipairs(priv.createParams) do
		local v
		if name ~= "_" then
			v = t[name]
			if structFields[name] or getters[name] then
				rawset(t, name, nil)
			end
			if v == true then  v = 1  end
			if v == false then  v = 0  end
		end
		v = v or def[i] or 0
		if type(def[i]) == "string" then
			v = tostring(v or def[i])
			if v == "" then  v = 0  end
		else
			v = tonumber(v)
			if v == nil then  error('"'..name..'" must be a number or boolean', 3 + (level or 0))  end
		end
		par[i] = v
	end
	t.?ptr = mem_new(unpack(par))
end

local preitem = {}
local item = {}

function item.NeedHandle(t)
	return t.?ptr
end
item.NeedSize = nullsub

local function item_destroy(t)
	if not t.Parent or not t.Parent.?ptr then
		local p = t.?ptr
		if p then
			callVMT(p, 0, 1)
			-- destroyedItem(t)
		end
	end
end

local function preitem_NeedHandleNoDestructor(t)
	local priv = GetPrivate(t)
	priv:createHandle(t)
	priv.class:new(t)
	priv.object = t
	t.NeedHandle = nil
	return t.?ptr
end

function item.Attach(t, d)
	if not t.Id and GetPrivate(t).NeedId then  -- assign id
		local id = 1
		local items = assert(d.Items)
		while items[id] do
			id = id + 1
		end
		rawset(t, "Id", nil)
		t.Id = id
		d.Items[id] = t
	end
end

local function preitem_NeedHandle(t)
	if t.Parent then
		item.Attach(t, t.Parent)
	end
	local NoId = not t.Id
	preitem_NeedHandleNoDestructor(t)
	GetPrivate(t).AutoDestructor = table_destructor(t, item_destroy)
	if NoId and GetPrivate(t).NeedId then
		rawset(t, "Id", false)  -- get Id when attached
	end

	return t.?ptr
end
preitem.NeedHandle = preitem_NeedHandle
function preitem.NeedSize(t)
	t:NeedHandle()
end

local function item_cmd_std(t, subtype, par)
	return call(0x5FF400, 1, t.Parent.?ptr, 512, subtype, t.Id, par)
end

local cmdbuf = malloc(0x20)

local function item_cmd(t, subtype, par)
	u4[cmdbuf] = 512
	u4[cmdbuf + 4] = subtype
	u4[cmdbuf + 8] = t.Id
	mem_fill(cmdbuf + 0xC, 0x8, 0)
	u4[cmdbuf + 0x18] = topointer(par)
	u4[cmdbuf + 0x1C] = t.Parent and t.Parent.?ptr or 0
	return callVMT(t.?ptr, 2, cmdbuf)
end
item.SendCmd = item_cmd

local function item_cmd_act(t, subtype, par)
	local old = t.Active
	t.Active = true
	local ret = item_cmd(t, subtype, par)
	t.Active = old
	return ret
end

function item:new(t)
	local privclass = GetPrivate(self)
	privclass.defnew(self, t)
	local priv = GetPrivate(t)
	t.RawFields = setmetatable({?ptr = t.?ptr}, getmetatable(t))

	local class = self
	local function index(t, a)
		local f = priv.getters[a]
		if f then
			return f(t, a)
		end
		return t.RawFields[a]
	end

	local function newindex(t, a, v)
		if v == true then
			v = 1
		elseif v == false then
			v = 0
		end
		local f = priv.setters[a]
		if f then
			return f(t, a, v)
		end
		if priv.structFields[a] then
			t.RawFields[a] = v
		else
			rawset(t, a, v)
		end
	end

	setmetatable(t, {__index = index, __newindex = newindex, __persist = nullpersist})
end

preitem.OnAction = nullsub
preitem.OnMouseAction = nullsub
preitem.OnClick = nullsub
preitem.OnRightClick = nullsub
preitem.OnMouseMove = nullsub
preitem.OnClose = nullsub

function preitem:ProcessAction(cmd)
	self:OnAction(cmd)
	if not self.Parent or not self.Disabled and not self.Shaded and self.Active then
		if cmd.Type == 512 then
			self:OnMouseAction(cmd)
			if cmd.Subtype == 13 then
				self:OnClick(cmd)
			elseif cmd.Subtype == 14 then
				self:OnRightClick(cmd)
			elseif cmd.Subtype == 10 then
				self:OnClose(cmd)
			end
		elseif cmd.Type == 4 then
			self:OnMouseMove(cmd)
		end
	end
end

function item:Redraw()
	local d = self.Parent
	if d and d.?ptr and not d.NoRedraw then
		call(0x4EDB20, 0)
		item_cmd(self, 2, 0)
		call(0x603190, 1, u4[0x6992D0], d.X + self.X, d.Y + self.Y, self.Width, self.Height)
	end
	-- local d = self.Parent
	-- if self.Visible and d then
		-- call(0x4EDB20, 0)
		-- callVMT(self.?ptr, 4)
		-- call(0x603190, 1, u4[0x6992D0], d.X + self.X, d.Y + self.Y, self.Width, self.Height)
	-- end
end

local function item_SetBounds(t, x, y, w, h)
	if x then  t.X = x  end
	if y then  t.Y = y  end
	if w then  t.Width = w  end
	if h then  t.Height = h  end
end
preitem.SetBounds = item_SetBounds

local function init_item(t)
	local priv = GetPrivate(t)
	local defaultParams = priv.defaultParams
	local setters = priv.setters
	for k,v in pairs(t) do
		if v == true then
			v = 1
		elseif v == false then
			v = 0
		end
		local f = setters[k]
		if f then
			f(t, k, v)
			rawset(t, k, nil)
		elseif priv.structFields[k] then
			t.RawFields[k] = v
			rawset(t, k, nil)
		end	
	end
	if t.Width <= 0 then
		t.Width = callVMT(t.?ptr, 6)
	end
	if t.Height <= 0 then
		t.Height = callVMT(t.?ptr, 5)
	end
end
item.Init = init_item

function preitem.Delete(v)
	if v.Owner then
		v.Owner:RemoveItem(v)
	end
	return v.Parent and dlg_Delete(v.Parent, v)
end
internal.DlgItemDelete = preitem.Delete

table_copy(preitem, item)

local function ItemClass(f, name, commonF)
	local pre = commonF and table_copy(commonF) or {}
	pre.ControlKind = name
	getters = {}
	setters = {}
	local StructFields
	local t = mem_struct(function(define)  f(define)  StructFields = define.members  end)

	createParams = {"_"}
	defaultParams = {t.?size}
	local priv = GetPrivate(t)
	currentPrivate = priv
	priv.createParams = createParams
	priv.defaultParams = defaultParams
	priv.getters = getters
	priv.setters = setters
	priv.defnew = t.new
	priv.createHandle = private_createHandle
	priv.structFields = StructFields
	priv.class = t
	priv.__index = priv

	table_copy(pre, t, true)
	table_copy(item, t, true)
	table_copy(preitem, pre)
	return t, pre
end

local function MakePreItem(class, t, preclass, NoXY)
	setmetatable(GetPrivate(t), GetPrivate(class))
	setmetatable(t, {__index = assert(preclass)})
	if not NoXY then
		t.X = rawget(t, "X") or 0
		t.Y = rawget(t, "Y") or 0
	end
	return t
end

------------------- classes

local function GetHumanPlayer()  -- !!! move to other file
	local p = u4[0x699538]
	if p ~= 0 and call(0x4CE670, 1, p) ~= 0 then
		return call(0x4CE6E0, 1, p)
	end
end
_G.GetHumanPlayer = GetHumanPlayer

local function PlayerColor()
	function set(t, a, v)
		if v then
			if v == -2 and t.Parent then
				v = t.Parent.PlayerColor or -1
			end
			if v < 0 or v >= 8 then
				v = GetHumanPlayer() or 0
			end
			item_cmd_act(t, 13, v)
		end
	end
	param("PlayerColor", true)
end

local function FlagParam(n, a)
	function get(t)
		return t.Flags:And(n) ~= 0
	end
	function set(t, a, v)
		if v and v ~= 0 then
			t.Flags = t.Flags:Or(n)
		else
			t.Flags = t.Flags:AndNot(n)
		end
	end
	param(a)
end

local function StateParam(n, a)  -- can use send cmd (5 - or, 6 - and not), it will redraw stuff!!!
	function get(t)
		return t.State:And(n) ~= 0
	end
	function set(t, a, v)
		if v and v ~= 0 then
			t.State = t.State:Or(n)
		else
			t.State = t.State:AndNot(n)
		end
	end
	param(a)
end


--------------------------------------------------------------------------------

local function f_Dlg(define)
	define
	.CustomType('ClassPtr', 0, function(o, obj, name, val)
		if val then
			roError(name, 3)
		else
			local p = u4[obj.?ptr]
			return p == dialogVMT and p or u4[p - 4]
		end
	end)
	.i4 'VMT'                
	.i4 'ZOrder'             
	.i4 'NextDialog'         
	.i4 'LastDialog'         
	.i4 'Flags'               -- 0x10 - draw shadow
	.i4 'State'               -- state flags
	.i4 'X'                  
	.i4 'Y'                  
	.i4 'Width'              
	.i4 'Height'             
	.u4 'BottomDlgItem'        -- topmost
	.u4 'TopDlgItem'
	.struct(TList)  'ItemsArray'
	.i4 'FocusedItemId'
	.i4 'field_44'           
	.i4 'DeactivatesCount'           
	.i4 'field_4C'           
	.i4 'field_50'            -- = 512
	.i4 'field_54'            -- = 10
	.i4 'field_58'            -- = 30721
	.i4 'field_5C'           
	.i4 'field_60'           
	.i4 'field_64'           
	FlagParam(16, "DropShadow")
end

local Dlg, preDlg = ItemClass(f_Dlg, "Dialog")
makeParams._[0x41AFA0].X.Y.Width[internal.ScreenWidth].Height[internal.ScreenHeight]._[18] ()

local lastHint


local function dlg_NeedXY(t, setX, setY)
	if setX == nil then
		t.X = floor((internal.ScreenWidth - t.Width)/2)
	end
	if setY == nil then
		t.Y = floor((internal.ScreenHeight - t.Height)/2)
	end
end

local function dlg_BuildDefaultBox(t, setX, setY, setW, setH)
	local backItems = {}
	GetPrivate(t).BackItems = backItems
	
	-- AutoSize
	local needAuto = t.Transparent or not t.BackgroundPcx
	local function AutoSize(X, W, v)
		if t["Expand"..W] then
			v = internal["Screen"..W] - (t[X] or 0)
			t[W] = v
		end
		if v or not needAuto and not t["Auto"..W] then
			return v
		end
		local w = t["Min"..W] or 100
		for _, v in ipairs(t) do
			if not v.Owner then
				v:NeedSize()
				w = max(w, v[X] + v[W])
			end
		end
		t[W] = w
		return w
	end
	setW = AutoSize("X", "Width", setW)
	setH = AutoSize("Y", "Height", setH)

	-- Background
	if t.Transparent then
		-- dlg_NeedXY(t, setX, setY)
		-- local pcx = dialog_Pcx{Id = -1, HiColor = true, Width = setW, Height = setH}
		-- local p = mem_new(56, 0x44DC40, t.Width, t.Height)
		-- local draw = u4[u4[0x6992D0] + 64]
		-- call(0x4EDB20, 0) -- Dlg_Redraw1
		-- call(0x44E0C0, 1, p, u4[draw + 48], t.X, t.Y, i4[draw + 36], i4[draw + 40], i4[draw + 44]) -- Copy screen to pcx
		-- call(0x4EDB20, 0) -- Dlg_Redraw1
		-- pcx.LoadedPcx = p
		-- table_insert(backItems, pcx)
	else
		local backPcx = t.BackgroundPcx or "diboxbck.pcx"
		local pcx = dialog_Pcx{File = backPcx, Id = -1, HiColor = t.BackgroundHiColor, Tile = true, Width = setW, Height = setH}
		pcx:NeedHandle()
		if setW == nil then
			t.Width = pcx.Width
		end
		if setH == nil then
			t.Height = pcx.Height
		end
		table_insert(backItems, pcx)
	end
	
	-- Border
	if t.Border or t.BorderHint or (t.Border ~= false and not t.Transparent and t.BackgroundPcx == nil) then
		local w, h = t.Width, t.Height
		local hint = t.BorderHint
		local borderDef = t.BorderDef or "dialgbox.def"
		
		local defaults = {File = borderDef, PlayerColor = -2, Id = -1}
		local def = dialog_Def(table_copy(defaults))
		def:NeedHandle()
		local dx, dy = def.Width, def.Height
		backItems[#backItems + 1] = def -- LT
		
		local function insert(t)
			backItems[#backItems + 1] = dialog_Def(table_copy(defaults, t))
		end
		defaults.Width = dx
		defaults.Height = dy
		defaults.Tile = true
		if h > dy*2 then
			insert{Y = dy, Height = h - dy*2, Frame = 4} -- L
			insert{Y = dy, Height = h - dy*2, X = w - dx, Frame = 5} -- R
		end
		if w > dx*2 then
			insert{X = dx, Width = w - dx*2, Frame = 6} -- T
			insert{X = dx, Width = w - dx*2, Y = h - dy, Frame = hint and 10 or 7} -- B
		end
		defaults.Tile = false
		insert{X = w - dx, Frame = 1} -- RT
		insert{Y = h - dy, Frame = hint and 8 or 2} -- LB
		insert{X = w - dx, Y = h - dy, Frame = hint and 9 or 3} -- RB

		if hint and t.Items.HintControl == nil then
			table_insert(backItems, dialog_Pcx{X = 8, Y = h - 26, Width = w - 16, Height = 18, File = t.BorderHintPcx or "statbar.pcx", Id = -1, Tile = true})
			Dlg.Add(t, dialog_Text{X = 8, Y = h - 26, Width = w - 16, Height = 18, Font = "SmalFont.fnt", Name = "HintControl", Text = t.Hint or ""})
		end
	end
	local area = dialog_Area{Name = "DialogArea", Width = t.Width, Height = t.Height, Id = -1}
	backItems[#backItems + 1] = area
	t.Items.DialogArea = area
end

local function GetPlayerColorParam(v)
	local set = GetPrivate(v).setters
	set = set and set.PlayerColor
	local c = set and v.PlayerColor
	return c, set
end

local function RefreshPlayerColors(t)
	for _,v in pairs(t.ItemByHandle) do
		local c = GetPlayerColorParam(v)
		if c then
			v.PlayerColor = c
		end
	end
end

local function dlg_updateHandle(t)
	local p = t.?ptr
	local priv = GetPrivate(t)
	
	if priv.NeedReattach then
		callVMT(p, 7)
		priv.NeedReattach = nil
	end
	
	local id = 1
	local wasAdded
	local function AddItem(v)
		local handle = v.NeedHandle and v:NeedHandle()
		if handle then
			t.ItemByHandle[handle] = v
			TList_Add(t.ItemsArray, handle)
			call(0x5FF270, 1, p, handle, -1)
		else
			v:NeedSize()
		end
		wasAdded = true
	end

	if not priv.BackItems.added then
		for _, v in ipairs(priv.BackItems) do
			AddItem(v)
		end
	end
	--msg()
	for _, v in ipairs(t) do
		if not GetPrivate(v).added then
			AddItem(v)
		end
	end
	--msg()

	-- init items
	if wasAdded then
		for _, v in ipairs(t) do -- init properties of items
			local vp = GetPrivate(v)
			if not vp.added then
				if v.Init then
					v:Init()
				end
				vp.added = true
			end
		end
	end
	if not priv.BackItems.added then -- init properties of background
		for _, v in ipairs(priv.BackItems) do
			v.Parent = t
			v.Id = -1
			v:Init()
		end
		priv.BackItems.added = true
	end
	RefreshPlayerColors(t)
	
	return p
end
Dlg.NeedHandle = dlg_updateHandle
Dlg.NeedSize = nullsub

function dlg_Delete(t, v)  -- declared before
	for k, v1 in pairs(t.Items) do
		if v1 == v then
			t.Items[k] = nil
		end
	end
	v.Parent = nil
	for i = #t, 1, -1 do
		if t[i] == v then
			table_remove(t, i)
		end
	end
	
	if t.?ptr and v.?ptr then
		-- remove from dialog
		local handle = v.?ptr
		t.ItemByHandle[handle] = nil
		TList_Remove(t.ItemsArray, handle)
		if GetPrivate(t).visible then
			call(0x5FF320, 1, t.?ptr, handle)
		end

		-- restore current player colors of defs and pictures
		local c = GetHumanPlayer() or 0
		local c1, set = GetPlayerColorParam(v)
		if c1 and c1 ~= c and rawget(v, "PlayerColor") == nil then
			set(v, "PlayerColor", c)
			RefreshPlayerColors(t)
		end

		-- FocusedItemId?
		-- destroy
		-- item_destroy(v)
		-- destroyedItem(v)
	end
end

local function dlg_destroy(t)
	dialogByHandle[t.?ptr] = nil
	for p = t.ItemsArray.Start, t.ItemsArray.End - 1, 4 do
		local p1 = u4[p]
		local it = t.ItemByHandle[p1]
		if it then
			destroyedItem(it)
		end
		callVMT(p1, 0, 1)
	end
	callVMT(t.?ptr, 0, 1)
end

function preDlg.NeedHandle(t)
	local priv = GetPrivate(t)
	local setX, setY, setW, setH = t.X, t.Y, t.Width, t.Height
	dlg_BuildDefaultBox(t, setX, setY, setW, setH)  -- must be before preitem_NeedHandleNoDestructor
	dlg_NeedXY(t, setX, setY)
	preitem_NeedHandleNoDestructor(t)
	t.VMT = dialogVMT
	local p = t.?ptr
	priv.AutoDestructor = table_destructor(t, dlg_destroy)
	
	dialogByHandle[p] = t
	init_item(t)
	return dlg_updateHandle(t)
end
preDlg.NeedSize = preDlg.NeedHandle

local function dlg_DoAdd(t, v)
	local n = v.Name
	if n then
		local last = t.Items[n]
		if n ~= "DialogArea" and (not last or last.Owner) then
			t.Items[n] = v
		elseif not v.Owner then
			error('Item named "'..n..'" already exists', 3)
		end
	end
	n = v.Id
	if n then
		if t.Items[n] then
			error('Item with this id ('..n..') already exists', 3)
		end
		t.Items[n] = v
	end
	v.Parent = t

	local f = v.Attach
	if f then  f(v, t)  end

	if GetPrivate(t).visible then
		t:NeedHandle()
	end
end

function preDlg.Add(t, v, ...)
	if v then
		t[#t + 1] = v
		dlg_DoAdd(t, v)
		t:Add(...)
		return v
	end
end

function P.new(t)
	t = MakePreItem(Dlg, t, preDlg, true)
	t.Items = {}
	t.ItemByHandle = {}
	local it = {}
	for i, v in ipairs(t) do
		it[i] = v
		t[i] = nil
	end
	t:Add(unpack(it))
	return t
end

function preDlg.Show(t, right)
	local p = t:NeedHandle()
	-- HoldOnTimeAtBattleStart

	--u4[0x6989E8] = 0 -- time...?
	call(0x597AA0, 0) -- StopAnimation

	if right then
		call(0x5F4B90, 1, p) -- call(0x603000, 1, u4[0x6992D0], p)
	else
		local mouse = u4[0x6992B0]
		local curt = u4[mouse+76]
		local curst = u4[mouse+80]
		call(0x50CEA0, 1, mouse, 0, 0) -- SetMouseCursor
		call(0x50D7B0, 1, mouse, 1) -- TurnMouseON
		callVMT(p, 6, 0) -- show
		call(0x50CEA0, 1, mouse, curst, curt) -- SetMouseCursor
	end
	call(0x597B50, 0) -- ContinueAnimation

	-- ContinueTimeAfterBattle

	return u4[u4[0x6992D0] + 56]
end

function Dlg.Redraw(t)
	if not t.NoRedraw then
		callVMT(t.?ptr, 5, true, -65535, 65535)
	end
end

function Dlg.ItemAtPos(t, x, y, IgnoreFlags)
	local p = t.TopDlgItem
	while p ~= 0 do
		if x >= i2[p + 0x18] and x < i2[p + 0x18] + i2[p + 0x1C] and
		   y >= i2[p + 0x1A] and y < i2[p + 0x1A] + i2[p + 0x1E] and
		   (IgnoreFlags and u2[p + 0x16]:And(6) ~= 0 or u2[p + 0x16]:And(0x102A) == 2) then
			local it = t.ItemByHandle[p]
			if it then
				return it
			end
		end
		p = u4[p + 8]
	end
end

function Dlg.ItemAtPosAbs(t, x, y, IgnoreFlags)
	return t:ItemAtPos(x - t.X, y - t.Y, IgnoreFlags)
end

preDlg.OnKeyDown = nullsub
preDlg.OnKeyUp = nullsub

function Dlg.ProcessAction(t, cmd)
	local type, itemId = cmd.Type, cmd.ItemId
	local it = (itemId ~= 0) and (itemId ~= -1) and t.Items[itemId] or nil
	if type == 4 or type == 512 and (not it or it.Disabled or it.Shaded or not it.Active) then
		it = t:ItemAtPosAbs(cmd.X, cmd.Y)
		if t.IgnoreFlags and it == t.Items.DialogArea then
			it = t:ItemAtPosAbs(cmd.X, cmd.Y, true)
		end
	end
	cmd.Item = it

	if type == 4 and t.Items.HintControl then
		local h = (it ~= nil) and it.Hint or ""
		if h == "" then
			h = t.Hint or ""
		end
		if h ~= "" then
			cmd.DefaultHandler = nil
		end
		if h ~= lastHint then
			lastHint = h
			t.Items.HintControl.Text = h
			t:Redraw()
		end
	end
	item.ProcessAction(t, cmd)
	if it then
		it:ProcessAction(cmd)
	end
	if type == 1 or type == 2 then
		cmd.Item = t.Items[t.FocusedItemId]
	end
	if type == 1 then
		t:OnKeyDown(cmd)
	elseif type == 2 then
		t:OnKeyUp(cmd)
	end
	if t.ShowLongHints and cmd.Type == 512 and cmd.Subtype == 14 and it and cmd.DefaultAction ~= false then
		local h = it.LongHint or it.Hint
		if h ~= nil and h ~= "" then
			Message(h, 4)
		end
	end
end

table_copy(preDlg, Dlg)
table_copy(preitem, preDlg)

--------------------------------------------------------------------------------

local function f_DlgItem(define)
	define
	.i4 'VMT'                
	.u4 'ParentPtr'          
	.u4 'LowerDlgItem'        
	.u4 'UpperDlgItem'        
	.i2 'Id'                 
	.i2 'field_12'           
	.i2 'Flags'              
	.i2 'State'  -- = 6 on create
	.i2 'X'                  
	.i2 'Y'                  
	.i2 'Width'              
	.i2 'Height'             
	.PChar 'field_20'  -- hint shown on right click if main doesn't exist
	.PChar 'field_24'  -- main hint shown on right click
	.i1 'field_28'           
	.i1 'field_29'           
	.i1 'field_2A'           
	.i1 'field_2B'           
	.i4 'DeactivatesCount'           
	function get(t, a, v)
		return t.State:And(32) ~= 0
	end
	function set(t, a, v)
		callVMT(t.?ptr, 9, v == 0 or not v)
	end
	param("Disabled")  -- Disabled, doesn't show hint, doesn't intercept clicks
	StateParam(1, "Pressed")
	StateParam(2, "Active")  -- Otherwise doesn't intercept clicks, ignores commands
	StateParam(4, "Visible")
	StateParam(8, "Shaded")  -- Disabled, drawn shaded, doesn't show hint, doesn't intercept clicks
end

local DlgItem = ItemClass(f_DlgItem)

--------------------------------------------------------------------------------

local DlgArea, PreDlgArea = ItemClass(f_DlgItem, "Area")
makeParams._[0x44FBE0].X[0].Y[0].Width[100].Height[20].Id[-1].Flags[1] ()

function P.Area(t)
	return MakePreItem(DlgArea, t, PreDlgArea)
end
dialog_Area = P.Area

--------------------------------------------------------------------------------

local function f_DlgFrame(define)
	f_DlgItem(define)
	define
	.i4 'Color'
	.i4 'Fill'

	function set(t, a, v)
		item_cmd_act(t, 8, v)
	end
	param('Color')
	function set(t, a, v)
		item_cmd_act(t, 63, v)
	end
	param('Fill')
end

local DlgFrame, PreDlgFrame = ItemClass(f_DlgFrame, "Frame")
makeParams._[0x44FE00].X[0].Y[0].Width[100].Height[100].Id[-1].Color[65535].Flags[1024] ()

function P.Frame(t)
	return MakePreItem(DlgFrame, t, PreDlgFrame)
end

--------------------------------------------------------------------------------

local function f_DlgTextScroll(define)
	f_DlgItem(define)
	define
	.u4 'Font'               
	.i4 'LinesCount'  -- Unsure
	.i4 'field_38'           
	.i4 'field_3C'           
	.i4 'field_40'           
	.i4 'field_44'           
	.i4 'field_48'           
	.i4 'field_4C'           
	.i4 'field_50'           
	.u4 'ScrollBar'          
	.i4 'field_58'           
end

local DlgTextScroll, PreDlgTextScroll = ItemClass(f_DlgTextScroll, "ScrollableText")
makeParams._[0x5BA360].Text[""].X[0].Y[0].Width[200].Height[200].Font[{"SmalFont.fnt"}].Color[1].Blue[0] ()

function PreDlgTextScroll.NeedSize(t)
	t.Width = t.Width or 200
	t.Height = t.Height or 200
end

function P.ScrollableText(t)
	return MakePreItem(DlgTextScroll, t, PreDlgTextScroll)
end

--------------------------------------------------------------------------------

local function f_DlgText(define)
	f_DlgItem(define)
	define
	.DynStr  'Text'
	function set(t, a, v)
		callVMT(t.?ptr, 13, tostring(v))
	end
	param()
	
	define
	.i4 'LoadedFont'
	.i4 'Color'
	.i4 'BackgroundColor'
	.i4 'Align'
	
	function get(t)
		local p = t.LoadedFont
		return mem_string(p + 4, 12)
	end
	param("Font")
end

-- returns longest line width
local function GetTextWidth(font, text)
	return min(call(0x4B56F0, 1, font, text) + 1, internal.ScreenWidth)
end

-- returns longest word width
local function GetTextMinWidth(font, text)
	return min(call(0x4B5770, 1, font, text) + 1, internal.ScreenWidth)
end

-- returns height for a given width
local function GetTextHeight(font, text, width)
	local lh = u1[font + 33]
	width = width or 0x7FFF0000--internal.ScreenWidth
	return min(max(1, call(0x4B5580, 1, font, text, width))*lh, internal.ScreenHeight)
end

local PreDlgTextF = {
	GetWidth = function(t)
		local p = assert(u4[t:NeedHandle() + 0x40])
		return GetTextWidth(p, t.Text)
	end,

	GetMinWidth = function(t)
		local p = assert(u4[t:NeedHandle() + 0x40])
		return GetTextMinWidth(p, t.Text)
	end,

	GetHeight = function(t, w)
		local p = assert(u4[t:NeedHandle() + 0x40])
		return GetTextHeight(p, t.Text, w)
	end,
}

local DlgText, PreDlgText = ItemClass(f_DlgText, "Text", PreDlgTextF)
makeParams._[0x5BC6A0].X[0].Y[0].Width[100].Height[18].Text[""].Font[{"SmalFont.fnt"}].Color[1].Id[-1].Align[5].BackgroundColor[0]._[8] ()

local function DlgText_UpdateSize(t, w, h, text)
	local p = assert(u4[t.?ptr + 0x40])
	t.MinWidth = t.MinWidth or GetTextMinWidth(p, text)
	if not w or t.AutoWidth then
		w = max(GetTextWidth(p, text), t.MinWidth or 0)
		t.Width = w
	end
	t.MinHeight = t.MinHeight or GetTextHeight(p, text, internal.ScreenWidth)
	if not h or t.AutoHeight then
		t.Height = max(GetTextHeight(p, text, w), t.MinHeight or 0)
	end
end

function DlgText.NeedSize(t)
	DlgText_UpdateSize(t, t.Width, t.Height, t.Text or "")
end

local function DlgText_createHandle(priv, t)
	local w, h, text = t.Width, t.Height, t.Text or ""
	t.AutoWidth = t.AutoWidth or t.AutoWidth == nil and not w and not t.ExpandWidth
	t.AutoHeight = t.AutoHeight or t.AutoHeight == nil and not h and not t.ExpandHeight
	private_createHandle(priv, t, 1)
	DlgText_UpdateSize(t, w, h, text)
end
GetPrivate(DlgText).createHandle = DlgText_createHandle

function P.Text(t)
	return MakePreItem(DlgText, t, PreDlgText)
end
dialog_Text = P.Text

--------------------------------------------------------------------------------

local function f_DlgTextOnPcx(define)
	f_DlgText(define)
	define
	.u4 'LoadedPcx'
	
	function get(t)
		local p = t.LoadedPcx
		return mem_string(p + 4, 12)
	end
	function set(t, a, v)
		v = tostring(v)
		local p = t.LoadedPcx
		if v ~= mem_string(p + 4, 12) then
			callVMT(t.LoadedPcx, 1)
			t.LoadedPcx = call(0x55AA10, 2, v)
		end
	end
	param("BackgroundPcx")	
end

local DlgTextOnPcx, PreDlgTextOnPcx = ItemClass(f_DlgTextOnPcx, "TextOnPcx", PreDlgTextF)
makeParams._[0x5BCB70].X[0].Y[0].Width[100].Height[20].Text[""].Font[{"SmalFont.fnt"}].BackgroundPcx[""].Color[1].Id[-1].Align[5]._[8] ()

GetPrivate(DlgTextOnPcx).createHandle = function(priv, t)
	assert(t.BackgroundPcx, "BackgroundPcx of TextOnPcx must be set")
	local w, h = t.Width, t.Height
	private_createHandle(priv, t, 1)
	local p = u4[t.?ptr + 0x50]
	if p ~= 0 and not w then
		t.Width = i4[p + 36]
	end
	if p ~= 0 and not h then
		t.Height = i4[p + 40]
	end
end

function P.TextOnPcx(t)
	return MakePreItem(DlgTextOnPcx, t, PreDlgTextOnPcx)
end

--------------------------------------------------------------------------------

local function f_DlgEdit(define)
	f_DlgText(define)
	define
	.u4 'LoadedPcx'          
	.i4 'LoadedPcx16'        
	.i2 'CaretPos'           
	.i2 'MaxLength'          
	.i2 'FieldSizeX'         
	.i2 'FieldSizeY'         
	.i2 'FieldX'             
	.i2 'FieldY'             
	.i2 'field_64'            -- = 1 on constructor
	.i2 'field_66'            -- = 1 if there's a border
	.i2 'field_68'           
	.i2 'field_6A'           
	.u1 'field_6C'           
	.b1 'Focused'            
	.u1 'field_6E'           
	.u1 'field_6F'           
--	function get(t)
--		local p = t.LoadedPcx
--		return mem_string(p + 4, 12)
--	end
--	function set(t, v)
--		item_cmd_act(t, 11, v)
--	end
--	param("Pcx")	
end

local DlgEdit, PreDlgEdit = ItemClass(f_DlgEdit, "Edit")
makeParams._[0x5BACD0].X.Y.Width[150].Height[30].MaxLength[-1].Text[""].Font[{"SmalFont.fnt"}].Color[4].Align[4].BackgroundPcx[""]._[0].Id[-1]._[0].Border[4].BorderWidth.BorderHeight ()
DlgEdit.OnIgnoreKey = nullsub
DlgEdit.OnTextChanged = nullsub
DlgEdit.OnKeyPressed = nullsub
DlgEdit.OnEnter = nullsub
DlgEdit.OnExit = nullsub

GetPrivate(DlgEdit).createHandle = function(priv, t)
	local w, h = t.Width, t.Height
	private_createHandle(priv, t, 1)
	t.VMT = editVMT
	local p = u4[t.?ptr + 0x50]
	if p ~= 0 and not w then
		t.Width = i4[p + 36]
	end
	if p ~= 0 and not h then
		t.Height = i4[p + 40]
	end
end

function PreDlgEdit.NeedSize(t)
	t.Width = t.Width or 150
	t.Height = t.Height or 30
end

function P.Edit(t)
	return MakePreItem(DlgEdit, t, PreDlgEdit)
end

function internal.Edit_IsKeyIgnored(p, cmdptr, ret)
	local cmd = KeyCmd:new(cmdptr)
	cmd.Result = ret
	local t = GetItemByHandle(p)
	if t then
		t:OnIgnoreKey(cmd)
	end
	return cmd.Result
end

function internal.Edit_KeyPressed(p, cmdptr, ret)
	local cmd = KeyCmd:new(cmdptr)
	cmd.Result = ret
	local d = dialogByHandle[u4[p + 4]]
	if d then
		local t = d.ItemByHandle[p]
		if t then
			if ret == 2 then
				t:OnTextChanged(cmd)
			else
				t:OnKeyPressed(cmd)
			end
		end
		if ret == 2 then
			d:Redraw()
		end
	end
	return cmd.Result
end

function internal.Edit_SetFocused(p, foc)
	local d = dialogByHandle[u4[p + 4]]
	if d then
		local t = d.ItemByHandle[p]
		if t then
			if foc then
				t:OnEnter()
			else
				t:OnExit()
			end
		end
		d:Redraw()
	end	
end

--------------------------------------------------------------------------------

local function f_DlgScroll(define)
	f_DlgItem(define)
	define
	.i4 'LoadedBtnDef'       
	.i4 'LoadedPcx'          
	.i4 'field_38'            -- Last Tick
	.i4 'Tick'               
	function set(t, a, v)
		item_cmd_act(t, 49, v)
	end
	param('Tick')
	
	define
	.i4 'BtnPosition'        
	.i4 'SizeFree'           
	.i4 'TicksCount'
	.i4 'SizeMax'            
	.i4 'BigStepSize'                
	.i4 'BtnSize2'           
	.i2 'MouseX'           
	.i2 'MouseY'           
	.b1 'CatchKeys'
	.skip(3)
	.i4 'field_60'           
	.i4 'CallBack'           
end

local DlgScroll, PreDlgScroll = ItemClass(f_DlgScroll, "ScrollBar")
makeParams._[0x5963C0].X.Y.Width[16].Height[16].Id[-1].TicksCount[1]._[scrollCallbackPtr].Blue.BigStepSize.CatchKeys ()
DlgScroll.OnScroll = nullsub

function PreDlgScroll.NeedSize(t)
	t.Width = t.Width or 16
	t.Height = t.Height or 16
end

function P.ScrollBar(t)
	return MakePreItem(DlgScroll, t, PreDlgScroll)
end

function internal.Scroll_Callback(p, pos)
	local t = GetItemByHandle(p)
	if t then
		t:OnScroll(pos)
	end
end

--------------------------------------------------------------------------------

local function f_DlgPcx(define)
	f_DlgItem(define)
	define
	.i4 'LoadedPcx'
	
	function get(t)
		local p = t.LoadedPcx
		return mem_string(p + 4, 12)
	end
	function set(t, a, v)
		item_cmd_act(t, 11, v)
	end
	param("File")	
	PlayerColor()
end

local PreDlgPcxF = {
	GetWidth = function(t)
		local p = u4[t:NeedHandle() + 0x30]
		return p ~= 0 and i4[p + 36] or 0
	end,

	GetHeight = function(t)
		local p = u4[t:NeedHandle() + 0x30]
		return p ~= 0 and i4[p + 40] or 0
	end,
}

local DlgPcx, PreDlgPcx = ItemClass(f_DlgPcx, "Pcx", PreDlgPcxF)
makeParams._.X[0].Y[0].Width.Height.Id[-1].File[""].Flags[2048] ()

function get(t, a)
	return (t.VMT == tilePcxVMT) or (t.VMT == tilePcx16VMT)
end
function set(t, a, v)
	local m = t.VMT
	if v then
		if m == 0x63BA94 then
			m = tilePcxVMT
		elseif m == 0x63BACC then
			m = tilePcx16VMT
		end
	else
		if m == tilePcxVMT then
			m = 0x63BA94
		elseif m == tilePcx16VMT then
			m = 0x63BACC
		end
	end
	t.VMT = m
end
param("Tile")

GetPrivate(DlgPcx).createHandle = function(priv, t)
	GetPrivate(t).defaultParams[2] = t.HiColor and 0x450340 or 0x44FFA0
	local w, h = t.Width, t.Height
	private_createHandle(priv, t, 1)
	local p = u4[t.?ptr + 0x30]
	if p ~= 0 and not w then
		t.Width = i4[p + 36]
	end
	if p ~= 0 and not h then
		t.Height = i4[p + 40]
	end
end

function P.Pcx(t)
	return MakePreItem(DlgPcx, t, PreDlgPcx)
end
dialog_Pcx = P.Pcx

--------------------------------------------------------------------------------

local function f_DlgDef1(define, button)
	f_DlgItem(define)
	define
	.i4 'LoadedDef'
	.i4 'Frame'
	.i4 (button and 'PressedFrame' or 'Animation')
	-- function set(t, a, v)  -- !!! WTF?
		-- t.RawFields.Frame = 0
		-- return true
	-- end param()
	
	define
	.i4 'Mirror'             
	.i4 'field_40'           
	.i2 'CloseDialog'        
	.i2 'field_46'           

	function get(t)
		local p = t.LoadedDef
		return mem_string(p + 4, 12)
	end
	function set(t, a, v)
		item_cmd_act(t, 9, v)
	end
	param("File")
	PlayerColor()
end

local PreDlgDefF = {
	GetWidth = function(t)
		local p = u4[t:NeedHandle() + 0x30]
		return p ~= 0 and i4[p + 48] or 0
	end,

	GetHeight = function(t)
		local p = u4[t:NeedHandle() + 0x30]
		return p ~= 0 and i4[p + 52] or 0
	end,
}

local DlgDef1, PreDlgDef1 = ItemClass(f_DlgDef1, "Def", PreDlgDefF)
makeParams._[0x4EA800].X[0].Y[0].Width.Height.Id[-1].File[""].Frame.Animation[1].Mirror.CloseDialog.Flags[16] ()

FlagParam(2, "MonsterAlign")
function get(t, a)
	return (t.VMT == tileDefVMT)
end
function set(t, a, v)
	t.VMT = v and tileDefVMT or 0x63EC48
end
param("Tile")

local function DlgDef1_createHandle(priv, t)
	local w, h = t.Width, t.Height
	private_createHandle(priv, t, 1)
	local p = u4[t.?ptr + 0x30]
	if p ~= 0 and not w then
		t.Width = i4[p + 48]
	end
	if p ~= 0 and not h then
		t.Height = i4[p + 52]
	end
end

GetPrivate(DlgDef1).createHandle = DlgDef1_createHandle


-- local function getTime()
	-- return call(0x4F8970, 0)
-- end


-- local DefAnimNode = {}
function P.Def(t)
	return MakePreItem(DlgDef1, t, PreDlgDef1)
end
dialog_Def = P.Def

-- function DlgDef1:ProcessAnimation(cmd)
	-- if self.OnAnimate.exists then
		-- local priv = GetPrivate(self)
		-- local need = priv.animTime
		-- local time = getTime()
		-- if need and time - need < 0 then
			-- return
		-- end
		-- priv.animTime = time + 100
		-- local f = priv.coroutine
		-- if f then
			-- coroutine_resume(f)
		-- else
			-- f = coroutine_create(self.OnAnimate)
			-- coroutine_resume(f, self, priv.animNode)
		-- end
		-- priv.coroutine = (coroutine_status(f) ~= "dead") and f or nil
	-- end
-- end


-- function DefAnimNode:AnimateFrame(n)
	-- if n then
	
	-- end
-- end

--------------------------------------------------------------------------------

local function f_DlgDef2(define)
	f_DlgDef1(define, true)
	define
	.struct(TList) 'HotKeys'
	.DynStr  'Text'
	function set(t, a, v)
		item_cmd_act(t, 3, v)
	end
	param()	
end

local DlgButtonStd = ItemClass(f_DlgDef2, "Button", PreDlgDefF)

local function f_DlgButton(define)
	f_DlgDef2(define)
	define  -- my additional fields
	.i4 'DrawX'
	.i4 'DrawY'
end

local DlgButton, PreDlgButton = ItemClass(f_DlgButton, "Button", PreDlgDefF)
makeParams._[0x455BD0].X[0].Y[0].Width.Height.Id[-1].File[""].Frame.PressedFrame[1].CloseDialog.HotKey.Flags[2] ()

GetPrivate(DlgButton).createHandle = function(priv, t)
	DlgDef1_createHandle(priv, t)
	t.VMT = buttonVMT
	u4[t.?ptr + 104] = 0
	u4[t.?ptr + 108] = 0
end


function P.Button(t)
	MakePreItem(DlgButton, t, PreDlgButton)
	return t	
end

--------------------------------------------------------------------------------

local function f_DlgCaptionButtonStd(define)
	f_DlgDef2(define)
	define
	.u4 'LoadedFont'
	.i4 'Color'
	
	function get(t)
		local p = t.LoadedFont
		return mem_string(p + 4, 12)
	end
	param("Font")	
end

local DlgCaptionButtonStd = ItemClass(f_DlgCaptionButtonStd, "TextButton", PreDlgDefF)

local function f_DlgCaptionButton(define)
	f_DlgCaptionButtonStd(define)
	define  -- my additional fields
	.i4 'DrawX'
	.i4 'DrawY'
end

local DlgCaptionButton, PreDlgCaptionButton = ItemClass(f_DlgCaptionButton, "TextButton", PreDlgDefF)
makeParams._[0x456730].X[0].Y[0].Width.Height.Id[-1].File[""].Text[""].Font[{"SmalFont.fnt"}].Frame.PressedFrame[1].CloseDialog.HotKey.Flags[2].Color[4] ()

-- GetPrivate(DlgCaptionButton).createHandle = DlgDef1_createHandle
GetPrivate(DlgCaptionButton).createHandle = function(priv, t)
	DlgDef1_createHandle(priv, t)
	t.VMT = captionButtonVMT
	u4[t.?ptr + 112] = 0
	u4[t.?ptr + 116] = 0
end

function P.TextButton(t)
	MakePreItem(DlgCaptionButton, t, PreDlgCaptionButton)
	-- GetPrivate(t).createHandle = function(priv, t)
		-- private_createHandle(priv, t, 1)
		-- t.VMT = captionButtonVMT
		-- u4[t.?ptr + 112] = 0
		-- u4[t.?ptr + 116] = 0
	-- end
	return t	
end

--------------------------------------------------------------------------------

local function f_DlgMainMenuButton(define)
	f_DlgDef2(define)
	define
	.u4 'CallbackPtr'
end

local DlgMainMenuButton, PreDlgMainMenuButton = ItemClass(f_DlgMainMenuButton, "Button", PreDlgDefF)

local MainMenuButtonCallback = mem_hookalloc()
mem_hook(MainMenuButtonCallback, function(d)
	d:pop()
	local btn = GetItemByHandle(d.esi)
	if btn then
		local cmd = MouseCmd:new(d.ecx)
		cmd.Result = 0
		btn:OnMouseAction(cmd)
		if cmd.Subtype == 13 then
			btn:OnClick(cmd)
		elseif cmd.Subtype == 14 then
			btn:OnRightClick(cmd)
		end
		return cmd.Result
	end
	return 0
end)

makeParams._[0x456A10].X[0].Y[0].Width.Height.Id[-1].File[""].CallbackPtr[MainMenuButtonCallback].Frame.PressedFrame[1] ()

GetPrivate(DlgMainMenuButton).createHandle = DlgDef1_createHandle

function DlgMainMenuButton:ProcessAction(cmd)
	self:OnAction(cmd)
	-- if not self.Disabled and not self.Shaded then
	if cmd.Type == 512 then
		self:OnMouseAction(cmd)
		if cmd.Subtype == 10 then
			self:OnClose(cmd)
		end
	elseif cmd.Type == 4 then
		self:OnMouseMove(cmd)
	end
	-- end
end

function P.MainMenuButton(t)
	MakePreItem(DlgMainMenuButton, t, PreDlgMainMenuButton)
	return t	
end


--------------------------------------------------------------------------------
--
--  Events and stuff
--
--------------------------------------------------------------------------------

local function existing_item(t, class)
	class:new(t)
	local priv = GetPrivate(t)
	setmetatable(priv, GetPrivate(class))
	priv.class = class
	priv.object = t
	return t
end

local ItemClassByVMT = {
	[0x63BA24] = DlgArea, [0x63BA5C] = DlgFrame, [0x63BA5C] = DlgFrame, [0x642D1C] = DlgTextScroll,
	[0x63BA94] = DlgPcx, [0x63BACC] = DlgPcx, [tilePcxVMT] = DlgPcx, [tilePcx16VMT] = DlgPcx,
	[0x63BB54] = DlgButtonStd, [0x63BB88] = DlgCaptionButtonStd, [0x63BBBC] = DlgButtonStd,
	[buttonVMT] = DlgButton, [captionButtonVMT] = DlgCaptionButton,
	[0x642DC0] = DlgText, [0x641BEC] = DlgText, [0x642DF8] = DlgTextOnPcx,
	[0x63A578] = DlgEdit, [0x63D4BC] = DlgEdit, [0x63EBF4] = DlgEdit, [0x640064] = DlgEdit,
	[0x640140] = DlgEdit, [0x640194] = DlgEdit, [0x640220] = DlgEdit, [0x640E40] = DlgEdit,
	[0x641B38] = DlgEdit, [0x641C24] = DlgEdit, [0x641C70] = DlgEdit, [0x642360] = DlgEdit,
	[0x6423CC] = DlgEdit, [0x642D50] = DlgEdit, [editVMT] = DlgEdit,
	[0x641B9C] = DlgScroll, [0x641D60] = DlgScroll,
	[0x63EC48] = DlgDef1, [0x641550] = DlgDef1, [0x641588] = DlgDef1, [0x6415C0] = DlgDef1,
	[0x6415F8] = DlgDef1, [0x64166C] = DlgDef1, [0x643BB0] = DlgDef1, [tileDefVMT] = DlgDef1,
}

local BadId = {[0] = true, [-1] = true}

local function existing_dialog(t)
	existing_item(t, Dlg)
	t.Items = {}
	t.ItemByHandle = {}
	local p = t.?ptr
	dialogByHandle[p] = t
	visibleDialogs[t] = true
	local priv = GetPrivate(t)
	priv.visible = true
	priv.BackItems = {added = true}
	priv.standardDlg = true
	local p = t.BottomDlgItem
	local n = 1
	while p ~= 0 do
		if not BadId[i2[p + 16]] then
			local class = ItemClassByVMT[u4[p]] or DlgItem
			local v = existing_item({?ptr = p}, class)
			t.Items[v.Id] = v
			v.Parent = t
			t.ItemByHandle[p] = v
			local vp = GetPrivate(v)
			vp.added = true
			vp.standardItem = true
		end
		p = u4[p + 12]
	end	
end

local function DialogProxy(d)
	local function proxy_index(t, a)
		if a == "?ptr" then
			return d
		elseif a == "ClassPtr" then
			return u4[u4[d] - 4]
		else
			setmetatable(t, nil)
			t.?ptr = d
			existing_dialog(t)
			return t[a]
		end
	end

	local function proxy_newindex(t, a, v)
		if a == "?ptr" or a == "ClassPtr" then
			roError(a)
		else
			setmetatable(t, nil)
			t.?ptr = d
			existing_dialog(t)
			t[a] = v
		end
	end
	
	return setmetatable({}, {__index = proxy_index, __newindex = proxy_newindex})
end

function P.ByHandle(d)
	local t = dialogByHandle[d]
	if t then
		GetPrivate(t).visible = true
		visibleDialogs[t] = true
	else
		t = DialogProxy(d)
	end
	return t
end
local dialogs_ByHandle = P.ByHandle

function P.Current()
	local p = u4[u4[0x6992D0] + 0x54]
	if p ~= 0 then
		return dialogs_ByHandle(p)
	end
end

function internal.DialogVMT0(d)  -- Dialog.Destroy
	local t = dialogByHandle[d]
	if t then
		dialogByHandle[d] = nil
		visibleDialogs[t] = nil
		destroyedDialog(t)
	end
end

local function InvokeDialogCallback(d, cmdptr, defHandler)
	local t = dialogByHandle[d]
	if t then
		local cmdrec = (cmdByType[u4[cmdptr]] or AnyCmd):new(cmdptr)
		cmdrec.Result = 0
		cmdrec.CloseDialog = false
		cmdrec.DefaultHandler = defHandler

		t:ProcessAction(cmdrec)
		
		if cmdrec.DefaultHandler then
			cmdrec.Result = cmdrec:DefaultHandler()
		end

		if cmdrec.CloseDialog then
			return call(0x41B0F0, 1, d, cmdptr)
		end

		return cmdrec.Result
	else
		return defHandler()
	end
end

function internal.DialogVMT3(d, cmdptr)  -- OnDialogAction
	--if cmdtype == 4 then
	--	callVMT(u4[0x6992D0], 2, cmd)
	--end

	-- local function defHandler()
		-- return callDefVMT(d, 3, cmdptr)
	-- end
	-- return InvokeDialogCallback(d, cmdptr, defHandler)
	return callDefVMT(d, 3, cmdptr)
end

function internal.DialogVMT8(d, deact)  -- (De)Activate
	local ret = callDefVMT(d, 8, deact)
	
	if deact == 0 then
		local t = dialogByHandle[d]
		if t and t.DeactivatesCount == 0 then
			-- set up the correct player colors
			RefreshPlayerColors(t)
		end
	end

	return ret
end

local BadDialogCallbacks = _G.table.invert{
	0x4FBDA0,  -- Main Menu
	0x4D5B50,  -- New, Load
	0x4EA2B0,  -- High Score
	0x45EED0,  -- Campaigns - pictures/movies of campaigns
	0x45AEC0,  -- Campaign - maps
	0x4F0F60,  -- Message
	0x4F7CB0,  -- Message
	0x46F300,  -- Combat Options
	0x4716E0,  -- Battle Results
	0x4E8910,  -- Hill Fort
	0x5A3380,  -- Combat: Select spell target
	0x5A3880,  -- Combat: Select teleport monster
	0x5A3AE0,  -- Combat: Select teleport place
	0x5A3010,  -- Combat: Sacrifice Select Whom To Revieve
	0x5A31A0,  -- Combat: Sacrifice Select Whom To Kill
	
-- Main combat dialog is problematic
}

function internal.DialogCallback(cmdptr, manager, callback)
	local d = u4[cmdptr + 0x1C]
	local function defHandler()
		return call(callback, 2, cmdptr, manager)
	end
	--_G.LastDialogCallback = callback
	
	if true then --BadDialogCallbacks[callback] then
		return InvokeDialogCallback(d, cmdptr, defHandler)
	else
		return defHandler()
	end
end

local ExitToContext = internal.ExitToContext


function internal.OnShowDialog(d, draw)
	local t = dialogs_ByHandle(d)
	local class = t.ClassPtr
	if class == 0x63FF60 or class == 0x63E6D8 then  -- Main Menu or Load/New Game in Main Menu
		ExitToContext("menu")
	elseif class == 0x63BC2C then  -- Campaign maps
		ExitToContext("campaign")
	end
	-- 0x63BCA4 - campaigns pack
	-- 0x641CBC - save game after w
	-- 0x63EBBC - win dragon
	-- 0x63EB98 - HiScore
	event("ShowDialog", t)
	if dialogByHandle[d] and t.OnShow then
		t:OnShow()
	end	
	if draw then
		callVMT(d, 5, true, -65535, 65535)
	end
end

function internal.OnHideDialog(d)
	local t = dialogByHandle[d]
	if t then
		if t.OnHide then
			t:OnHide()
		end	
		event("HideDialog", t)
		local priv = GetPrivate(t)
		priv.visible = false
		priv.NeedReattach = true
		visibleDialogs[t] = nil
		-- restore current player colors of defs and pictures
		if u4[0x6AAD14] ~= 0 then
			local c = GetHumanPlayer() or 0
			for _,v in pairs(t.ItemByHandle) do
				local c1, set = GetPlayerColorParam(v)
				if c1 and c1 ~= c and rawget(v, "PlayerColor") == nil then
					set(v, "PlayerColor", c)
				end
			end
		end
		-- mark standard dialog as destroyed
		if priv.standardDlg then
			dialogByHandle[d] = nil
			destroyedDialog(t)
		end
	end

	lastHint = nil
end

local function f_Panel(define)
	define
	[0x0].ro.i4  'VMT'
	[0x4].i4  'X'
	[0x8].i4  'Y'
	[0xC].i4  'Width'
	[0x10].i4  'Height'
	[0x14].struct(TList)  'ItemsArray'
	[0x24].ro.i4  'ParentPtr'
	[0x28].i4  'MinId'
	[0x2C].i4  'MaxId'
	[0x30].skip(4)
	
	local c = define.class
	
	function define.index(t, a)
		if a == "Parent" then
			local p = t.ParentPtr
			return dialogByHandle[p] or DialogProxy(p)
		end
	end
	function c:Add(t, ...)
		if t then
			if t.X then
				t.X = t.X + self.X
			end
			if t.Y then
				t.Y = t.Y + self.Y
			end
			self.Parent:Add(t)
			
			for _, t in ipairs(t.Items or {t}) do
				local id = t.Id
				if id then
					if id < self.MinId then
						self.MinId = id
					end
					if id > self.MaxId then
						self.MaxId = id
					end
				end
			end
		end
		self:Add(...)
		return t
	end
end

local DlgPanel = mem_struct(f_Panel)

function internal.AfterBuildResBar(p, small)
	event("ShowResBarPanel", DlgPanel:new(p), small ~= 0)
end


internal.dofile(internal.CoreScriptsPath.."DialogsComposite.lua")
internal.dofile(internal.CoreScriptsPath.."DialogsPredefined.lua")
internal.dofile(internal.CoreScriptsPath.."options dialog.lua")



-- More types: 641550 and near; 640E40; near DlgDef1_VTable
-- 640194 - transparent Edit?

-- permanents
return {callVMT, GetPrivate, dialogByHandle, visibleDialogs}

--[[

DlgLvl1VTable
5167904 - Dlg_Redraw1

Commands:

Type = 512

Any Control Subtypes:
2 - redraw
52 - set X (no redraw)
53 - set Y (no redraw)
61 - set DX (no redraw)

Def Subtypes:
9 - set file name
4 - set frame
12 - set group
8 - set CloseDialog
10 - set (device) palette
13 - set player, whose colors to use

Def2 Subtypes:
9 - set file name
3 - set caption
10 - set (device) palette
13 - set player, whose colors to use

Pcx Subtypes:
11 - set file name
10 - set (device) palette
13 - set player, whose colors to use

Scroll Subtypes:
49 - set Tick
50 - set TicksCount

Edit Subtypes:
51 - set MaxLength
3 - set Text
7 - get Text?

Frame Subtypes:
8 - set color
63 - set Fill

Unprepared commands:

Type = 8   - mouse left
Type = 16  - mouse left up
Type = 32  - mouse right
Type = 64  - mouse right up

Prepared commands:

Type 512 subtypes:
[   10 - close dialog]
[   12 - left mouse button push]
[   13 - left mouse button release]
[   14 - right mouse button push] 

Type 4 - mouse move (show hint)
Type 1 - key down (subtype = unk_keycode)
Type 2 - key up (subtype = unk_keycode)
Type 0 - for timing, happens each cycle

video is drawn in VMT[5]

State flags:
4 - Visible
2 - React on clicks
8 or 0x1000 - Don't react on clicks?
0x20 - Edit doesn't react on clicks

Item VMT:
0   destroy
1   ?
2   ProcessCmd
3   ?
4   Draw
5   GetHeight
6   GetWidth
7   ShowHint (callParentVMT4)
8   ?
9   SetEnabled
10  GetFocus
11  LoseFocus
12  (De)Activate
13  ? (SetText in Edit)

Dlg VTable: 63A6A8


]]