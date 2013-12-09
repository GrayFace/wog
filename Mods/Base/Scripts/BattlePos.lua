-- By GrayFace

version = 0.0

local ?v, ?z, BU = ?v, ?z, BU
local floor, abs, max = math.floor, math.abs, math.max

local function chk(v, max)
	return v and v >= 0 and v <= max
end

function hex2xy(pos)
	if chk(pos, 186) then
		local y = floor(pos / 17)
		local x = pos % 17 * 2 + (y + 1) % 2
		return x, y
	end
end

function xy2hex(x, y)
	if chk(x, 33) and chk(y, 10) and (x + y) % 2 == 1 then
		return floor(x / 2) + y * 17
	end
end

function distance(pos1, pos2)
	local x1, y1 = P.hex2xy(pos1)
	local x2, y2 = P.hex2xy(pos2)
	if y1 and y2 then
		local x = abs(x1 - x2)
		local y = abs(y1 - y2)
		return y + max(0, (x - y)/2)
	end
end

function isclear(pos, AvoidCorpses)
	if chk(pos, 186) then
		local x = pos % 17
		return x ~= 0 and x ~= 16 and -- inaccessible first and last hexes in a row
		       BU:O(pos, ?v) == 0 and -- obstacle
		       BU:E(pos, ?v) < 0 and  -- alive monster
		       (not AvoidCorpses or BU:D(pos, ?v) < 0) -- corpse
	end
end
