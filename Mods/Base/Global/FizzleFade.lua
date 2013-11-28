
local i4, i2, i1, u4, u2, u1, i8, u8 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.i8, mem.u8
local call = mem.call

-- Fade: use fizzle

mem.autohook(0x603210, function(d)
	local ret = u4[d.esp]
	if ret >= 0x44D770 and ret < 0x44D990 or ret >= 0x5991B0 and ret < 0x599350 or not Options.FizzleFade then
		return
	end
	if i4[d.esp + 4] == 1 then
		mem.call(0x603280, 1, d.ecx, 0, 0, 800, 600)  -- SaveFizzleSource
		i4[d.ecx + 0x48] = (u1[d.esp + 12] ~= 0 and 1 or 0)
	else
		i4[d.ecx + 0x48] = 0
		if u4[d.ecx + 0x4C] ~= 0 then
			mem.call(0x603380, 1, d.ecx, 0, 0, 800, 600, 25)  -- FizzleForward (25*8 msec)
		end
	end
	d:ret(0xC)
	return true
end)


--[[
local function FadeOut(d)
	i4[d.ecx + 0x48] = 1
	mem.call(0x603280, 1, d.ecx, 0, 0, 800, 600)  -- SaveFizzleSource
	d:ret(0xC)
end

local function FadeIn(d)
	if u4[d.ecx + 0x4C] == 0 then
		return d:push(0x603210)
	end
	i4[d.ecx + 0x48] = 0
	mem.call(0x603380, 1, d.ecx, 0, 0, 800, 600, 25)  -- FizzleForward
	d:ret(0xC)
end

-- town transition
mem.hook(0x5C6FAE, FadeOut)
mem.hook(0x5C71F0, FadeIn)
mem.hook(0x5C7609, FadeOut)
mem.hook(0x4B0AE6, FadeIn)
-- another transition

]]






--mem.hook(0x5C708E, FadeOut)


-- mem.autohook(0x4B0AB9, function(d)
	-- --print(u4[i4[0x6992D0] + 0x4C])
	-- if u4[i4[0x6992D0] + 0x4C] ~= 0 then
		-- mem.call(0x603380, 1, i4[0x6992D0], 0, 0, 800, 600, 300)  -- FizzleForward
	-- end
-- end)

-- u1[0x603809+2] = 15
-- u1[0x603731+2] = 30

-- u1[0x60393A+2] = 30  -- in

-- i4[603A42+1] = i4[603A42+1] 
