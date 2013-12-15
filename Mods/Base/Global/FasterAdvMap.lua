-- By GrayFace

local i4 = mem.i4

function global.events.EnterMap()
	local delay = Options.FasterAdvMap and 150 or 180
	i4[0x408A7E+3] = delay
	i4[0x408A97+1] = delay
	
	i4[0x419719+3] = delay
	i4[0x419732+1] = delay

	i4[0x419374+3] = delay
	i4[0x41938D+1] = delay
	
	i4[0x40A4E2+3] = delay
	i4[0x40A4FB+1] = delay
	
	i4[0x40A259+3] = delay
	i4[0x40A272+1] = delay
	
	i4[0x4098BC+3] = delay
	i4[0x4098D5+1] = delay
	
	i4[0x408F3A+3] = delay
	i4[0x408F53+1] = delay
	
	i4[0x4061DB+3] = delay
	i4[0x4061F4+1] = delay
	
	i4[0x4060C7+3] = delay
	i4[0x4060E0+1] = delay
	
	i4[0x40F213+3] = delay
	i4[0x40F22C+1] = delay
end
