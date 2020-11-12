/*
  pointer.c -- pointer module;

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

ALLEGRO_MOUSE_STATE mouse_state;
struct pos mouse_pos;
struct pos selection_pos, last_selection_pos;

void
init_mouse (void)
{
  if (! al_install_mouse ())
    warning ("cannot install mouse");

  al_register_event_source (event_queue, al_get_mouse_event_source ());
}

void
finalize_mouse (void)
{
  al_uninstall_mouse ();
}

struct mouse_coord *
get_mouse_coord (struct mr *mr, struct mouse_coord *m)
{
  mr_save_origin (mr, &m->o);

  int w = al_get_display_width (display);
  int h = al_get_display_height (display);
  int sw, sh;
  mr_get_resolution (mr, &sw, &sh);

  al_get_mouse_state (&mouse_state);
  m->sx = mouse_state.x;
  m->sy = mouse_state.y;
  int x = (mouse_state.x * (sw - 1)) / w;
  int y = (mouse_state.y * (sh - 1)) / h;

  int flags = screen_flags | potion_flags;

  if (flags & ALLEGRO_FLIP_HORIZONTAL)
    x = (sw - 1) - x;

  if (flags & ALLEGRO_FLIP_VERTICAL)
    y = (sh - 1) - y;

  m->c.l = &global_level;

  if (y < 3 || y >= sh - 8 || x < 0 || x > sw - 1) {
    m->x = m->y = -1;
    invalid_coord (&m->c);
  } else {
    m->x = x / ORIGINAL_WIDTH;
    m->y = (y - 3) / ROOM_HEIGHT;
    m->c.room = mr_coord_room (mr, m->x, m->y);
    m->c.room = (m->c.room > 0) ? m->c.room : 0;
  }

  m->c.x = x % ORIGINAL_WIDTH;
  m->c.y = ((y - 3) % ROOM_HEIGHT) + 3;

  return m;
}

struct pos *
get_mouse_pos (struct mr *mr, struct pos *p)
{
  struct mouse_coord m;
  get_mouse_coord (mr, &m);

  if (! is_valid_coord (&m.c)) {
    invalid_pos (p);
    return p;
  }

  int ry = fmod (m.c.y - 3, PLACE_HEIGHT);

  pos_gen (&m.c, p, 0, 3);

  /* if (edit == EDIT_NONE) { */
  /*   invalid_pos (p); */
  /*   return p; */
  /* } */

  struct pos p0;

  switch (fake (p)) {
  case MIRROR: case CHOMPER:
  case WALL: case PILLAR: case BIG_PILLAR_TOP:
  case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
  case ARCH_TOP_MID: case ARCH_TOP_SMALL:
  case ARCH_TOP_LEFT: case ARCH_TOP_RIGHT: break;
  default:
    if (is_arch_top (prel (p, &p0, +0, -1)))
      break;
    if (ry >= 60) pos_gen (&m.c, p, 0, 3);
    else if (ry >= 50) pos_gen (&m.c, p, 23 - 2.5 * (ry - 50), 3);
    else pos_gen (&m.c, p, 23, 3);
    break;
  }

  struct pos np; npos (p, &np);
  if (! np.room) {
    invalid_pos (p);
    return p;
  }

  return p;
}

void
set_mouse_coord (struct mr *mr, struct mouse_coord *m)
{
  if (m->c.x < 0 || m->c.x >= ORIGINAL_WIDTH
      || m->c.y < 0 || m->c.y >= ORIGINAL_HEIGHT)
    return;

  mr_restore_origin (mr, &m->o);

  if (! m->c.room) {
    al_set_mouse_xy (display, m->sx, m->sy);
    return;
  }

  mr_scroll_into_view (mr, m->c.room);
  int x, y; mr_room_coord (mr, m->c.room, &x, &y);

  struct mouse_coord m0;
  get_mouse_coord (mr, &m0);
  if (m0.x >= 0 && m0.y >= 0 &&
      mr_coord_room (mr, m0.x, m0.y) == m->c.room) {
    x = m0.x;
    y = m0.y;
  }

  int tw, th;
  mr_get_resolution (mr, &tw, &th);

  int w = al_get_display_width (display);
  int h = al_get_display_height (display);

  int mx = ((ORIGINAL_WIDTH * x + m->c.x + 1) * w) / tw;
  int my = ((ROOM_HEIGHT * y + m->c.y + 1) * h) / th;

  mx = min_int (mx, w - 1);
  my = min_int (my, h - 1);

  int flags = screen_flags | potion_flags;

  if (flags & ALLEGRO_FLIP_HORIZONTAL)
    mx = (w - 1) - mx;

  if (flags & ALLEGRO_FLIP_VERTICAL)
    my = (h - 1) - my;

  al_set_mouse_xy (display, mx, my);

  do {
    al_get_mouse_state (&mouse_state);
  } while (mouse_state.x != mx || mouse_state.y != my);

  mr->select_cycles = SELECT_CYCLES;
}

void
set_mouse_pos (struct mr *mr, struct pos *p)
{
  struct mouse_coord m;

  struct pos np; npos (p, &np);

  m.o.w = mr->w;
  m.o.h = mr->h;
  m.o.room = mr->room = np.room;

  tile_coord (&np, _m, &m.c);

  mr_scroll_into_view (mr, np.room);
  int x, y; mr_room_coord (mr, np.room, &x, &y);

  m.o.x = mr->x = x;
  m.o.y = mr->y = y;

  set_mouse_coord (mr, &m);
}

void
set_mouse_room (struct mr *mr, int room)
{
  struct mouse_coord m;
  mr_scroll_into_view (mr, room);
  mr_save_origin (mr, &m.o);
  new_coord (&m.c, &global_level, room,
             ORIGINAL_WIDTH / 2, ORIGINAL_HEIGHT / 2);
  set_mouse_coord (mr, &m);
}
