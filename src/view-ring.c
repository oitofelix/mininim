/*
  view-ring.c -- view ring module;

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

struct view_ring global_view_ring;

bool
is_valid_view_ring (struct view_ring *vr)
{
  return vr && vr->o && vr->nmemb;
}

void
destroy_view_ring (struct view_ring *vr)
{
  if (! is_valid_view_ring (vr)) return;
  destroy_array ((void **) &vr->o, &vr->nmemb);
  vr->c_nmemb = 0;
}

void
view_ring_update (struct mr *mr, struct view_ring *vr)
{
  if (! is_valid_view_ring (vr)) return;
  mr_save_origin (mr, &vr->o[vr->c_nmemb - 1]);
}

void
view_ring_add (struct mr *mr, struct view_ring *vr)
{
  view_ring_update (mr, vr);
  struct mr_origin o;
  mr_save_origin (mr, &o);
  vr->o = add_to_array
    (&o, 1, vr->o, &vr->nmemb, vr->c_nmemb, sizeof (*vr->o));
  vr->c_nmemb++;
}

bool
view_ring_delete (struct mr *mr, struct view_ring *vr)
{
  if (! is_valid_view_ring (vr)) return false;
  if (vr->nmemb == 1) return false;
  vr->o = remove_from_array
    (vr->o, &vr->nmemb, vr->c_nmemb - 1, 1, sizeof (*vr->o));
  vr->c_nmemb = max_int (1, vr->c_nmemb - 1);

  view_ring_restore (mr, vr);

  return true;
}

bool
view_ring_can_go_next (struct view_ring *vr, int dir)
{
  return vr->nmemb > 1 && is_valid_view_ring (vr);
}

bool
view_ring_go_next (struct mr *mr, struct view_ring *vr, int dir)
{
  if (! view_ring_can_go_next (vr, dir)) return false;

  view_ring_update (mr, vr);

  if (dir >= 0) {
    if (vr->c_nmemb == vr->nmemb) vr->c_nmemb = 1;
    else vr->c_nmemb++;
  } else {
    if (vr->c_nmemb == 1) vr->c_nmemb = vr->nmemb;
    else vr->c_nmemb--;
  }

  view_ring_restore (mr, vr);

  return true;
}

void
view_ring_restore (struct mr *mr, struct view_ring *vr)
{
  if (! is_valid_view_ring (vr)) return;
  struct mr_origin *o = &vr->o[vr->c_nmemb - 1];
  ui_mr_set_dim (mr, o->w, o->h, false);
  mr_restore_origin (mr, o);
}
