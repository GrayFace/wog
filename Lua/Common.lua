-- shared by WoG, MMExtension and GameExtension

local type = type
local unpack = unpack
local assert = assert
local format = string.format

local next = next
local pairs = pairs
local ipairs = ipairs
local tonumber = tonumber
local tostring = tostring
local rawget = rawget
local rawset = rawset
local getfenv = getfenv
local setfenv = setfenv
local pcall = pcall
local xpcall = xpcall
local pcall2
local getmetatable = debug.getmetatable
local d_setmetatable = debug.setmetatable
local d_getinfo = debug.getinfo
local os_time = os.time
local loadfile = loadfile
local loadstring = loadstring
local require = require
local table_insert = table.insert
local table_remove = table.remove
local table_concat = table.concat
local floor = math.floor
local ceil = math.ceil
local abs = math.abs
local string_sub = string.sub
local string_gsub = string.gsub
local string_match = string.match
local string_lower = string.lower
local coroutine_create = coroutine.create
local coroutine_resume = coroutine.resume
local coroutine_running = coroutine.running
local coroutine_main = coroutine.main
local dofile = dofile
local ffi = require "ffi"

local _G = _G
local internal = debug.getregistry() -- internals
local CoreScriptsPath = internal.CoreScriptsPath
local nullpersist = internal.nullpersist

local function setmetatable(t, m)
	d_setmetatable(t, m)
	return t
end

----------- Error function ------------

local error = _G.error
local SetErrorLevel = internal.SetErrorLevel

local error = function(msg, level)
	SetErrorLevel(level or 1)
	return error(msg, level)
end
_G.error = error

-----------

local function nullsub()
end
local offsets = offsets or {}

dofile(CoreScriptsPath.."RSFunctions.lua")
local table_swap = table.swap
local table_move = table.move
PrintToFile("InternalLog.txt")  -- temporary

----------- No globals from this point ------------

local _NOGLOBALS

----------- General functions ------------

local function assertnum(v, level, msg)
	local v1 = tonumber(v)
	if v1 then
		return v1
	end
	return error(msg or "number expected, got "..type(v), level or 1)
end
_G.assertnum = assertnum

local function pcall2_ret(ok, ...)
	if not ok then
		internal.ErrorMessage((...))
	end
	return ok, ...
end

local function pcall2(...)
	return pcall2_ret(pcall(...))
end
_G.pcall2 = pcall2

function _G.xpcall2(...)
	return pcall2_ret(xpcall(...))
end

function _G.dofile2(path, ...)
	local chunk, err = loadfile(path)
	if chunk == nil then
		internal.ErrorMessage(err)
		return false, err
	else
		return pcall2_ret(pcall(chunk, ...))
	end
end

local function coroutine_resume2(...)
	return pcall2_ret(coroutine_resume(...))
end
_G.coroutine.resume2 = coroutine_resume2

----------- mem ------------

local mem_internal = _G.mem
internal.RSMem = mem_internal
local mem = mem_internal
mem.pcall2 = pcall2
mem.free = offsets.free or mem.free
mem.StaticAlloc = mem.malloc
mem.malloc = offsets.malloc or mem.malloc
mem.realloc = offsets.realloc or mem.realloc
mem.new = offsets.new or mem.new
do
	-- compile Asm string into bytecode string
	local CompileAsm = internal.CompileAsm
	
	mem.AsmProlog = [[
		use32
		ptr equ
		macro pushstr string
		{
			local ..after
			call ..after
			db string,0
		..after:
		}
		_loadreg_ equ
		_loadreg_end_ equ
		macro savereg a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20
		{
			if a1 eq
			else
				push a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 a16 a17 a18 a19 a20
			end if
			_loadreg_ equ pop a20 a19 a18 a17 a16 a15 a14 a13 a12 a11 a10 a9 a8 a7 a6 a5 a4 a3 a2 a1
			_loadreg_end_ equ _loadreg_
		}
		macro loadreg
		{
			if _loadreg_ eq pop
			else
				_loadreg_
			end if
			_loadreg_end_ equ
		}
	]]
	mem.AsmEpilog = [[
	
		if _loadreg_end_ eq pop
		else
			_loadreg_end_
		end if]]
	
	local StartCode, StartLines
	
	local function check(s, level)
		if type(s) ~= "string" then
			error("string expected, got "..type(s), level + 1)
		end
		
		if mem.AsmProlog ~= StartCode then
			StartCode, StartLines = string_gsub(mem.AsmProlog, "\n", "\n")
		end
		return StartCode..s..mem.AsmEpilog, StartLines
	end
	
	function mem.CompileAsm(level, FromMem)
		if FromMem then
			local macro, MacroLines = string_gsub([[
				absolute equ near -%d + 
			]], "\n", "\n")
			return function(s)
				local s, n = check(s, level + 1)
				return function(p)
					local r, err = CompileAsm(format(macro, p or 0)..s, n + MacroLines)
					return r or (error(err, FromMem + 1))
				end
			end
		else
			return function(s)
				local r, err = CompileAsm(check(s, level + 1))
				return r or (error(err, level + 1))
			end
		end
	end
end

function mem.struct_callback(t, class, fields, offs, rofields)
	local meta = getmetatable(t)
	meta.members = fields
	meta.offsets = offs
	meta.class = class
	return t
end

dofile(CoreScriptsPath.."RSMem.lua")

local mem = _G.mem
_G.table.destructor = mem.general.table_destructor
mem.general.table_destructor = nil
_G.table.copy(mem.general, _G)
mem.general = nil
local call = mem.call
local malloc = mem.malloc
local StaticAlloc = mem.StaticAlloc
local free = mem.free
local i4, i2, i1, u8, u4, u2, u1, pchar = mem.i4, mem.i2, mem.i1, mem.u8, mem.u4, mem.u2, mem.u1, mem.pchar
local mem_string = mem.string
local mem_copy = mem.copy
local mem_fill = mem.fill
local IgnoreProtection = mem.IgnoreProtection

_G.package.path = CoreScriptsPath.."?.lua"
-- mobdebug
do
	-- function _G.print(s)  -- report mobdebug errors
		-- error(s)
	-- end
	local debug = _G.debug
	local mobdebug = require("mobdebug")
	debug.mobdebug = mobdebug
	debug.start = debug.mobdebug.start
	debug.on = debug.mobdebug.on
	debug.off = debug.mobdebug.off
	debug.done = debug.mobdebug.done
	debug.stop = debug.mobdebug.done
	mobdebug.connecttimeout = 0.001
	local serv = mobdebug.connect("localhost", mobdebug.port)
	if serv then
		serv:close()
	else
		_G.jit.on()
	end
end

dofile(CoreScriptsPath.."dump.lua")

--------- bit

local bit = require("bit")
local bit_And = bit.band
local bit_Not = bit.bnot
_G.bit = bit
bit.Or = bit.bor
bit.And = bit.band
bit.Xor = bit.bxor
bit.AndNot = function(a, b)
	return bit_And(a, bit_Not(b))
end
bit.Not = bit.bnot

local numIndex = _G.table.copy(bit)

do
	local n = 1
	for i = 0,63 do
		bit[i] = n
		n = n*2
	end
end

function bit.ToTable(x, t)
	t = t or {}
	if x < 0 then  x = x + 4294967296  end
	local i = 1
	local b = 1                              -- current bit
	local d = 2                              -- current divider
	while x >= b do
		if x % d >= b then
			t[i] = true
		end
		b = d
		d = d*2
		i = i + 1
	end
	return t
end

function bit.FromTable(t)
	local x = 0
	local b = 1
	for i = 1,32 do
		if t[i] then
			x = x + b
		end
		b = b*2
	end
	if x < 2147483648 then
		return x
	end
	return x - 4294967296
end

--------- math

-- function _G.math.round(val)
-- 	if val >= 0 then
-- 		return floor(val + 0.5)
-- 	else
-- 		return ceil(val - 0.5)
-- 	end
-- end
_G.math.round = bit.tobit

--------- numIndex

function numIndex.ToHex(v)
	return format("%X", v)
end
numIndex.tohex = numIndex.ToHex

function numIndex.Div(v1, v2)
	return floor(v1/v2)
--	if v1 > v2 then
--		floor
--	else
--	end
end

numIndex.div = numIndex.Div

numIndex.round = _G.math.round
numIndex.floor = _G.math.floor
numIndex.ceil = _G.math.ceil

setmetatable(0, {__index = numIndex})

--------- color

--color = {}
--
--function color.ToRGB(c)
--	local r = floor(c:And(63488)*33/8192)
--	local g = floor(c:And(2016)*65/512)
--	local b = floor(c:And(31)*33/4)
--	return r, g, b
--end
--
--function color.RGB(r, g, b)
--	return r:And(248)*256 + g:And(252)*8 + floor(b/8)
--end
--
----local function Color16To24(c)
----	local r = floor(c:And(63488)*33/8192)
----	local g = (c:And(2016)*65/2):And(65280)
----	local b = (c:And(31)*540672):And(16711680)
----	return r + g + b
----end

--------- os

if offsets.exit then
	function _G.os.exit(code)
		call(offsets.exit, 0, code or 0)
	end
end

local FindStruct = mem.struct(function(define)
	define
	.u4  'FileAttributes'
	.alt.u8x  'CreationTime'
	.u4  'CreationTimeLow'
	.u4  'CreationTimeHigh'
	.alt.u8x  'LastAccessTime'
	.u4  'LastAccessTimeLow'
	.u4  'LastAccessTimeHigh'
	.alt.u8x  'LastWriteTime'
	.u4  'LastWriteTimeLow'
	.u4  'LastWriteTimeHigh'
	.i8  'FileSize'
	.skip(8)
	.string(260)  'FileName'
	.string(14)  'AlternateFileName'
end)

local function findNext(data)
	if data then
		local ret = internal.FindNext(data)
		if ret then
			local m = getmetatable(data)
			return m.path..ret, m.struct
		end
	end
end

function _G.path.find(filter, dir)
	local data, path, p = internal.FindStart(filter, not not dir)
	if data == nil then
		return nullsub
	end
	local m = getmetatable(data)
	m.path = path
	m.__gc = internal.FindClose
	m.__persist = nullpersist
	m.struct = FindStruct:new(p)
	return findNext, data
end
_G.path.Find = _G.path.find

local function FindFirst(filter, dir)  -- instead of FileExists/DirectoryExists
	local data, path, p = internal.FindStart(filter, not not dir)
	if data then
		local r = internal.FindNext(data)
		internal.FindClose(data)
		return r and path..r
	end
end
_G.path.FindFirst = FindFirst
_G.path.findfirst = FindFirst

local dirBuf
local function GetCurrentDirectory()
	dirBuf = dirBuf or malloc(260)
	u1[dirBuf] = 0
	call(internal.GetCurrentDirectory, 0, 260, dirBuf)
	return mem_string(dirBuf)
end
_G.path.GetCurrentDirectory = GetCurrentDirectory

function _G.path.SetCurrentDirectory(dir)
	-- return call(internal.SetCurrentDirectory, 0, dir) ~= 0
	local s = GetCurrentDirectory()
	call(internal.SetCurrentDirectory, 0, dir)
	return s
end

local path_noslash = _G.path.noslash
local path_dir = _G.path.dir
local CreateDirectoryPtr = internal.CreateDirectory
local GetLastErrorPtr = internal.GetLastErrorPtr

local function DoCreateDir(dir)
	-- 183 = already exists
	return call(CreateDirectoryPtr, 0, dir, 0) ~= 0 or GetLastErrorPtr and call(GetLastErrorPtr, 0, dir, 0) == 183
end

local function CreateDirectory(dir)
	dir = path_noslash(dir)
	if dir == "" or #dir == 2 and string_sub(dir, -1) == ":" or DoCreateDir(dir) then
		return true
	end
	local dir1 = path_dir(dir)
	if dir1 ~= dir then
		CreateDirectory(dir1)
	end
	return DoCreateDir(dir)
end
_G.path.CreateDirectory = CreateDirectory

local oldSave = _G.io.SaveString
function _G.io.SaveString(path, ...)
	CreateDirectory(path_dir(path))
	return oldSave(path, ...)
end

ffi.cdef[[
typedef enum {
	FO_MOVE         = 0x0001,
	FO_COPY         = 0x0002,
	FO_DELETE       = 0x0003,
	FO_RENAME       = 0x0004,
	___size         = 0xFFFFFFFF
} FILEOP_FUNC;

typedef enum {
	FOF_MULTIDESTFILES        = 0x0001,
	FOF_CONFIRMMOUSE          = 0x0002,
	FOF_SILENT                = 0x0004,  // don't create progress/report
	FOF_RENAMEONCOLLISION     = 0x0008,
	FOF_NOCONFIRMATION        = 0x0010,  // Don't prompt the user.
	FOF_WANTMAPPINGHANDLE     = 0x0020,  // Fill in SHFILEOPSTRUCT.hNameMappings
                                      // Must be freed using SHFreeNameMappings
	FOF_ALLOWUNDO             = 0x0040,
	FOF_FILESONLY             = 0x0080,  // on *.*, do only files
	FOF_SIMPLEPROGRESS        = 0x0100,  // means don't show names of files
	FOF_NOCONFIRMMKDIR        = 0x0200,  // don't confirm making any needed dirs
	FOF_NOERRORUI             = 0x0400,  // don't put up error UI
	FOF_NOCOPYSECURITYATTRIBS = 0x0800,  // dont copy NT file Security Attributes
	FOF_NORECURSION           = 0x1000  // don't recurse into directories.
} FILEOP_FLAGS;

typedef struct {
	void *          hwnd;
	FILEOP_FUNC     wFunc;
	const char *    pFrom;
	const char *    pTo;
	FILEOP_FLAGS    fFlags;
	bool            fAnyOperationsAborted;
	void *          hNameMappings;
	const char *    lpszProgressTitle; // only used if FOF_SIMPLEPROGRESS
} SHFILEOPSTRUCTA, *LPSHFILEOPSTRUCTA;

int __stdcall SHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp);
bool __stdcall DeleteFileA(const char *lpFileName);
bool __stdcall MoveFileA(const char *lpExistingFileName, const char *lpNewFileName);
bool __stdcall CopyFileA(const char *lpExistingFileName, const char *lpNewFileName, bool bFailIfExists);
]]

local SHFileOperation = ffi.load("Shell32", true).SHFileOperationA
local SHDeleteFlags = ffi.C.FOF_NOCONFIRMATION + ffi.C.FOF_NOERRORUI + ffi.C.FOF_SILENT
local DeleteFile = ffi.C.DeleteFileA
function _G.path.DeleteFile(fname, NoRecycle)
	if not NoRecycle then
		local fos = ffi.new("SHFILEOPSTRUCTA")
		fos.wFunc = "FO_DELETE"
		fos.pFrom = path_noslash(fname).."\000"
		fos.fFlags = SHDeleteFlags
		return SHFileOperation(fos) == 0;
	else
		return DeleteFile(fname);
	end
end
_G.path.MoveFile = ffi.C.MoveFileA
_G.path.CopyFile = ffi.C.CopyFileA

local AppPath = _G.AppPath or _G.path.addslash(_G.path.GetCurrentDirectory())
_G.AppPath = AppPath

--------- debug, events

local function FunctionFile(f)
	if type(f) == "number" then
		f = f + 1
	end
	local s = d_getinfo(f, "S").source
	return s:sub(1, 1) == '@' and s:sub(2) or ""
end
_G.debug.FunctionFile = FunctionFile

dofile(CoreScriptsPath.."EventsList.lua")
