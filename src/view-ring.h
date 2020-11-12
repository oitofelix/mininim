/*
  view-ring.h -- view ring module;

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

#ifndef MININIM_VIEW_RING_H
#define MININIM_VIEW_RING_H

/* variables */
extern struct view_ring global_view_ring;

/* functions */
bool is_valid_view_ring (struct view_ring *vr);
void destroy_view_ring (struct view_ring *vr);
void view_ring_add (struct mr *mr, struct view_ring *vr);
void view_ring_update (struct mr *mr, struct view_ring *vr);
bool view_ring_delete (struct mr *mr, struct view_ring *vr);
bool view_ring_can_go_next (struct view_ring *vr, int dir);
bool view_ring_go_next (struct mr *mr, struct view_ring *vr, int dir);
void view_ring_restore (struct mr *mr, struct view_ring *vr);

#endif	/* MININIM_VIEW_RING_H */
