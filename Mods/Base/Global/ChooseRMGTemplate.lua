-- By GrayFace
local i4, i2, i1, u4, u2, u1, call, pchar = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.call, mem.pchar

local TEXT = require "text_ChooseRMGTemplate"

mem.autohook2(0x549E6E, function(d)
	if not Options.ChooseRMGTemplate then
		return
	end
	local texts = {}
	local rmg = d.esi
	for ptemplate = u4[rmg + 0x10D4], u4[rmg + 0x10D8] - 1, 4 do
		texts[#texts + 1] = pchar[u4[ptemplate] + 4]
	end
	local t = dialogs.CheckBoxesDialog{Texts = texts, SelectedItem = d.edx + 1, Radio = true,
	                                   Caption = TEXT.Caption, CancelButton = true}
	if t.Result then
		d.edx = t.SelectedItem - 1
	else
		d.esp = d.ebp
		d.ebp = d:pop()
		assert(d:pop() == 0x54C511) -- return address 54C511
		i4[d.ebp - 4] = -1
		call(0x5382E0, 1, rmg)
		d.edi = d:pop()
		d.esi = d:pop()
		d.eax = -1  -- error value for which there's no error message prepared
		d:push(0x54C52E)
		return true
	end
end)
