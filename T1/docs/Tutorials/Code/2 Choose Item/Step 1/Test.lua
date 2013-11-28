
dlg = dialogs.new{}
dlg:Add(
	dialogs.AlignV{
		Margin = 50, SpaceY = 20, AlignX = 0.5,
		
		dialogs.Text{Text = "Caption", ExpandWidth = 1},
		dialogs.AlignH{
			dialogs.Def{File = "twcrport.def", Frame = 6},
			dialogs.Def{File = "twcrport.def", Frame = 6},
			dialogs.Def{File = "twcrport.def", Frame = 6},
		},
		dialogs.Button{File = "iokay.def", CloseDialog = true, Id = 30722, HotKey = 28},
	}
)

dlg:Show()
