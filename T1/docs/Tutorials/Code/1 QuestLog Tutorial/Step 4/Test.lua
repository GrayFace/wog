
dlg = dialogs.new{BackgroundPcx = "QuestLog.pcx", DropShadow = false}
dlg:Add(
	dialogs.AlignV{
		X = 36, Y = 112, Width = 333 - 36, Height = 343,
		Margin = 5, SpaceY = 10,
		dialogs.AlignH{
			ExpandWidth = 1, SpaceX = 8,
			dialogs.Def{File = "twcrport.def", Frame = 5},
			dialogs.Text{ExpandHeight = 1, ExpandWidth = 1, Align = 4, Text = "Archers are delicious"},
		},
		dialogs.AlignH{
			ExpandWidth = 1, SpaceX = 8,
			dialogs.Def{File = "twcrport.def", Frame = 6},
			dialogs.Text{ExpandHeight = 1, ExpandWidth = 1, Align = 5, Text = "Griffins as well"},
			dialogs.Def{File = "twcrport.def", Frame = 7},
		},
		dialogs.AlignH{
			ExpandWidth = 1, SpaceX = 8, AlignX = 0.5,
			dialogs.Def{File = "twcrport.def", Frame = 24},
			dialogs.Def{File = "twcrport.def", Frame = 25},
		},
		dialogs.AlignH{
			ExpandWidth = 1, SpaceX = 8,
			dialogs.Def{File = "twcrport.def", Frame = 54},
			dialogs.AlignV{ExpandWidth = 1},
			dialogs.Def{File = "twcrport.def", Frame = 55},
			dialogs.Def{File = "twcrport.def", Frame = 56},
			dialogs.AlignV{ExpandWidth = 1},
			dialogs.Def{File = "twcrport.def", Frame = 57},
		},
		
		dialogs.AlignH{Shaded = true,
			ExpandWidth = 1, SpaceX = 8, --ExpandHeight = 1,
			dialogs.Def{File = "twcrport.def", Frame = 5},
			dialogs.Text{ExpandHeight = 1, ExpandWidth = 1, Align = 4, Text = "Archers are delicious"},
		},
		dialogs.AlignH{Shaded = true,
			ExpandWidth = 1, SpaceX = 8,
			dialogs.Def{File = "twcrport.def", Frame = 6},
			dialogs.Text{ExpandHeight = 1, ExpandWidth = 1, Align = 5, Text = "Griffins as well"},
			dialogs.Def{File = "twcrport.def", Frame = 7},
		},
		dialogs.AlignH{Shaded = true,
			ExpandWidth = 1, SpaceX = 8, AlignX = 0.5,
			dialogs.Def{File = "twcrport.def", Frame = 24},
			dialogs.Def{File = "twcrport.def", Frame = 25},
		},
		dialogs.AlignH{Shaded = true,
			ExpandWidth = 1, SpaceX = 8,
			dialogs.Def{File = "twcrport.def", Frame = 54},
			dialogs.AlignV{ExpandWidth = 1},
			dialogs.Def{File = "twcrport.def", Frame = 55},
			dialogs.Def{File = "twcrport.def", Frame = 56},
			dialogs.AlignV{ExpandWidth = 1},
			dialogs.Def{File = "twcrport.def", Frame = 57},
		},
		ScrollBar = dialogs.ScrollBar{X = 335, Y = 112, Height = 343, CatchKeys = true},
	},
	dialogs.Button{X = 324, Y = 470, File = "QLexit.def", CloseDialog = true, HotKey = 28},
	nil
)
-- debug.start()
dlg:Show()
