-- By GrayFace

function global.events.EnterMap()
	if Options.Trees == "bright" then
		LD:T(?v, 11, ModName.."/Lods/Trees.lod")
	elseif Options.Trees == "move" then
		LD:T(?v, 11, ModName.."/Lods/TreeMove.lod")
	end
	if Options.NewGrid then
		LD:T(?v, 11, ModName.."/Lods/Grid.lod")
	end
end
