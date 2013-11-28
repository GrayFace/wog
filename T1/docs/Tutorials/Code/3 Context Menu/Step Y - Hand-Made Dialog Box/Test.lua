
local options = {
	"Fish shaped crackers",
	"Fish shaped candies",
	"Fish shaped ethyl benzene",
	"12 medium geosynthetic membranes",
	"Unsaturated polyester resin",
	"Injector needle driver",
}

-- need transparent full screen dialog for clicks reaction
dlg = dialogs.new{ExpandWidth = 1, ExpandHeight = 1, Transparent = true,
	OnClick = function(t, cmd)
		cmd.CloseDialog = cmd.Item.Name == "DialogArea" or cmd.Item.Shaded
	end,
	OnRightClick = function(t, cmd)
		cmd.CloseDialog = cmd.Item.Name == "DialogArea" or cmd.Item.Shaded
	end,
	OnKeyUp = function(t, cmd)
		cmd.CloseDialog = cmd.Key == 1
	end,
}

dlg:Add(
	dialogs.AlignLayers{Name = "Content", X = 300, Y = 200,
		-- dialog shadow
		dialogs.AlignV{ExpandWidth = 1, ExpandHeight = 1, Margin = 9, MarginRight = -7, MarginBottom = -7,
			dialogs.Area{ExpandWidth = 1, ExpandHeight = 1, Shaded = true},
		},
		dialogs.AlignV{ExpandWidth = 1, ExpandHeight = 1, Margin = 8, MarginRight = -8, MarginBottom = -8,
			dialogs.Area{ExpandWidth = 1, ExpandHeight = 1, Shaded = true},
		},
		-- dialog border
		dialogs.Pcx{File = "diboxbck.pcx", Tile = true, ExpandWidth = 1, ExpandHeight = 1},
		dialogs.AlignV{ExpandWidth = 1, ExpandHeight = 1,
			dialogs.AlignH{ExpandWidth = 1,
				dialogs.Def{File = "dialgbox.def", Frame = 0, PlayerColor = -2},
				dialogs.Def{File = "dialgbox.def", Frame = 6, PlayerColor = -2, Tile = true, ExpandWidth = 1},
				dialogs.Def{File = "dialgbox.def", Frame = 1, PlayerColor = -2},
			},
			dialogs.AlignH{ExpandWidth = 1, ExpandHeight = 1,
				dialogs.Def{File = "dialgbox.def", Frame = 4, PlayerColor = -2, Tile = true, ExpandHeight = 1},
				dialogs.AlignH{ExpandWidth = 1},
				dialogs.Def{File = "dialgbox.def", Frame = 5, PlayerColor = -2, Tile = true, ExpandHeight = 1},
			},
			dialogs.AlignH{ExpandWidth = 1,
				dialogs.Def{File = "dialgbox.def", Frame = 2, PlayerColor = -2},
				dialogs.Def{File = "dialgbox.def", Frame = 7, PlayerColor = -2, Tile = true, ExpandWidth = 1},
				dialogs.Def{File = "dialgbox.def", Frame = 3, PlayerColor = -2},
			},
		},
		-- content
		dialogs.AlignV{Name = "Main",
			Margin = 19, AlignX = 0.5, AlignY = 0.5, MinWidth = 64*2, MinHeight = 64*2, SpaceY = 3,
			
			dialogs.CheckGroup{
				Name = "Group", ExpandWidth = 1,
				Texts = options,
				CloseDialog = true,
				States = 1,  -- always off
				BorderHeight = 8,
				Border = true,
				FillVisible = false,
			},
		},
		-- end of content
	},
	nil
)

local content = dlg.Items.Content
local main = dlg.Items.Main
local group = dlg.Items.Group
main:NeedSize()  -- ensure Width and Height are set
local x, y = UX:M(?v, ?v)  -- get mouse coordinates
local function Coord(x, w, sw, dx1, dx2)
	x = (x - dx1 <= sw - w and x - dx1 or x + dx2 + 1 - w)
	return math.max(0, math.min(x, sw - w))
end

-- this placement is a bit different from that of standard context menus,
-- due to thick border and optional header it works better.
-- (e.g. compare the behavior when mouse is close to the right side of the screen)
local d = -1
content.X = Coord(x, main.Width, ScreenWidth, d, d)
content.Y = Coord(y, main.Height, ScreenHeight, group.Y + 3, d)

local id = dlg:Show()
-- return index of selected item within the check group
local item = dlg.Items[id]
return item and item.ItemIndex
