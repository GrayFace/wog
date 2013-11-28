-- by GrayFace aka sergroj

local P = require "BattlePos"

local arg = ERM.x

FU(MC("hex2xy"):S(10)).? = function()
	if FU:X(1, ?v) == 1 then -- if ?x1
		arg[1] = P.xy2hex(arg[2], arg[3]) or -1
	else
		local x, y = P.hex2xy(arg[1])
		arg[2] = x or -1
		arg[3] = y or -1
	end
end

FU(MC("hexDistance"):S(11)).? = function()
	arg[1] = P.distance(arg[2], arg[3]) or -1
end

FU(MC("hexIsClear"):S(610)).? = function()
	arg[1] = P.isclear(arg[2], arg[3] ~= 0) and 1 or 0
end
