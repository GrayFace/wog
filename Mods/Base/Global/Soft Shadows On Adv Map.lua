-- By GrayFace

function global.events.EnterContext()
	if context == "map" then
		local on = Options.SoftShadow
		mem.i1[0x47D5C5+1] = on and 0x23 or 0x09
		mem.i1[0x47D45E+1] = on and 0x1F or 0x05
	end
end
