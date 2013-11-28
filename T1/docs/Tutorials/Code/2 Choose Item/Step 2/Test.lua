
dlg = dialogs.new{}
dlg:Add(
	dialogs.AlignV{
		Margin = 40, AlignX = 0.5, MinWidth = 350,
		
		dialogs.Text{Text = "Caption", ExpandWidth = true, Font = "MedFont.fnt", Color = 19},
		15,
		dialogs.Text{Text = "Main Text", ExpandWidth = true, Font = "MedFont.fnt"},
		25,
		dialogs.AlignH{
			SpaceX = 25,
			dialogs.AlignV{
				AlignX = 0.5,
				dialogs.Def{File = "twcrport.def", Frame = 6},
				dialogs.Text{Text = "Text", ExpandWidth = true},
			},
			dialogs.AlignV{
				AlignX = 0.5,
				dialogs.Def{File = "twcrport.def", Frame = 6},
				dialogs.Text{Text = "Text", ExpandWidth = true},
			},
			dialogs.AlignV{
				AlignX = 0.5,
				dialogs.Def{File = "twcrport.def", Frame = 6},
				dialogs.Text{Text = "Text", ExpandWidth = true},
			},
		},
		30,
		dialogs.AlignLayers{AlignX = 0.5, AlignY = 0.5,
			dialogs.Pcx{File = "box64x30.pcx"},
			dialogs.Button{Name = "Btn", File = "iokay.def", CloseDialog = true, Id = 30722, HotKey = 28},
		},
	}
)

dlg:Show()
