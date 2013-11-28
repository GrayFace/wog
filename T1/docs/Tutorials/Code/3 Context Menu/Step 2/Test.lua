
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
		dialogs.CheckGroup{
			ExpandWidth = 1,
			Texts = options,
			CloseDialog = true,
			States = 1,  -- always off
			BorderHeight = 8,
			Border = true,
			FillVisible = false,
		},
	},
	nil
)

local main = dlg.Items.Main
main:NeedSize()  -- ensure Width and Height are set
dlg.Width = main.Width
dlg.Height = main.Height

dlg:Show()
