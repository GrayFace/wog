-- by GrayFace

-- Sets thinking radius to WoG 3.58 default (4096) instead of SoD default (32000).
-- WoG 3.59 normally uses SoD default.

if new and Options.FasterAI then
	UN:J4(0x1000)
end
