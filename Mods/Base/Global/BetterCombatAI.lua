-- By GrayFace
local i4, i2, i1, u4, u2, u1, call = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.call

AIMulByMonType = {
	[145] = 1/100,
	[146] = 1/5,
	[147] = 1/9,
	[148] = 1/9,
}

mem.hookaround(0x4357E0, 1, 6, 
	function(d, CombatUnk, mon1, mon1hp, mon2, mon2hp, canShoot, unreal)
		d:push(d.ebp)
		d.ebp = d.esp
		d.esp = d.esp - 8
		d:push(0x4357E6)
	end,
	function(d, CombatUnk, mon1, mon1hp, mon2, mon2hp, canShoot, unreal)
		local mul = mon2 ~= 0 and AIMulByMonType[i4[mon2 + 0x34]]
		if mul then
			d.eax = d.eax*mul
		end
	end, 6
)