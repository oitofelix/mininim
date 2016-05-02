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

struct coord *
get_mouse_coord (struct coord *c)
{
  int w = al_get_display_width (display);
  int h = al_get_display_height (display);
  int sw, sh;
  mr_get_resolution (&sw, &sh);

  al_get_mouse_state (&mouse_state);
  int x = (mouse_state.x * (sw - 1)) / w;
  int y = (mouse_state.y * (sh - 1)) / h;

  if (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
    x = sw - x;

  if (screen_flags & ALLEGRO_FLIP_VERTICAL)
    y = sh - y;

  if (y < 3 || y >= sh - 8 || x < 0 || x > sw - 1) c->room = -1;
  else {
    c->room = mr.cell[x / ORIGINAL_WIDTH][(y - 3) / ROOM_HEIGHT].room;
    c->room = (c->room != -1) ? c->room : 0;
  }

  c->x = x % ORIGINAL_WIDTH;
  c->y = ((y - 3) % ROOM_HEIGHT) + 3;

  return c;
}

struct pos *
get_mouse_pos (struct pos *p)
{
  struct coord c;
  get_mouse_coord (&c);

  int ry = (c.y - 3) % PLACE_HEIGHT;

  pos_gen (&c, p, 0, 3);

  if (edit == EDIT_NONE) {
    *p = (struct pos) {-1,-1,-1};
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
    if (ry >= 60) pos_gen (&c, p, 0, 3);
    else if (ry >= 50) pos_gen (&c, p, 23 - 2.5 * (ry - 50), 3);
    else pos_gen (&c, p, 23, 3);
    break;
  }

  struct pos np; npos (p, &np);
  if (! np.room) {
    *p = (struct pos) {-1,-1,-1};
    return p;
  }

  return p;
}

void
set_mouse_coord (struct coord *c)
{
  room_view = c->room;

  if (c->x < 0 || c->x >= ORIGINAL_WIDTH
      || c->y < 0 || c->y >= ORIGINAL_HEIGHT)
    return;

  if (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
    c->x = ORIGINAL_WIDTH - c->x;

  if (screen_flags & ALLEGRO_FLIP_VERTICAL)
    c->y = ORIGINAL_HEIGHT - c->y;

  int w = al_get_display_width (display);
  int h = al_get_display_height (display);

  int mx = (c->x * w) / ORIGINAL_WIDTH;
  int my = (c->y * h) / ORIGINAL_HEIGHT;

  if (! al_set_mouse_xy (display, mx, my))
    error (0, 0, "%s (%i,%i): cannot set mouse xy coordinates",
           __func__, c->x, c->y);
}

void
set_mouse_pos (struct pos *p)
{
  struct coord c;
  con_m (p, &c);
  set_mouse_coord (&c);
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
