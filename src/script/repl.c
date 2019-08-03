/*
  repl.c -- Lua REPL module;

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

/******************************************************************************
* Copyright (C) 1994-2006 Tecgraf, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include "mininim.h"

#define MININIM_HISTORY_ENV "MININIM_HISTORY"
#define MININIM_HISTSIZE_ENV "MININIM_HISTSIZE"
#define MININIM_HISTSIZE_DEFAULT 1024

#if WINDOWS_PORT
int repl_priority = 512;
#else
int repl_priority = 1;
#endif

char *myhist;
static char *lhandler_line;

static void
lhandler (char *line)
{
  rl_callback_handler_remove ();
  if (! line) {
    fprintf (rl_outstream, "\n");
    L_debug_main_thread ();
    lhandler_line = xasprintf ("%s", "");
  } else lhandler_line = line;
}

static bool should_update_prompt;
bool repl_prompt_ready;

void
repl_update_prompt (void)
{
  should_update_prompt = true;
}

/* Read a line from the terminal with line editing */
static int lua_readline(lua_State *L, const char *prompt)
{
  rl_callback_handler_install (prompt, lhandler);

  should_update_prompt = false;
  repl_prompt_ready = true;

  unlock_lua ();
  struct timeval timeout;
  fd_set set;
  while (! lhandler_line && ! al_get_thread_should_stop (repl_thread)
         && ! should_update_prompt) {
    FD_ZERO (&set);
    FD_SET (fileno (rl_instream), &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    int r = select (FD_SETSIZE, &set, NULL, NULL, &timeout);
    /* QUESTION: it used to be a thread locking around
       rl_callback_read_char, but it was removed because it didn't
       play nice with the Windows port.  Is it practically
       safer/useful to use that for other systems? */
    if (r > 0) rl_callback_read_char ();
  }
  lock_lua ();
  repl_prompt_ready = false;

  if (! lhandler_line) return 0;

  lua_pushstring(L, lhandler_line);
  lua_pushliteral(L, "\n");
  lua_concat(L, 2);
  free(lhandler_line);
  lhandler_line = NULL;
  return 1;
}

/* Add a line to the history */
static void lua_saveline(lua_State *L, const char *s)
{
  const char *p;
  for (p = s; isspace(*p); p++) ;
  if (*p) {
    size_t n = strlen(s)-1;
    if (s[n] != '\n') {
      add_history(s);
    } else {
      lua_pushlstring(L, s, n);
      s = lua_tostring(L, -1);
      add_history(s);
      lua_pop(L, 1);
    }
  }
}

/* Reserved lua keywords */
static const char * const reskeywords[] = {
  "and", "break", "do", "else", "elseif", "end", "false",
  "for", "function", "if", "in", "local", "nil", "not", "or",
  "repeat", "return", "then", "true", "until", "while", NULL
};

static int valididentifier(const char *s)
{
  if (!(isalpha(*s) || *s == '_')) return 0;
  for (s++; *s; s++) if (!(isalpha(*s) || isdigit(*s) || *s == '_')) return 0;
  return 1;
}

/* Dynamically resizable match list */
typedef struct {
  char **list;
  size_t idx, allocated, matchlen;
} dmlist;

/* Add prefix + string + suffix to list and compute common prefix */
static int dmadd(dmlist *ml, const char *p, size_t pn, const char *s, int suf)
{
  char *t = NULL;

  if (ml->idx+1 >= ml->allocated &&
      !(ml->list = realloc(ml->list, sizeof(char *)*(ml->allocated += 32))))
    return -1;

  if (s) {
    size_t n = strlen(s);
    if (!(t = (char *)malloc(sizeof(char)*(pn+n+(suf?2:1))))) return 1;
    memcpy(t, p, pn);
    memcpy(t+pn, s, n);
    n += pn;
    t[n] = suf;
    if (suf) t[++n] = '\0';

    if (ml->idx == 0) {
      ml->matchlen = n;
    } else {
      size_t i;
      for (i = 0; i < ml->matchlen && i < n && ml->list[1][i] == t[i]; i++) ;
      ml->matchlen = i;	/* Set matchlen to common prefix */
    }
  }

  ml->list[++ml->idx] = t;
  return 0;
}

/* Get __index field of metatable of object on top of stack */
static int getmetaindex(lua_State *L)
{
  if (!lua_getmetatable(L, -1)) { lua_pop(L, 1); return 0; }
  lua_pushstring(L, "__index");
  lua_rawget(L, -2);
  lua_replace(L, -2);
  if (lua_isnil(L, -1) || lua_rawequal(L, -1, -2)) { lua_pop(L, 2); return 0; }
  lua_replace(L, -2);
  return 1;
} /* 1: obj -- val, 0: obj -- */

/* Get field from object on top of stack. Avoid calling metamethods */
static int safegetfield(lua_State *L, const char *s, size_t n)
{
  int i = 20; /* Avoid infinite metatable loops */
  do {
    if (lua_istable(L, -1)) {
      lua_pushlstring(L, s, n);
      lua_rawget(L, -2);
      if (!lua_isnil(L, -1)) { lua_replace(L, -2); return 1; }
      lua_pop(L, 1);
    }
  } while (--i > 0 && getmetaindex(L));
  /* lua_pop(L, 1); */
  return 0;
} /* 1: obj -- val, 0: obj -- */

/* Completion function */
static char **mycomplete(const char *text, int start, int end)
{
  dmlist ml;
  const char *s;
  size_t i, n, dot;
  int savetop;

  rl_completion_suppress_append = 1;

  if (!(text[0] == '\0' || isalpha(text[0]) || text[0] == '_')) return NULL;

  ml.list = NULL;
  ml.idx = ml.allocated = ml.matchlen = 0;

  savetop = lua_gettop(repl_L);

  /* Lua 5.0 */
  /* lua_pushvalue(repl_L, LUA_GLOBALSINDEX); */
  /* ------- */
  /* Lua 5.3 */
  lua_pushvalue(repl_L, LUA_REGISTRYINDEX);
  lua_pushinteger(repl_L, LUA_RIDX_GLOBALS);
  lua_gettable(repl_L, -2);
  /* ------- */
  for (n = (size_t)(end-start), i = dot = 0; i < n; i++)
    if (text[i] == '.' || text[i] == ':') {
      if (!safegetfield(repl_L, text+dot, i-dot)) goto error; /* invalid prefix */
      dot = i+1; /* points to first char after dot/colon */
    }

  /* Add all matches against keywords if there is no dot/colon */
  if (dot == 0)
    for (i = 0; (s = reskeywords[i]) != NULL; i++)
      if (!strncmp(s, text, n) && dmadd(&ml, NULL, 0, s, ' ')) goto error;

  /* Add all valid matches from all tables/metatables */
  i = 20; /* Avoid infinite metatable loops */
  do {
    if (lua_istable(repl_L, -1))
      for (lua_pushnil(repl_L); lua_next(repl_L, -2); lua_pop(repl_L, 1))
        if (lua_type(repl_L, -2) == LUA_TSTRING) {
          s = lua_tostring(repl_L, -2);
          /* Only match names starting with '_' if explicitly requested */
          if (!strncmp(s, text+dot, n-dot) && valididentifier(s) &&
              (*s != '_' || text[dot] == '_')) {
            int suf = ' '; /* default suffix is a space */
            switch (lua_type(repl_L, -1)) {
            case LUA_TTABLE:	suf = '.'; break; /* No way to guess ':' */
            case LUA_TFUNCTION:	suf = '('; break;
            case LUA_TUSERDATA:
              if (lua_getmetatable(repl_L, -1))
                { lua_pop(repl_L, 1); suf = ':'; }
              break;
            }
            if (dmadd(&ml, text, dot, s, suf)) goto error;
          }
        }
  } while (--i > 0 && getmetaindex(repl_L));

  if (ml.idx > 1) {
    /* list[0] holds the common prefix of all matches (may be "") */
    if (!(ml.list[0] = (char *)malloc(sizeof(char)*(ml.matchlen+1)))) {
error:
      lua_settop(repl_L, savetop);
      return NULL;
    }
    memcpy(ml.list[0], ml.list[1], ml.matchlen);
    ml.list[0][ml.matchlen] = '\0';
    /* Add the NULL list terminator */
    if (dmadd(&ml, NULL, 0, NULL, 0)) goto error;
  } else if (ml.idx == 1) {
    ml.list[0] = ml.list[1];		/* Only return common prefix */
    ml.list[1] = NULL;
  }

  lua_settop(repl_L, savetop);
  return ml.list;
}

/* Initialize library */
static void lua_initline(lua_State *L, char *pname)
{
  char *s;

  /* This allows for $if lua ... $endif in ~/.inputrc */
  rl_readline_name = pname;
  /* Break words at every non-identifier character except '.' and ':' */
  rl_completer_word_break_characters =
    "\t\r\n !\"#$%&'()*+,-/;<=>?@[\\]^`{|}~";
  rl_completer_quote_characters = "\"'";
  rl_completion_append_character = '\0';
  rl_attempted_completion_function = mycomplete;
  rl_initialize();

  /* Start using history, optionally set history size and load history file */
  using_history ();
  int myhistsize;
  s = getenv (MININIM_HISTSIZE_ENV);
  if (s) myhistsize = atoi (s);
  else myhistsize = MININIM_HISTSIZE_DEFAULT;
  stifle_history (myhistsize);
  myhist = getenv (MININIM_HISTORY_ENV);
  if (! myhist) myhist = history_filename;
  read_history (myhist);
}

/* Finalize library */
static void lua_exitline(lua_State *L)
{
  /* Optionally save history file */
  write_history (myhist);
}

static void lstop (lua_State *L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(L, NULL, 0, 0);
  lua_pushnil(L);
  luaL_error(L, "thread interrupted");
}


static void laction (int i) {
  lua_sethook(repl_L, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static void l_message (const char *msg) {
  fprintf(rl_outstream, "%s\n", msg);
}


static int report (lua_State *L, int status) {
  const char *msg;
  if (status) {
    msg = lua_tostring(L, -1);
    if (msg == NULL) {
      const char *str;
      lua_getglobal(L, "LUA_DEFAULT_ERROR");  /* try global variable */
      str = lua_tostring(L, -1);
      lua_pop(L, 1);
      if (str) {
        if (*str != '\0') msg = str;
      } else msg = "(error with no message)";
    }
    if (msg) l_message (msg);
    lua_pop(L, 1);
  }
  return status;
}

void
repl_multithread (lua_State *L, lua_Debug *ar)
{
  unlock_lua ();
  lock_lua ();
  if (al_get_thread_should_stop (repl_thread))
    luaL_error (L, "main thread terminated");
}

static int lcall (lua_State *L, int narg, int clear) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */

  lua_pushcfunction (L, L_TRACEBACK);
  lua_insert(L, base);  /* put it under chunk and args */
  sighandler_t handler = signal (SIGINT, laction);
  if (repl_priority > 0)
    lua_sethook (L, repl_multithread, LUA_MASKCOUNT, repl_priority);
  status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
  lua_sethook(L, NULL, 0, 0);
  signal (SIGINT, handler);
  lua_remove(L, base);  /* remove traceback function */
  return status;
}


static char *get_prompt (lua_State *L, int firstline) {
  char *p = xasprintf
    ("%s%s%s%s%s", firstline ? "MININIM" : ".......",
     L_debugging || L_profiling? ":" : "",
     L_debugging ? "D" : "",
     L_profiling ? "P" : "",
     firstline ? "> " : ">> ");
  return p;
}


static int incomplete (lua_State *L, int status) {
  if (status == LUA_ERRSYNTAX &&
         strstr(lua_tostring(L, -1), "near `<eof>'") != NULL) {
    lua_pop(L, 1);
    return 1;
  }
  else
    return 0;
}


static int load_string (lua_State *L) {
  int status;
  lua_settop(L, 0);

  char *prompt = get_prompt(L, 1);
  int rl_status = lua_readline(L, prompt);
  al_free (prompt);
  if (! rl_status) return -1;

  if (lua_tostring(L, -1)[0] == '=') {  /* line starts with `=' ? */
    lua_pushfstring(L, "return %s", lua_tostring(L, -1)+1);/* `=' -> `return' */
    lua_remove(L, -2);  /* remove original line */
  }

  for (;;) {  /* repeat until gets a complete line */
    status = luaL_loadbuffer(L, lua_tostring(L, 1), lua_rawlen(L, 1), "=stdin");
    if (!incomplete(L, status)) break;  /* cannot try to add lines? */
    char *prompt = get_prompt(L, 0);
    int rl_status = lua_readline(L, prompt);
    al_free (prompt);
    if (! rl_status) return -1;
    lua_concat(L, lua_gettop(L));  /* join lines */
  }

  lua_saveline(L, lua_tostring(L, 1));
  lua_remove(L, 1);  /* remove line */

  return status;
}


static void manual_input (lua_State *L) {
  int status;
  lua_initline(L, "mininim");
  while (! al_get_thread_should_stop (repl_thread)) {
    status = load_string(L);
    if (status == -1) {
      if (! al_get_thread_should_stop (repl_thread)
          && ! should_update_prompt)
        fprintf (rl_outstream, "\n");
      continue;
    }
    if (status == 0) status = lcall(L, 0, 0);
    report(L, status);
    if (status == 0) {
      fmt_begin (5);
      get_cstack_report (L, NULL);
      char *fmt = fmt_end ();
      char *cstack_report = get_cstack_report (L, fmt);

      if (strcmp (cstack_report, ""))
        fprintf (stderr, "%s\n", cstack_report);
      al_free (cstack_report);

      al_free (fmt);
    }
  }
  lua_settop(L, 0);  /* clear stack */
  fputs("\n", rl_outstream);
  lua_exitline(L);
}

static int pmain (lua_State *L) {
  lua_pop (L, 1);
  manual_input(L);
  return 0;
}

ALLEGRO_THREAD *repl_thread;
lua_State *repl_L;
int repl_thread_ref = LUA_NOREF;
ALLEGRO_COND *repl_cond;

void *
repl (ALLEGRO_THREAD *thread, void *L)
{
  lock_lua ();

  al_broadcast_cond (repl_cond);

  rl_instream = stdin;
  rl_outstream = stderr;

  if (! L_debugging) {
    fprintf (rl_outstream,
             "MININIM %s %s\n\n", VERSION, about_dialog.text);
    fprintf (rl_outstream, LUA_VERSION " " LUA_COPYRIGHT "\n");
  }

  int status;

  /* Lua 5.0 */
  /* status = lua_cpcall (L, &pmain, NULL); */
  /* ------- */

  /* Lua 5.3 */
  lua_pushcfunction (L, &pmain);
  lua_pushlightuserdata (L, NULL);
  status = lua_pcall (L, 1, 0, 0);
  /* ------- */

  report (L, status);

  rl_callback_handler_remove ();

  unlock_lua ();
  return NULL;
}
