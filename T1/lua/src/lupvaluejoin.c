/*
** debug.upvalueid and debug.upvaluejoin functions from Lua 5.2 (work2) ported by Sergey Rozhenko into Lua 5.1
** See Copyright Notice in lua.h
*/

#define lupvaluejoin_c
#define LUA_LIB

#include "lua.h"

#include "lgc.h"
#include "lstate.h"
#include "lauxlib.h"
#include "lualib.h"

#define api_checkex(l,e,msg)	luai_apicheck(l,(e) && msg)

static TValue *index2addr (lua_State *L, int idx) { /* index2adr from lapi.c */
  if (idx > 0) {
    TValue *o = L->base + (idx - 1);
    api_check(L, idx <= L->ci->top - L->base);
    if (o >= L->top) return cast(TValue *, luaO_nilobject);
    else return o;
  }
  else if (idx > LUA_REGISTRYINDEX) {
    api_check(L, idx != 0 && -idx <= L->top - L->base);
    return L->top + idx;
  }
  else switch (idx) {  /* pseudo-indices */
    case LUA_REGISTRYINDEX: return registry(L);
    case LUA_ENVIRONINDEX: {
      Closure *func = curr_func(L);
      sethvalue(L, &L->env, func->c.env);
      return &L->env;
    }
    case LUA_GLOBALSINDEX: return gt(L);
    default: {
      Closure *func = curr_func(L);
      idx = LUA_GLOBALSINDEX - idx;
      return (idx <= func->c.nupvalues)
                ? &func->c.upvalue[idx-1]
                : cast(TValue *, luaO_nilobject);
    }
  }
} 

static UpVal **getupvalref (lua_State *L, int fidx, int n, Closure **pf) {
  Closure *f;
  Proto *p;
  StkId fi = index2addr(L, fidx);
  api_checkex(L, ttisfunction(fi), "function expected");
  f = clvalue(fi);
  api_checkex(L, !f->c.isC, "Lua function expected");
  p = f->l.p;
  api_checkex(L, (1 <= n && n <= p->sizeupvalues), "invalid upvalue index");
  if (pf) *pf = f;
  return &f->l.upvals[n - 1];  /* get its upvalue pointer */
} 

LUA_API void *lua_upvalueid (lua_State *L, int fidx, int n) {
  Closure *f;
  StkId fi = index2addr(L, fidx);
  api_checkex(L, ttisfunction(fi), "function expected");
  f = clvalue(fi);
  if (f->c.isC) {
    api_checkex(L, 1 <= n && n <= f->c.nupvalues, "invalid upvalue index");
    return &f->c.upvalue[n - 1];
  }
  else return *getupvalref(L, fidx, n, NULL);
}

LUA_API void lua_upvaluejoin (lua_State *L, int fidx1, int n1,
                                            int fidx2, int n2) {
  Closure *f1;
  UpVal **up1 = getupvalref(L, fidx1, n1, &f1);
  UpVal **up2 = getupvalref(L, fidx2, n2, NULL);
  *up1 = *up2;
  luaC_objbarrier(L, f1, *up2);
}

static int checkupval (lua_State *L, int argf, int argnup) {
  lua_Debug ar;
  int nup = luaL_checkint(L, argnup);
  luaL_checktype(L, argf, LUA_TFUNCTION);
  lua_pushvalue(L, argf);
  lua_getinfo(L, ">u", &ar);
  luaL_argcheck(L, 1 <= nup && nup <= ar.nups, argnup, "invalid upvalue index");
  return nup;
}


static int db_upvalueid (lua_State *L) {
  int n = checkupval(L, 1, 2);
  lua_pushlightuserdata(L, lua_upvalueid(L, 1, n));
  return 1;
}

static int db_upvaluejoin (lua_State *L) {
  int n1 = checkupval(L, 1, 2);
  int n2 = checkupval(L, 3, 4);
  luaL_argcheck(L, !lua_iscfunction(L, 1), 1, "Lua function expected");
  luaL_argcheck(L, !lua_iscfunction(L, 3), 3, "Lua function expected");
  lua_upvaluejoin(L, 1, n1, 3, n2);
  return 0;
}


static const luaL_Reg dbupvals[] = { 
  {"upvalueid", db_upvalueid},
  {"upvaluejoin", db_upvaluejoin},
  {NULL, NULL}
};

LUALIB_API int luaopen_upvaluejoin (lua_State *L) {
  luaL_register(L, LUA_DBLIBNAME, dbupvals);
  return 1;
}
