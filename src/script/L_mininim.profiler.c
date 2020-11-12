/*
  L_mininim.profiler.c -- mininim.profiler script module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

bool L_profiling;

struct profiler_stats {
  char *id;
  int depth;
  uint64_t count;
  double total_time;
  double start_time;
} *profiler_stats;
size_t profiler_stats_nmemb;

static void reset (void);
static char *report (const char *fmt);
static int compare_profiler_stats_by_id (const void *is0, const void *is1);
static int compare_profiler_stats_by_score (const void *is0, const void *is1);
static struct profiler_stats *get_profiler_stats (char *id);

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

static DECLARE_LUA (start);
static DECLARE_LUA (stop);

void
define_L_mininim_profiler (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_PROFILER);

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
    if (! strcasecmp (key, "start")) {
      lua_pushcfunction (L, start);
      return 1;
    } else if (! strcasecmp (key, "stop")) {
      lua_pushcfunction (L, stop);
      return 1;
    } else if (! strcasecmp (key, "report")) {
      fmt_begin (3);
      report (NULL);
      char *fmt = fmt_end ();
      char *s = report (fmt);
      lua_pushstring (L, s);
      al_free (s);
      al_free (fmt);
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
    if (! strcasecmp (key, "report")) {
      reset ();
      return 0;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushstring (L, "MININIM PROFILER INTERFACE");
  return 1;
}
END_LUA

BEGIN_LUA (start)
{
  L_profiling = true;
  lua_sethook (main_L, profiler_hook, LUA_MASKCALL | LUA_MASKRET
               | (L_debugging ? LUA_MASKLINE : 0), 0);
  return 0;
}
END_LUA

BEGIN_LUA (stop)
{
  L_profiling = false;
  if (L_debugging)
    lua_sethook (main_L, debugger_hook,
                 LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);
  else lua_sethook (main_L, NULL, 0, 0);
  return 0;
}
END_LUA

void
reset (void)
{
  size_t i;
  for (i = 0; i < profiler_stats_nmemb; i++)
    al_free (profiler_stats[i].id);
  destroy_array ((void *) &profiler_stats, &profiler_stats_nmemb);
}

char *
report (const char *fmt)
{
  if (! profiler_stats_nmemb)
    return xasprintf ("No profiling statistics collected");

  if (fmt)
    qsort (profiler_stats, profiler_stats_nmemb, sizeof (*profiler_stats),
           compare_profiler_stats_by_score);

  char *header = fmt_row (fmt, "", "Count", "Time", "Function", "");

  char *hl = hline ('=');
  char *report = xasprintf ("%s\n%s\n%s\n", hl, header, hl);
  al_free (header);
  al_free (hl);

  size_t i;
  for (i = 0; i < profiler_stats_nmemb; i++) {
    struct profiler_stats *s = &profiler_stats[i];
    char *count = xasprintf ("%" PRIu64, s->count);
    char *total_time = xasprintf ("%.4f", s->total_time);
    char *old_report = report;
    report = fmt_row (fmt, old_report,
                      count, total_time,
                      s->id, i + 1 < profiler_stats_nmemb ? "\n" : "");
    al_free (old_report);
    al_free (count);
    al_free (total_time);
  }

  if (fmt)
    qsort (profiler_stats, profiler_stats_nmemb, sizeof (*profiler_stats),
           compare_profiler_stats_by_id);

  return report;
}

int
compare_profiler_stats_by_id (const void *is0, const void *is1)
{
  struct profiler_stats *s0 = (struct profiler_stats *) is0;
  struct profiler_stats *s1 = (struct profiler_stats *) is1;
  return strcmp (s0->id, s1->id);
}

int
compare_profiler_stats_by_score (const void *is0, const void *is1)
{
  struct profiler_stats *s0 = (struct profiler_stats *) is0;
  struct profiler_stats *s1 = (struct profiler_stats *) is1;
  if (s0->count < s1->count) return 1;
  else if (s0->count > s1->count) return -1;
  else if (s0->total_time < s1->total_time) return 1;
  else if (s0->total_time > s1->total_time) return -1;
  else return 0;
}

struct profiler_stats *
get_profiler_stats (char *id)
{
  struct profiler_stats s, *rs;
  s.id = id;

  rs = bsearch (&s, profiler_stats, profiler_stats_nmemb, sizeof (s),
                compare_profiler_stats_by_id);

  return rs;
}

void
profiler_hook (lua_State *L, lua_Debug *ar)
{
  double time = al_get_time ();
  char *id;

  switch (ar->event) {
  case LUA_HOOKCALL:
    lua_getinfo (L, "Sn", ar);
    id = ar_tostring (ar);
    if (id) {
      struct profiler_stats *s = get_profiler_stats (id);
      if (s) {
        /* if a tail call function remove it from the list */
        if (s->total_time == 0) {
          size_t i = s - profiler_stats;
          profiler_stats =
            remove_from_array (profiler_stats, &profiler_stats_nmemb, i, 1,
                               sizeof (*s));
          break;
        }
        if (s->depth++ == 0) s->start_time = time;
        s->count++;
        al_free (id);
      } else {
        struct profiler_stats s;
        s.id = id;
        s.count = 1;
        s.total_time = 0;
        s.start_time = time;
        s.depth = 1;

        profiler_stats =
          add_to_array (&s, 1, profiler_stats, &profiler_stats_nmemb,
                        profiler_stats_nmemb, sizeof (s));

        qsort (profiler_stats, profiler_stats_nmemb, sizeof (*profiler_stats),
               compare_profiler_stats_by_id);
      }
    }
    break;
  case LUA_HOOKRET:
    lua_getinfo (L, "Sn", ar);
    id = ar_tostring (ar);
    if (id) {
      struct profiler_stats *s = get_profiler_stats (id);
      if (s) {
        if (--s->depth == 0) s->total_time += time - s->start_time;
      }
      al_free (id);
    }
    break;
  default: break;
  }

  if (lua_gethook (L) == profiler_hook && L_debugging)
    debugger_hook (L, ar);
}
