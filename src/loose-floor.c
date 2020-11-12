/*
  loose-floor.c -- loose floor module;

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

struct loose_floor *loose_floor = NULL;
size_t loose_floor_nmemb = 0;
static bool must_sort;

struct loose_floor *
init_loose_floor (struct pos *p, struct loose_floor *l)
{
  npos (p, &l->p);
  l->original_pos = l->p;
  l->i = 0;
  l->resist = LOOSE_FLOOR_RESISTENCE;
  l->action = NO_LOOSE_FLOOR_ACTION;
  l->state = 0;
  l->cant_fall = ext (p);
  l->broken = false;
  return l;
}

void
register_loose_floor (struct pos *p)
{
  struct loose_floor l;

  init_loose_floor (p, &l);

  loose_floor =
    add_to_array (&l, 1, loose_floor, &loose_floor_nmemb,
                  loose_floor_nmemb, sizeof (l));

  sort_loose_floors ();
}

void
sort_loose_floors (void)
{
  qsort (loose_floor, loose_floor_nmemb, sizeof (struct loose_floor),
         compare_loose_floors);
}

int
compare_loose_floors (const void *l0, const void *l1)
{
  return cpos (&((struct loose_floor *) l0)->original_pos,
               &((struct loose_floor *) l1)->original_pos);
}

struct loose_floor *
copy_loose_floor (struct loose_floor *to, struct loose_floor *from)
{
  /* loose floor has no useful state to preserve */
  return to;
}

bool
should_remove_loose_floor (struct loose_floor *l)
{
  return (fg (&l->original_pos) != LOOSE_FLOOR
          && l->action != RELEASE_LOOSE_FLOOR
          && l->action != FALL_LOOSE_FLOOR
          && peq (&l->p, &l->original_pos))
    || l->action == BROKEN_LOOSE_FLOOR;
}

struct loose_floor *
loose_floor_at_pos (struct pos *p)
{
  struct loose_floor l;
  l.original_pos = *p;

  struct loose_floor *ll;

 search:
  ll = bsearch (&l, loose_floor, loose_floor_nmemb, sizeof (l),
                compare_loose_floors);

  if (! ll && fg (p) == LOOSE_FLOOR) {
    register_loose_floor (p);
    goto search;
  }

  return ll;
}

struct loose_floor *
falling_loose_floor_at_pos (struct pos *p)
{
  size_t i;
  struct loose_floor *l;
  for (i = 0; i < loose_floor_nmemb; i++) {
    l = &loose_floor[i];
    if (l->action == FALL_LOOSE_FLOOR
        && peq (&l->p, p))
      return l;
  }
  return NULL;
}

void
remove_loose_floor (struct loose_floor *l)
{
  size_t i =  l - loose_floor;
  loose_floor =
    remove_from_array (loose_floor, &loose_floor_nmemb, i, 1, sizeof (*l));
}

void
release_loose_floor (struct pos *p, struct actor *a)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (fg (p) == LOOSE_FLOOR
      && peq (&l->p, &l->original_pos)
      && (l->action == NO_LOOSE_FLOOR_ACTION
          || l->action == SHAKE_LOOSE_FLOOR)
      && ! l->cant_fall) {
    kid_haptic (a, KID_HAPTIC_COLLISION);
    l->action = RELEASE_LOOSE_FLOOR;
    l->i = 0;
  }
}

void
shake_loose_floor (struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (fg (p) == LOOSE_FLOOR
      && peq (&l->p, &l->original_pos)
      && l->action == NO_LOOSE_FLOOR_ACTION) {
    l->action = SHAKE_LOOSE_FLOOR;
    l->i = 0;
  }
}

void
no_action_loose_floor (struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (fg (p) == LOOSE_FLOOR
      && peq (&l->p, &l->original_pos)
      && (l->action == NO_LOOSE_FLOOR_ACTION
          || l->action == SHAKE_LOOSE_FLOOR)) {
    l->action = NO_LOOSE_FLOOR_ACTION;
    l->i = 0;
  }
}

void
fall_loose_floor (struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (fg (p) == LOOSE_FLOOR
      && peq (&l->p, &l->original_pos)
      && l->action == RELEASE_LOOSE_FLOOR) {
    register_tile_undo (&undo, &l->p,
                       NO_FLOOR, MIGNORE, MIGNORE, MIGNORE,
                       NULL, false, "LOOSE FLOOR RELEASE");
    struct rect r; rect_object (&r, "FLOOR", p);
    l->f.flip = 0;
    l->f.c = r.c;
    l->f.c.y += 3;
    l->action = FALL_LOOSE_FLOOR;
    l->i = 0;
    l->state = 2;
    l->f.parent_id = -1;
    l->resist = LOOSE_FLOOR_RESISTENCE;
    l->f.b = get_correct_falling_loose_floor_bitmap (false);
  }
}

void
break_loose_floor (struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (l->action == FALL_LOOSE_FLOOR) {
    l->action = BROKEN_LOOSE_FLOOR;
    l->i = 0;
  }
}

void
compute_loose_floors (void)
{
  size_t i;

  must_sort = false;

  for (i = 0; i < loose_floor_nmemb;) {
    struct loose_floor *l = &loose_floor[i];
    if (! should_remove_loose_floor (l)) {
      i++; continue;
    }
    /* eprintf ("(%i,%i,%i) --- %i\n", */
    /* 	     l->p.room, l->p.floor, l->p.place, l->action); */

    remove_loose_floor (l);
  }

  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];

    switch (l->action) {
    case SHAKE_LOOSE_FLOOR:
      compute_loose_floor_shake (l);
      break;
    case RELEASE_LOOSE_FLOOR:
      compute_loose_floor_release (l);
      break;
    case FALL_LOOSE_FLOOR: compute_loose_floor_fall (l); break;
    default: break;
    }
  }

  if (must_sort) sort_loose_floors ();
}

void
compute_loose_floor_shake (struct loose_floor *l)
{
  int prev_state = l->state;

  switch (l->i) {
  case 0: l->state = 1;
    /* alert_guards (&l->p); */
    sample_random_loose_floor (&l->p); l->i++; break;
  case 1: l->state = 0; l->i++; break;
  case 2: l->state = 2;
    sample_random_loose_floor (&l->p); l->i++; break;
  case 3: l->state = 0;
    no_action_loose_floor (&l->p); break;
  }

  if (prev_state != l->state)
    register_changed_pos (&l->p);
}

void
compute_loose_floor_release (struct loose_floor *l)
{
  if (l->resist-- > 0) {
    l->state = 0;
    return;
  }

  int prev_state = l->state;

  switch (l->i) {
  case 0: l->state = 1;
    alert_guards (&l->p);
    sample_random_loose_floor (&l->p); l->i++; break;
  case 1: l->state = 0; l->i++; break;
  case 2: l->state = 2;
    sample_random_loose_floor (&l->p); l->i++; break;
  case 3: l->state = 2; l->i++; break;
  case 4: l->state = 0; l->i++; break;
  case 5: l->state = 0; l->i++; break;
  case 6: l->state = 0; l->i++; break;
  case 7: l->state = 2;
    sample_random_loose_floor (&l->p); l->i++; break;
  case 8: l->state = 2; l->i++; break;
  case 9: l->state = 2; l->i++; break;
  case 10: fall_loose_floor (&l->p); break;
  }

  if (prev_state != l->state)
    register_changed_pos (&l->p);
}

ALLEGRO_BITMAP *
get_correct_falling_loose_floor_bitmap (bool broken)
{
  if (broken) return bitmap_object_part ("BROKEN_FLOOR", "MAIN");
  else return bitmap_object_index ("LOOSE_FLOOR", 2);
}

void
compute_loose_floor_fall (struct loose_floor *l)
{
  int speed = 3 * ++l->i;
  if (speed > 29) speed = 29;

  struct frame nf;
  struct frame_offset fo;
  fo.b = l->f.b = get_correct_falling_loose_floor_bitmap (l->broken);
  fo.dx = 0;
  fo.dy = speed;
  next_frame (&l->f, &nf, &fo);
  struct coord mbo_f, mbo_nf;
  struct pos fpmbo_f, nfpmbo_f, fpmbo_nf, nfpmbo_nf;
  enum tile_fg fcmbo_f;
  survey (_mbo, posf, &l->f, &mbo_f, &fpmbo_f, &nfpmbo_f);
  fcmbo_f = fg (&fpmbo_f);
  survey (_mbo, posf, &nf, &mbo_nf, &fpmbo_nf, &nfpmbo_nf);

  struct pos p;

  /* hit character */
  int i;
  for (i = 0; i < actor_nmemb; i++) {
    struct coord kmt, ambo_f, ambo_nf; struct pos kpmt;
    struct actor *a = &actor[i];
    if (is_actor_dead (a)
        || is_actor_fall (a)
        || a->immortal
        || a->loose_floor_immune)
      continue;
    survey (_mt, pos, &a->f, &kmt, &kpmt, NULL);
    coord2room (&mbo_f, kpmt.room, &ambo_f);
    coord2room (&mbo_nf, kpmt.room, &ambo_nf);
    if (peq (&nfpmbo_f, &kpmt)
        && ambo_f.y <= kmt.y
        && ambo_nf.y >= kmt.y
        && ! a->hit_by_loose_floor
        && ! is_kid_hang_or_climb (a)
        && a->action != kid_fall) {
      a->splash = true;
      a->current_hp--;
      a->uncrouch_slowly = true;
      /* ensure kid doesn't crouch in thin air (might occur when hit
         while jumping, for example) */
      place_on_the_ground (&a->f, &a->f.c);
      alert_guards (&kpmt);
      if (a->id == current_kid_id) {
        global_mr.flicker = 2;
        global_mr.color = get_flicker_blood_color ();
      }
      if (a->current_hp <= 0) {
        a->p = kpmt;
        actor_die_suddenly (a);
        a->death_reason = LOOSE_FLOOR_DEATH;
      } else if (a->type == KID) {
        a->hit_by_loose_floor = true;
        play_audio (&hit_wall_audio, NULL, a->id);
        a->next_action = kid_crouch;
        kid_haptic (a, KID_HAPTIC_HARM);
      }
    }
  }

  /* fall */
  if (is_traversable (&fpmbo_f)
      || (peq (&fpmbo_f, &fpmbo_nf)
          && (fpmbo_nf.floor + 1) * PLACE_HEIGHT - mbo_nf.y > 8)) {
    /* the floor hit a rigid structure */
    if (is_rigid_tile (&fpmbo_nf)) prel (&fpmbo_nf, &p, -1, 0);
    /* the floor continue to fall */
    else {
      l->f = nf;
      if (is_traversable (&fpmbo_nf)) l->p = fpmbo_nf;
      must_sort = true;
      return;
    }
    /* the floor hit the ground */
  } else {
    p = fpmbo_f;
    switch (fcmbo_f) {
    case LOOSE_FLOOR: /* loose floor isn't ground */
      l->f = nf;
      l->broken = true;
      l->f.b = get_correct_falling_loose_floor_bitmap (true);
      l->p = p;
      l->i = 0;
      register_tile_undo (&undo, &p,
                         NO_FLOOR, MIGNORE, MIGNORE, MIGNORE,
                         NULL, false, "LOOSE FLOOR CHAIN RELEASE");
      kid_haptic_for_range (&p, _m, 3 * PLACE_WIDTH,
                            KID_HAPTIC_LOOSE_FLOOR_BREAKING);
      play_audio (&broken_floor_audio, &p, -1);
      alert_guards (&p);
      return;
    case OPENER_FLOOR: break_opener_floor (&p); break;
    case CLOSER_FLOOR: break_closer_floor (&p); break;
    case SPIKES_FLOOR: break_spikes_floor (&p); break;
    case LEVEL_DOOR: break_level_door (&p); break;
    default: break;
    }
  }

  /* reach here only if the floor hit a rigid structure or the
     ground */
  enum tile_fg f = fg (&p);
  if (f != LEVEL_DOOR && f != OPENER_FLOOR && f != CLOSER_FLOOR) {
    register_tile_undo (&undo, &p,
                       BROKEN_FLOOR, MIGNORE, NO_ITEM, MIGNORE,
                       NULL, false, "LOOSE FLOOR BREAKING");
  }
  shake_loose_floor_row (&p);
  break_loose_floor (&l->original_pos);
  play_audio (&broken_floor_audio, &p, -1);
  kid_haptic_for_range (&p, _m, 3 * PLACE_WIDTH,
                        KID_HAPTIC_LOOSE_FLOOR_BREAKING);
  alert_guards (&p);
}

void
shake_loose_floor_row (struct pos *p)
{
  struct pos ps;

  for (ps = *p; is_shockwave_medium (&ps)
         && abs (p->place - ps.place) < SHOCKWAVE_RADIUS; ps.place--)
    if (fg (&ps) == LOOSE_FLOOR) shake_loose_floor (&ps);

  for (prel (p, &ps, +0, +1); is_shockwave_medium (&ps)
         && abs (p->place - ps.place) < SHOCKWAVE_RADIUS; ps.place++)
    if (fg (&ps) == LOOSE_FLOOR) shake_loose_floor (&ps);
}

void
sample_random_loose_floor (struct pos *p)
{
  switch (prandom (2)) {
  case 0: play_audio (&loose_floor_00_audio, p, -1);
  case 1: play_audio (&loose_floor_01_audio, p, -1);
  case 2: play_audio (&loose_floor_02_audio, p, -1);
  }
}

void
draw_falling_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct loose_floor *l = falling_loose_floor_at_pos (p);
  if (! l) return;

  if (l->action == FALL_LOOSE_FLOOR) {
    struct frame f = l->f;
    frame2room (&f, room_view, &f.c);
    f.b = get_correct_falling_loose_floor_bitmap (l->broken);
    push_drawn_rectangle (bitmap);
    draw_frame (bitmap, &f);
    struct pos pb; prel (p, &pb, +1, +0);
    draw_tile_fg_front (bitmap, &pb, &f);
    redraw_drawn_rectangle (pop_drawn_rectangle (), &pb);
  } else return;
}

void
draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (is_fake (p)) {
    switch (abs (ext (p)) % 2) {
    case 0: draw_object_part (bitmap, "LOOSE_FLOOR", 1, p);
      break;
    case 1: draw_object_part (bitmap, "LOOSE_FLOOR", 2, p);
      break;
    }
    return;
  }

  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_object (bitmap, "FLOOR", p); break;
  case 1: draw_object_part (bitmap, "LOOSE_FLOOR", 1, p); break;
  case 2: draw_object_part (bitmap, "LOOSE_FLOOR", 2, p); break;
  }
}

void
loose_floor_fall_debug (void)
{
  int i;
  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];
    if (l->action != FALL_LOOSE_FLOOR) continue;
    struct pos pv; pos2room (&l->p, room_view, &pv);
    struct coord cv; coord2room (&l->f.c, room_view, &cv);
    eprintf ("(%i,%i,%i) == (%i,%i,%i) <%i,%f,%f> <%i,%f,%f> ? %i ? %i\n",
	     l->p.room, l->p.floor, l->p.place,
	     pv.room, pv.floor, pv.place,
	     l->f.c.room, l->f.c.x, l->f.c.y,
	     cv.room, cv.x, cv.y,
	     peq (&l->p, &pv),
	     cpos (&l->p, &pv));
    draw_falling_loose_floor
      (global_mr.cell[global_mr.dx][global_mr.dy].screen,
       &loose_floor[i].p);
  }
}
