
local AllowCancel = true

dlg = dialogs.new{}
dlg:Add(
	dialogs.AlignV{
		Margin = 40, AlignX = 0.5, MinWidth = 350,
		
		dialogs.Text{Text = "Caption", ExpandWidth = true, Font = "MedFont.fnt", Color = 19},
		15,
		dialogs.Text{Text = "Main Text", ExpandWidth = true, Font = "MedFont.fnt"},
		25,
		dialogs.AlignH{
			SpaceX = 25, AlignY = 0.7,
			dialogs.AlignV{
				AlignX = 0.5,
				dialogs.Pcx{File = "HPL002KN.pcx"},
				dialogs.Text{Text = "Text", ExpandWidth = true},
			},
			dialogs.AlignV{
				AlignX = 0.5,
				dialogs.Def{File = "twcrport.def", Frame = 6},
				dialogs.Text{Text = "Text", ExpandWidth = true},
			},
			dialogs.AlignV{
				AlignX = 0.5,
				dialogs.Def{File = "artifact.def", Frame = 6},
				dialogs.Text{Text = "Text", ExpandWidth = true},
			},
		},
		30,
		dialogs.AlignH{AlignX = 0.5, AlignY = 0.5, SpaceX = 16,
			dialogs.AlignLayers{AlignX = 0.5, AlignY = 0.5,
				dialogs.Pcx{File = "box64x30.pcx"},
				dialogs.Button{Name = "Btn", File = "iokay.def", CloseDialog = true, Id = 30722, HotKey = 28, Disabled = true},
			},
			AllowCancel and dialogs.AlignLayers{AlignX = 0.5, AlignY = 0.5,
				dialogs.Pcx{File = "box64x30.pcx"},
				dialogs.Button{File = "icancel.def", CloseDialog = true, Id = 30721, HotKey = 1},
			} or nil,
		},
	}
)

dlg:Show()