/*
  sel-ring.c -- selection ring module;

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

struct sel_ring global_sel_ring;


/**********************************************************************
 * Rectangular Selection
 **********************************************************************/

struct rect_sel *
new_rect_sel (struct mr *mr, struct rect_sel *rs,
              enum rect_sel_type type,
              struct pos *a, struct pos *b)
{
  rs->type = type;
  rs->level = a->l;

  if (type == RECT_SEL_INV) return rs;

  struct pos na;
  npos (a, &na);

  struct pos nb;
  npos (b, &nb);

  struct mr nmr;
  memset (&nmr, 0, sizeof (nmr));
  nmr.room = mr->room;

  mr_fit_stretch (&nmr);

  int ax, ay;
  bool a_visible = mr_coord (&nmr, na.room, -1, &ax, &ay);

  int bx, by;
  bool b_visible = mr_coord (&nmr, nb.room, -1, &bx, &by);

  if (! a_visible || ! b_visible) return NULL;

  int tlx = min_int (ax, bx);
  int tly = min_int (ay, by);

  int brx = max_int (ax, bx);
  int bry = max_int (ay, by);

  int trx = brx;
  int try = tly;

  int blx = tlx;
  int bly = bry;

  /* allocate */
  int w = brx - tlx - 1;
  if (w > 0) {
    rs->w_nmemb = w;
    rs->t = xcalloc (rs->w_nmemb, sizeof (*rs->t));
    rs->b = xcalloc (rs->w_nmemb, sizeof (*rs->b));
  } else {
    rs->w_nmemb = 0;
    rs->t = rs->b = NULL;
  }

  int h = bry - tly - 1;
  if (h > 0) {
    rs->h_nmemb = h;
    rs->l = xcalloc (rs->h_nmemb, sizeof (*rs->l));
    rs->r = xcalloc (rs->h_nmemb, sizeof (*rs->r));
  } else {
    rs->h_nmemb = 0;
    rs->l = rs->r = NULL;
  }

  rs->c_nmemb = rs->w_nmemb * rs->h_nmemb;
  rs->c = xcalloc (rs->c_nmemb, sizeof (*rs->c));

  /* fill in rooms */
  rs->tl = nmr.cell[tlx][tly].room;
  rs->br = nmr.cell[brx][bry].room;
  rs->tr = nmr.cell[trx][try].room;
  rs->bl = nmr.cell[blx][bly].room;

  for (int i = 0; i < rs->w_nmemb; i++) {
    rs->t[i] = nmr.cell[tlx + 1 + i][tly].room;
    rs->b[i] = nmr.cell[tlx + 1 + i][bry].room;
  }

  for (int i = 0; i < rs->h_nmemb; i++) {
    rs->l[i] = nmr.cell[tlx][tly + 1 + i].room;
    rs->r[i] = nmr.cell[brx][tly + 1 + i].room;
  }

  for (int i = 0; i < rs->w_nmemb; i++)
    for (int j = 0; j < rs->h_nmemb; j++)
      rs->c[i * rs->h_nmemb + j] = nmr.cell[tlx + 1 + i][tly + 1 + j].room;

  /* compute corner in-room coordinates */
  int afloor = na.floor, aplace = na.place;
  if (ax == tlx && ay == tly) {
    afloor += 0;
    aplace += 0;
  } else if (ax == trx && ay == try) {
    afloor += 0;
    aplace += (brx - tlx) * PLACES;
  } else if (ax == blx && ay == bly) {
    afloor += (bry - tly) * FLOORS;
    aplace += 0;
  } else if (ax == brx && ay == bry) {
    afloor += (bry - tly) * FLOORS;
    aplace += (brx - tlx) * PLACES;
  } else assert (false);

  int bfloor = nb.floor, bplace = nb.place;
  if (bx == tlx && by == tly) {
    bfloor += 0;
    bplace += 0;
  } else if (bx == trx && by == try) {
    bfloor += 0;
    bplace += (brx - tlx) * PLACES;
  } else if (bx == blx && by == bly) {
    bfloor += (bry - tly) * FLOORS;
    bplace += 0;
  } else if (bx == brx && by == bry) {
    bfloor += (bry - tly) * FLOORS;
    bplace += (brx - tlx) * PLACES;
  } else assert (false);

  rs->tl_floor = min_int (afloor, bfloor);
  rs->tl_place = min_int (aplace, bplace);

  rs->br_floor = max_int (afloor, bfloor) - (bry - tly) * FLOORS;
  rs->br_place = max_int (aplace, bplace) - (brx - tlx) * PLACES;

  return rs;
}

bool
is_valid_rect_sel (struct rect_sel *rs)
{
  return rs->tl || rs->tr || rs->bl || rs->br;
}

void
destroy_rect_sel (struct rect_sel *rs)
{
  if (rs->type != RECT_SEL_INV) {
    destroy_array ((void **) &rs->t, &rs->w_nmemb);
    destroy_array ((void **) &rs->b, &rs->w_nmemb);
    destroy_array ((void **) &rs->l, &rs->h_nmemb);
    destroy_array ((void **) &rs->r, &rs->h_nmemb);
    destroy_array ((void **) &rs->c, &rs->c_nmemb);
  }
  memset (rs, 0, sizeof (*rs));
}

bool
is_room_in_rect_sel (struct rect_sel *rs, int _room)
{
  int room = room_val (rs->level->room_nmemb, _room);

  if (! room) return false;

  if (rs->type == RECT_SEL_INV) return false;

  if (rs->tl == room || rs->tr == room
      || rs->bl == room || rs->br == room)
    return true;

  for (size_t i = 0; i < rs->w_nmemb; i++)
    if (rs->t[i] == room || rs->b[i] == room)
      return true;

  for (size_t i = 0; i < rs->h_nmemb; i++)
    if (rs->l[i] == room || rs->r[i] == room)
      return true;

  for (size_t i = 0; i < rs->c_nmemb; i++)
    if (rs->c[i] == room) return true;

  return false;
}

bool
is_pos_in_rect_sel (struct rect_sel *rs, struct pos *p)
{
  struct pos np;
  npos (p, &np);

  if (! is_room_in_rect_sel (rs, np.room))
    return false;

  if ((rs->tl == np.room
       && (np.floor < rs->tl_floor
           || np.place < rs->tl_place))
      || (rs->tr == np.room
          && (np.floor < rs->tl_floor
              || np.place > rs->br_place))
      || (rs->bl == np.room
          && (np.floor > rs->br_floor
              || np.place < rs->tl_place))
      || (rs->br == np.room
          && (np.floor > rs->br_floor
              || np.place > rs->br_place)))
    return false;

  for (size_t i = 0; i < rs->w_nmemb; i++)
    if ((rs->t[i] == np.room && np.floor < rs->tl_floor)
        || (rs->b[i] == np.room && np.floor > rs->br_floor))
      return false;

  for (size_t i = 0; i < rs->h_nmemb; i++)
    if ((rs->l[i] == np.room && np.place < rs->tl_place)
        || (rs->r[i] == np.room && np.place > rs->br_place))
      return false;

  return true;
}




/**********************************************************************
 * Selection Set
 **********************************************************************/

bool
is_valid_sel_set (struct sel_set *ss)
{
  return ss && ss->rs && ss->nmemb;
}

bool
is_room_possibly_in_sel_set (struct sel_set *ss, int room)
{
  if (! is_valid_sel_set (ss)) return false;
  for (size_t i = 0; i < ss->c_nmemb; i++)
    if (ss->rs[i].type == RECT_SEL_ADD
        && is_room_in_rect_sel (&ss->rs[i], room))
      return true;
  return false;
}

bool
is_pos_in_sel_set (struct sel_set *ss, struct pos *p)
{
  if (! is_valid_sel_set (ss)) return false;
  struct pos np; npos (p, &np);

  /* optimization */
  if (! is_room_possibly_in_sel_set (ss, np.room))
    return false;

  bool r = false;
  for (size_t i = 0; i < ss->c_nmemb; i++) {
    if (ss->rs[i].type == RECT_SEL_INV)
      r = ! r;
    else if (is_pos_in_rect_sel (&ss->rs[i], &np))
      r = ss->rs[i].type == RECT_SEL_ADD;
  }

  return r;
}

bool
is_room_in_sel_set (struct sel_set *ss, int _room)
{
  if (! is_valid_sel_set (ss)) return false;
  int room = room_val (ss->rs[0].level->room_nmemb, _room);

  /* optimization */
  if (! is_room_possibly_in_sel_set (ss, room))
    return false;

  struct pos p; new_pos (&p, ss->rs[0].level, room, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      if (is_pos_in_sel_set (ss, &p))
        return true;
  return false;
}

bool
is_empty_sel_set (struct sel_set *ss)
{
  if (! is_valid_sel_set (ss)) return true;
  for (int room = 1; room < ss->rs[0].level->room_nmemb; room++)
    if (is_room_in_sel_set (ss, room)) return false;
  return true;
}

void
destroy_sel_set_tail (struct sel_set *ss)
{
  if (! is_valid_sel_set (ss)) return;
  for (size_t i = ss->c_nmemb; i < ss->nmemb; i++)
    destroy_rect_sel (&ss->rs[i]);
  ss->nmemb = ss->c_nmemb;
  ss->rs = xrealloc (ss->rs, ss->nmemb * sizeof (* ss->rs));
}

void
destroy_sel_set (struct sel_set *ss)
{
  if (! is_valid_sel_set (ss)) return;
  ss->c_nmemb = 0;
  destroy_sel_set_tail (ss);
}

bool
is_rect_sel_subset_of_sel_set (struct rect_sel *a, struct sel_set *ss)
{
  if (! is_valid_sel_set (ss)) return false;
  struct pos p; new_pos (&p, ss->rs[0].level, -1, -1, -1);
  for (p.room = 1; p.room < p.l->room_nmemb; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        if (is_pos_in_rect_sel (a, &p) && ! is_pos_in_sel_set (ss, &p))
          return false;
  return true;
}

bool
is_rect_sel_and_sel_set_inter_empty (struct rect_sel *a, struct sel_set *ss)
{
  if (! is_valid_sel_set (ss)) return true;
  struct pos p; new_pos (&p, ss->rs[0].level, -1, -1, -1);
  for (p.room = 1; p.room < p.l->room_nmemb; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        if (is_pos_in_rect_sel (a, &p) && is_pos_in_sel_set (ss, &p))
          return false;
  return true;
}

bool
is_rect_sel_helpful_for_sel_set (struct rect_sel *a, struct sel_set *ss)
{
  if (a->type == RECT_SEL_ADD)
    return ! is_rect_sel_subset_of_sel_set (a, ss);
  else if (a->type == RECT_SEL_SUB)
    return ! is_rect_sel_and_sel_set_inter_empty (a, ss);
  else return true;
}

bool
sel_set_can_undo (struct sel_set *ss, int dir)
{
  return (ss->c_nmemb > 0 || dir >= 0)
    && (ss->c_nmemb < ss->nmemb || dir < 0)
    && is_valid_sel_set (ss);
}

bool
sel_set_undo_pass (struct sel_set *ss, int dir)
{
  if (! sel_set_can_undo (ss, dir)) return false;

  if (dir >= 0) ss->c_nmemb++;
  else ss->c_nmemb--;

  return true;
}



/**********************************************************************
 * Selection Ring
 **********************************************************************/

bool
is_valid_sel_ring (struct sel_ring *sr)
{
  return sr && sr->ss && sr->nmemb;
}

void
destroy_sel_ring (struct sel_ring *sr)
{
  if (! is_valid_sel_ring (sr)) return;
  for (size_t i = 0; i < sr->nmemb; i++) destroy_sel_set (&sr->ss[i]);
  destroy_array ((void **) &sr->ss, &sr->nmemb);
  sr->c_nmemb = 0;
}

bool
is_room_in_sel_ring (struct sel_ring *sr, int room)
{
  if (! is_valid_sel_ring (sr)) return false;
  return is_room_in_sel_set (&sr->ss[sr->c_nmemb - 1], room);
}

bool
is_pos_in_sel_ring (struct sel_ring *sr, struct pos *p)
{
  if (! is_valid_sel_ring (sr)) return false;
  return is_pos_in_sel_set (&sr->ss[sr->c_nmemb - 1], p);
}

bool
sel_ring_can_undo (struct sel_ring *sr, int dir)
{
  if (! is_valid_sel_ring (sr)) return false;
  return sel_set_can_undo (&sr->ss[sr->c_nmemb - 1], dir);
}

bool
sel_ring_undo_pass (struct sel_ring *sr, int dir)
{
  if (! is_valid_sel_ring (sr)) return false;
  return sel_set_undo_pass (&sr->ss[sr->c_nmemb - 1], dir);
}

bool
add_rect_sel_to_sel_ring (struct mr *mr, struct sel_ring *sr,
                              enum rect_sel_type type,
                              struct pos *a, struct pos *b)
{
  if (! is_valid_sel_ring (sr)) new_sel_ring_entry (sr);

  struct sel_set *ss = &sr->ss[sr->c_nmemb - 1];
  struct rect_sel rs;
  bool success = new_rect_sel (mr, &rs, type, a, b);
  if (! success) return false;

  if (! is_rect_sel_helpful_for_sel_set (&rs, ss)) {
    destroy_rect_sel (&rs);
    return true;
  }

 if (type == RECT_SEL_INV && ss->c_nmemb > 0
      && ss->rs[ss->c_nmemb - 1].type == RECT_SEL_INV)
   sel_set_undo_pass (ss, -1);
 else {
   destroy_sel_set_tail (ss);

   ss->rs = add_to_array
     (&rs, 1, ss->rs, &ss->nmemb, ss->nmemb, sizeof (*ss->rs));
   ss->c_nmemb = ss->nmemb;
 }

  return true;
}

void
new_sel_ring_entry (struct sel_ring *sr)
{
  struct sel_set ss;
  memset (&ss, 0, sizeof (ss));
  sr->ss = add_to_array
    (&ss, 1, sr->ss, &sr->nmemb, sr->c_nmemb, sizeof (*sr->ss));
  sr->c_nmemb++;
}

void
del_sel_ring_entry (struct sel_ring *sr)
{
  destroy_sel_set (&sr->ss[sr->c_nmemb - 1]);
  sr->ss = remove_from_array
    (sr->ss, &sr->nmemb, sr->c_nmemb - 1, 1, sizeof (*sr->ss));
  sr->c_nmemb = max_int (sr->nmemb ? 1 : 0, sr->c_nmemb - 1);
}

bool
sel_ring_can_go_next (struct sel_ring *sr, int dir)
{
  return sr->nmemb > 1 && is_valid_sel_ring (sr);
}

bool
sel_ring_go_next (struct sel_ring *sr, int dir)
{
  if (! sel_ring_can_go_next (sr, dir)) return false;

  if (dir >= 0) {
    if (sr->c_nmemb == sr->nmemb) sr->c_nmemb = 1;
    else sr->c_nmemb++;
  } else {
    if (sr->c_nmemb == 1) sr->c_nmemb = sr->nmemb;
    else sr->c_nmemb--;
  }

  return true;
}

size_t
sel_ring_ss_c_nmemb (struct sel_ring *sr)
{
  if (! is_valid_sel_ring (sr)) return 0;
  return sr->ss[sr->c_nmemb - 1].c_nmemb;
}

size_t
sel_ring_ss_nmemb (struct sel_ring *sr)
{
  if (! is_valid_sel_ring (sr)) return 0;
  return sr->ss[sr->c_nmemb - 1].nmemb;
}
