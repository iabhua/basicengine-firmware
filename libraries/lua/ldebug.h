/*
** $Id: ldebug.h $
** Auxiliary functions from Debug Interface module
** See Copyright Notice in lua.h
*/

#ifndef ldebug_h
#define ldebug_h


#include "lstate.h"


#define pcRel(pc, p)	(cast_int((pc) - (p)->code) - 1)

#define resethookcount(L)	(L->hookcount = L->basehookcount)

/*
** mark for entries in 'lineinfo' array that has absolute information in
** 'abslineinfo' array
*/
#define ABSLINEINFO	(-0x80)

LUAI_FUNC int luaG_getfuncline (const Proto *f, int pc);
LUAI_FUNC const char *luaG_findlocal (lua_State *L, CallInfo *ci, int n,
                                                    StkId *pos);
LUAI_FUNC l_noret luaG_typeerror (lua_State *L, const TValue *o,
                                                const char *opname);
#define luaG_typeerror_P(L, o, opname) luaG_typeerror(L, o, PSTR(opname))
LUAI_FUNC l_noret luaG_forerror (lua_State *L, const TValue *o,
                                               const char *what);
LUAI_FUNC l_noret luaG_concaterror (lua_State *L, const TValue *p1,
                                                  const TValue *p2);
LUAI_FUNC l_noret luaG_opinterror (lua_State *L, const TValue *p1,
                                                 const TValue *p2,
                                                 const char *msg);
#define luaG_opinterror_P(L, p1, p2, msg) luaG_opinterror(L, p1, p2, PSTR(msg))
LUAI_FUNC l_noret luaG_tointerror (lua_State *L, const TValue *p1,
                                                 const TValue *p2);
LUAI_FUNC l_noret luaG_ordererror (lua_State *L, const TValue *p1,
                                                 const TValue *p2);
LUAI_FUNC l_noret __luaG_runerror (lua_State *L, const char *fmt, ...);
#define luaG_runerror(L, fmt, ...) __luaG_runerror(L, PSTR(fmt), ##__VA_ARGS__)

LUAI_FUNC const char *luaG_addinfo (lua_State *L, const char *msg,
                                                  TString *src, int line);
LUAI_FUNC l_noret luaG_errormsg (lua_State *L);
LUAI_FUNC int luaG_traceexec (lua_State *L, const Instruction *pc);


#endif
