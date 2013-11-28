local _G = _G
local internal = debug.getregistry()

local pairs = pairs
local table_copy = table.copy
local i4, i2, i1, u4, u2, u1 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1
local Message = Message
local ErrorMessage = debug.ErrorMessage
local events = events
local dialogs_MainMenuButton = dialogs.MainMenuButton
local malloc = mem.malloc
local mem_copy = mem.copy
local mem_free = mem.free

CampaignPacks = {}
local campaigns = CampaignPacks

internal.AutoPersist.ChosenCampaign = true

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local function MainMenuCampaign(t)
	internal.ChosenCampaign = nil
	for name, c in pairs(campaigns) do
		if c.Button then
			local but = dialogs_MainMenuButton(table_copy(c.Button))
			t:Add(but)
			function but:OnClick(cmd)
				if cmd.Flags:And(2) == 0 then
					cmd.Type = 512
					cmd.Subtype = 10
					cmd.ItemId = 0
					cmd.Result = 2
				end
				internal.ChosenCampaign = name
			end
			function but:OnRightClick(cmd)
				Message(but.Hint or "Select "..name.." campaign", 4)
			end
		end
		function t:OnMouseMove(cmd)
			t:Redraw()
		end
	end
end

local function CampaignSubcampaigns(t)
end

function events.ShowDialog(t)
	if t.ClassPtr == 0x63BC08 then
		return MainMenuCampaign(t)
	elseif t.ClassPtr == 0x63BCA4 then
		return CampaignSubcampaigns(t)
	end
end


-- prepare pictures, videos, strings

local backups = {}
local allocs = {}

local function ChangeStr(p, str)
	
	if backups[p] then
		u4[p] = backups[p]
	else
		backups[p] = u4[p]
	end
	
	if str ~= nil then
		local al = malloc(#str + 1)
		mem_copy(al, str, #str + 1)
		u4[p] = al
		if allocs[p] then
			mem_free(allocs[p])
		end
		allocs[p] = al
	end
end

local CampReindex = {3, 4, 1, 2, 5, 6, 7}

local function BeforeCampaignSubcampaigns()
	local pack = campaigns[internal.ChosenCampaign]
	if internal.ChosenCampaign ~= nil and not pack then
		ErrorMessage(('Campaign "%s" not found'):format(internal.ChosenCampaign))
	end
	local camps = (pack or {}).Campaigns or {}

	-- mem.IgnoreProtection(true)

	ChangeStr(0x45E987, (pack or {}).Picture)
	for i = 0, 6 do
		local c = camps[CampReindex[i + 1]] or {}
		ChangeStr(0x6A6058 + i*8, c.Label)
		ChangeStr(0x6A605C + i*8, c.Label)
		ChangeStr(0x66CB10 + i*4, c.File)
		ChangeStr(0x66C8C0 + i*0x50, c.Picture)
		ChangeStr(internal.VidArr + 0x190 + i*0x14, pack and (c.Video or "") or nil)
	end

	-- mem.IgnoreProtection(false)
end

_G.mem.hook(0x45E620, function(d)
	BeforeCampaignSubcampaigns()
	local ret = d:pop()
	d:push(d.ebp)
	d.ebp = d.esp
	d:push(-1)
	d:push(ret)
end)

-- Ignore voices if they aren't present
internal.SilentLoadWav[0x488CDC] = true

-- Remove voices in custom campaigns (temporary!!!)
_G.mem.hook(0x488BD4, function(d)
	if internal.ChosenCampaign == nil and u1[d.edi] ~= 0 then
		u4[d.esp] = 0x488BDB
	end
end)

--[[
Need change:
Ref{0x488BAC, 0x4BF480, 0x4BFE09, 0x4C942F, 0x4D91F6, 0x4D9252, 0x4DAAFF, 0x4DAFAF, 0x4DB95E, 0x4DBDD0, 0x4E1DC4, 0x4F1D01, 0x5CEA09, 0x5F19D6, 0x5F2E03}
EndRef{0x52487E}
4D9250
]]

--[[
BigCampaignIndex:
20 = custom
13 - 19 = SoD

]]
