-- Shift all speeds, so old Normal becomes new Slow, old Fast becomes new Normal, new Fast is even faster

local r4 = mem.r4

function global.events.EnterContext()
	if context ~= "map" then
		return
	end

	mem.IgnoreProtection(true)

	-- r4[0x63CF7C] = 0.63  -- keep slow speed at 1 for graphics tests
	r4[0x63CF7C+4] = Options.FasterCombat and 0.4 or 0.63
	r4[0x63CF7C+8] = Options.FasterCombat and 0.25 or 0.4

	mem.IgnoreProtection(false)
end
