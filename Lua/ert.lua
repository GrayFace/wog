
local internal = debug.getregistry()

local error = error
local assert = assert
local pairs = pairs
local tonumber = tonumber

local io_LoadString = io.LoadString
local string_match = string.match
local string_split = string.split

local malloc = mem.malloc
local mfree = mem.free
local topointer = mem.topointer
local mcopy = mem.copy

local _NOGLOBALS

----------- No globals from this point ------------

local ERTBuf = {}  -- needed, because Lua strings must be strictly read-only and ERM code can't be trusted
internal.ERT = {}
internal.AutoPersist.ERT = true
internal.StrToERT = {}

function internal.ClearERT()
	internal.ERT = {}
	for k, v in pairs(ERTBuf) do
		mfree(v)
	end
	ERTBuf = {}
	internal.StrToERT = {}
end

function internal.GetERT(i)
	local p = ERTBuf[i]
	if p then
		return p
	end
	local s = internal.ERT[i]
	if s then
		p = malloc(#s + 1)
		ERTBuf[i] = p
		internal.StrToERT[p] = i
		mcopy(p, s, #s + 1)
		return p
	end
	--error("ERT string z"..i.." wasn't found")
end

function internal.LoadERT(path, replace)
	local t = string_split(assert(io_LoadString(path)), "\r\n", true)
	for _, s in pairs(t) do
		local k, v = string_match(s, "([^\t]*)\t([^\t]*)")
		k = tonumber(k)
		if k then
			if not replace and internal.ERT[k] then
				error(path..": ERT string index z"..k.." is already used", 0)
			end
			internal.ERT[k] = v
			local p = ERTBuf[k]
			if p then
				mfree(p)
				ERTBuf[k] = nil
				internal.StrToERT[p] = nil
			end
		end
	end
end
