local internal = debug.getregistry()
local assert = assert

local i4, i2, i1, u4, u2, u1 = mem.i4, mem.i2, mem.i1, mem.u4, mem.u2, mem.u1
local call = mem.call
local mem_copy = mem.copy
local mem_hook = mem.hook
local mem_hookalloc = mem.hookalloc
local IgnoreProtection = mem.IgnoreProtection
local malloc = mem.malloc
local GetProcAddress = mem.GetProcAddress
local And = bit.And

local ReadFile = internal.ReadFile
local SetFilePointer = internal.SetFilePointer

local lib = mem.LoadDll("BinkW32new.dll")

if not lib then
	return
end

----------- No globals from this point ------------

local _NOGLOBALS

---------------------------------------------------

local Versions = {}

local function HookImport(p, name, narg, f)
	IgnoreProtection(true)
	
	local old = u4[p]
	local new = GetProcAddress(lib, name)
	assert(new ~= 0)
	local buf = mem_hookalloc()
	u4[p] = buf
	mem_hook(buf, function(d)
		d:pop()  -- buf return address
		--print(u4[d.esp], name, d:getparams(0, narg))
		--newstarted = newstarted or Versions[d:getparams(0, narg)]
		d.eax = f(old, new, d:getparams(0, narg))
		d:ret(narg*4)
	end)
	
	IgnoreProtection(false)
end


local function BinkHook(p, name, narg)
	HookImport(p, name, narg,
		function(old, new, bink, ...)
			return call(Versions[bink] and new or old, 0, bink, ...)
		end
	)
end

BinkHook(0x63A38C, "_BinkPause@8", 2)
BinkHook(0x63A3A0, "_BinkDoFrame@4", 1)
BinkHook(0x63A3A4, "_BinkGoto@12", 3)
BinkHook(0x63A3A8, "_BinkNextFrame@4", 1)
BinkHook(0x63A3B0, "_BinkWait@4", 1)


local BinkPause = GetProcAddress(lib, "_BinkPause@8")
local readbuf = malloc(4)
local bytesRead = malloc(4)

HookImport(0x63A390, "_BinkOpen@8", 2,
	function(old, new, hfile, flags)
		call(ReadFile, 0, hfile, readbuf, 4, bytesRead, 0)
		call(SetFilePointer, 0, hfile, -i4[bytesRead], 0, 1);
		local ret
		if u1[readbuf + 3] > 0x62 then
			-- need testing:
			--ret = call(new, 0, hfile, 0x8800000 + (flags:And(0x400000) ~= 0 and 0x2000 or 0)
			-- for now:
			ret = call(new, 0, hfile, 0x8800000 + 0x2000)  -- preload whole file to avoid a crash
			-- wrong:
			-- if flags:And(0x400000) ~= 0 then  -- open paused
				-- call(BinkPause, 0, ret, 1)
			-- end
			Versions[ret] = true
		else
			ret = call(old, 0, hfile, flags)
			Versions[ret] = nil
		end
		return ret
	end
)

HookImport(0x63A394, "_BinkClose@4", 1,
	function(old, new, bink)
		local ret = call(Versions[bink] and new or old, 0, bink)
		Versions[bink] = nil
		return ret
	end
)

local function ConvMode(mode, new)
	if new then
		if mode == 0 then
			mode = 1
		elseif mode == 1 then
			mode = 3
		elseif mode == 2 then
			mode = 8
		elseif mode == 3 then
			mode = 10
		end
	end
	return mode
end

HookImport(0x63A39C, "_BinkCopyToBuffer@28", 7,
	function(old, new, bink, buf, stride, h, x, y, mode)
		return call(Versions[bink] and new or old, 0, bink, buf, stride, h, x, y, ConvMode(mode, Versions[bink]))
	end
)

HookImport(0x63A3B4, "_BinkGetRects@8", 2,
	function(old, new, bink, mode)
		return call(Versions[bink] and new or old, 0, bink, ConvMode(mode, Versions[bink]))
	end
)


local NewBinkMiles = GetProcAddress(lib, "_BinkOpenMiles@4")
assert(NewBinkMiles ~= 0)

HookImport(0x63A3B8, "_BinkSetSoundSystem@8", 2,
	function(old, new, ss, handle)
		call(old, 0, ss, handle)
		call(new, 0, NewBinkMiles, handle)
	end
)


local bufnew = malloc(0x100)
HookImport(0x63A3AC, "_BinkGetSummary@8", 2,
	function(old, new, bink, buf)
		if Versions[bink] then
			local ret = call(new, 0, bink, bufnew)
			mem_copy(buf, bufnew, 13*4)
			mem_copy(buf + 13*4, bufnew + 14*4, 3*4)
			mem_copy(buf + 16*4, bufnew + 19*4, 12*4)
		else
			return call(old, 0, bink, buf)
		end
	end
)

