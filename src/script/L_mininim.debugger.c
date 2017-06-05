/*
  L_mininim.debugger.c -- mininim.debugger script module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

#define L_ERROR_EVENT -1
#define L_INVOKED_EVENT -2
#define L_BREAK_EVENT -3
#define L_WATCH_EVENT -4
#define FULL_TRACEBACK_MAX_FRAMES 32

bool L_debugging;
ALLEGRO_COND *debug_cond;

char *bt_on_error;

struct {
  enum debug_action {
    DEBUG_CONTINUE, DEBUG_STOPPED, DEBUG_STEP, DEBUG_NEXT, DEBUG_FINISH,
    DEBUG_INVOKED, DEBUG_BREAK, DEBUG_WATCH
  } action;

  int nframes;
  int break_id;
} debug;

struct debugger_break {
  int id;
  int type;
  char *fn_pattern;
  int line;
  int cond_ref;
  int value_ref;
  bool enabled;
} *debugger_break;
size_t debugger_break_nmemb;

static int upvalue_index_by_name (lua_State *L, int level, const char *name);
static int local_index_by_name (lua_State *L, int level, const char *name);
static void start_debugging (void);
static void stop_debugging (void);
static bool are_there_active_breaks (void);
static bool enable_debugger_break (lua_State *L, int id, bool enabled);
static void enable_all_debugger_breaks (lua_State *L, bool enabled);
static bool delete_debugger_break (lua_State *L, int id);
static void delete_all_debugger_breaks (lua_State *L);
static void invoke_debugger (lua_State *L, int event, enum debug_action a);
static void print_full_backtrace (lua_State *L, int event);
static void print_compact_backtrace (lua_State *L, int event);
static char *value_tostring (lua_State *L, int index);
static int get_nframes (lua_State *L);
static int get_nlocals (lua_State *L, int level);
static char *debugger_breaks_report (lua_State *L, const char *fmt);
static char *get_frame_header (lua_State *L, int level, int event);
static char *get_upvalues_report (lua_State *L, int level, const char *fmt);
static char *get_locals_report (lua_State *L, int level, const char *fmt);
static const char *event_tostring (int event);
static char *get_file_line (const char *filename, size_t n);
static char *get_full_backtrace_report (lua_State *L, int event);
static char *get_compact_backtrace_report (lua_State *L, int event);

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

static DECLARE_LUA (debug_here);
static DECLARE_LUA (br);
static DECLARE_LUA (lbr);
static DECLARE_LUA (dbr);
static DECLARE_LUA (cont);
static DECLARE_LUA (next);
static DECLARE_LUA (step);
static DECLARE_LUA (finish);
static DECLARE_LUA (getlocal);
static DECLARE_LUA (getupvalue);
static DECLARE_LUA (setlocal);
static DECLARE_LUA (setupvalue);
static DECLARE_LUA (bt);
static DECLARE_LUA (cbt);

void
define_L_mininim_debugger (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_DEBUGGER);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  /* global shortcuts */
  lua_register (L, "_debug", debug_here);
  lua_register (L, "_br", br);
  lua_register (L, "_lbr", lbr);
  lua_register (L, "_dbr", dbr);
  lua_register (L, "_cont", cont);
  lua_register (L, "_next", next);
  lua_register (L, "_step", step);
  lua_register (L, "_finish", finish);
  lua_register (L, "_getlocal", getlocal);
  lua_register (L, "_getupvalue", getupvalue);
  lua_register (L, "_setlocal", setlocal);
  lua_register (L, "_setupvalue", setupvalue);
  lua_register (L, "_bt", bt);
  lua_register (L, "_cbt", cbt);

  /* Backtrace on error */
  set_string_var (&bt_on_error, "FULL");

  lua_pop (L, 1);
}

BEGIN_LUA (__eq)
{
  lua_pushboolean (L, true);
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "debug")) {
      lua_pushcfunction (L, debug_here);
      return 1;
    } else if (! strcasecmp (key, "br")) {
      lua_pushcfunction (L, br);
      return 1;
    } else if (! strcasecmp (key, "lbr")) {
      lua_pushcfunction (L, lbr);
      return 1;
    } else if (! strcasecmp (key, "dbr")) {
      lua_pushcfunction (L, dbr);
      return 1;
    } else if (! strcasecmp (key, "cont")) {
      lua_pushcfunction (L, cont);
      return 1;
    } else if (! strcasecmp (key, "next")) {
      lua_pushcfunction (L, next);
      return 1;
    } else if (! strcasecmp (key, "step")) {
      lua_pushcfunction (L, step);
      return 1;
    } else if (! strcasecmp (key, "finish")) {
      lua_pushcfunction (L, finish);
      return 1;
    } else if (! strcasecmp (key, "getlocal")) {
      lua_pushcfunction (L, getlocal);
      return 1;
    } else if (! strcasecmp (key, "getupvalue")) {
      lua_pushcfunction (L, getupvalue);
      return 1;
    } else if (! strcasecmp (key, "setlocal")) {
      lua_pushcfunction (L, setlocal);
      return 1;
    } else if (! strcasecmp (key, "setupvalue")) {
      lua_pushcfunction (L, setupvalue);
      return 1;
    } else if (! strcasecmp (key, "bt")) {
      lua_pushcfunction (L, bt);
      return 1;
    } else if (! strcasecmp (key, "cbt")) {
      lua_pushcfunction (L, cbt);
      return 1;
    } else if (! strcasecmp (key, "bt_on_error")) {
      lua_pushstring (L, bt_on_error);
      return 1;
    } else if (! strcasecmp (key, "src")) {
      lua_Debug ar;
      if (lua_getstack (L, 1, &ar)) {
        lua_getinfo (L, "S", &ar);
        if (ar.source[0] == '@') lua_pushstring (L, ar.source + 1);
        else lua_pushnil (L);
      } else lua_pushnil (L);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "bt_on_error")) {
      const char *value = lua_tostring (L, 3);
      if (! value) return 0;
      if (! strcasecmp (value, "FULL") || ! strcasecmp (value, "COMPACT"))
        set_string_var (&bt_on_error, value);
      return 0;
    } else break;
  default: break;
  }
  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushstring (L, "MININIM DEBUGGER INTERFACE");
  return 1;
}
END_LUA

BEGIN_LUA (debug_here)
{
  if (L != main_L) return luaL_error (L, "Only main thread may be debugged");
  invoke_debugger (L, L_INVOKED_EVENT, DEBUG_INVOKED);
  return 0;
}
END_LUA

BEGIN_LUA (br)
{
  struct debugger_break b;

  if (! lua_gettop (L)) {
    enable_all_debugger_breaks (L, true);
    if (are_there_active_breaks ()) start_debugging ();
    return 0;
  } else if (lua_isnumber (L, 1)) {
    int id = lua_tonumber (L, 1);
    if (! enable_debugger_break (L, id, true))
      return luaL_error (L, "%d is not a valid break point", id);
    start_debugging ();
    return 0;
  } else if (lua_isfunction (L, 1)) {
    b.id = debugger_break_nmemb;
    b.type = DEBUG_WATCH;
    b.fn_pattern = NULL;
    b.line = -1;
    b.cond_ref = LUA_NOREF;
    L_set_registry_by_ref (L, &b.cond_ref);
    b.value_ref = LUA_NOREF;
    b.enabled = true;
  } else if (lua_isstring (L, 1) && lua_isnumber (L, 2)
      && lua_isfunction (L, 3)) {
    b.id = debugger_break_nmemb;
    b.type = DEBUG_BREAK;
    b.fn_pattern = xasprintf ("%s", lua_tostring (L, 1));
    b.line = lua_tonumber (L, 2);
    b.cond_ref = LUA_NOREF;
    L_set_registry_by_ref (L, &b.cond_ref);
    b.value_ref = LUA_NOREF;
    b.enabled = true;
  } else if (lua_isstring (L, 1) && lua_isnumber (L, 2)) {
    b.id = debugger_break_nmemb;
    b.type = DEBUG_BREAK;
    b.fn_pattern = xasprintf ("%s", lua_tostring (L, 1));
    b.line = lua_tonumber (L, 2);
    b.cond_ref = LUA_NOREF;
    b.value_ref = LUA_NOREF;
    b.enabled = true;
  } else if (lua_isstring (L, 1)) {
    b.id = debugger_break_nmemb;
    b.type = DEBUG_BREAK;
    b.fn_pattern = xasprintf ("%s", lua_tostring (L, 1));
    b.line = -1;
    b.cond_ref = LUA_NOREF;
    b.value_ref = LUA_NOREF;
    b.enabled = true;
  } else return luaL_error
           (L, "invalid arguments, expecting (fn_pattern, [line_number, "
            "[cond_func]] or (watch_func)");

  debugger_break =
    add_to_array (&b, 1, debugger_break, &debugger_break_nmemb,
                  debugger_break_nmemb, sizeof (b));

  start_debugging ();

  return 0;
}
END_LUA

char *
debugger_breaks_report (lua_State *L, const char *fmt)
{
  if (! debugger_break_nmemb) return xasprintf ("%s", "");

  char *header = fmt_row (fmt, "", "Id", "Type", "File", "Line",
                          "Cond", "Value", "Enabled", "");

  char *hl = hline ('=');
  char *report = xasprintf ("%s\n%s\n%s\n", hl, header, hl);
  al_free (header);
  al_free (hl);

  int i;
  for (i = 0; i < debugger_break_nmemb; i++) {
    struct debugger_break *b = &debugger_break[i];

    char *id = xasprintf ("%i", b->id);

    char *line;
    if (b->line >= 0) line = xasprintf ("%i", b->line);
    else line = xasprintf ("-");

    char *cond;
    if (b->cond_ref != LUA_NOREF) {
      L_get_registry_by_ref (L, b->cond_ref);
      cond = value_tostring (L, -1);
      lua_pop (L, 1);
    } else cond = xasprintf ("-");

    char *value;
    if (b->value_ref != LUA_NOREF) {
      L_get_registry_by_ref (L, b->value_ref);
      value = value_tostring (L, -1);
      lua_pop (L, 1);
    } else value = xasprintf ("-");

    char *old_report = report;
    report = fmt_row (fmt, old_report, id,
                      b->type == DEBUG_BREAK ? "B" : "W",
                      b->fn_pattern ? b->fn_pattern : "-",
                      line, cond, value,
                      b->enabled ? "YES" : "NO",
                      i + 1 < debugger_break_nmemb ? "\n" : "");
    al_free (old_report);

    al_free (id);
    al_free (line);
    al_free (cond);
    al_free (value);
  }

  return report;
}

BEGIN_LUA (lbr)
{
  fmt_begin (7);
  debugger_breaks_report (L, NULL);
  char *fmt = fmt_end ();

  char *debugger_breaks = debugger_breaks_report (L, fmt);
  if (strcmp (debugger_breaks, ""))
    fprintf (stderr, "%s\n", debugger_breaks);
  al_free (debugger_breaks);

  al_free (fmt);
  return 0;
}
END_LUA

BEGIN_LUA (dbr)
{
  if (lua_isnumber (L, 1)) {
    int id = lua_tonumber (L, 1);
    bool r = lua_toboolean (L, 2)
      ? delete_debugger_break (L, id)
      : enable_debugger_break (L, id, false);
    if (! r) return luaL_error (L, "%d is not a valid break point", id);
  } else {
    if (lua_toboolean (L, 1)) delete_all_debugger_breaks (L);
    else enable_all_debugger_breaks (L, false);
  }

  if (debug.action == DEBUG_CONTINUE && ! are_there_active_breaks ())
    stop_debugging ();

  return 0;
}
END_LUA

BEGIN_LUA (cont)
{
  if (debug.action != DEBUG_STOPPED)
    return luaL_error (L, "Debugger not stopped\n");

  debug.action = DEBUG_CONTINUE;
  al_broadcast_cond (debug_cond);
  return 0;
}
END_LUA

BEGIN_LUA (next)
{
  if (debug.action != DEBUG_STOPPED)
    return luaL_error (L, "Debugger not stopped\n");

  debug.action = DEBUG_NEXT;
  al_broadcast_cond (debug_cond);
  return 0;
}
END_LUA

BEGIN_LUA (step)
{
  if (debug.action != DEBUG_STOPPED)
    return luaL_error (L, "Debugger not stopped\n");

  debug.action = DEBUG_STEP;
  al_broadcast_cond (debug_cond);
  return 0;
}
END_LUA

BEGIN_LUA (finish)
{
  if (debug.action != DEBUG_STOPPED)
    return luaL_error (L, "Debugger not stopped\n");

  debug.action = DEBUG_FINISH;
  al_broadcast_cond (debug_cond);
  return 0;
}
END_LUA

BEGIN_LUA (bt)
{
  if (debug.action != DEBUG_STOPPED)
    return luaL_error (L, "Debugger not stopped\n");

  print_full_backtrace (main_L, L_INVOKED_EVENT);
  return 0;
}
END_LUA

BEGIN_LUA (cbt)
{
  if (debug.action != DEBUG_STOPPED)
    return luaL_error (L, "Debugger not stopped\n");

  print_compact_backtrace (main_L, L_INVOKED_EVENT);
  return 0;
}
END_LUA

BEGIN_LUA (getlocal)
{
  int nargs = lua_gettop (L);
  int level;
  int local;
  int arglocaln;

  if (nargs == 1) {
    level = 0;
    arglocaln = 1;
  } else {
    level = luaL_checknumber (L, 1);
    arglocaln = 2;
  }

  if (lua_isnumber (L, arglocaln)) local = lua_tonumber (L, arglocaln);
  else if (lua_isstring (L, arglocaln))
    local = local_index_by_name (main_L, level, lua_tostring (L, arglocaln));
  else return luaL_argerror (L, arglocaln, "expecting local index or name");

  lua_Debug ar;
  if (! lua_getstack (main_L, level, &ar)) return 0;

  const char *name = lua_getlocal (main_L, &ar, local);

  if (name) {
    lua_xmove (main_L, L, 1);
    return 1;
  } else return 0;
}
END_LUA

BEGIN_LUA (getupvalue)
{
  int nargs = lua_gettop (L);
  int level;
  int up;
  int argupn;

  if (nargs == 1) {
    level = 0;
    argupn = 1;
  } else {
    level = luaL_checknumber (L, 1);
    argupn = 2;
  }

  if (lua_isnumber (L, argupn)) up = lua_tonumber (L, argupn);
  else if (lua_isstring (L, argupn))
    up = upvalue_index_by_name (main_L, level, lua_tostring (L, argupn));
  else return luaL_argerror (L, argupn, "expecting upvalue index or name");

  lua_Debug ar;
  if (! lua_getstack (main_L, level, &ar)) return 0;

  lua_getinfo (main_L, "f", &ar);
  const char *name = lua_getupvalue (main_L, -1, up);
  lua_remove (main_L, -2);

  if (name) {
    lua_xmove (main_L, L, 1);
    return 1;
  } else {
    lua_pop (main_L, 1);
    return 0;
  }
}
END_LUA

BEGIN_LUA (setlocal)
{
  int nargs = lua_gettop (L);
  int level;
  int local;
  int arglocaln;

  if (nargs == 2) {
    level = 0;
    arglocaln = 1;
  } else {
    level = luaL_checknumber (L, 1);
    arglocaln = 2;
  }

  if (lua_isnumber (L, arglocaln)) local = lua_tonumber (L, arglocaln);
  else if (lua_isstring (L, arglocaln))
    local = local_index_by_name (main_L, level, lua_tostring (L, arglocaln));
  else return luaL_argerror (L, arglocaln, "expecting local index or name");

  /* check new value */
  luaL_checkany(L, arglocaln + 1);

  lua_Debug ar;
  if (! lua_getstack (main_L, level, &ar)) return 0;

  lua_xmove (L, main_L, 1);
  const char *name = lua_setlocal (main_L, &ar, local);

  lua_pushstring (L, name);
  return 1;
}
END_LUA

BEGIN_LUA (setupvalue)
{
  int nargs = lua_gettop (L);
  int level;
  int up;
  int argupn;

  if (nargs == 2) {
    level = 0;
    argupn = 1;
  } else {
    level = luaL_checknumber (L, 1);
    argupn = 2;
  }

  if (lua_isnumber (L, argupn)) up = lua_tonumber (L, argupn);
  else if (lua_isstring (L, argupn))
    up = upvalue_index_by_name (main_L, level, lua_tostring (L, argupn));
  else return luaL_argerror (L, argupn, "expecting upvalue index or name");

  /* check new value */
  luaL_checkany(L, argupn + 1);

  lua_Debug ar;
  if (! lua_getstack (main_L, level, &ar)) return 0;

  lua_getinfo (main_L, "f", &ar);
  lua_xmove (L, main_L, 1);
  const char *name = lua_setupvalue (main_L, -2, up);
  lua_pop (main_L, 1);

  lua_pushstring (L, name);
  return 1;
}
END_LUA

BEGIN_LUA (L_TRACEBACK)
{
  char *report;
  if (! strcmp (bt_on_error, "FULL")
      && get_nframes (L) <= FULL_TRACEBACK_MAX_FRAMES)
    report = get_full_backtrace_report (L, L_ERROR_EVENT);
  else report = get_compact_backtrace_report (L, L_ERROR_EVENT);

  char *old_report = report;
  report = xasprintf ("%s\n\nerror: %s", old_report, lua_tostring (L, -1));
  al_free (old_report);

  lua_pushstring (L, report);
  al_free (report);

  return 1;
}
END_LUA

int
upvalue_index_by_name (lua_State *L, int level, const char *name)
{
  lua_Debug ar;
  if (! lua_getstack (L, level, &ar)) return -1;

  lua_getinfo (L, "f", &ar);

  const char *cname;
  int up = 0;
  do {
    cname = lua_getupvalue (L, -1, ++up);
    lua_pop (L, 1);
  } while (cname && strcmp (name, cname));

  lua_pop (L, 1);

  return cname ? up : -1;
}

int
local_index_by_name (lua_State *L, int level, const char *name)
{
  lua_Debug ar;
  if (! lua_getstack (L, level, &ar)) return -1;

  const char *cname;
  int local = 0;
  do {
    cname = lua_getlocal (L, &ar, ++local);
    lua_pop (L, 1);
  } while (cname && strcmp (name, cname));

  return cname ? local : -1;
}

bool
are_there_active_breaks (void)
{
  size_t i;
  for (i = 0; i < debugger_break_nmemb; i++)
    if (debugger_break[i].enabled) return true;
  return false;
}

void
start_debugging (void)
{
  bool was_debugging = L_debugging;
  L_debugging = true;
  lua_sethook (main_L, debugger_hook,
               LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);
  if (! was_debugging) {
    if (repl_prompt_ready) fprintf (stderr, "\n");
    repl_update_prompt ();
  }
}

void
stop_debugging (void)
{
  bool was_debugging = L_debugging;
  L_debugging = false;
  if (L_profiling)
    lua_sethook (main_L, profiler_hook, LUA_MASKCALL | LUA_MASKRET, 0);
  else lua_sethook (main_L, NULL, 0, 0);
  if (was_debugging) {
    if (repl_prompt_ready) fprintf (stderr, "\n");
    repl_update_prompt ();
  }
}

bool
enable_debugger_break (lua_State *L, int id, bool enabled)
{
  size_t i;
  for (i = 0; i < debugger_break_nmemb; i++) {
    struct debugger_break *b = &debugger_break[i];
    if (b->id == id) {
      b->enabled = enabled;
      return true;
    }
  }
  return false;
}

void
enable_all_debugger_breaks (lua_State *L, bool enabled)
{
  size_t i;
  for (i = 0; i < debugger_break_nmemb; i++)
    debugger_break[i].enabled = enabled;
}

bool
delete_debugger_break (lua_State *L, int id)
{
  size_t i;
  for (i = 0; i < debugger_break_nmemb; i++) {
    struct debugger_break *b = &debugger_break[i];
    if (b->id == id) {
      al_free (b->fn_pattern);
      luaL_unref (L, LUA_REGISTRYINDEX, b->cond_ref);
      luaL_unref (L, LUA_REGISTRYINDEX, b->value_ref);
      debugger_break =
        remove_from_array (debugger_break, &debugger_break_nmemb, i, 1,
                           sizeof (*debugger_break));
      return true;
    }
  }
  return false;
}

void
delete_all_debugger_breaks (lua_State *L)
{
  size_t i;
  for (i = 0; i < debugger_break_nmemb; i++) {
    struct debugger_break *b = &debugger_break[i];
    al_free (b->fn_pattern);
    luaL_unref (L, LUA_REGISTRYINDEX, b->cond_ref);
    luaL_unref (L, LUA_REGISTRYINDEX, b->value_ref);
  }
  destroy_array ((void **) &debugger_break, &debugger_break_nmemb);
}

void
invoke_debugger (lua_State *L, int event, enum debug_action a)
{
  if (L != main_L) return;
  start_debugging ();
  lua_Debug ar;
  lua_getstack (L, 1, &ar);
  ar.event = event;
  debug.action = a;
  debugger_hook (L, &ar);
}

void
print_full_backtrace (lua_State *L, int event)
{
  char *hl = hline ('-');
  char *report = get_full_backtrace_report (L, event);
  if (strcmp (report, ""))
    fprintf (stderr, "%s\n%s\n", report, hl);
  al_free (report);
  al_free (hl);
}

void
print_compact_backtrace (lua_State *L, int event)
{
  char *hl = hline ('-');
  char *report = get_compact_backtrace_report (L, event);
  if (strcmp (report, ""))
    fprintf (stderr, "%s\n%s\n", report, hl);
  al_free (report);
  al_free (hl);
}

char *
L_typename (lua_State *L, int index)
{
  index = lua_abs_index (L, index);
  char *tname;
  int type = lua_type (L, index);
  switch (type) {
  case LUA_TNONE: return xasprintf ("<no value>");
  case LUA_TNIL: return xasprintf ("nil");
  case LUA_TBOOLEAN: return xasprintf ("boolean");
  case LUA_TSTRING: return xasprintf ("string");
  case LUA_TTHREAD: return xasprintf ("thread");
  case LUA_TNUMBER: {
    lua_Number n = lua_tonumber (L, index);
    if (n == (int64_t) n) return xasprintf ("integer");
    else return xasprintf ("double");
  }
  case LUA_TFUNCTION:
    if (lua_iscfunction (L, index)) return xasprintf ("C function");
    else return xasprintf ("function");
  case LUA_TTABLE:
    lua_rawgeti (L, index, 1);
    if (lua_isnil (L, -1)) tname = xasprintf ("table");
    else tname = xasprintf ("array");
    lua_pop (L, 1);
    return tname;
  case LUA_TLIGHTUSERDATA:
    return xasprintf ("light userdata");
  case LUA_TUSERDATA:
    if (! lua_getmetatable (L, index)) return xasprintf ("userdata");
    lua_rawget (L, LUA_REGISTRYINDEX);
    const char *str = lua_tostring (L, -1);
    tname = xasprintf ("%s", str);
    lua_pop (L, 1);
    return tname;
  default: return xasprintf ("<unknown type>");
  }
}

char *
value_tostring (lua_State *L, int index)
{
  index = lua_abs_index (L, index);
  int type = lua_type (L, index);
  switch (type) {
  case LUA_TNONE: return xasprintf ("<no value>");
  case LUA_TNIL: return xasprintf ("nil");
  case LUA_TBOOLEAN:
    if (lua_toboolean (L, index)) return xasprintf ("true");
    else return xasprintf ("false");
  case LUA_TNUMBER: {
    lua_Number n = lua_tonumber (L, index);
    if (n == (int64_t) n) return xasprintf ("%ji", (int64_t) n);
    else return xasprintf ("%f", n);
  }
  case LUA_TSTRING: {
    int max = 40;
    const char *str = lua_tostring (L, index);
    return xasprintf ("\"%.*s\"%s", max, str,
                      strlen (str) > max ? "..." : "");
  }
  case LUA_TTABLE: {
    const void *table = lua_topointer (L, index);
    return xasprintf ("<table:%p>", table);
  }
  case LUA_TTHREAD: {
    lua_State *thread = lua_tothread (L, index);
    return xasprintf ("<thread:%p>", thread);
  }
  case LUA_TLIGHTUSERDATA: {
    void *light_userdata = lua_touserdata (L, index);
    return xasprintf ("<light userdata:%p>", light_userdata);
  }
  case LUA_TFUNCTION: {
    lua_Debug ar;
    lua_pushvalue (L, index);
    lua_getinfo (L, ">Sn", &ar);
    char *id = ar_tostring (&ar);
    if (id) return id;
    if (lua_iscfunction (L, index)) {
      lua_CFunction cfunction = lua_tocfunction (L, index);
      return xasprintf ("<C function:%p>", cfunction);
    } else {
      const void *function = lua_topointer (L, index);
      return xasprintf ("<function:%p>", function);
    }
  }
  case LUA_TUSERDATA:
    if (! lua_getmetatable (L, index)) {
      void *userdata = lua_touserdata (L, index);
      return xasprintf ("<userdata:%p>", userdata);
    }
    lua_pushstring (L, "__tostring");
    lua_rawget (L, -2);
    lua_remove (L, -2);
    if (! lua_isfunction (L, -1)) {
      lua_pop (L, 1);
      void *userdata = lua_touserdata (L, index);
      return xasprintf ("<userdata:%p>", userdata);
    }
    lua_pushvalue (L, index);
    if (lua_pcall (L, 1, 1, 0)) {
      lua_pop (L, 1);
      void *userdata = lua_touserdata (L, index);
      return xasprintf ("<userdata:%p>", userdata);
    }
    const char *str = lua_tostring (L, -1);
    char *value = xasprintf ("<%s>", str);
    lua_pop (L, 1);
    return value;
  default: return "<unknown value>";
  }
}

char *
get_file_line (const char *filename, size_t n)
{
  FILE *file = fopen (filename, "r");
  if (! file) return xasprintf ("%s", "");
  size_t i;
  char *line = NULL;
  size_t line_size = 0;
  for (i = 0; i < n; i++) {
    if (line) {
      al_free (line);
      line = NULL;
      line_size = 0;
    }
    ssize_t r = getline (&line, &line_size, file);
    if (r < 0) {
      al_free (line);
      fclose (file);
      return xasprintf ("%s", "");
    }
  }
  fclose (file);

  repl_str_char (line, '\n', '\0');

  return line;
}

char *
ar_tostring (lua_Debug *ar)
{
  bool main_chunk = ! strcmp (ar->what, "main");

  if (! strcmp (ar->what, "Lua") || main_chunk) {
    char *s;
    s = xasprintf ("%s:%i", ar->short_src,
                   main_chunk ? ar->currentline : ar->linedefined);

    if (strcmp (ar->namewhat, "")) {
      if (strcmp (ar->name, "?")) {
        char *old_s = s;
        s = xasprintf ("%s (%s '%s')", old_s, ar->namewhat, ar->name);
        al_free (old_s);
      } else {
        char *old_s = s;
        s = xasprintf ("%s (%s)", old_s, ar->namewhat);
        al_free (old_s);
      }
    }

    if (! main_chunk && ar->source[0] == '@') {
      char *line;
      line = get_file_line (ar->source + 1, ar->linedefined);

      char *old_line = line;
      line = trim_string (old_line);
      al_free (old_line);

      char *old_s = s;
      s = xasprintf ("%s \"%s\"", old_s, line);
      al_free (old_s);

      al_free (line);
    }
    return s;
  } else if (! strcmp (ar->what, "C")) {
    if (ar->name) return xasprintf ("%s", ar->name);
    else return NULL;
  } else return NULL;
}

int
get_nframes (lua_State *L)
{
  int level;
  lua_Debug ar;
  for (level = 0; lua_getstack (L, level, &ar); level++);
  return level - 1;
}

int
get_nlocals (lua_State *L, int level)
{
  lua_Debug ar;
  if (! lua_getstack (L, level, &ar)) return 0;

  int i;
  for (i = 1; lua_getlocal (L, &ar, i); i++)
    lua_pop (L, 1);

  return i - 1;
}

char *
get_frame_header (lua_State *L, int level, int event)
{
  lua_Debug ar;
  if (! lua_getstack (L, level, &ar)) return xasprintf ("%s", "");

  lua_getinfo (L, "Snl", &ar);

  char *id = ar_tostring (&ar);
  if (! id) id = xasprintf ("[C function]");

  char *header = xasprintf ("#%i %s", level, id);
  al_free (id);

  if (level == 0) {
    char *old_header = header;
    header = xasprintf ("%s %s", old_header, event_tostring (event));
    al_free (old_header);

    if (event == L_BREAK_EVENT) {
      char *old_header = header;
      header = xasprintf ("%s %i", old_header, debug.break_id);
      al_free (old_header);
    } else if (event == L_WATCH_EVENT) {
      int i = debug.break_id;
      L_get_registry_by_ref (L, debugger_break[i].value_ref);
      char *value = value_tostring (L, -1);
      lua_pop (L, 1);
      char *old_header = header;
      header = xasprintf ("%s %i %s", old_header, i, value);
      al_free (old_header);
      al_free (value);
    }
  }

  return header;
}

char *
get_upvalues_report (lua_State *L, int level, const char *fmt)
{
  lua_Debug ar;
  int i;
  if (! lua_getstack (L, level, &ar)) return xasprintf ("%s", "");
  lua_getinfo (L, "u", &ar);

  char *report = xasprintf ("%s", "");
  const char *name;
  lua_getinfo(L, "f", &ar);
  for (i = 1; (name = lua_getupvalue (L, -1, i)); i++) {
    char *number = xasprintf ("%i", i);
    char *tname = L_typename (L, -1);
    char *value = value_tostring (L, -1);
    if (! strcmp (name, "")) name = "<no name>";
    char *old_report = report;
    report = fmt_row (fmt, old_report,
                      "U", number, tname, name, value,
                      i < ar.nups ? "\n" : "");
    al_free (old_report);
    al_free (number);
    al_free (tname);
    al_free (value);
    lua_pop (L, 1);
  }
  lua_pop (L, 1);

  return report;
}

char *
get_locals_report (lua_State *L, int level, const char *fmt)
{
  lua_Debug ar;
  int i;
  if (! lua_getstack (L, level, &ar)) return xasprintf ("%s", "");

  char *report = xasprintf ("%s", "");
  int nlocals = get_nlocals (L, level);
  for (i = 1; i <= nlocals; i++) {
    const char *name = lua_getlocal (L, &ar, i);
    char *number = xasprintf ("%i", i);
    char *tname = L_typename (L, -1);
    char *value = value_tostring (L, -1);
    char *old_report = report;
    report = fmt_row (fmt, old_report,
                      "L", number, tname, name, value,
                      i < nlocals ? "\n" : "");
    al_free (old_report);
    al_free (number);
    al_free (tname);
    al_free (value);
    lua_pop(L, 1);
  }

  return report;
}

char *
get_cstack_report (lua_State *L, const char *fmt)
{
  int nvalues = lua_gettop (L);
  char *report = xasprintf ("%s", "");
  int i;
  for (i = 1; i <= nvalues; i++) {
    char *number = xasprintf ("%i", i);
    char *tname = L_typename (L, i);
    char *value = value_tostring (L, i);
    char *old_report = report;
    report = fmt_row (fmt, old_report, "S", number, tname,
                      "<no name>", value, i < nvalues ? "\n" : "");
    al_free (old_report);
    al_free (number);
    al_free (tname);
    al_free (value);
  }
  return report;
}

const char *
event_tostring (int event)
{
  switch (event) {
  case LUA_HOOKLINE: return "LINE";
  case LUA_HOOKCALL: return "CALL";
  case LUA_HOOKRET: return "RETURN";
  case LUA_HOOKTAILRET: return "TAIL RETURN";
  case LUA_HOOKCOUNT: return "COUNT";
  case L_ERROR_EVENT: return "ERROR";
  case L_INVOKED_EVENT: return "INVOKED";
  case L_BREAK_EVENT: return "BREAK";
  case L_WATCH_EVENT: return "WATCH";
  default: return "UNKNOWN";
  }
}

char *
get_full_backtrace_report (lua_State *L, int event)
{
  int nframes = get_nframes (L);
  int level;
  char *report = xasprintf ("%s", "");
  for (level = nframes; level >= 0; level--) {
    lua_Debug ar;
    lua_getstack (L, level, &ar);
    lua_getinfo (L, "Snl", &ar);

    /* frame header */
    char *frame_header = get_frame_header (L, level, event);
    if (strcmp (frame_header, "")) {
      char *hl = hline ('-');
      char *old_report = report;
      report = xasprintf ("%s%s%s\n%s\n", old_report,
                          level < nframes ? "\n\n" : "",
                          hl,
                          frame_header);
      al_free (old_report);
      al_free (hl);
    }
    al_free (frame_header);

    /* get variables table format */
    bool cstack = ! strcmp (ar.what, "C") && level == 0;
    fmt_begin (5);
    if (cstack) get_cstack_report (L, NULL);
    get_upvalues_report (L, level, NULL);
    get_locals_report (L, level, NULL);
    char *fmt = fmt_end ();

    /* C stack */
    if (cstack) {
      char *cstack_report = get_cstack_report (L, fmt);
      if (strcmp (cstack_report, "")) {
        char *old_report = report;
        report = xasprintf ("%s\n%s", old_report, cstack_report);
        al_free (old_report);
      }
      al_free (cstack_report);
    }

    /* upvalues */
    char *upvalues_report = get_upvalues_report (L, level, fmt);
    if (strcmp (upvalues_report, "")) {
      char *old_report = report;
      report = xasprintf ("%s\n%s", old_report, upvalues_report);
      al_free (old_report);
    }
    al_free (upvalues_report);

    /* locals */
    char *locals_report = get_locals_report (L, level, fmt);
    if (strcmp (locals_report, "")) {
      char *old_report = report;
      report = xasprintf ("%s\n%s", old_report, locals_report);
      al_free (old_report);
    }
    al_free (locals_report);

    /* free variables table format */
    al_free (fmt);

    /* current line */
    if (((level == 0 && event == LUA_HOOKCALL) || ar.currentline >= 0)
        && ar.source[0] == '@') {
      int n = (level == 0 && event == LUA_HOOKCALL)
        ? ar.linedefined + 1 : ar.currentline;
      char *line = get_file_line (ar.source + 1, n);
      char *old_line = line;
      line = trim_string (old_line);
      al_free (old_line);
      if (strcmp (line, "")) {
        char *old_report = report;
        report = xasprintf ("%s\n\n%i: \"%s\"", old_report, n, line);
        al_free (old_report);
      }
      al_free (line);
    }
  }
  return report;
}

char *
get_compact_backtrace_report (lua_State *L, int event)
{
  int nframes = get_nframes (L);
  int level;
  char *report = xasprintf ("%s", "");
  for (level = nframes; level >= 0; level--) {
    lua_Debug ar;
    lua_getstack (L, level, &ar);
    lua_getinfo (L, "Snl", &ar);

    /* frame header */
    char *frame_header = get_frame_header (L, level, event);
    if (strcmp (frame_header, "")) {
      char *hl = hline ('-');
      char *old_report = report;
      report = xasprintf ("%s%s%s%s%s", old_report,
                          level == nframes ? hl : "",
                          level == nframes ? "\n" : "",
                          level < nframes ? "\n" : "", frame_header);
      al_free (old_report);
      al_free (hl);
    }
    al_free (frame_header);
  }

  return report;
}

void
debugger_hook (lua_State *L, lua_Debug *ar)
{
  if (debug.action == DEBUG_CONTINUE && ! are_there_active_breaks ()) {
    stop_debugging ();
    goto end;
  }

  lua_getinfo (L, "Snl", ar);
  char *id = ar_tostring (ar);
  if (! id) goto end;
  al_free (id);

  int event = ar->event;

  ssize_t i;
  for (i = 0; i < debugger_break_nmemb; i++) {
    struct debugger_break *b = &debugger_break[i];

    switch (b->type) {
    case DEBUG_BREAK:
      if (! b->enabled) continue;

      if (b->fn_pattern && ar->source[0] != '@') continue;

      if (b->fn_pattern &&
          fnmatch (b->fn_pattern, ar->source + 1, FNM_EXTMATCH))
        continue;

      if (b->line >= 0 && ar->currentline != b->line)
        continue;

      if (b->cond_ref != LUA_NOREF) {
        L_get_registry_by_ref (L, b->cond_ref);
        if (ar->source[0] == '@') lua_pushstring (L, ar->source + 1);
        else lua_pushnil (L);
        if (ar->currentline >= 0) lua_pushnumber (L, ar->currentline);
        else lua_pushnil (L);
        if (! L_call (L, 2, 1)) {
          delete_debugger_break (L, b->id);
          i--;
          continue;
        }
        bool b = lua_toboolean (L, -1);
        lua_pop (L, 1);
        if (! b) continue;
      }

      debug.action = DEBUG_BREAK;
      debug.break_id = b->id;
      event = L_BREAK_EVENT;
      goto break_check_end;

    case DEBUG_WATCH:
      if (! b->enabled) continue;

      L_get_registry_by_ref (L, b->cond_ref);
      if (ar->source[0] == '@') lua_pushstring (L, ar->source + 1);
      else lua_pushnil (L);
      if (ar->currentline >= 0) lua_pushnumber (L, ar->currentline);
      else lua_pushnil (L);
      if (! L_call (L, 2, 1)) {
        delete_debugger_break (L, b->id);
        i--;
        continue;
      }
      L_get_registry_by_ref (L, b->value_ref);
      if (lua_equal (L, -1, -2)) {
        lua_pop (L, 2);
        continue;
      }
      lua_pop (L, 1);
      L_set_registry_by_ref (L, &b->value_ref);

      debug.action = DEBUG_WATCH;
      debug.break_id = b->id;
      event = L_WATCH_EVENT;
      goto break_check_end;

    default: assert (false);
    }
  }
 break_check_end:

  switch (debug.action) {
  case DEBUG_CONTINUE: goto end;
  case DEBUG_BREAK: break;
  case DEBUG_WATCH: break;
  case DEBUG_INVOKED: break;
  case DEBUG_STOPPED: default: assert (false); break;
  case DEBUG_STEP: break;
  case DEBUG_NEXT: {
    int nframes = get_nframes (L);
    if (nframes <= debug.nframes) break;
    else goto end;
  }
  case DEBUG_FINISH: {
    int nframes = get_nframes (L);
    if (nframes < debug.nframes) break;
    else goto end;
  }
  }

  /* if (repl_prompt_ready) fprintf (stderr, "\n"); */

  print_full_backtrace (L, event);

  debug.nframes = get_nframes (L);
  debug.action = DEBUG_STOPPED;

  repl_update_prompt ();
  al_wait_cond (debug_cond, repl_mutex);

end:
  if (lua_gethook (L) == debugger_hook && L_profiling)
    profiler_hook (L, ar);
}
