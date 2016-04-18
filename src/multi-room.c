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

struct multi_room mr;

int mr_room, mr_x, mr_y;

static void
destroy_multi_room (void)
{
  int x, y;

  if (mr.cell) {
    for (x = 0; x < mr.w; x++) {
      for (y = 0; y < mr.h; y++) destroy_bitmap (mr.cell[x][y].screen);
      al_free (mr.cell[x]);
    }
    al_free (mr.cell);
  };

  destroy_bitmap (screen);
  destroy_bitmap (effect_buffer);
  destroy_bitmap (black_screen);
  destroy_bitmap (wall_cache);
}

bool
set_multi_room (int w, int h)
{
  if (w == mr.w && h == mr.h) return true;

  int sw = ORIGINAL_WIDTH * w;
  int sh = ROOM_HEIGHT * h + 11;

  ALLEGRO_BITMAP *b = create_bitmap (sw, sh);
  if (! b) return false;
  destroy_bitmap (b);

  destroy_multi_room ();

  mr.h = h;
  mr.w = w;

  screen = create_bitmap (sw, sh);
  effect_buffer = create_bitmap (sw, sh);
  black_screen = create_bitmap (sw, sh);
  wall_cache = create_bitmap (sw, sh);

  int x, y;
  mr.cell = xcalloc (w, sizeof (* mr.cell));
  for (x = 0; x < w; x++) {
    mr.cell[x] = xcalloc (h, sizeof (** mr.cell));
    for (y = 0; y < h; y++) {
      int x0 = ORIGINAL_WIDTH * x;
      int y0 = ROOM_HEIGHT * y;
      int sw = ORIGINAL_WIDTH;
      int sh = ORIGINAL_HEIGHT - (y < h - 1 ? 8 : 0);
      mr.cell[x][y].screen = al_create_sub_bitmap (screen, x0, y0, sw, sh);
      mr.cell[x][y].wall = al_create_sub_bitmap (wall_cache, x0, y0, sw, sh);
    }
  }

  return true;
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

void
mr_map_room_adj (int r, int x, int y)
{
  int rl = roomd (r, LEFT);
  int rr = roomd (r, RIGHT);
  int ra = roomd (r, ABOVE);
  int rb = roomd (r, BELOW);

  mr.cell[x][y].room = r;
  mr.cell[x][y].done = true;
  if (x > 0) mr.cell[x - 1][y].room = rl;
  if (x < mr.w - 1) mr.cell[x + 1][y].room = rr;
  if (y > 0) mr.cell[x][y - 1].room = ra;
  if (y < mr.h - 1) mr.cell[x][y + 1].room = rb;
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

void
mr_map_rooms (void)
{
  int x, y;
  clear_multi_room_cells ();
  mr_map_room_adj (mr.room, mr.x, mr.y);
  while (next_multi_room_cell (&x, &y))
    mr_map_room_adj (mr.cell[x][y].room, x, y);
}

int
mr_count_rooms (void)
{
  int x, y, c = 0;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      c += (mr.cell[x][y].room > 0) ? 1 : 0;
  return c;
}

int
mr_count_no_rooms_above (void)
{
  int x, y, c = 0;
  for (y = 0; y < mr.h; y++) {
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room > 0) return c;
    c++;
  }
  return c;
}

int
mr_count_no_rooms_below (void)
{
  int x, y, c = 0;
  for (y = mr.h - 1; y >= 0 ; y--) {
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room > 0) return c;
    c++;
  }
  return c;
}

void
mr_center_room (int room)
{
  mr.room = room;

  int x, y, lc = 0, c = 0, ld = INT_MAX;
  int lx = mr.x;
  int ly = mr.y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      mr.x = x;
      mr.y = y;
      mr_map_rooms ();
      c = mr_count_rooms ();
      int ca = mr_count_no_rooms_above ();
      int cb = mr_count_no_rooms_below ();
      int d = abs (ca - cb);
      if (c >= lc && (c > lc || d < ld)) {
        lx = x;
        ly = y;
        lc = c;
        ld = d;
      }
    }

  mr.x = lx;
  mr.y = ly;

  mr.select_cycles = SELECT_CYCLES;
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
    mr.room = r;
    nmr_coord (mr.x + dx, mr.y + dy, &mr.x, &mr.y);
  }

  mr.select_cycles = SELECT_CYCLES;
}

void
mr_view_trans (enum dir d)
{
  int x, y, dx = +0, dy = +0;

  mr.select_cycles = 0;

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      int r = mr.cell[x][y].room;
      if (r <= 0) continue;
      r = roomd (r, d);
      if (r) {
       mr.room = r;
       mr.x = x;
       mr.y = y;
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

  mr.x += dx;
  mr.y += dy;
}


static int last_level;
static int last_mr_room, last_mr_x, last_mr_y, last_mr_w, last_mr_h;
static enum em last_em;
static enum vm last_vm;
static int last_hgc;
static enum hue last_hue;
struct pos last_mouse_pos;

void
draw_multi_rooms (void)
{
  int x, y;

  mr_map_rooms ();

  if (anim_cycle == 0
      || mr.room != last_mr_room
      || mr.x != last_mr_x
      || mr.y != last_mr_y
      || mr.w != last_mr_w
      || mr.h != last_mr_h
      || em != last_em
      || vm != last_vm
      || hgc != last_hgc
      || hue != last_hue
      || level.number != last_level) {
    update_wall_cache (em, vm);
    last_em = em;
    last_vm = vm;
    last_hgc = hgc;
    last_hue = hue;
    last_mr_room = mr.room;
    last_mr_x = mr.x;
    last_mr_y = mr.y;
    last_mr_w = mr.w;
    last_mr_h = mr.h;
    last_level = level.number;
  }

  if (mouse_pos.room != last_mouse_pos.room
      || mouse_pos.floor != last_mouse_pos.floor
      || mouse_pos.place != last_mouse_pos.place) {
    if (is_valid_pos (&mouse_pos))
      update_wall_cache_pos (&mouse_pos, em, vm);
    if (is_valid_pos (&last_mouse_pos))
      update_wall_cache_pos (&last_mouse_pos, em, vm);
    last_mouse_pos = mouse_pos;
  }

  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++) {
      mr.dx = x;
      mr.dy = y;
      draw_level_room (mr.cell[x][y].screen, mr.cell[x][y].room > 0
                       ? mr.cell[x][y].room : 0);
    }

  draw_bitmap (wall_cache, screen, 0, 0, 0);

  if (mr.select_cycles > 0) {
    al_hold_bitmap_drawing (false);

    int x0 = ORIGINAL_WIDTH * mr.x;
    int y0 = ROOM_HEIGHT * mr.y + 3;
    int x1 = x0 + ORIGINAL_WIDTH;
    int y1 = y0 + ROOM_HEIGHT;
    draw_rectangle (screen, x0, y0, x1, y1, RED, 1);

    mr.select_cycles--;
  }

  mr.dx = mr.dy = -1;
}

bool
is_kid_visible (void)
{
  struct anim *k = get_anim_by_id (current_kid_id);

  int x, y;
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room == k->f.c.room)
        return true;

  return false;
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
  for (x = 0; x < mr.w; x++)
    for (y = 0; y < mr.h; y++)
      if (mr.cell[x][y].room == room) {
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
          *rx = x;
          *ry = y;
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
    xasprintf (&text, "MULTI-ROOM %ix%i", mr_w, mr_h);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
    return false;
  }

  if (! set_multi_room (mr.w + dw, mr.h + dh)) {
    draw_bottom_text (NULL, "VIDEO CARD LIMIT REACHED", 0);
    return false;
  }

  mr_w = mr.w;
  mr_h = mr.h;
  mr_center_room (mr.room);
  xasprintf (&text, "MULTI-ROOM %ix%i", mr_w, mr_h);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
  return true;
}
