/*
  multi-room.c -- multi-room module;

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

ALLEGRO_BITMAP *room0;
bool con_caching;

ALLEGRO_COLOR room0_wall_color[3][4][11];

struct multi_room mr;
int room_view;

struct pos *changed_pos = NULL;
size_t changed_pos_nmemb = 0;

int *changed_room = NULL;
size_t changed_room_nmemb = 0;

void
optimize_changed_pos (void)
{
  if (changed_pos_nmemb < FLOORS * PLACES) return;
  size_t total = 0;
  struct pos p; new_pos (&p, &global_level, -1, -1, -1);
  for (p.room = 1; p.room < ROOMS; p.room++) {
    int per_room = 0;
    if (has_room_changed (p.room)) continue;
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        if (get_changed_pos (&p)
            && ++per_room > OPTIMIZE_CHANGED_POS_THRESHOLD) {
          register_changed_room (p.room);
          goto next_room;
        }
        if (++total > changed_pos_nmemb) return;
      }
  next_room:;
  }
}

void
register_changed_pos (struct pos *p)
{
  struct pos np; npos (p, &np);

  if (get_changed_pos (&np)) return;

  changed_pos =
    add_to_array (&np, 1, changed_pos, &changed_pos_nmemb,
                  changed_pos_nmemb, sizeof (*changed_pos));

  qsort (changed_pos, changed_pos_nmemb, sizeof (*changed_pos),
         (m_comparison_fn_t) cpos);
}

struct pos *
get_changed_pos (struct pos *p)
{
  return bsearch (p, changed_pos, changed_pos_nmemb, sizeof (*p),
                  (m_comparison_fn_t) cpos);
}

struct pos *
get_changed_pos_by_room (int room)
{
  struct pos p;
  p.room = room;

  return bsearch (&p, changed_pos, changed_pos_nmemb, sizeof (p),
                  (m_comparison_fn_t) cpos_by_room);
}

void
remove_changed_pos (struct pos *p)
{
  size_t i =  p - changed_pos;
  changed_pos =
    remove_from_array (changed_pos, &changed_pos_nmemb, i, 1, sizeof (*p));
}

void
register_changed_room (int room)
{
  if (has_room_changed (room)) return;

  changed_room =
    add_to_array (&room, 1, changed_room, &changed_room_nmemb,
                  changed_room_nmemb, sizeof (room));

  qsort (changed_room, changed_room_nmemb, sizeof (room),
         (m_comparison_fn_t) cint);

  struct pos p;
  new_pos (&p, &global_level, room, -1, -1);

  p.place = 0;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    register_changed_pos (&p);

  p.place = PLACES - 1;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    register_changed_pos (&p);

}

bool
has_room_changed (int room)
{
  return bsearch (&room, changed_room, changed_room_nmemb, sizeof (room),
                  (m_comparison_fn_t) cint);
}

static void
destroy_multi_room (void)
{
  int x, y;

  if (mr.cell) {
    for (x = 0; x < mr.w; x++) {
      for (y = 0; y < mr.h; y++) {
        destroy_bitmap (mr.cell[x][y].screen);
        destroy_bitmap (mr.cell[x][y].cache);
      }
      al_free (mr.cell[x]);
      al_free (mr.last.cell[x]);
    }
    al_free (mr.cell);
    al_free (mr.last.cell);
  };
}

void
redim_multi_room (int w, int h)
{
  destroy_multi_room ();

  mr.w = w;
  mr.h = h;

  if (! cutscene && mr.fit_mode == MR_FIT_NONE) {
    mr.fit_w = mr.w;
    mr.fit_h = mr.h;
  }

  int x, y;
  mr.cell = xcalloc (w, sizeof (* mr.cell));
  mr.last.cell = xcalloc (w, sizeof (* mr.last.cell));
  for (x = 0; x < w; x++) {
    mr.cell[x] = xcalloc (h, sizeof (** mr.cell));
    mr.last.cell[x] = xcalloc (h, sizeof (** mr.last.cell));
    for (y = 0; y < h; y++) {
      mr.cell[x][y].screen = NULL;
      mr.cell[x][y].cache = NULL;
    }
  }
}

void
create_multi_room_bitmaps (void)
{
  set_target_backbuffer (display);
  al_set_new_bitmap_flags (ALLEGRO_VIDEO_BITMAP);

  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++) {
      int sw = cutscene ? CUTSCENE_WIDTH : ORIGINAL_WIDTH;
      int sh = cutscene ? CUTSCENE_HEIGHT : ORIGINAL_HEIGHT
        - (y < mr.h - 1 ? 8 : 0);
      destroy_bitmap (mr.cell[x][y].screen);
      destroy_bitmap (mr.cell[x][y].cache);
      mr.cell[x][y].screen = create_bitmap (sw, sh);
      mr.cell[x][y].cache = create_bitmap (sw, sh);
      clear_bitmap (mr.cell[x][y].screen, BLACK);
      clear_bitmap (mr.cell[x][y].cache, BLACK);
    }
}

void
set_multi_room (int w, int h)
{
  redim_multi_room (w, h);
  create_multi_room_bitmaps ();
}

void
mr_get_resolution (int *w, int *h)
{
  *w = cutscene ? CUTSCENE_WIDTH : ORIGINAL_WIDTH * mr.w;
  *h = cutscene ? CUTSCENE_HEIGHT : ROOM_HEIGHT * mr.h + 11;
}

void
clear_multi_room_cells (void)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++) {
      mr.cell[x][y].done = false;
      mr.cell[x][y].room = -1;
    }
}

bool
next_multi_room_cell (int *rx, int *ry)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room > 0
          && ! mr.cell[x][y].done) {
        *rx = x;
        *ry = y;
        return true;
      }

  return false;
}

int
mr_count_rooms (void)
{
  int x, y, c = 0;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      c += (mr.cell[x][y].room > 0) ? 1 : 0;
  return c;
}

bool
mr_topmost_cell (int *rx, int *ry)
{
  int x, y;
  for (y = 0; y < mr.h; y++)
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room > 0) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

bool
mr_leftmost_cell (int *rx, int *ry)
{
  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room > 0) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

bool
mr_bottommost_cell (int *rx, int *ry)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = mr.w - 1; x >= 0; x--)
      if (mr.cell[x][y].room > 0) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

bool
mr_rightmost_cell (int *rx, int *ry)
{
  int x, y;
  for (x = mr.w - 1; x >= 0; x--)
    for (y = mr.h - 1; y >= 0; y--)
      if (mr.cell[x][y].room > 0) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

void
mr_center_room (int room)
{
  int x, y, lc = 0, c = 0, ld = INT_MAX;
  float ldc = INFINITY;
  int lx = mr.x;
  int ly = mr.y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      mr_set_origin (room, x, y);
      c = mr_count_rooms ();
      int cx, cy;
      mr_topmost_cell (&cx, &cy);
      int ca = cy;
      mr_bottommost_cell (&cx, &cy);
      int cb = (mr.h - 1) - cy;
      mr_leftmost_cell (&cx, &cy);
      int cl = cx;
      mr_rightmost_cell (&cx, &cy);
      int cr = (mr.w - 1) - cx;;
      int d = abs (ca - cb) + abs (cl - cr);
      float dc = dist_cart (x, y, (mr.w - 1) / 2, (mr.h - 1) / 2);
      if (c > lc
          || (c == lc && d < ld)
          || (c == lc && d == ld && dc < ldc)) {
        lx = x;
        ly = y;
        lc = c;
        ld = d;
        ldc = dc;
      }
    }

  mr_set_origin (room, lx, ly);
  mr.select_cycles = SELECT_CYCLES;
}

void
mr_map_room (int r, int x, int y)
{
  int rl = roomd (&global_level, r, LEFT);
  int rr = roomd (&global_level, r, RIGHT);
  int ra = roomd (&global_level, r, ABOVE);
  int rb = roomd (&global_level, r, BELOW);

  mr.cell[x][y].room = r;
  mr.cell[x][y].done = true;
  if (x > 0 && mr.cell[x - 1][y].room == -1)
    mr.cell[x - 1][y].room = rl;
  if (x < mr.w - 1 && mr.cell[x + 1][y].room == -1)
    mr.cell[x + 1][y].room = rr;
  if (y > 0 && mr.cell[x][y - 1].room == -1)
    mr.cell[x][y - 1].room = ra;
  if (y < mr.h - 1 && mr.cell[x][y + 1].room == -1)
    mr.cell[x][y + 1].room = rb;
}

struct mr_origin *
mr_save_origin (struct mr_origin *o)
{
  o->w = mr.w;
  o->h = mr.h;
  o->x = mr.x;
  o->y = mr.y;
  o->room = mr.room;
  return o;
}

void
mr_restore_origin (struct mr_origin *o)
{
  if (o->w == mr.w && o->h == mr.h)
    mr_set_origin (o->room, o->x, o->y);
  else mr_center_room (o->room);
}

void
mr_set_origin (int room, int rx, int ry)
{
  int old_mr_room = mr.room;
  mr.room = room;
  mr.x = rx;
  mr.y = ry;

  int x, y;
  clear_multi_room_cells ();
  mr_map_room (mr.room, mr.x, mr.y);
  while (next_multi_room_cell (&x, &y))
    mr_map_room (mr.cell[x][y].room, x, y);
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++) {
      if (mr.cell[x][y].room < 0) mr.cell[x][y].room = 0;
    }

  if (old_mr_room != mr.room) navigation_menu ();
}

void
mr_stabilize_origin (struct mr_origin *o, enum dir d)
{
  int x, y, tx, ty;
  float ld, cd = INFINITY;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room) {
        ld = dist_cart (x, y, o->x, o->y);
        if (ld < cd) {
          tx = x;
          ty = y;
          cd = ld;
        }
      }

  mr_set_origin (mr.cell[tx][ty].room, tx, ty);
}

void
mr_focus_room (int room)
{
  int x, y;
  if (mr_coord (room, -1, &x, &y))
    mr_set_origin (room, x, y);
  else mr_center_room (room);
  mr.select_cycles = SELECT_CYCLES;
}

void
mr_focus_cell (int x, int y)
{
  if (x < 0 || y < 0) return;
  if (! mr.cell[x][y].room) return;
  mr_set_origin (mr.cell[x][y].room, x, y);
  mr.select_cycles = SELECT_CYCLES;
}

void
mr_focus_mouse (void)
{
  struct mouse_coord m;
  get_mouse_coord (&m);
  mr_focus_cell (m.x, m.y);
}

void
mr_select_trans (enum dir d)
{
  int dx = +0, dy = +0;
  switch (d) {
  case LEFT: dx = -1; break;
  case RIGHT: dx = +1; break;
  case ABOVE: dy = -1; break;
  case BELOW: dy = +1; break;
  }

  int r = roomd (&global_level, mr.room, d);
  if (r) {
    nmr_coord (mr.x + dx, mr.y + dy, &mr.x, &mr.y);
    mr_set_origin (r, mr.x, mr.y);
  }

  mr.select_cycles = SELECT_CYCLES;
}

void
mr_view_trans (enum dir d)
{
  int x, y, dx = +0, dy = +0;

  mr.select_cycles = SELECT_CYCLES;

  struct mr_origin o;
  mr_save_origin (&o);

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      int r = mr.cell[x][y].room;
      if (r <= 0) continue;
      r = roomd (&global_level, r, d);
      if (r) {
        mr_set_origin (r, x, y);
        mr_stabilize_origin (&o, d);
        return;
      }
    }

  switch (d) {
  case RIGHT:
    if (mr.x > 0) dx = -1; break;
  case LEFT:
    if (mr.x < mr.w - 1) dx = +1; break;
  case BELOW:
    if (mr.y > 0) dy = -1; break;
  case ABOVE:
    if (mr.y < mr.h - 1) dy = +1; break;
  }

  mr_set_origin (mr.room, mr.x + dx, mr.y + dy);
  mr_stabilize_origin (&o, d);
}

void
mr_view_page_trans (enum dir d)
{
  int x, y;

  mr.select_cycles = SELECT_CYCLES;

  struct mr_origin o;
  mr_save_origin (&o);

  switch (d) {
  case RIGHT:
    mr_rightmost_cell (&x, &y);
    mr_set_origin (mr.cell[x][y].room, 0, y);
    mr_view_trans (RIGHT);
    break;
  case LEFT:
    mr_leftmost_cell (&x, &y);
    mr_set_origin (mr.cell[x][y].room, mr.w - 1, y);
    mr_view_trans (LEFT);
    break;
  case BELOW:
    mr_bottommost_cell (&x, &y);
    mr_set_origin (mr.cell[x][y].room, x, 0);
    mr_view_trans (BELOW);
    break;
  case ABOVE:
    mr_topmost_cell (&x, &y);
    mr_set_origin (mr.cell[x][y].room, x, mr.h - 1);
    mr_view_trans (ABOVE);
    break;
  }

  mr_stabilize_origin (&o, d);
}

bool
has_mr_view_changed (void)
{
  if (mr.w != mr.last.w || mr.h != mr.last.h) return true;

  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (mr.last.cell[x][y].room != mr.cell[x][y].room)
        return true;

  return false;
}

void
mr_update_last_settings (void)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      mr.last.cell[x][y].room = mr.cell[x][y].room;

  mr.last.w = mr.w;
  mr.last.h = mr.h;
  mr.last.x = mr.x;
  mr.last.y = mr.y;
  mr.last.room = mr.room;

  mr.last.level = global_level.n;
  mr.last.em = em;
  mr.last.vm = vm;
  mr.last.hgc = hgc;
  mr.last.hue = hue;
  mr.last.mouse_pos = mouse_pos;
  mr.last.display_width = al_get_display_width (display);
  mr.last.display_height = al_get_display_height (display);

  mr.full_update = false;
  if (mr.busy && ! is_dedicatedly_replaying ()) {
    mr.busy = false;
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
  }
}

void
draw_animated_background (ALLEGRO_BITMAP *bitmap, int room)
{
  room_view = room;

  struct pos p; new_pos (&p, &global_level, room, -1, -1);

  for (p.floor = FLOORS; p.floor >= 0; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_no_floor_selection (bitmap, &p);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      if (fake (&p) == WALL) continue;
      draw_fire (bitmap, &p, vm);
      draw_balcony_stars (bitmap, &p, vm);
    }
}

void
draw_animated_foreground (ALLEGRO_BITMAP *bitmap, int room)
{
  room_view = room;

  struct pos p; new_pos (&p, &global_level, room, -1, -1);

  /* loose_floor_fall_debug (); */

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_falling_loose_floor (bitmap, &p, em, vm);
    }

  draw_anims (bitmap, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_potion (bitmap, &p, em, vm);
      if (is_sword (&p)) draw_sword (bitmap, &p, vm, false);
    }

  /* editor graphics */
  switch (edit) {
  case EDIT_GUARD:
  case EDIT_GUARD_SELECT:
  case EDIT_GUARD_SKILL:
  case EDIT_SKILL_LEGACY_TEMPLATES:
  case EDIT_GUARD_TYPE:
  case EDIT_GUARD_STYLE:
  case EDIT_GUARD_SKILL_ATTACK:
  case EDIT_GUARD_SKILL_COUNTER_ATTACK:
  case EDIT_GUARD_SKILL_DEFENSE:
  case EDIT_GUARD_SKILL_COUNTER_DEFENSE:
  case EDIT_GUARD_SKILL_ADVANCE:
  case EDIT_GUARD_SKILL_RETURN:
  case EDIT_GUARD_SKILL_REFRACTION:
  case EDIT_GUARD_SKILL_EXTRA_LIFE:
  case EDIT_GUARD_LIVES:
    draw_start_guards (bitmap, vm);
    break;
  case EDIT_KID:
    draw_start_kid (bitmap, vm);
    break;
  default: break;
  }

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_box (bitmap, &p, vm);
}

void
update_room0_cache (enum em em, enum vm vm)
{
  room_view = 0;
  struct pos mouse_pos_bkp = mouse_pos;
  invalid_pos (&mouse_pos);
  con_caching = true;

  if (! room0) room0 = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);

  clear_bitmap (room0, TRANSPARENT_COLOR);

  mr.dx = 0;
  mr.dy = 0;
  draw_room (room0, 0, em, vm);

  con_caching = false;
  mouse_pos = mouse_pos_bkp;
}

void
update_cache_room (int room, enum em em, enum vm vm)
{
  int x, y;
  con_caching = true;

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room == room) {
        if (room) {
        room_view = mr.cell[x][y].room;
        mr.dx = x;
        mr.dy = y;
        clear_bitmap (mr.cell[x][y].cache, TRANSPARENT_COLOR);
        draw_room (mr.cell[x][y].cache, mr.cell[x][y].room, em, vm);
        break;
        } else draw_bitmap (room0, mr.cell[x][y].cache, 0, 0, 0);
      }
  con_caching = false;
}

void
update_cache (enum em em, enum vm vm)
{
  struct mr_room_list l;
  mr_get_room_list (&l);

  update_cache_room (0, em, vm);

  size_t i;
  for (i = 0; i < l.nmemb; i++)
    update_cache_room (l.room[i], em, vm);

  mr_destroy_room_list (&l);
}

void
update_cache_pos (struct pos *p, enum em em, enum vm vm)
{
  struct pos q = *p;
  int x, y;

  for (q.room = 1; q.room < ROOMS; q.room++) {
    for (y = mr.h - 1; y >= 0; y--)
      for (x = 0; x < mr.w; x++)
        if (mr.cell[x][y].room == q.room)
          for (q.floor = FLOORS; q.floor >= -1; q.floor--)
            for (q.place = -1; q.place < PLACES; q.place++)
              if (peq (&q, p) && ! has_room_changed (q.room)) {
                ALLEGRO_BITMAP *bitmap = mr.cell[x][y].cache;

                room_view = q.room;
                mr.dx = x;
                mr.dy = y;
                con_caching = true;

                push_clipping_rectangle
                  (bitmap,
                   PLACE_WIDTH * q.place - 1,
                   PLACE_HEIGHT * q.floor - 17,
                   2 * PLACE_WIDTH + 1, PLACE_HEIGHT + 3 + 17);

                clear_bitmap (bitmap, TRANSPARENT_COLOR);

                struct pos p0 = q;
                for (p0.floor = q.floor + 1; p0.floor >= q.floor - 1;
                     p0.floor--)
                  for (p0.place = q.place - 2; p0.place <= q.place + 1;
                       p0.place++)
                    draw_conbg (bitmap, &p0, em, vm);

                for (p0.floor = q.floor + 1; p0.floor >= q.floor - 1;
                     p0.floor--)
                  for (p0.place = q.place - 2; p0.place <= q.place + 1;
                       p0.place++)
                    draw_confg (bitmap, &p0, em, vm);

                pop_clipping_rectangle ();

                con_caching = false;
                goto next_room;
              }
  next_room:;
  }
}

void
mr_busy (void)
{
  if (mr.busy) return;
  mr.busy = true;
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);
}

void
draw_multi_rooms (void)
{
  int x, y;

  mr_set_origin (mr.room, mr.x, mr.y);

  bool mr_full_update = has_mr_view_changed ()
    || mr.full_update;

  if (mr_full_update) {
    mr_busy ();
    force_full_redraw = true;
  }

  if (anim_cycle == 0) {
    generate_wall_colors_for_room (0, room0_wall_color);
  }

  if (em == PALACE && vm == VGA
      && (mr_full_update
          || em != mr.last.em
          || vm != mr.last.vm))
    generate_wall_colors ();

  if (mouse_pos.room != mr.last.mouse_pos.room
      || mouse_pos.floor != mr.last.mouse_pos.floor
      || mouse_pos.place != mr.last.mouse_pos.place) {
    if (is_valid_pos (&mouse_pos))
      register_changed_pos (&mouse_pos);

    if (is_valid_pos (&mr.last.mouse_pos))
      register_changed_pos (&mr.last.mouse_pos);
  }

  if (anim_cycle == 0
      || em != mr.last.em
      || vm != mr.last.vm
      || hgc != mr.last.hgc
      || hue != mr.last.hue) {
    update_room0_cache (em, vm);
    force_full_redraw = true;
  }

  size_t i;

  if (anim_cycle == 0
      || mr_full_update
      || em != mr.last.em
      || vm != mr.last.vm
      || hgc != mr.last.hgc
      || hue != mr.last.hue
      || global_level.n != mr.last.level) {
    update_cache (em, vm);
  } else {
    bool depedv =
      ((em == DUNGEON && vm == VGA)
       || (em == DUNGEON && vm == EGA)
       || (em == PALACE && vm == EGA));

    /* optmize changed pos list */
    optimize_changed_pos ();

    /* update cache pos */
    for (i = 0; i < changed_pos_nmemb; i++) {
      update_cache_pos (&changed_pos[i], em, vm);
      struct pos pl; prel (&changed_pos[i], &pl, +0, -1);
      if (depedv && fake (&pl) == WALL)
        update_cache_pos (&pl, em, vm);
    }

    /* update cache room */
    for (i = 0; i < changed_room_nmemb; i++)
      update_cache_room (changed_room[i], em, vm);

    /* kept together so update_cache_pos and update_cache_room can
       access each other's arrays */
    destroy_array ((void **) &changed_pos, &changed_pos_nmemb);
    destroy_array ((void **) &changed_room, &changed_room_nmemb);
  }

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      clear_bitmap (mr.cell[x][y].screen, (mr.flicker > 0 && mr.flicker % 2)
                    ? mr.color : BLACK);

      if (! mr.cell[x][y].room) continue;
      mr.dx = x;
      mr.dy = y;
      draw_animated_background (mr.cell[x][y].screen,
                                mr.cell[x][y].room);
    }

  if (mr.flicker > 0) mr.flicker--;

  struct mr_room_list l;
  mr_get_room_list (&l);

  int xm, ym;
  if (! no_room_drawing)
    for (i = 0; i < l.nmemb; i++) {
      mr_coord (l.room[i], -1, &xm, &ym);
      for (y = mr.h - 1; y >= 0; y--)
        for (x = 0; x < mr.w; x++)
          if (mr.cell[x][y].room == l.room[i])
            draw_bitmap (mr.cell[xm][ym].cache,
                         mr.cell[x][y].screen, 0, 0, 0);
    }

  mr_destroy_room_list (&l);

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      if (! mr.cell[x][y].room) continue;
      mr.dx = x;
      mr.dy = y;
      draw_animated_foreground (mr.cell[x][y].screen, mr.cell[x][y].room);
    }

  mr_update_last_settings ();
}

void
nmr_coord (int x, int y, int *rx, int *ry)
{
  *rx = x;
  *ry = y;

  bool m;

  do {
    m = false;

    if (*ry < 0) {
      *ry += mr.h;
      m = true;
    } else if (*ry >= mr.h) {
      *ry -= mr.h;
      m = true;
    } else if (*rx < 0) {
      *rx += mr.w;
      m = true;
    } else if (*rx >= mr.w) {
      *rx -= mr.w;
      m = true;
    }
  } while (m);
}

bool
mr_coord (int room, enum dir dir, int *rx, int *ry)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (room && mr.cell[x][y].room == room) {
        switch (dir) {
        case LEFT:
          nmr_coord (x - 1, y, rx, ry);
          return true;
        case RIGHT:
          nmr_coord (x + 1, y, rx, ry);
          return true;
        case ABOVE:
          nmr_coord (x, y - 1, rx, ry);
          return true;
        case BELOW:
          nmr_coord (x, y + 1, rx, ry);
          return true;
        default:
          if (rx) *rx = x;
          if (ry) *ry = y;
          return true;
        }
      }

  return false;
}

bool
mr_room_list_has_room (struct mr_room_list *l, int room)
{
  return bsearch (&room, l->room, l->nmemb, sizeof (room),
                  (m_comparison_fn_t) cint);
}

struct mr_room_list *
mr_get_room_list (struct mr_room_list *l)
{
  l->room = NULL;
  l->nmemb = 0;

  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++) {
      int room = mr.cell[x][y].room;
      if (room && ! mr_room_list_has_room (l, room)) {
        l->room = add_to_array (&room, 1, l->room, &l->nmemb,
                                l->nmemb, sizeof (*l->room));
        qsort (l->room, l->nmemb, sizeof (*l->room),
               (m_comparison_fn_t) cint);
      }
    }

  return l;
}

void
mr_destroy_room_list (struct mr_room_list *l)
{
  destroy_array ((void **) &l->room, &l->nmemb);
}

int
mr_count_uniq_rooms (void)
{
  struct mr_room_list l;
  int c = mr_get_room_list (&l)->nmemb;
  mr_destroy_room_list (&l);
  return c;
}

void
multi_room_fit_stretch (void)
{
  int w = 1;
  int h = 1;
  int lc = 1, c = 1;
  int wc = 1, hc = 1;

 repeat:
  do {
    lc = c;
    redim_multi_room (++w, h);
    mr_center_room (mr.room);
    c = mr_count_uniq_rooms ();
    /* printf ("W: room: %i, width: %i, height: %i, count: %i\n", */
    /*         mr.room, mr.w, mr.h, c); */
  } while (c > lc);
  redim_multi_room (--w, h);
  mr_center_room (mr.room);

  if (c < hc) return;
  wc = c;

  do {
    lc = c;
    redim_multi_room (w, ++h);
    mr_center_room (mr.room);
    c = mr_count_uniq_rooms ();
    /* printf ("H: room: %i, width: %i, height: %i, count: %i\n", */
    /*         mr.room, mr.w, mr.h, c); */
  } while (c > lc);
  redim_multi_room (w, --h);
  mr_center_room (mr.room);

  hc = c;

  if (hc > wc) goto repeat;
}

void
multi_room_fit_ratio (void)
{
  multi_room_fit_stretch ();
  if (mr.w < mr.h) redim_multi_room (mr.h, mr.h);
  else if (mr.w > mr.h) redim_multi_room (mr.w, mr.w);
}

void
apply_mr_fit_mode (void)
{
  int w, h;

  mr_busy ();

  if (rendering == NONE_RENDERING || rendering == AUDIO_RENDERING) {
    mr.fit_w = 1;
    mr.fit_h = 1;
    mr.fit_mode = MR_FIT_NONE;
  }

  if (! mr.room) {
    struct pos p; get_kid_start_pos (&p);
    mr_center_room (p.room);
  }

  switch (mr.fit_mode) {
  case MR_FIT_NONE:
    w = mr.fit_w;
    h = mr.fit_h;
    break;
  case MR_FIT_STRETCH:
    multi_room_fit_stretch ();
    w = mr.w;
    h = mr.h;
    break;
  case MR_FIT_RATIO:
    multi_room_fit_ratio ();
    w = mr.w;
    h = mr.h;
    break;
  }

  set_multi_room (w, h);
  mr_center_room (mr.room);
}

bool
is_room_visible (int room)
{
  int x, y;
  return mr_coord (room, -1, &x, &y);
}

bool
is_frame_visible_at_room (struct frame *f, int room)
{
  struct frame nf;
  nf = *f;

  frame2room (&nf, room, &nf.c);

  struct coord tl, tr, bl, br;
  _tl (&nf, &tl);
  _tr (&nf, &tr);
  _bl (&nf, &bl);
  _br (&nf, &br);

  if (nf.c.room == room &&
      ((tl.x >= 0 && tl.x <= ORIGINAL_WIDTH
        && tl.y >=0 && tl.y <= ORIGINAL_HEIGHT)
       || (tr.x >= 0 && tr.x <= ORIGINAL_WIDTH
           && tr.y >=0 && tr.y <= ORIGINAL_HEIGHT)
       || (bl.x >= 0 && bl.x <= ORIGINAL_WIDTH
           && bl.y >=0 && bl.y <= ORIGINAL_HEIGHT)
       || (br.x >= 0 && br.x <= ORIGINAL_WIDTH
           && br.y >=0 && br.y <= ORIGINAL_HEIGHT)))
    return true;
  else return false;
}

bool
is_frame_visible (struct frame *f)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room
          && is_frame_visible_at_room (f, mr.cell[x][y].room))
        return true;
  return false;
}

bool
is_pos_visible (struct pos *p)
{
  struct pos np; npos (p, &np);

  if (is_room_visible (np.room)) return true;

  int y;
  for (y = mr.h - 1; y >= 0; y--)
    if (mr.cell[0][y].room) {
      struct pos p1;
      new_pos (&p1, p->l, mr.cell[0][y].room, -1, -1);
      p1.place = -1;
      for (p1.floor = 0; p1.floor < FLOORS; p1.floor++)
        if (peq (p, &p1)) return true;
    }

  return false;
}
