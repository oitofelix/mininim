/*
  guard-die.c -- guard die module;

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

struct frameset guard_die_frameset[GUARD_DIE_FRAMESET_NMEMB];
struct frameset fat_guard_die_frameset[GUARD_DIE_FRAMESET_NMEMB];
struct frameset vizier_die_frameset[GUARD_DIE_FRAMESET_NMEMB];
struct frameset skeleton_die_frameset[GUARD_DIE_FRAMESET_NMEMB];
struct frameset shadow_die_frameset[GUARD_DIE_FRAMESET_NMEMB];

static void init_guard_die_frameset (void);
static void init_fat_guard_die_frameset (void);
static void init_vizier_die_frameset (void);
static void init_skeleton_die_frameset (void);
static void init_shadow_die_frameset (void);
static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

/* guard */
ALLEGRO_BITMAP *guard_die_00, *guard_die_01, *guard_die_02, *guard_die_03,
  *guard_die_04, *guard_die_05, *guard_die_spiked_00, *guard_die_chopped_00;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_die_00, *fat_guard_die_01, *fat_guard_die_02,
  *fat_guard_die_03, *fat_guard_die_04, *fat_guard_die_05;

/* vizier */
ALLEGRO_BITMAP *vizier_die_00, *vizier_die_01, *vizier_die_02, *vizier_die_03,
  *vizier_die_04, *vizier_die_05;

/* skeleton */
ALLEGRO_BITMAP *skeleton_die_00, *skeleton_die_01, *skeleton_die_02, *skeleton_die_03,
  *skeleton_die_04, *skeleton_die_05, *skeleton_die_spiked_00,
  *skeleton_die_chopped_00;

/* shadow */
ALLEGRO_BITMAP *shadow_die_00, *shadow_die_01, *shadow_die_02, *shadow_die_03,
  *shadow_die_04, *shadow_die_05, *shadow_die_spiked_00,
  *shadow_die_chopped_00;

static void
init_guard_die_frameset (void)
{
  struct frameset frameset[GUARD_DIE_FRAMESET_NMEMB] =
    {{guard_die_00,-1,0},{guard_die_01,+0,0},{guard_die_02,-3,+1},
     {guard_die_03,-2,+2},{guard_die_04,+0,+0},{guard_die_05,-2,+0}};

  memcpy (&guard_die_frameset, &frameset,
          GUARD_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_fat_guard_die_frameset (void)
{
  struct frameset frameset[GUARD_DIE_FRAMESET_NMEMB] =
    {{fat_guard_die_00,-1,0},{fat_guard_die_01,+0,0},{fat_guard_die_02,-3,+1},
     {fat_guard_die_03,-2,+2},{fat_guard_die_04,+0,+0},{fat_guard_die_05,+0,+0}};

  memcpy (&fat_guard_die_frameset, &frameset,
          GUARD_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_vizier_die_frameset (void)
{
  struct frameset frameset[GUARD_DIE_FRAMESET_NMEMB] =
    {{vizier_die_00,-1,0},{vizier_die_01,+0,0},{vizier_die_02,-3,+1},
     {vizier_die_03,-2,+2},{vizier_die_04,+0,+0},{vizier_die_05,-2,+0}};

  memcpy (&vizier_die_frameset, &frameset,
          GUARD_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_skeleton_die_frameset (void)
{
  struct frameset frameset[GUARD_DIE_FRAMESET_NMEMB] =
    {{skeleton_die_00,-8,+0},{skeleton_die_01,+0,+2},{skeleton_die_02,+0,+2},
     {skeleton_die_03,+0,+0},{skeleton_die_04,+0,+0},{skeleton_die_05,+0,+0}};

  memcpy (&skeleton_die_frameset, &frameset,
          GUARD_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

static void
init_shadow_die_frameset (void)
{
  struct frameset frameset[GUARD_DIE_FRAMESET_NMEMB] =
    {{shadow_die_00,-1,+0},{shadow_die_01,+0,+0},{shadow_die_02,-3,+1},
     {shadow_die_03,-2,+1},{shadow_die_04,+0,+2},{shadow_die_05,+2,+1}};

  memcpy (&shadow_die_frameset, &frameset,
          GUARD_DIE_FRAMESET_NMEMB * sizeof (struct frameset));
}

struct frameset *
get_guard_die_frameset (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_die_frameset;
  case FAT_GUARD: return fat_guard_die_frameset;
  case VIZIER: return vizier_die_frameset;
  case SKELETON: return skeleton_die_frameset;
  case SHADOW: return shadow_die_frameset;
  }
}

void
load_guard_die (void)
{
  /* guard */
  guard_die_00 = load_bitmap (GUARD_DIE_00);
  guard_die_01 = load_bitmap (GUARD_DIE_01);
  guard_die_02 = load_bitmap (GUARD_DIE_02);
  guard_die_03 = load_bitmap (GUARD_DIE_03);
  guard_die_04 = load_bitmap (GUARD_DIE_04);
  guard_die_05 = load_bitmap (GUARD_DIE_05);
  guard_die_spiked_00 = load_bitmap (GUARD_DIE_SPIKED_00);
  guard_die_chopped_00 = load_bitmap (GUARD_DIE_CHOPPED_00);

  /* fat guard */
  fat_guard_die_00 = load_bitmap (FAT_GUARD_DIE_00);
  fat_guard_die_01 = load_bitmap (FAT_GUARD_DIE_01);
  fat_guard_die_02 = load_bitmap (FAT_GUARD_DIE_02);
  fat_guard_die_03 = load_bitmap (FAT_GUARD_DIE_03);
  fat_guard_die_04 = load_bitmap (FAT_GUARD_DIE_04);
  fat_guard_die_05 = load_bitmap (FAT_GUARD_DIE_05);

  /* vizier */
  vizier_die_00 = load_bitmap (VIZIER_DIE_00);
  vizier_die_01 = load_bitmap (VIZIER_DIE_01);
  vizier_die_02 = load_bitmap (VIZIER_DIE_02);
  vizier_die_03 = load_bitmap (VIZIER_DIE_03);
  vizier_die_04 = load_bitmap (VIZIER_DIE_04);
  vizier_die_05 = load_bitmap (VIZIER_DIE_05);

  /* skeleton */
  skeleton_die_00 = load_bitmap (SKELETON_DIE_00);
  skeleton_die_01 = load_bitmap (SKELETON_DIE_01);
  skeleton_die_02 = load_bitmap (SKELETON_DIE_02);
  skeleton_die_03 = load_bitmap (SKELETON_DIE_03);
  skeleton_die_04 = load_bitmap (SKELETON_DIE_04);
  skeleton_die_05 = load_bitmap (SKELETON_DIE_05);
  skeleton_die_spiked_00 = load_bitmap (SKELETON_DIE_SPIKED_00);
  skeleton_die_chopped_00 = load_bitmap (SKELETON_DIE_CHOPPED_00);

  /* shadow */
  shadow_die_00 = load_bitmap (SHADOW_DIE_00);
  shadow_die_01 = load_bitmap (SHADOW_DIE_01);
  shadow_die_02 = load_bitmap (SHADOW_DIE_02);
  shadow_die_03 = load_bitmap (SHADOW_DIE_03);
  shadow_die_04 = load_bitmap (SHADOW_DIE_04);
  shadow_die_05 = load_bitmap (SHADOW_DIE_05);
  shadow_die_spiked_00 = load_bitmap (SHADOW_DIE_SPIKED_00);
  shadow_die_chopped_00 = load_bitmap (SHADOW_DIE_CHOPPED_00);

  /* frameset */
  init_guard_die_frameset ();
  init_fat_guard_die_frameset ();
  init_vizier_die_frameset ();
  init_skeleton_die_frameset ();
  init_shadow_die_frameset ();
}

void
unload_guard_die (void)
{
  /* guard */
  destroy_bitmap (guard_die_00);
  destroy_bitmap (guard_die_01);
  destroy_bitmap (guard_die_02);
  destroy_bitmap (guard_die_03);
  destroy_bitmap (guard_die_04);
  destroy_bitmap (guard_die_05);
  destroy_bitmap (guard_die_spiked_00);
  destroy_bitmap (guard_die_chopped_00);

  /* fat guard */
  destroy_bitmap (fat_guard_die_00);
  destroy_bitmap (fat_guard_die_01);
  destroy_bitmap (fat_guard_die_02);
  destroy_bitmap (fat_guard_die_03);
  destroy_bitmap (fat_guard_die_04);
  destroy_bitmap (fat_guard_die_05);

  /* vizier */
  destroy_bitmap (vizier_die_00);
  destroy_bitmap (vizier_die_01);
  destroy_bitmap (vizier_die_02);
  destroy_bitmap (vizier_die_03);
  destroy_bitmap (vizier_die_04);
  destroy_bitmap (vizier_die_05);

  /* skeleton */
  destroy_bitmap (skeleton_die_00);
  destroy_bitmap (skeleton_die_01);
  destroy_bitmap (skeleton_die_02);
  destroy_bitmap (skeleton_die_03);
  destroy_bitmap (skeleton_die_04);
  destroy_bitmap (skeleton_die_05);
  destroy_bitmap (skeleton_die_spiked_00);
  destroy_bitmap (skeleton_die_chopped_00);

  /* shadow */
  destroy_bitmap (shadow_die_00);
  destroy_bitmap (shadow_die_01);
  destroy_bitmap (shadow_die_02);
  destroy_bitmap (shadow_die_03);
  destroy_bitmap (shadow_die_04);
  destroy_bitmap (shadow_die_05);
  destroy_bitmap (shadow_die_spiked_00);
  destroy_bitmap (shadow_die_chopped_00);
}

void
guard_resurrect (struct anim *g)
{
  struct coord nc; struct pos np, pm;
  survey (_m, pos, &g->f, &nc, &pm, &np);
  g->current_lives = g->total_lives;
  g->death_reason = NO_DEATH;
  g->action = guard_normal;
  place_frame (&g->f, &g->f, get_guard_normal_bitmap (g->type),
               &pm, g->f.dir == LEFT ? +16 : +22, +14);
  place_on_the_ground (&g->f, &g->f.c);
  reset_murder_spikes_floor (g->id);
}

void
raise_skeleton (struct anim *s)
{
  s->oaction = s->action;
  s->action = raise_skeleton;
  s->f.flip = (s->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (s->oaction != raise_skeleton) {
    play_sample (skeleton_sample, NULL, s->id);
    place_frame (&s->f, &s->f, skeleton_die_frameset[2].frame,
                 &s->p, (s->f.dir == LEFT) ? +12 : +0, +43);
    s->i = 2;
  }

  if (s->i-- == 0) {
    guard_vigilant (s);
    return;
  }

  s->fo.b = skeleton_die_frameset[s->i].frame;
  s->fo.dx = -skeleton_die_frameset[s->i + 1].dx;
  s->fo.dy = -skeleton_die_frameset[s->i + 1].dy;
  next_frame (&s->f, &s->f, &s->fo);
}

ALLEGRO_BITMAP *
get_guard_die_spiked_bitmap (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_die_spiked_00;
  case SKELETON: return skeleton_die_spiked_00;
  case SHADOW: return shadow_die_spiked_00;
  }
}

void
guard_die_spiked (struct anim *g)
{
  if (con (&g->p)->fg != SPIKES_FLOOR) {
    guard_die_properly (g);
    return;
  }

  g->oaction = g->action;
  g->action = guard_die_spiked;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (g->oaction != guard_die_spiked) {
    g->current_lives = 0;
    g->splash = true;
    g->death_reason = SPIKES_DEATH;

    assert (con (&g->p)->fg == SPIKES_FLOOR);
    struct spikes_floor *s = spikes_floor_at_pos (&g->p);
    s->i = 4;
    s->state = 5;
    s->inactive = true;
    s->murdered_anim = g->id;

    if (g->type == SKELETON)
      play_sample (skeleton_sample, NULL, g->id);
    else play_sample (spiked_sample, NULL, g->id);

    struct anim *ke = get_anim_by_id (g->oenemy_id);
    if (ke && ke->id == current_kid_id
        && g->death_reason != SHADOW_FIGHT_DEATH)
      play_sample (glory_sample, NULL, ke->id);
    g->oenemy_id = -1;

    if (ke) upgrade_skill (&ke->skill, &g->skill);
  }

  int dy;
  if (g->type == SKELETON) dy = +45;
  else dy = (g->f.dir == LEFT) ? +32 : +31;

  ALLEGRO_BITMAP *bitmap = get_guard_die_spiked_bitmap (g->type);
  place_frame (&g->f, &g->f, bitmap,
               &g->p, (g->f.dir == LEFT) ? +8 : +9, dy);

  g->xf.b = NULL;
}

ALLEGRO_BITMAP *
get_guard_die_chopped_bitmap (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_die_chopped_00;
  case SKELETON: return skeleton_die_chopped_00;
  case SHADOW: return shadow_die_chopped_00;
  }
}

void
guard_die_chopped (struct anim *g)
{
  if (con (&g->p)->fg != CHOPPER) {
    guard_die_properly (g);
    return;
  }

  g->oaction = g->action;
  g->action = guard_die_chopped;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  g->current_lives = 0;

  int dx, dy;

  if (g->type == SHADOW) {
    dx = (g->f.dir == LEFT) ? -8 : -7;
    dy = +47;
  } else {
    dx = (g->f.dir == LEFT) ? -10 : -13;
    dy = (g->type == SKELETON) ? +45 : +43;
  }

  ALLEGRO_BITMAP *bitmap = get_guard_die_chopped_bitmap (g->type);
  place_frame (&g->f, &g->f, bitmap, &g->p, dx, dy);

  if (g->oaction != guard_die_chopped) {
    struct anim *ke = get_anim_by_id (g->oenemy_id);
    if (ke && ke->id == current_kid_id
        && g->death_reason != SHADOW_FIGHT_DEATH)
      play_sample (glory_sample, NULL, ke->id);
    g->oenemy_id = -1;

    if (ke) upgrade_skill (&ke->skill, &g->skill);
  }

  g->xf.b = NULL;
}

void
guard_die_suddenly (struct anim *g)
{
  if (con (&g->p)->fg == SPIKES_FLOOR
      || con (&g->p)->fg == CHOPPER) {
    guard_die_properly (g);
    return;
  }

  g->oaction = g->action;
  g->action = guard_die_suddenly;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  g->current_lives = 0;

  struct frameset *frameset = get_guard_die_frameset (g->type);

  int dy = (g->type == SKELETON) ? +44 : +47;

  place_frame (&g->f, &g->f, frameset[5].frame,
               &g->p, (g->f.dir == LEFT)
               ? +9 : +4, dy);

  if (g->oaction != guard_die_suddenly) {
    struct anim *ke = get_anim_by_id (g->oenemy_id);
    if (ke && ke->id == current_kid_id
        && g->death_reason != SHADOW_FIGHT_DEATH)
      play_sample (glory_sample, NULL, ke->id);
    g->oenemy_id = -1;

    if (ke) upgrade_skill (&ke->skill, &g->skill);

    if (con (&g->p)->fg == CLOSER_FLOOR)
      closer_floor_at_pos (&g->p)->unresponsive = true;
  }

  g->xf.b = NULL;

  g->hit_by_loose_floor = false;

  /* fall */
  struct coord nc;
  struct pos np, pm;
  survey (_m, pos, &g->f, &nc, &pm, &np);
  if (is_strictly_traversable (&pm)) {
    guard_fall (g);
    return;
  }

  /* depressible floors */
  update_depressible_floor (g, -12, -28);
}

void
guard_die (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_die;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  if (g->oaction != guard_die) {
    place_frame (&g->f, &g->f, guard_die_frameset[0].frame,
                 &g->p, (g->f.dir == LEFT)
                 ? +13 : +21, (g->type == SHADOW) ? +18 : +17);
    g->i = -1, g->j = 0;
    if (g->type == SKELETON)
      play_sample (skeleton_sample, NULL, g->id);

    struct anim *ke = get_anim_by_id (g->oenemy_id);
    if (ke && ke->id == current_kid_id
        && g->death_reason != SHADOW_FIGHT_DEATH)
      play_sample (glory_sample, NULL, ke->id);
    g->oenemy_id = -1;

    if (ke) upgrade_skill (&ke->skill, &g->skill);

    if (con (&g->p)->fg == CLOSER_FLOOR)
      closer_floor_at_pos (&g->p)->unresponsive = true;

    g->xf.b = NULL;
  }

  g->current_lives = 0;

  g->i = g->i < 5 ? g->i + 1 : 5;

  struct frameset *frameset = get_guard_die_frameset (g->type);
  select_frame (g, frameset, g->i);

  if (g->j >= 1) g->fo.dx = g->fo.dy = 0;
  if (g->i == 5) g->j = 1;
  if (g->j == 1) g->j++;

  return true;
}

static bool
physics_in (struct anim *g)
{
  /* fall */
  struct coord nc;
  struct pos np, pm;
  survey (_m, pos, &g->f, &nc, &pm, &np);
  if (is_strictly_traversable (&pm)) {
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  if (g->i == 0) update_depressible_floor (g, -6, -12);
  else if (g->i == 1) update_depressible_floor (g, -5, -12);
  else if (g->i == 2) update_depressible_floor (g, -4, -13);
  else if (g->i == 3) update_depressible_floor (g, -2, -18);
  else if (g->i == 4) update_depressible_floor (g, 0, -27);
  else if (g->i == 5) update_depressible_floor (g, -12, -28);
  else keep_depressible_floor (g);
}

void
guard_die_properly (struct anim *g)
{
  switch (con (&g->p)->fg) {
  case SPIKES_FLOOR: guard_die_spiked (g); break;
  case CHOPPER: guard_die_chopped (g); break;
  default: guard_die_suddenly (g); break;
  }
}

bool
is_guard_dead (struct frame *f)
{
  int i;

  /* guard */
  for (i = 0; i < GUARD_DIE_FRAMESET_NMEMB; i++)
    if (f->b == guard_die_frameset[i].frame) return true;

  if (f->b == guard_die_spiked_00) return true;
  if (f->b == guard_die_chopped_00) return true;

  /* fat guard */
  for (i = 0; i < GUARD_DIE_FRAMESET_NMEMB; i++)
    if (f->b == fat_guard_die_frameset[i].frame) return true;

  /* vizier */
  for (i = 0; i < GUARD_DIE_FRAMESET_NMEMB; i++)
    if (f->b == vizier_die_frameset[i].frame) return true;

  /* skeleton */
  for (i = 0; i < GUARD_DIE_FRAMESET_NMEMB; i++)
    if (f->b == skeleton_die_frameset[i].frame) return true;

  if (f->b == skeleton_die_spiked_00) return true;
  if (f->b == skeleton_die_chopped_00) return true;

  /* shadow */
  for (i = 0; i < GUARD_DIE_FRAMESET_NMEMB; i++)
    if (f->b == shadow_die_frameset[i].frame) return true;

  if (f->b == shadow_die_spiked_00) return true;
  if (f->b == shadow_die_chopped_00) return true;

  return false;
}

bool
is_guard_chopped (struct frame *f)
{
  /* guard */
  if (f->b == guard_die_chopped_00) return true;

  /* skeleton */
  if (f->b == skeleton_die_chopped_00) return true;

  /* shadow */
  if (f->b == shadow_die_chopped_00) return true;

  return false;
}
