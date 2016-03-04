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

struct pos *
get_mouse_pos (struct pos *p)
{
  int w = al_get_display_width (display);
  int h = al_get_display_height (display);

  struct coord c;
  al_get_mouse_state (&mouse_state);
  c.x = (mouse_state.x * ORIGINAL_WIDTH) / w;
  c.y = (mouse_state.y * ORIGINAL_HEIGHT) / h;
  c.room = room_view;

  int ry = (c.y - 3) % PLACE_HEIGHT;

  posf (&c, p);

  if (p->floor == -1 || p->floor > 2
      || edit == EDIT_NONE) {
    *p = (struct pos) {-1,-2,-1};
    return p;
  }

  switch (con (p)->fg) {
  case WALL: case PILLAR: case BIG_PILLAR_TOP:
  case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
  case ARCH_TOP_MID: case ARCH_TOP_SMALL:
  case ARCH_TOP_LEFT: case ARCH_TOP_RIGHT:
    return p;
    break;
  default:
    if (ry >= 60) return pos_gen (&c, p, 0, 3);
    else if (ry >= 50)
      return pos_gen (&c, p, 23 - 2.5 * (ry - 50), 3);
    else return pos_gen (&c, p, 23, 3);
    break;
  }
}
