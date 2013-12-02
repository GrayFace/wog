
local i4, i2, i1, u4, u2, u1, i8, u8 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.i8, mem.u8
local call = mem.call

-- Fade: use fizzle

mem.autohook(0x603210, function(d)
	local ret = u4[d.esp]
	if not Options.FizzleFade or ret >= 0x44D770 and ret < 0x44D990 
	   or ret >= 0x5991B0 and ret < 0x599350 or ret == 0x4F055A then
		return
	end
	if i4[d.esp + 4] == 1 then
		mem.call(0x603280, 1, d.ecx, 0, 0, 800, 600)  -- SaveFizzleSource
		if u1[d.esp + 12] ~= 0 then
			i4[d.ecx + 0x48] = 1
		end
	else
		i4[d.ecx + 0x48] = 0
		if u4[d.ecx + 0x4C] ~= 0 then
			mem.call(0x603380, 1, d.ecx, 0, 0, 800, 600, 25)  -- FizzleForward (25*8 msec)
		end
	end
	d:ret(0xC)
	return true
end)
