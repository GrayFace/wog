
#define LUA_LIB

#include "lua.h"

#include "lgc.h"
#include "ldo.h"
#include "lstate.h"
#include "lauxlib.h"
#include "lualib.h"

#define api_checkex(l,e,msg)	luai_apicheck(l,(e) && msg)

#if LUA_VERSION_NUM < 502

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

#else

static TValue *index2addr (lua_State *L, int idx) { /* index2addr from lapi.c */
  CallInfo *ci = L->ci;
  if (idx > 0) {
    TValue *o = ci->func + idx;
    api_check(L, idx <= ci->top - (ci->func + 1), "unacceptable index");
    if (o >= L->top) return cast(TValue *, luaO_nilobject);
    else return o;
  }
  else if (idx > LUA_REGISTRYINDEX) {
    api_check(L, idx != 0 && -idx <= L->top - (ci->func + 1), "invalid index");
    return L->top + idx;
  }
  else if (idx == LUA_REGISTRYINDEX)
    return &G(L)->l_registry;
  else {  /* upvalues */
    idx = LUA_REGISTRYINDEX - idx;
    api_check(L, idx <= MAXUPVAL + 1, "upvalue index too large");
    if (ttislcf(ci->func))  /* light C function? */
      return cast(TValue *, luaO_nilobject);  /* it has no upvalues */
    else {
      Closure *func = clvalue(ci->func);
      return (idx <= func->c.nupvalues)
             ? &func->c.upvalue[idx-1]
             : cast(TValue *, luaO_nilobject);
    }
  }
}
#endif


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

/* ================================ Persist ================================ */

/* Makes a Lua function with n upvalues
static void MakeUpvalFunction(lua_State *L, size_t n)
{
	char buf[8];
	luaL_Buffer b;
	size_t i;

	luaL_buffinit(L, &b);
	luaL_addstring(&b, "local a0");
	for(i = 1; i < n; i++)
	{
		luaL_addstring(&b, ", a");
		_itoa(i, buf, 36);
		luaL_addstring(&b, buf);
	}
	luaL_addstring(&b, "; return function()  return a0");
	for(i = 1; i < n; i++)
	{
		luaL_addstring(&b, " + a");
		_itoa(i, buf, 36);
		luaL_addstring(&b, buf);
	}
	luaL_addstring(&b, "; end");
	luaL_pushresult(&b);
	luaL_loadstring(L, lua_tostring(L, -1));
	lua_remove(L, -2);
	lua_call(L, 0, 1);
}
*/

static const char * MakeUpvalFunction = "local a;  return function()  return a;  end";

/* Persists thread using custom persistance, returns 2 functions: unpersister and initializer */
static int PersistThread(lua_State *L)
{
	lua_State *L2;
	StkId o;
	size_t i, n, index;

	luaL_checktype(L, 1, LUA_TTHREAD);
	lua_settop(L, 1);
	L2 = lua_tothread(L, 1);
	if(L2 == L)
		luaL_error(L, "cannot persist currently running thread");
	/* Thread information table:
	{
		stack = { nstack, stack...},  -- stack
		calls = { size, ncalls, calls...},  -- function calls information
		max_stack_top, stackbase, status, env,
		upvals = { stackindex, upfunction, ... },    -- stack index and function holding upvalue, repeat for all upvalues
	}
	*/

	/* Return creation function */
	lua_pushvalue(L, lua_upvalueindex(1));

	/* Construct initialization table */
	lua_createtable(L, 0, 0);

#define store(v)  (lua_pushnumber(L, v), lua_rawseti(L, -2, index++))

	/* Persist stack */
	lua_createtable(L, L2->top - L2->stack + 1, 0);
	index = 1;
	store(L2->top - L2->stack);
	for(o = L2->stack; o < L2->top; o++)
	{
		setobj2s(L, L->top, o);
		L->top++;
		lua_rawseti(L, -2, index++);
	}
	lua_setfield(L, -2, "stack");

	/* Persist call stack */
	lua_createtable(L, n*6 + 1, 0);
	index = 1;
	n = (L2->ci - L2->base_ci) + 1;
	/* size is a power of 2, pick closest to the actually needed 'n' */
	for(i = L2->size_ci; i >= n*2; i /= 2);
	store(i);
	for(i = 0; i < n; i++)
	{
		CallInfo *ci = L2->base_ci + i;
		store(ci->base - L2->stack);
		store(ci->func - L2->stack);
		store(ci->top - L2->stack);
		if(ci->top > o)
			o = ci->top;
		store(ci->nresults);
		store(ci->tailcalls);
		store((ci != L2->base_ci) ? (ci->savedpc - ci_func(ci)->l.p->code) : 0);
	}
	lua_setfield(L, -2, "calls");

	/* Persist some other parameters of thread */
	index = 1;
	store(o - L2->stack);
	store(L2->base - L2->stack);
	store(L2->status);
#if LUA_VERSION_NUM < 502
	lua_getfenv(L, 1);
	lua_rawseti(L, -2, index++);
#endif
	/* Not persisted:
		errfunc - not allowed for coroutines anyway
		hooks - questionable and I don't want to deal with them due to small knowlage of Lua internals
	*/


	/* Persist open upvalues */
	if(L2->openupval != NULL)
	{
		GCObject *gco;
		UpVal *uv;

		/* Count upvalues */
		n = 0;
		for(gco = L2->openupval; gco != NULL; gco = gco2uv(gco)->next)
			n++;

		/* Each open upvalue is stored in a function, this function creates them */
		luaL_loadstring(L, MakeUpvalFunction);

		/* Persist indexes of open upvalues and bind the function to open upvalues */
		lua_createtable(L, n*2, 0);
		index = 1;

		for(gco = L2->openupval; gco != NULL; gco = uv->next)
		{
			Closure *f;
			uv = gco2uv(gco);

			/* Just a check that upvalue is open */
			lua_assert(uv->v != &uv->u.value);

			/* Store stack index of upvalue */
			store(uv->v - L2->stack);

			/* Create a function for upvalue */
			lua_pushvalue(L, -2);
			lua_call(L, 0, 1);

			/* Assign upvalue to function */
			*getupvalref(L, -1, 1, &f) = uv;
			luaC_objbarrier(L, f, uv);

			/* Add function to table */
			lua_rawseti(L, -2, index++);
		}
		luaL_checktype(L, -1, LUA_TTABLE);
		lua_setfield(L, -3, "upvals");
		lua_pop(L, 1);
	}

	/* Set up __call metamethod */
	lua_pushvalue(L, lua_upvalueindex(2));
	lua_setfield(L, -2, "__call");
	lua_pushvalue(L, -1);
	lua_setmetatable(L, -2);
	return 2;
}

/* =============================== Unpersist =============================== */

/* Creates an empty thread */
static int CreateThread(lua_State *L)
{
	lua_newthread(L);
	return 1;
}

/* Taken from pluto without changes.
 * Does basically the opposite of luaC_link().
 * Right now this function is rather inefficient; it requires traversing the
 * entire root GC set in order to find one object. If the GC list were doubly
 * linked this would be much easier, but there's no reason for Lua to have
 * that. */
static void gcunlink(lua_State *L, GCObject *gco)
{
	GCObject *prevslot;
	if(G(L)->rootgc == gco) {
		G(L)->rootgc = G(L)->rootgc->gch.next;
		return;
	}

	prevslot = G(L)->rootgc;
	while(prevslot->gch.next != gco) {
		lua_assert(prevslot->gch.next != NULL);
		prevslot = prevslot->gch.next;
	}

	prevslot->gch.next = prevslot->gch.next->gch.next;
}

/* Returns tonumber(t[n]), where t is at index -1 */
static ptrdiff_t getint(lua_State *L, int n)
{
	lua_Number r;
	lua_rawgeti(L, -1, n);
	r = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return (ptrdiff_t)r;
}

/* Returns lua_isnil(t[n]), where t is at index -1 */
static int isnil(lua_State *L, int n)
{
	int r;
	lua_rawgeti(L, -1, n);
	r = lua_isnil(L, -1);
	lua_pop(L, 1);
	return r;
}

/* Initializes thread */
static int UnpersistThread(lua_State *L)
{
	lua_State *L2;
	ptrdiff_t i, n, n2, index;

	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTHREAD);
	L2 = lua_tothread(L, 2);
	lua_settop(L, 2);
	lua_insert(L, 1);

	/* Unpersist highest top of CallInfos */
	n2 = getint(L, 1);

	/* Unpersist stack */
	lua_getfield(L, -1, "stack");
	n = getint(L, 1);
	if (n + 1 > n2)  n2 = n + 1;
	L2->top = L2->base = L2->stack;
	luaD_checkstack(L2, (ptrdiff_t)n2);
	lua_xmove(L, L2, 1);
	for (i = 2; i <= n; i++)
		lua_rawgeti(L2, 1, i + 1);
	lua_rawgeti(L2, 1, 2);
	lua_replace(L2, 1);
	/* Pluto says that the stack must be valid at least to the highest value among the CallInfos */
	/* 'top' and the values up to there must be filled with 'nil' */
	for(i = n; i < n2; i++)
		setnilvalue(L2->stack + i);

	/* Unpersist call stack */
	lua_getfield(L, -1, "calls");
	index = 1;
	luaD_reallocCI(L2, getint(L, index++));
	for(i = 0; !isnil(L, index); i++)
	{
		ptrdiff_t v;
		CallInfo *ci = L2->base_ci + i;

		ci->base = L2->stack + getint(L, index++);
		ci->func = L2->stack + getint(L, index++);
		ci->top = L2->stack + getint(L, index++);
		lua_assert(ci->base - L2->stack <= n2 && ci->func - L2->stack <= n2 && ci->top - L2->stack <= n2);
		ci->nresults = getint(L, index++);
		ci->tailcalls = getint(L, index++);
		v = getint(L, index++);
		ci->savedpc = (ci != L2->base_ci) ? ci_func(ci)->l.p->code + v : 0;
		L2->ci = ci;
	}
	L2->savedpc = L2->ci->savedpc;
	lua_pop(L, 1);

	/* Unpersist some other parameters of thread */
	index = 2;
	L2->base = L2->stack + getint(L, index++);
	L2->status = getint(L, index++);
#if LUA_VERSION_NUM < 502
	lua_rawgeti(L, -1, index++);
	if(!lua_isnil(L, -1))
		lua_setfenv(L, 1);
	else
		lua_pop(L, 1);
#endif

	/* Unpersist open upvalues */
	lua_getfield(L, -1, "upvals");
	index = 1;
	if(!lua_isnil(L, -1))
	{
		global_State *g = G(L2);
		GCObject **nextslot = &L2->openupval;
		while(!isnil(L, index))
		{
			UpVal* uv;
			ptrdiff_t pos = getint(L, index++);
			lua_rawgeti(L, -1, index++);
			uv = *getupvalref(L, -1, 1, NULL);
			/* Copy value from function, because functions sharing upvalue could have been called during unpersistance */
			setobj2s(L2, L2->stack + pos, uv->v);
			lua_pop(L, 1);

			uv->v = L2->stack + pos;
			gcunlink(L, (GCObject*)uv);
			uv->marked = luaC_white(g);
			*nextslot = (GCObject*)uv;
			nextslot = &uv->next;
			uv->u.l.prev = &g->uvhead;
			uv->u.l.next = g->uvhead.u.l.next;
			uv->u.l.next->u.l.prev = uv;
			g->uvhead.u.l.next = uv;
		}
		*nextslot = NULL;
	}
	lua_pop(L, 1 + 2);

	return 0;
}

/* ========================== Persist C closures =========================== */

static int PersistFunction(lua_State *L)
{
	lua_CFunction f;
	int i;
	luaL_checktype(L, 1, LUA_TFUNCTION);
	f = lua_tocfunction(L, 1);
	lua_settop(L, 1);
	if(!f)  return 1;
	if(!lua_getupvalue(L, 1, 1))  return 1;

	/* initialization table */
	lua_createtable(L, 1, 1);
	lua_insert(L, 2);
	lua_rawseti(L, 2, 1);
	for(i = 2; lua_getupvalue(L, 1, i); i++)
		lua_rawseti(L, 2, i);
	lua_pushvalue(L, lua_upvalueindex(2));
#if LUA_VERSION_NUM < 502
	lua_getfenv(L, -1);
	lua_getfenv(L, 1);
	if(!lua_rawequal(L, -1, -2))
		lua_rawseti(L, 2, i);
	lua_settop(L, 3);
#endif
	lua_setfield(L, 2, "__call");
	lua_pushvalue(L, 2);
	lua_setmetatable(L, 2);
	
	/* creation table */
	lua_createtable(L, 2, 1);
	lua_pushlightuserdata(L, f);
	lua_rawseti(L, 3, 1);
	lua_pushinteger(L, i - 1);
	lua_rawseti(L, 3, 2);
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setfield(L, 3, "__call");
	lua_pushvalue(L, 3);
	lua_setmetatable(L, 3);

	lua_replace(L, 1);
	return 2;
}

/* (t) Creates C closure with function t[1] and number of upvalues t[2] */
static int makecclosure(lua_State *L)
{
	lua_CFunction f;
	lua_Integer i, n;

	luaL_checktype(L, 1, LUA_TTABLE);
	lua_rawgeti(L, 1, 1);
	luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
	f = (lua_CFunction)lua_topointer(L, -1);
	lua_rawgeti(L, 1, 2);
	n = luaL_checkinteger(L, -1);
	lua_settop(L, 0);
	luaL_checkstack(L, n, "RSPersistThread.makecclosure");
	for(i = n; i > 0; i--)
		lua_pushnil(L);
	lua_pushcclosure(L, f, n);
	return 1;
}

/* (t, f) Initializes upvalues and fenv of f with values from t */
static int initcclosure(lua_State *L)
{
	int i = 0;
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	do{
		lua_rawgeti(L, 1, ++i);
	}while(lua_setupvalue(L, 2, i));

#if LUA_VERSION_NUM < 502
	if(!lua_isnil(L, -1))
		lua_setfenv(L, 1);
#endif
	return 0;
}

/* Get C function address as lightuserdata */
static int getcfunction(lua_State *L)
{
	lua_CFunction f = lua_tocfunction(L, 1);
	lua_settop(L, 0);
	if (f == NULL)
		return 0;
	lua_pushlightuserdata(L, f);
	return 1;
}

/* =============================== Register ================================ */

/* Pushes existing metatable or creates a new one */
static void needmetatable(lua_State *L)
{
	if(!lua_getmetatable(L, -1))
	{
		lua_createtable(L, 0, 1);
		lua_pushvalue(L, -1);
		lua_setmetatable(L, -3);
	}
}

/* Resisters a function in permanents tables */
static void Reg(lua_State *L, int n, const char * name)
{
	lua_pushvalue(L, -1);
	lua_pushstring(L, name);
	lua_pushvalue(L, -1);
	lua_pushvalue(L, -3);
	lua_settable(L, n + 1);
	lua_settable(L, n);
}

/* Returns PermanentsSave, PermanentsLoad, getcfunction */
LUA_API int RSPersistMore(lua_State *L)
{
	int n = lua_gettop(L) + 1;
	lua_createtable(L, 0, 7); /* n:    PersistSave */
	lua_createtable(L, 0, 7); /* n+1:  PersistLoad */
	
	/* Register functions for threads persistance */
	lua_pushthread(L);
	needmetatable(L);
	lua_pushcfunction(L, CreateThread);
	Reg(L, n, "RSPersistThread.CreateThread");
	lua_pushcfunction(L, UnpersistThread);
	Reg(L, n, "RSPersistThread.UnpersistThread");
	lua_pushcclosure(L, PersistThread, 2);
	Reg(L, n, "RSPersistThread.PersistThread");
	lua_createtable(L, 0, 0); /* empty environment table for functions that are created for open upvalues */
	lua_setfenv(L, -2);
	lua_setfield(L, -2, "__persist");
	lua_pop(L, 2);
	
	/* Register functions for C closures persistance */
	lua_pushcfunction(L, getcfunction);
	Reg(L, n, "RSPersistThread.getcfunction");
	needmetatable(L);
	lua_pushcfunction(L, makecclosure);
	Reg(L, n, "RSPersistThread.makecclosure");
	lua_pushcfunction(L, initcclosure);
	Reg(L, n, "RSPersistThread.initcclosure");
	lua_pushcclosure(L, PersistFunction, 2);
	Reg(L, n, "RSPersistThread.PersistFunction");
	lua_setfield(L, -2, "__persist");
	lua_pop(L, 1);

	return 3;
}
