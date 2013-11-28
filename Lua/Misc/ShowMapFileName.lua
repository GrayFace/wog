local i4, i2, i1, u4, u2, u1, call = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.call

local TextHandle, MapNameReady

mem.hook(0x57A745, function(d)
	MapNameReady = false
	TextHandle = call(0x5BC6A0, 1, d:getparams(1, 11))
	d.eax = TextHandle
	d:ret(0x2C)
end)

mem.hook(0x585A3F, function(d)
	MapNameReady = true
	d:push(0x583850)
end)

mem.hook(0x58385B, function(d)
	if u1[d.ebx + 100] == 0 and MapNameReady then
		call(0x57CA50, 1, TextHandle, u4[d.ebx + 0x1060] + 0x33D)
	end

	d.eax = i4[d.ebx + 0x1054]
end, 6)
