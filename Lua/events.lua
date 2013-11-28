
local _G = _G
local internal = debug.getregistry()

local d_debug = debug.debug
local i4, i2, i1, u4, u2, u1 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1
local call = mem.call
local mem_hook = mem.hook
local mem_autohook = mem.autohook
local mem_autohook2 = mem.autohook2
local mem_nop = mem.nop
local IgnoreProtection = mem.IgnoreProtection

IgnoreProtection(true)

----------- No globals from this point ------------

local _NOGLOBALS

----------- Ctrl+F1 ------------

local GetKeyState = internal.GetKeyState

function internal.WindowProc(wnd, msg, wp, lp, std)
	if msg == 0x0100 and wp == 0x70 and GetKeyState(0x11) < 0 then  -- Ctrl+F1
		d_debug()
		return 0
	end
	return call(std, 0, wnd, msg, wp, lp)
end

----------- Ignore missing wav (for campaign voices) ------------

local SilentLoadWav = {}
internal.SilentLoadWav = SilentLoadWav

mem_hook(0x55C715, function(d)
	if SilentLoadWav[u4[u4[d.ebp] + 4]] then
		d.esp = d.esp + 8
		u4[d.esp] = 0x55C8BC
	else
		d.ecx = d.ebp - 0xCC
	end
end, 6)

----------- SoD bug: screen blinking on transactions ------------

-- returning to adv map screen
mem_nop(0x4B0AB4, 5)
mem_autohook(0x4B0B0C, function(d)
	if d.ebx == i4[0x6992B8] then
		call(0x4F8810, 1, u4[0x698A34])
	end
end)

-- battle start
mem_nop(0x462B8A, 5)
mem_autohook(0x462C1E, function(d)
	call(0x4F8810, 1, u4[0x698A34])
end)

-- misc
mem_nop(0x4075BB, 5)
mem_autohook(0x407677, function(d)
	call(0x4F8810, 1, u4[0x698A34])
end)

----------- SoD bug: Tavern rumors crash on game start ------------

i4[0x4CCC0B+1] = 255

----------- Fizzle: 16 grades instead of 8 ------------

u1[0x6035DD+2] = 16
u1[0x60348B+2] = 15
mem_autohook(0x60343D, function(d)
	i4[d.ebp + 0x18] = i4[d.ebp + 0x18]/2
end)

----------- Horizontal scroll support for keys ------------

mem_hook(0x59652E, function(d)
	local dlg = d.esi
	local old = u4[dlg + 0x58]
	u4[dlg + 0x58] = (i4[d.ebp + 8] == 0 and 0x7FFF7FFF or 0)
	call(0x596F40, 1, dlg)
	u4[dlg + 0x58] = old
	d.esp = d.esp + 4
	u4[d.esp] = 0x59665A
end)

-----------------------

IgnoreProtection(false)
