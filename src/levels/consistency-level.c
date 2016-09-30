/*
  consistency-level.c -- consistency level module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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
  create_anim (&anima[0], 0, NULL, 0);
  anima[1].controllable = true;
}

static void
end (struct pos *p)
{
  next_level = global_level.n + 1;
  quit_anim = NEXT_LEVEL;
}

void
next_consistency_level (struct level *lv, int n)
{
  random_seed = n;
  /* random_seed = time (NULL); */
  /* printf ("LEVEL NUMBER: %u\n", random_seed); */

  randomize_memory (lv, sizeof (*lv));

  size_t i;
  random_pos (lv, &lv->start_pos);
  for (i = 0; i < EVENTS; i++)
    random_pos (lv, &event (lv, i)->p);
  for (i = 0; i < GUARDS; i++)
    random_pos (lv, &guard (lv, i)->p);

  /* for (i = 0; i < 2; i++) fix_level (lv); */

  lv->n = n;
  lv->nominal_n = n;
  lv->start = start;
  lv->special_events = NULL;
  lv->end = end;
  lv->next_level = next_consistency_level;
  lv->cutscene = NULL;

  lv->em = DUNGEON;
  lv->hue = HUE_NONE;
}
