
local AllowCancel = true

local choices = {
	dialogs.Pcx{File = "HPL002KN.pcx", Caption = "Text"},
	dialogs.Def{File = "twcrport.def", Frame = 6, Caption = "Text"},
	dialogs.Def{File = "artifact.def", Frame = 6, Caption = "Text"},
}

dlg = dialogs.new{}
dlg:Add(
	dialogs.AlignV{
		Margin = 40, AlignX = 0.5, MinWidth = 350,
		
		dialogs.Text{Text = "Caption", ExpandWidth = true, Font = "MedFont.fnt", Color = 19},
		15,
		dialogs.Text{Text = "Main Text", ExpandWidth = true, Font = "MedFont.fnt"},
		25,
		dialogs.AlignH{Name = "List", SpaceX = 25, AlignY = 0.5,
		},
		25,
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

local list = dlg.Items.List
local lastFrame

local function FrameClick(t)
	if lastFrame then
		lastFrame.Visible = false
	end
	lastFrame = t
	t.Visible = true
	dlg.Items.Btn.Disabled = false
	dlg:Redraw()
end

for i, it in ipairs(choices) do
	list:AddItem(dialogs.AlignV{AlignX = 0.5, SpaceY = 3,
		dialogs.AlignLayers{
			dialogs.AlignH{it, Margin = 3},
			dialogs.Frame{Index = i, Visible = false, OnClick = FrameClick,
				Color = color.GamePal(45), ExpandWidth = 1, ExpandHeight = 1,
			},
		},
		dialogs.Text{Text = it.Caption, ExpandWidth = true},
	})
end

dlg:Show()
