
local options = {
	"Fish shaped crackers",
	"Fish shaped candies",
	"Fish shaped ethyl benzene",
	"12 medium geosynthetic membranes",
	"Unsaturated polyester resin",
	"Injector needle driver",
}

local function IsOutside(t, x, y)
	return x < t.X or x >= t.X + t.Width or y < t.Y or y >= t.Y + t.Height
end

dlg = dialogs.new{
	OnClick = function(t, cmd)
		cmd.CloseDialog = IsOutside(t, cmd.X, cmd.Y)
	end,
	OnRightClick = function(t, cmd)
		cmd.CloseDialog = IsOutside(t, cmd.X, cmd.Y)
	end,
	OnKeyUp = function(t, cmd)
		cmd.CloseDialog = cmd.Key == 1
	end,
}

dlg:Add(
	-- transparent area to catch clicks outside the dialog
	dialogs.Area{X = -ScreenWidth, Y = -ScreenHeight, Width = ScreenWidth*2, Height = ScreenHeight*2},
	-- content
	dialogs.AlignV{Name = "Main",
		Margin = 19, AlignX = 0.5, AlignY = 0.5, MinWidth = 64*2, MinHeight = 64*2, SpaceY = 3,
		
		dialogs.Text{Text = "Don't forget garnishes such as:", --[[ExpandWidth = 1,]] Font = "MedFont.fnt", Color = 19},
		dialogs.AlignH{Name = "Group", AlignY = 0.5, SpaceX = 1, ExpandWidth = 1,
			dialogs.CheckGroup{
				Texts = options,
				CloseDialog = true,
				States = 1,  -- always off
				BorderHeight = 8,
				Border = true,
				FillVisible = false,
				Height = 94,
				MarginTop = -1, MarginBottom = -1,
				ScrollBar = "Scroll",
			},
			dialogs.ScrollBar{Name = "Scroll", ExpandHeight = 1, CatchKeys = true},
		},
	},
	nil
)

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
dlg.X = Coord(x, main.Width, ScreenWidth, d, d)
dlg.Y = Coord(y, main.Height, ScreenHeight, group.Y + 3, d)
dlg.Width = main.Width
dlg.Height = main.Height

local id = dlg:Show()
-- return index of selected item within the check group
local item = dlg.Items[id]
return item and item.ItemIndex
