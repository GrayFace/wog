
local options = {
	"Fish shaped crackers",
	"Fish shaped candies",
	"Fish shaped ethyl benzene",
	"12 medium geosynthetic membranes",
	"Unsaturated polyester resin",
	"Injector needle driver",
}

dlg = dialogs.new{}
dlg:Add(
	dialogs.AlignV{Margin = 19,
		dialogs.CheckGroup{
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

dlg:Show()
