/*
  guard.c -- guard module;

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

#include <stdio.h>
#include <string.h>
#include "prince.h"
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/keyboard.h"
#include "kernel/array.h"
#include "kernel/random.h"
#include "kernel/timer.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/level.h"
#include "engine/room.h"
#include "engine/door.h"
#include "engine/loose-floor.h"
#include "engine/opener-floor.h"
#include "engine/closer-floor.h"
#include "engine/sword.h"
#include "engine/fight.h"
#include "guard.h"

/* bitmaps */
ALLEGRO_BITMAP *guard_life, *guard_splash;

/* sounds */
ALLEGRO_SAMPLE *guard_hit_sample;

void
load_guard (void)
{
  load_guard_normal ();

  /* bitmaps */
  guard_life = load_bitmap (GUARD_LIFE);
  guard_splash = load_bitmap (GUARD_SPLASH);

  /* sounds */
  guard_hit_sample = load_sample (GUARD_HIT_SAMPLE);
}

void
unload_guard (void)
{
  unload_guard_normal ();

  /* bitmaps */
  al_destroy_bitmap (guard_life);
  al_destroy_bitmap (guard_splash);

  /* sounds */
  al_destroy_sample (guard_hit_sample);
}

struct anim *
create_guard (struct anim *g0, struct anim *g1, struct pos *p, enum dir dir)
{
  if (! g0) {
    g1->f.b = guard_normal_00;
    g1->fo.b = guard_normal_00;
    g1->action = guard_normal;
    g1->total_lives = 3;
    g1->current_lives = 3;
    g1->enemy_id = -1;
    g1->skill.defense_prob = 70;
    g1->skill.counter_attack_prob = 70;
    g1->skill.counter_defense_prob = 70;

    place_frame (&g1->f, &g1->f, guard_normal_00, p,
                 g1->f.dir == LEFT ? +22 : +31, +15);
    update_depressible_floor (g1, -4, -10);
  }

  return g1;
}

void
draw_guard_frame (ALLEGRO_BITMAP *bitmap, struct anim *g, enum vm vm)
{
  struct coord c;

  if (g->invisible) return;

  struct frame f = g->f;
  struct frame_offset xf = g->xf;

  /* palette pal = get_palette (vm); */
  /* f.b = apply_palette (f.b, pal); */
  /* xf.b = apply_palette (xf.b, pal); */

  /* if (hgc) { */
  /*   f.b = apply_palette (f.b, hgc_palette); */
  /*   xf.b = apply_palette (xf.b, hgc_palette); */
  /* } */

  draw_frame (bitmap, &f);
  draw_xframe (bitmap, &f, &xf);

  /* if (g->splash) { */
  /*   ALLEGRO_BITMAP *splash = apply_palette (v_guard_splash, pal); */
  /*   if (hgc) splash = apply_palette (splash, hgc_palette); */
  /*   draw_bitmapc (splash, bitmap, splash_coord (&g->f, &c), g->f.flip); */
  /* } */
}
