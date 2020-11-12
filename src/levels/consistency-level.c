/*
  consistency-level.c -- consistency level module;

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

static void start (void);
static void end (struct pos *p);

static void
start (void)
{
  create_actor (&actor[0], 0, NULL, 0);
  actor[1].controllable = true;
}

static void
end (struct pos *p)
{
  next_level_number = global_level.n + 1;
  quit_anim = NEXT_LEVEL;
}

struct level *
next_consistency_level (struct level *l, int n)
{
  random_seed = n;
  /* random_seed = time (NULL); */
  /* eprintf ("LEVEL NUMBER: %u\n", random_seed); */

  randomize_memory (l, sizeof (*l));

  size_t i;
  random_pos (l, &l->start_pos);
  for (i = 0; i < l->event_nmemb; i++)
    random_pos (l, &event (l, i)->p);
  for (i = 0; i < l->guard_nmemb; i++)
    random_pos (l, &guard (l, i)->p);

  l->n = n;
  l->nominal_n = n;
  l->start = start;
  l->special_events = NULL;
  l->end = end;
  l->next_level = next_consistency_level;
  l->cutscene = NULL;

  l->em = prandom (1) ? DUNGEON : PALACE;
  l->hue = HUE_NONE;
  l->has_sword = true;
  return l;
}
