
local hiddens = {}

function table.hidden(t)
	local v = rawget(t, hiddens)
	if v == nil then
		v = {}
		rawset(t, hiddens, v)
	end
	return v
end

local nextOrig = next

local function nextSkipHidden(t, ...)
	local k, v = nextOrig(t, ...)
	if k == hiddens then
	  return nextOrig(t, k)
	end
	return k, v
end
next = nextSkipHidden

function pairs(t)
	return nextSkipHidden, t, nil
end
