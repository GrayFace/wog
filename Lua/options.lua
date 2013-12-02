
local _G = _G
local internal = debug.getregistry()

local print = print
local Message = Message
local type = type
local unpack = unpack
local error = error
local pairs = pairs
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
local min = math.min
local max = math.max
local format = string.format
local string_match = string.match
local string_lower = string.lower
local string_find = string.find
local string_sub = string.sub
local table_copy = table.copy
local table_concat = table.concat
local path_find = path.find
local path_FindFirst = path.FindFirst
local path_AddSlash = path.AddSlash
local io_SaveString = io.SaveString
local io_LoadString = io.LoadString
local ReadIniString = ReadIniString
local loadfile = loadfile
local loadstring = loadstring
local debug_getinfo = debug.getinfo

local i4 = mem.i4
local mem_copy = mem.copy
local LoadTextTable = LoadTextTable
local WogOptionsPtr = internal.WogOptionsPtr

local SetupPath = path_AddSlash(ReadIniString("WoGification", "SetupDir", AppPath.."Data/zvs/Setup/"))
SetupPath = SetupPath..ReadIniString("WoGification", "SetupFile", "Setup.lua")

local UN_P = ERM.UN.P
local ?v = ?v

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local OptionMods = {}
internal.OptionMods = OptionMods
local OptionDefs = {}
internal.OptionDefs = OptionDefs
local OptionERM = {}
internal.OptionERM = OptionERM

function internal.NewOptionsTable(t)
	return {Active = t, State = table_copy(t)}
end

function internal.CopyOptionsTable(t)
	return {Active = table_copy(t.Active), State = table_copy(t.State)}
end

----------- Options access

local function GetWogOption(k, erm)
	local v = internal.context == "map" and UN_P(erm, ?v) or i4[WogOptionsPtr + erm*4]
	if erm >= 1 and erm <= 4 then
		v = (v == 0 and 1 or 0)
	end
	if v == 0 or v == 1 then
		local v1 = (v == 1)
		local o = OptionDefs[k]
		if not o.Values then
			v = v1
		else
			for _, a in ipairs(o.Values) do
				if a.Value == v1 then
					v = v1
					break
				end
			end
		end
	end
	return v
end

local function SetWogOption(erm, v)
	v = tonumber(v) or v and 1 or 0
	if erm >= 1 and erm <= 4 then
		v = (v == 0 and 1 or 0)
	end
	if internal.context == "map" then
		UN_P(erm, v)
	else
		i4[WogOptionsPtr + erm*4] = v
	end
end

local function AddModName(name)
	if string_find(name, ".", 1, true) then
		return name
	end
	local mod = assert(getfenv(debug_getinfo(3, 'f').func).ModName)
	return format("%s.%s", mod, name)
end

local function Options_index(_, k)
	k = AddModName(k)
	local erm = OptionERM[k]
	if erm then
		return GetWogOption(k, erm)
	end
	return internal.CurOptions.Active[k]
end

local function Options_newindex(_, k, v)
	k = AddModName(k)
	local erm = OptionERM[k]
	if erm then
		SetWogOption(erm, v)
	else
		internal.CurOptions.Active[k] = v
	end
end

_G.Options = setmetatableW({}, {__index = Options_index, __newindex = Options_newindex})

function internal.CopyToWogOptions()
	local base = WogOptionsPtr + 1000*4
	for k, erm in pairs(OptionERM) do
		local v = internal.CurOptions.Active[k]
		i4[base + erm*4] = tonumber(v) or v and 1 or 0
	end
	mem_copy(WogOptionsPtr, base, 1000*4)
end

function internal.CopyFromWogOptions()
	for k, erm in pairs(OptionERM) do
		local v = GetWogOption(k, erm)
		internal.CurOptions.Active[k] = v
		internal.CurOptions.State[k] = v
	end
end

----------- Options definition

local CURRENT, INFO = -1, 0

local function CreateModDefs(mod)
	OptionMods[mod] = {}
	OptionDefs[mod..".Enabled"] = {Name = "Enabled", Text = mod, On = true}
end

local function LoadDefsFile(mod, fname)
	local Cats = OptionMods[mod]
	local modPt = mod.."."
	
	local function NotGroup(t, pattern, tt)
		local name = string_match(t.Name, pattern)
		if not name then
			return true
		end
		t.Text = t.Text or name
		if name == "" then
			name = t.Text
		end
		t.Name = name
		local t1 = tt[name]
		if not t1 then
			t1 = {[INFO] = t}
			tt[name] = t1
			tt[#tt + 1] = t1
		end
		tt[CURRENT] = t1
	end
	
	local function NeedCur(t)
		t[1] = t[1] or {}
		t[CURRENT] = t[CURRENT] or t[#t]
		return t[CURRENT]
	end
	
	for _, t in ipairs(LoadTextTable(fname)) do
		if not t.Name or t.Name == "" then
			-- do nothing
		elseif t.Name == "Enabled" then
			OptionDefs[mod..".Enabled"] = t
			t.On = true
		elseif NotGroup(t, "^category:(.*)", Cats) and NotGroup(t, "^group:(.*)", NeedCur(Cats)) then
			local group = NeedCur(NeedCur(Cats))
			local name = modPt..t.Name
			local it = OptionDefs[name]
			if it then
				-- value of an option
				assert(t.Value ~= nil and it.Value ~= nil, "option already exists: "..name)
				local v = it.Values
				if not v then
					v = {}
					it.Values = v
					if it.Value ~= nil then
						v[1] = table_copy(it)
						it.Text = nil
					end
				end
				v[#v + 1] = t
			else
				-- normal option
				if string_find(t.Name, "[%.\r\n\"%z]") or string_sub(t.Name, 1, 1) == "-" then
					error("illigal option name: "..tostring(t.Name))
				end
				t.Text = t.Text or t.Name
				if t.ERM then
					local n = tonumber(t.ERM)
					OptionERM[name] = n
					if not n or n < 0 or n > 999 then
						error("invalid ERM value in option "..name)
					end
				end
				OptionDefs[name] = t
				group[#group + 1] = t
			end
			t.Name = name
		end
	end
end

local GoodValTypes = {number = 1, boolean = 1, string = 2}

local function ProcessDefs()
	local defaults = {}
	for k, t in pairs(OptionDefs) do
		defaults[k] = not not t.On
		if t.Values then
			for _, t1 in ipairs(t.Values) do
				if t1.On then
					defaults[k] = t1.Value
				end
				local okVal = GoodValTypes[type(t1.Value)]
				if not okVal or t.ERM and okVal ~= 1 then
					error("option has an invalid value: "..t.Name)
				end
			end
		end
	end
	internal.CurOptions = internal.NewOptionsTable(defaults)
	-- load default options files
	internal.DefOptions = internal.CopyOptionsTable(internal.CurOptions)
	-- make dependance tables on demand
	
	-- check ERM indeces conflicts
	-- local erm = {}
	-- for k, v in pairs(OptionERM) do
		-- if erm[v] then
			-- error('2 options with the same ERM index: '..erm[v]..", "..k)
		-- end
		-- erm[v] = k
	-- end
end


local function SetupOptions(t)
	-- !!! dependancies
	local o = internal.CurOptions.Active
	for k, v in pairs(t.Active) do
		o[k] = v
	end
	local o = internal.CurOptions.State
	for k, v in pairs(t.State or {}) do
		o[k] = v
	end
end
internal.SetupOptions = SetupOptions

local function DoSaveOptions(o, t)
	for k, v in pairs(o) do
		t[#t + 1] = "\t\t["
		t[#t + 1] = tostring2(k)
		t[#t + 1] = "] = "
		t[#t + 1] = tostring2(v)
		t[#t + 1] = ",\n"
	end
end

function internal.SaveOptions(fname)
	local t, o = {}, internal.CurOptions
	t[#t + 1] = "return {\n\tActive = {\n"
	DoSaveOptions(o.Active, t)
	t[#t + 1] = "\t}\n\}"
	-- t[#t + 1] = "\t},\n\tState = {\n"
	-- DoSaveOptions(o.State, t)
	-- t[#t + 1] = "\t}\n}"
	io_SaveString(fname or SetupPath, table_concat(t))
end

-- local function LoadOptions(f, mod)
	-- local t = setmetatable({}, {__index = OptionsIndex})
	-- setfenv(f, {})
	-- local on, states = f()
	-- if mod then
		-- local k, v = next(t)
		-- if next(t, k) or respectName then
			-- t = {[mod] = t[mod]}
		-- else  -- ignore mod name in Defaults.lua file
			-- t = {[mod] = v}
		-- end
	-- end
	-- return t
-- end

function internal.LoadOptions(fname)
	local f = assert(loadfile(fname))
	setfenv(f, {})
	SetupOptions(f())
	-- make sure there are no conflicts
end

function internal.SaveGameOptions()
	local t = {}
	for k, v in pairs(internal.CurOptions.Active) do
		if not OptionERM[k] then
			t[k] = v
		end
	end
	return t
end

function internal.LoadGameOptions(t)
	local cur = internal.CurOptions.Active
	internal.CurOptions.Active = t
	for k, v in pairs(cur) do
		if OptionDefs[k] and OptionDefs[k].Cosmetic then
			t[k] = v
		end
	end
end

function internal.LoadModOptions(mod, path)
	if path_FindFirst(path.."Options\\*.txt") then  -- !!! temporary, until mods on/off is done
		CreateModDefs(mod)
	end
	for f in path_find(path.."Options\\*.txt") do
		LoadDefsFile(mod, f)
	end
	-- for f in path_find(path.."Options\\Defaults.lua") do
		-- SetupOptions(LoadOptions(assert(loadfile(f)), mod))
	-- end
end

function internal.AllOptionsLoaded()
	ProcessDefs()
	local fname = path_FindFirst(SetupPath)
	if fname then
		internal.LoadOptions(fname)
		internal.CopyToWogOptions()
	else
		local fname = path_AddSlash(ReadIniString("WoGification", "Options_File_Path", ".\\"))
		fname = fname..ReadIniString("WoGification", "Options_File_Name", "WoGSetupEx.dat")
		fname = path_FindFirst(fname) or path_FindFirst("WoGSetupEx.dat")
		if fname then
			local s = io_LoadString(fname)
			mem_copy(WogOptionsPtr, s, min(#s, 1000*4))
			mem_copy(WogOptionsPtr + 1000*4, WogOptionsPtr, 1000*4)
		end
		internal.CopyFromWogOptions()
	end
	internal.BaseOptions = internal.CopyOptionsTable(internal.CurOptions)
end
