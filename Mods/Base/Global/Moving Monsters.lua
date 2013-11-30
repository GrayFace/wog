
function global.events.EnterContext()
	if context == "map" then
		local ptr = debug.getregistry().EnableMovingMonstersPtr
		mem.i4[ptr] = Options.MonStandAnim and 1 or 0
	end
end
