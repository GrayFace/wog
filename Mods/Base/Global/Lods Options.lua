
function global.events.EnterContext()
	if context ~= "map" then
		return
	end
	if Options.BrighterTrees then
		LD:T(?v, 10, ModName.."/Lods/Trees.lod")
	end
	if Options.NewGrid then
		LD:T(?v, 10, ModName.."/Lods/Grid.lod")
	end
end
