
dlg = dialogs.new{BackgroundPcx = "QuestLog.pcx", DropShadow = false}
dlg:Add(
	dialogs.AlignH{
		X = 36, Y = 112, Width = 333 - 36,
		Margin = 5, SpaceX = 8,
		dialogs.Def{File = "twcrport.def", Frame = 5},
		dialogs.Text{ExpandHeight = 1, ExpandWidth = 1, Align = 4, Text = "Archers are delicious"},
	},
	dialogs.ScrollBar{X = 335, Y = 112, Height = 343},
	dialogs.Button{X = 324, Y = 470, File = "QLexit.def", CloseDialog = true, HotKey = 28},
	nil
)

dlg:Show()
