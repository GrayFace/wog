
dlg = dialogs.new{BackgroundPcx = "QuestLog.pcx", DropShadow = false}
dlg:Add(
	dialogs.Def{X = 36+5, Y = 112+5, File = "twcrport.def", Frame = 5},
	dialogs.Text{X = 36+5+58+8, Y = 112+5, Height = 64, Width = 333-5-(36+5+58+8), Align = 4, Text = "Archers are delicious"},
	dialogs.ScrollBar{X = 335, Y = 112, Height = 343},
	dialogs.Button{X = 324, Y = 470, File = "QLexit.def", CloseDialog = true, HotKey = 28},
	nil
)

dlg:Show()
