/*
  xmouse.c -- xmouse module;

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

ALLEGRO_MOUSE_STATE mouse_state;
struct pos mouse_pos;

void
init_mouse (void)
{
  if (! al_install_mouse ())
    error (-1, 0, "%s (void): cannot install mouse", __func__);
}

void
finalize_mouse (void)
{
  al_uninstall_mouse ();
}

ALLEGRO_EVENT_SOURCE *
get_mouse_event_source (void)
{
  ALLEGRO_EVENT_SOURCE *event_source = al_get_mouse_event_source ();
  if (! event_source)
    error (-1, 0, "%s: failed to get mouse event source", __func__);
  return event_source;
}

struct mouse_coord *
get_mouse_coord (struct mouse_coord *m)
{
  mr_save_origin (&m->mr);

  int w = al_get_display_width (display);
  int h = al_get_display_height (display);
  int sw, sh;
  mr_get_resolution (&sw, &sh);

  al_get_mouse_state (&mouse_state);
  m->sx = mouse_state.x;
  m->sy = mouse_state.y;
  int x = (mouse_state.x * (sw - 1)) / w;
  int y = (mouse_state.y * (sh - 1)) / h;

  if (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
    x = (sw - 1) - x;

  if (screen_flags & ALLEGRO_FLIP_VERTICAL)
    y = (sh - 1) - y;

  m->c.l = &global_level;

  if (y < 3 || y >= sh - 8 || x < 0 || x > sw - 1) {
    m->x = m->y = -1;
    invalid_coord (&m->c);
  } else {
    m->x = x / ORIGINAL_WIDTH;
    m->y = (y - 3) / ROOM_HEIGHT;
    m->c.room = mr.cell[m->x][m->y].room;
    m->c.room = (m->c.room > 0) ? m->c.room : 0;
  }

  m->c.x = x % ORIGINAL_WIDTH;
  m->c.y = ((y - 3) % ROOM_HEIGHT) + 3;

  return m;
}

struct pos *
get_mouse_pos (struct pos *p)
{
  struct mouse_coord m;
  get_mouse_coord (&m);

  if (! is_valid_coord (&m.c)) {
    invalid_pos (p);
    return p;
  }

  int ry = (m.c.y - 3) % PLACE_HEIGHT;

  pos_gen (&m.c, p, 0, 3);

  if (edit == EDIT_NONE) {
    invalid_pos (p);
    return p;
  }

  struct pos p0;

  switch (con (p)->fg) {
  case MIRROR: case CHOPPER:
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
set_mouse_coord (struct mouse_coord *m)
{
  if (m->c.x < 0 || m->c.x >= ORIGINAL_WIDTH
      || m->c.y < 0 || m->c.y >= ORIGINAL_HEIGHT)
    return;

  int x, y;

  mr_restore_origin (&m->mr);

  if (! m->c.room) {
    al_set_mouse_xy (display, m->sx, m->sy);
    return;
  }

  if (! mr_coord (m->c.room, -1, &x, &y)) {
    mr_center_room (m->c.room);
    x = mr.x;
    y = mr.y;
  }

  struct mouse_coord m0;
  get_mouse_coord (&m0);
  if (m0.x >= 0 && m0.y >= 0 &&
      mr.cell[m0.x][m0.y].room == m->c.room) {
    x = m0.x;
    y = m0.y;
  }

  int tw, th;
  mr_get_resolution (&tw, &th);

  int w = al_get_display_width (display);
  int h = al_get_display_height (display);

  int mx = ((ORIGINAL_WIDTH * x + m->c.x + 1) * w) / tw;
  int my = ((ROOM_HEIGHT * y + m->c.y + 1) * h) / th;

  mx = min_int (mx, w - 1);
  my = min_int (my, h - 1);

  if (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
    mx = (w - 1) - mx;

  if (screen_flags & ALLEGRO_FLIP_VERTICAL)
    my = (h - 1) - my;

  if (! al_set_mouse_xy (display, mx, my))
    error (0, 0, "%s (%p): cannot set mouse xy coordinates (%i,%i)",
           __func__, m, mx, my);

  do {
    al_get_mouse_state (&mouse_state);
  } while (mouse_state.x != mx || mouse_state.y != my);

  mr.select_cycles = SELECT_CYCLES;
}

void
set_mouse_pos (struct pos *p)
{
  struct mouse_coord m;

  struct pos np; npos (p, &np);

  m.mr.w = mr.w;
  m.mr.h = mr.h;
  m.mr.room = mr.room = np.room;

  con_m (&np, &m.c);

  int x, y;
  if (! mr_coord (np.room, -1, &x, &y)) {
    mr_center_room (np.room);
    x = mr.x;
    y = mr.y;
  }

  m.mr.x = mr.x = x;
  m.mr.y = mr.y = y;

  set_mouse_coord (&m);
}

void
set_mouse_room (int room)
{
  struct mouse_coord m;

  int x, y;
  if (mr_coord (room, -1, &x, &y))
    mr_set_origin (room, x, y);

  mr_save_origin (&m.mr);
  new_coord (&m.c, &global_level, room, ORIGINAL_WIDTH / 2, ORIGINAL_HEIGHT / 2);
  set_mouse_coord (&m);

  if (! room) {
    mr_center_room (0);
    mr.select_cycles = 0;
    int w = al_get_display_width (display);
    int h = al_get_display_height (display);
    al_set_mouse_xy (display, w / 2, h / 2);
  }
}

void
set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR id)
{
  if (! al_set_system_mouse_cursor (display, id))
    error (0, 0, "%s (%i): cannot set system mouse cursor",
           __func__, id);
}

void
show_mouse_cursor (void)
{
  if (! al_show_mouse_cursor (display))
    error (0, 0, "%s (void): cannot show mouse cursor",
           __func__);
}

void
hide_mouse_cursor (void)
{
  if (! al_hide_mouse_cursor (display))
    error (0, 0, "%s (void): cannot hide mouse cursor",
           __func__);
}
