
local target = loadstring("return getfenv(1)")()
local env = getfenv(1)
local setfenv = setfenv
local mem = mem
setfenv(1, target)
i4, i2, i1, u4, u2, u1, i8, u8, pchar = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1, mem.i8, mem.u8, mem.pchar
setfenv(1, env)

function target.GetTxt(off)
	return mem.pchar[mem.u4[mem.u4[0x6A5DC4] + 32] + off]
end

target.internal = debug.getregistry()






-- FindRef

local SodExe
local FuncsList

function _G.FR(p, p1, start, stop)
	SodExe = SodExe or io.LoadString(internal.CoreScriptsPath.."../Build/heroes3.ex_")
	FuncsList = FuncsList or require("FuncsList.lua").Funcs
	p1 = p1 or p + 1
	local t = {}
	local t1 = {}
	local off = mem.topointer(SodExe) - 0x400000
	
	for nfu, fu in ipairs(FuncsList) do
		local i, j = fu + off, (FuncsList[nfu + 1] or 0x639F00) + off
		while i < j do
			local n = mem.GetInstructionSize(i)
			local _, IsJump = mem.GetHookSize(i)
			if not IsJump then
				local op = u1[i]
				for i = i + 1, i + n - 4 do
					local v = u4[i]
					if v >= p and v < p1 then
						local t = (op ~= 0x68 and t or t1)  -- put "push XXX" in t1
						t[#t+1] = format("0x%X", i - off)
					end
				end
			end
			i = i + n
		end
	end
	return "Ref{".._G.table.concat(t, ", ").."}", "Ref{".._G.table.concat(t1, ", ").."}"
end


-- function _G.FR(p, p1, start, stop)
	-- SodExe = SodExe or io.LoadString[[c:\_WoG\wog359\trunk\Build\heroes3.ex_]]
	-- p1 = p1 or p + 1
	-- local t = {}
	-- local t1 = {}
	-- local off = mem.topointer(SodExe) - 0x400000
	-- -- local i, j = (start or 0x401000) + off, (stop or 0x639F00) + off
	-- -- while i < j do
		-- -- local n = mem.GetInstructionSize(i)
		-- -- local _, IsJump = mem.GetHookSize(i)
		-- -- if not IsJump then
			-- -- local op = u1[i]
			-- -- for i = i + 1, i + n - 4 do
				-- -- local v = u4[i]
				-- -- if v >= p and v < p1 then
					-- -- local t = (op ~= 0x68 and t or t1)  -- put "push XXX" in t1
					-- -- t[#t+1] = format("0x%X", i - off)
				-- -- end
			-- -- end
		-- -- end
		-- -- i = i + n
	-- -- end
	-- local last
	-- for i = (start or 0x401000) + off, (stop or 0x639F00) + off do
		-- local n = mem.GetInstructionSize(i)
		-- local _, IsJump = mem.GetHookSize(i)
		-- if not IsJump then
			-- local op = u1[i]
			-- for i = i + 1, i + n - 4 do
				-- local v = u4[i]
				-- if v >= p and v < p1 then
					-- if i ~= last then
						-- local t = (op ~= 0x68 and t or t1)  -- put "push XXX" in t1
						-- t[#t+1], last = format("0x%X", i), i
					-- end
				-- end
			-- end
		-- end
	-- end	
	-- -- for i = start or 0x401000, stop or 0x63A000 do
		-- -- local v = u4[i]
		-- -- if v >= p and v < p1 then
			-- -- t[#t+1] = format("0x%X", i)
		-- -- end
	-- -- end	
	-- return "Ref{".._G.table.concat(t, ", ").."}", "Ref{".._G.table.concat(t1, ", ").."}"
-- end
