/*
  multi-room.c -- multi-room module;

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
register_changed_pos (struct pos *p)
{
  struct pos np; npos (p, &np);

  if (get_changed_pos (&np)) return;

  changed_pos =
    add_to_array (&np, 1, changed_pos, &changed_pos_nmemb,
                  changed_pos_nmemb, sizeof (np));

  qsort (changed_pos, changed_pos_nmemb, sizeof (np), (m_comparison_fn_t) cpos);
}

struct pos *
get_changed_pos (struct pos *p)
{
  return bsearch (p, changed_pos, changed_pos_nmemb, sizeof (* p),
                  (m_comparison_fn_t) cpos);
}

void
register_changed_room (int room)
{
  if (has_room_changed (room)) return;

  changed_room =
    add_to_array (&room, 1, changed_room, &changed_room_nmemb,
                  changed_room_nmemb, sizeof (room));

  qsort (changed_room, changed_room_nmemb, sizeof (room), (m_comparison_fn_t) cint);

  struct pos p;
  p.room = room;
  p.floor = 0;
  for (p.place = 0; p.place < PLACES; p.place++)
    register_changed_pos (&p);

  p.place = 0;
  for (p.floor = 1; p.floor < FLOORS; p.floor++)
    register_changed_pos (&p);

  p.place = 9;
  for (p.floor = 1; p.floor < FLOORS; p.floor++)
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
      int sw = ORIGINAL_WIDTH;
      int sh = ORIGINAL_HEIGHT - (y < mr.h - 1 ? 8 : 0);
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
  *w = ORIGINAL_WIDTH * mr.w;
  *h = ROOM_HEIGHT * mr.h + 11;
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
  int rl = roomd (r, LEFT);
  int rr = roomd (r, RIGHT);
  int ra = roomd (r, ABOVE);
  int rb = roomd (r, BELOW);

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

  int r = roomd (mr.room, d);
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
      r = roomd (r, d);
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

  return;
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

  mr.last.level = level.number;
  mr.last.em = em;
  mr.last.vm = vm;
  mr.last.hgc = hgc;
  mr.last.hue = hue;
  mr.last.mouse_pos = mouse_pos;
  mr.last.display_width = al_get_display_width (display);
  mr.last.display_height = al_get_display_height (display);
}

void
draw_animated_background (ALLEGRO_BITMAP *bitmap, int room)
{
  room_view = room;

  struct pos p;
  p.room = room;

  for (p.floor = FLOORS; p.floor >= 0; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_no_floor_selection (bitmap, &p);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      if (con (&p)->fg == WALL) continue;
      draw_fire (bitmap, &p, vm);
      draw_balcony_stars (bitmap, &p, vm);
    }
}

void
draw_animated_foreground (ALLEGRO_BITMAP *bitmap, int room)
{
  room_view = room;

  struct pos p;
  p.room = room;

  /* loose_floor_fall_debug (); */

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_falling_loose_floor (bitmap, &p, em, vm);
    }

  draw_anims (bitmap, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_potion (bitmap, &p, em, vm);
      if (is_sword (&p)) draw_sword (bitmap, &p, vm);
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
  mouse_pos.room = -1;
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
  static bool recursive = false, recursive_01 = false;

  int x, y;

  struct pos pbl; prel (p, &pbl, +1, -1);
  struct pos pb; prel (p, &pb, +1, +0);
  struct pos pbr; prel (p, &pbr, +1, +1);

  struct pos pl; prel (p, &pl, +0, -1);
  struct pos pr; prel (p, &pr, +0, +1);

  struct pos pa; prel (p, &pa, -1, +0);
  struct pos pal; prel (p, &pal, -1, -1);
  struct pos par; prel (p, &par, -1, +1);

  struct pos parr; prel (&par, &parr, +0, +1);
  struct pos prr; prel (&pr, &prr, +0, +1);
  struct pos pbrr; prel (&pbr, &pbrr, +0, +1);

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (p->room && mr.cell[x][y].room == p->room) {
        room_view = p->room;
        mr.dx = x;
        mr.dy = y;

        int cx, cy, cw, ch;
        switch (con (p)->fg) {
        default:
          cx = PLACE_WIDTH * p->place;
          cy = PLACE_HEIGHT * p->floor - 13;
          cw = 2 * PLACE_WIDTH;
          ch = PLACE_HEIGHT + 3 + 13;
          break;
        }

        set_target_bitmap (mr.cell[x][y].cache);
        al_set_clipping_rectangle (cx, cy, cw, ch);
        al_clear_to_color (TRANSPARENT_COLOR);

        con_caching = true;

        draw_conbg (mr.cell[x][y].cache, &pbl, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pb, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pbr, em, vm);

        draw_conbg (mr.cell[x][y].cache, &pl, em, vm);
        draw_conbg (mr.cell[x][y].cache, p, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pr, em, vm);

        draw_conbg (mr.cell[x][y].cache, &pal, em, vm);
        draw_conbg (mr.cell[x][y].cache, &pa, em, vm);
        draw_conbg (mr.cell[x][y].cache, &par, em, vm);

        draw_confg_right (mr.cell[x][y].cache, &pbl, em, vm, true);
        draw_confg_top (mr.cell[x][y].cache, &pbl, em, vm, true);
        draw_confg_right (mr.cell[x][y].cache, &pb, em, vm, true);
        draw_confg_top (mr.cell[x][y].cache, &pb, em, vm, true);

        draw_confg_right (mr.cell[x][y].cache, &pl, em, vm, false);
        draw_confg_top (mr.cell[x][y].cache, &pl, em, vm, false);

        draw_confg (mr.cell[x][y].cache, p, em, vm, true);

        draw_confg_base (mr.cell[x][y].cache, &pr, em, vm);
        draw_confg_left (mr.cell[x][y].cache, &pr, em, vm, true);

        draw_confg_right (mr.cell[x][y].cache, &pal, em, vm, true);
        draw_confg (mr.cell[x][y].cache, &pa, em, vm, true);
        draw_confg_base (mr.cell[x][y].cache, &par, em, vm);
        draw_confg_left (mr.cell[x][y].cache, &par, em, vm, false);

        draw_confg_left (mr.cell[x][y].cache, &parr, em, vm, false);
        draw_confg_left (mr.cell[x][y].cache, &prr, em, vm, false);
        draw_confg_left (mr.cell[x][y].cache, &pbrr, em, vm, false);

        al_reset_clipping_rectangle ();
        con_caching = false;

        goto end;
      }

 end:;

  /* if (is_room_visible (p->room)) printf ("%i,%i,%i\n", p->room, p->floor, p->place); */

  bool depedv =
    ((em == DUNGEON && vm == VGA)
     || (em == DUNGEON && vm == EGA)
     || (em == PALACE && vm == EGA));

  if (! recursive_01 && depedv && con (&pl)->fg == WALL) {
    recursive_01 = true;
    update_cache_pos (&pl, em, vm);
    recursive_01 = false;
  }

  if (! recursive && p->place == -1) {
    struct pos p0;
    p0.room = roomd (p->room, LEFT);
    p0.floor = p->floor;
    p0.place = PLACES - 1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == -1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.floor = FLOORS - 1;
    p0.place = p->place;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == 0) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.floor = FLOORS;
    p0.place = p->place;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, RIGHT);
    p0.floor = p->floor;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == FLOORS - 1) {
    struct pos p0;
    p0.room = roomd (p->room, BELOW);
    p0.floor = -1;
    p0.place = p->place;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == -1 && p->place == -1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.room = roomd (p0.room, LEFT);
    p0.floor = FLOORS - 1;
    p0.place = PLACES - 1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == 0 && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.room = roomd (p0.room, RIGHT);
    p0.floor = FLOORS;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == -1 && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, ABOVE);
    p0.room = roomd (p0.room, RIGHT);
    p0.floor = FLOORS - 1;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == FLOORS - 1 && p->place == -1) {
    struct pos p0;
    p0.room = roomd (p->room, LEFT);
    p0.room = roomd (p0.room, BELOW);
    p0.floor = -1;
    p0.place = PLACES - 1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  if (! recursive && p->floor == FLOORS - 1 && p->place == PLACES - 1) {
    struct pos p0;
    p0.room = roomd (p->room, BELOW);
    p0.room = roomd (p0.room, RIGHT);
    p0.floor = -1;
    p0.place = -1;
    recursive = true;
    update_cache_pos (&p0, em, vm);
    recursive = false;
  }

  /* if (is_room_visible (p->room) && ! recursive) printf ("----------------------------\n"); */
}

void
draw_multi_rooms (void)
{
  int x, y;

  mr_set_origin (mr.room, mr.x, mr.y);

  bool mr_view_changed = has_mr_view_changed ();

  if (mr_view_changed) force_full_redraw = true;

  if (anim_cycle == 0) {
    generate_wall_colors_for_room (0, room0_wall_color);
  }

  if (em == PALACE && vm == VGA
      && (mr_view_changed
          || em != mr.last.em
          || vm != mr.last.vm))
    generate_wall_colors ();

  if (mr_view_changed)
    generate_stars ();

  if (mouse_pos.room != mr.last.mouse_pos.room
      || mouse_pos.floor != mr.last.mouse_pos.floor
      || mouse_pos.place != mr.last.mouse_pos.place) {
    if (is_valid_pos (&mouse_pos))
      update_cache_pos (&mouse_pos, em, vm);
    if (is_valid_pos (&mr.last.mouse_pos))
      update_cache_pos (&mr.last.mouse_pos, em, vm);
  }

  if (anim_cycle == 0
      || em != mr.last.em
      || vm != mr.last.vm
      || hgc != mr.last.hgc
      || hue != mr.last.hue) {
    update_room0_cache (em, vm);
    force_full_redraw = true;
  }

  if (anim_cycle == 0
      || mr_view_changed
      || em != mr.last.em
      || vm != mr.last.vm
      || hgc != mr.last.hgc
      || hue != mr.last.hue
      || level.number != mr.last.level) {
    update_cache (em, vm);
  }

  size_t i;
  for (i = 0; i < changed_pos_nmemb; i++)
    update_cache_pos (&changed_pos[i], em, vm);
  destroy_array ((void **) &changed_pos, &changed_pos_nmemb);

  for (i = 0; i < changed_room_nmemb; i++)
    update_cache_room (changed_room[i], em, vm);
  destroy_array ((void **) &changed_room, &changed_room_nmemb);

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

  /* if (! no_room_drawing) */
  /*   for (y = mr.h - 1; y >= 0; y--) */
  /*     for (x = 0; x < mr.w; x++) */
  /*       if (mr.cell[x][y].room) */
  /*         draw_bitmap (mr.cell[x][y].cache, mr.cell[x][y].screen, 0, 0, 0); */

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      if (! mr.cell[x][y].room) continue;
      mr.dx = x;
      mr.dy = y;
      draw_animated_foreground (mr.cell[x][y].screen, mr.cell[x][y].room);
    }

  /* if (mr.select_cycles > 0) { */
  /*   int t = max_int (mr.w, mr.h); */
  /*   draw_rectangle (mr.cell[mr.x][mr.y].screen, 0, 3, */
  /*                   ORIGINAL_WIDTH - t, */
  /*                   3 + ROOM_HEIGHT - t, RED, t); */
  /*   mr.select_cycles--; */
  /* } */

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
ui_set_multi_room (int dw, int dh)
{
  char *text;

  if (mr.w + dw < 1 || mr.h + dh < 1) {
    xasprintf (&text, "MULTI-ROOM %ix%i", mr.w, mr.h);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
    return false;
  }

  struct mouse_coord m;
  get_mouse_coord (&m);

  if (mr.w + dw != mr.w || mr.h + dh != mr.h)
    set_multi_room (mr.w + dw, mr.h + dh);

  mr_center_room (mr.room);

  if (mr_coord (m.c.room, -1, NULL, NULL))
    set_mouse_coord (&m);

  xasprintf (&text, "MULTI-ROOM %ix%i", mr.w, mr.h);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
  return true;
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
  int lc, c = 1;

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

  int wc = c;

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

  if (c > wc) goto repeat;
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
      p1.room = mr.cell[0][y].room;
      p1.place = -1;
      for (p1.floor = 0; p1.floor < FLOORS; p1.floor++)
        if (peq (p, &p1)) return true;
    }

  return false;
}
