DumpLimit = 10000

-- For debugging and expirimenting. Shows 't', expanding all tables in it up to the 'depth' level.
function dump(t, depth)
	local buf = {}
	local bufn = 0
	local ShowN
	local tables = {}
	local ptrs = {}
	local ShowTable
	depth = depth and depth * 2 or math.huge

	local function Write(str)
		bufn = bufn + 1
		buf[bufn] = str
	end
	
	local function Key(v)
		if type(v) == "string" and v ~= "" then
			local v1 = ("[%q]"):format(v)
			if string.sub(v1, 3, -3) ~= v then
				v = v1
			end
			return v..' = '
		elseif v == nil then
			return ""
		elseif type(v) == "table" then
			tables[v] = tables[v] or false  -- when shown needs address added
		end
		return '['..(v == "" and '""' or tostring(v))..'] = '
	end
	
	local function Val(v)
		if type(v) == "string" then
			return '"'..v..'"'
		else
			return tostring(v)
		end
	end

	local function FindSameStruct(v)
		local ptr = v["?ptr"]
		local class = structs.class(v)
		local same = ptrs[ptr]
		while same and class ~= structs.class(same) do
			same = ptrs[same]
		end
		return same
	end
	
	local function ShowVal(k, v, space)
		if bufn ~= ShowN then
			buf[bufn] = buf[bufn]..','
		end
		if type(v) == "table" then
			local n = tables[v]
			local SameStruct = not n and structs and FindSameStruct(v)
			if SameStruct then
				n = tables[SameStruct]
				if n then
					v = SameStruct
				end
			end
			if not n then  -- not already shown
				if #space < depth then
					Write(("%s%s{"):format(space, Key(k)))
					if n == false then  -- needs address added
						tables[v] = 0
						buf[bufn] = buf[bufn].."  -- "..Val(v)
					else
						tables[v] = bufn
					end
					if structs then
						local ptr = v["?ptr"]
						if ptr then
							ptrs[v] = ptrs[ptr]
							ptrs[ptr] = v
						end
					end
					
					ShowTable(v, space)
					return Write(space.."}")
				else
					tables[v] = false  -- when shown needs address added
				end
			elseif n ~= 0 then  -- address not already added
				tables[v] = 0
				buf[n] = buf[n].."  -- "..Val(v)
			end
			if SameStruct then
				return Write(("%s%sclone %s"):format(space, Key(k), Val(v)))
			end
		end
		Write(("%s%s%s"):format(space, Key(k), Val(v)))
	end
	
	local function ShowArray(t, space)
		-- local i = 1
		for k, v in t do
			ShowVal(nil, v, space)
			-- if k == i then
				-- ShowVal(nil, v, space)
				-- i = i + 1
			-- else
				-- ShowVal(k, v, space)
			-- end
		end
	end

	local function ShowStruct(t, space)
		for k, v in structs.enum(t) do
			ShowVal(k, v, space)
		end
	end
	
	function ShowTable(t, space)
		if bufn > DumpLimit then
			return Write(space.."  ...")
		end
		ShowN = bufn
		space = space.."  "
		if not structs or not pcall(ShowStruct, t, space) and not pcall(ShowArray, t, space) then  -- !!!
		--do
			local i = 1
			for k, v in sortpairs(t) do
				if k == i then
					i, k = i + 1, nil
				elseif k == i + 1 then
					ShowVal(nil, nil, space)
					i, k = i + 2, nil
				elseif k == i + 2 then
					ShowVal(nil, nil, space)
					ShowVal(nil, nil, space)
					i, k = i + 3, nil
				end
				ShowVal(k, v, space)
			end
		end
	end
	
	if type(t) == "table" and depth ~= 0 then
		Write("{")
		tables[t] = bufn
		local ptr = t["?ptr"]
		if ptr then
			ptrs[ptr] = t
		end
		ShowTable(t, "")
		Write("}")
	else
		Write(Val(t))
	end
	return table.concat(buf, "\n")
end

