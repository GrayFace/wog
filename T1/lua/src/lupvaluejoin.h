/*
** debug.upvalueid and debug.upvaluejoin functions from Lua 5.2 (work2) ported by Sergey Rozhenko into Lua 5.1
** See Copyright Notice in lua.h
*/

#ifndef lupvaluejoin_h
#define lupvaluejoin_h

#include "lua.h"

LUA_API void *(lua_upvalueid) (lua_State *L, int fidx, int n);
LUA_API void  (lua_upvaluejoin) (lua_State *L, int fidx1, int n1,
                                               int fidx2, int n2);

LUALIB_API int (luaopen_upvaluejoin) (lua_State *L);

#endif
