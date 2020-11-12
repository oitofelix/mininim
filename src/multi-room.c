/*
  multi-room.c -- multi-room module;

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

struct mr global_mr;

/* destruction */
static void destroy_mr_cells (struct mr *mr);

/* dimension */
static void mr_redim (struct mr *mr, int w, int h);
static void mr_fit_stretch (struct mr *mr);
static void mr_fit_ratio (struct mr *mr);

/* query info */
static int mr_count_rooms (struct mr *mr);
static bool mr_topmost_cell (struct mr *mr, int *rx, int *ry);
static bool mr_leftmost_cell (struct mr *mr, int *rx, int *ry);
static bool mr_bottommost_cell (struct mr *mr, int *rx, int *ry);
static bool mr_rightmost_cell (struct mr *mr, int *rx, int *ry);

/* view */
static bool is_valid_mr_coord (int x, int y);
static void mr_map_rooms (struct mr *mr, int room, int x, int y);
static void mr_focus_cell (struct mr *mr, int x, int y);



/*******************************************************************
 * destruction
 *******************************************************************/

void
destroy_mr_cells (struct mr *mr)
{
  if (mr->cell) {
    for (int x = 0; x < mr->w; x++) {
      for (int y = 0; y < mr->h; y++) {
        al_destroy_bitmap (mr->cell[x][y].screen);
        al_destroy_bitmap (mr->cell[x][y].cache);
      }
      al_free (mr->cell[x]);
      al_free (mr->last.cell[x]);
    }
  }

  al_free (mr->cell);
  al_free (mr->last.cell);
}

void
destroy_mr (struct mr *mr)
{
  destroy_mr_cells (mr);
  al_free (mr->rlink);
  al_free (mr->coord);
}





/*******************************************************************
 * dimension
 *******************************************************************/

void
mr_set_dim (struct mr *mr, int w, int h)
{
  if (w < 1 || h < 1) return;

  mr_redim (mr, w, h);

  for (int x = 0; x < mr->w; x++)
    for (int y = 0; y < mr->h; y++) {
      int sw = cutscene ? CUTSCENE_WIDTH : ORIGINAL_WIDTH;
      int sh = cutscene ? CUTSCENE_HEIGHT : ORIGINAL_HEIGHT
        - (y < mr->h - 1 ? 8 : 0);
      sw = OW (sw);
      sh = OH (sh);
      al_destroy_bitmap (mr->cell[x][y].screen);
      al_destroy_bitmap (mr->cell[x][y].cache);
      mr->cell[x][y].screen = create_bitmap (sw, sh);
      mr->cell[x][y].cache = create_bitmap (sw, sh);
      clear_bitmap (mr->cell[x][y].screen, BLACK);
      clear_bitmap (mr->cell[x][y].cache, BLACK);
    }
}

void
mr_redim (struct mr *mr, int w, int h)
{
  destroy_mr_cells (mr);

  int x, y;
  mr->cell = xcalloc (w, sizeof (* mr->cell));
  mr->last.cell = xcalloc (w, sizeof (* mr->last.cell));
  for (x = 0; x < w; x++) {
    mr->cell[x] = xcalloc (h, sizeof (** mr->cell));
    mr->last.cell[x] = xcalloc (h, sizeof (** mr->last.cell));
    for (y = 0; y < h; y++) {
      mr->cell[x][y].screen = NULL;
      mr->cell[x][y].cache = NULL;
    }
  }

  mr->w = w;
  mr->h = h;

  mr_set_origin (mr, mr->room, mr->x, mr->y, NULL, 0);
}

void
mr_fit_stretch (struct mr *mr)
{
  int w = mr->max_x - mr->min_x + 1;
  int h = mr->max_y - mr->min_y + 1;

  int tl = mr_coord_room (mr, mr->min_x, mr->min_y);
  int tr = mr_coord_room (mr, mr->max_x, mr->min_y);
  int bl = mr_coord_room (mr, mr->min_x, mr->max_y);
  int br = mr_coord_room (mr, mr->max_x, mr->max_y);

  if (tl) mr_set_room (mr, tl, 0, 0);
  else if (tr) mr_set_room (mr, tr, w - 1, 0);
  else if (bl) mr_set_room (mr, bl, 0, h - 1);
  else if (br) mr_set_room (mr, br, w - 1, h - 1);
  else assert (false);

  mr_redim (mr, w, h);
}

void
mr_fit_ratio (struct mr *mr)
{
  mr_fit_stretch (mr);
  if (mr->w < mr->h) mr_redim (mr, mr->h, mr->h);
  else if (mr->w > mr->h) mr_redim (mr, mr->w, mr->w);
}

void
apply_mr_fit_mode (struct mr *mr, enum mr_fit_mode mode)
{
  mr_busy (mr);

  if (! is_video_rendering ()) {
    mr->fit_w = 1;
    mr->fit_h = 1;
    mode = MR_FIT_NONE;
  }

  if (! mr->room) {
    struct pos p; get_kid_start_pos (&p);
    mr_best_view (mr, p.room);
  }

  if (! cutscene && mr->fit_mode == MR_FIT_NONE) {
    mr->fit_w = mr->w;
    mr->fit_h = mr->h;
    mr->fit_room = mr_central_room (mr);
  }

  switch (mode) {
  case MR_FIT_NONE:
    mr_set_dim (mr, mr->fit_w, mr->fit_h);
    mr_center_room (mr, mr->fit_room);
    break;
  case MR_FIT_STRETCH:
    mr_fit_stretch (mr);
    mr_set_dim (mr, mr->w, mr->h);
    break;
  case MR_FIT_RATIO:
    mr_fit_ratio (mr);
    mr_set_dim (mr, mr->w, mr->h);
    mr_best_view (mr, mr_central_room (mr));
    break;
  }

  mr->fit_mode = mode;
}






/*******************************************************************
 * query info
 *******************************************************************/

void
mr_get_resolution (struct mr *mr, int *ret_w, int *ret_h)
{
  int w = cutscene ? CUTSCENE_WIDTH : ORIGINAL_WIDTH * mr->w;
  int h = cutscene ? CUTSCENE_HEIGHT : ROOM_HEIGHT * mr->h
    + BOTTOM_ROW_HEIGHT;
  *ret_w = w;
  *ret_h = h;
}

int
mr_central_room (struct mr *mr)
{
  for (int r = 0; ; r++) {
    for (int i = 0; i <= r; i++) {
      int room = mr_coord_room (mr, mr->w / 2 - i, mr->h / 2 - r);
      if (room) return room;
      room = mr_coord_room (mr, mr->w / 2 + i, mr->h / 2 - r);
      if (room) return room;
      room = mr_coord_room (mr, mr->w / 2 - i, mr->h / 2 + r);
      if (room) return room;
      room = mr_coord_room (mr, mr->w / 2 + i, mr->h / 2 + r);
      if (room) return room;
    }

    for (int i = 0; i <= r; i++) {
      int room = mr_coord_room (mr, mr->w / 2 - r, mr->h / 2 - i);
      if (room) return room;
      room = mr_coord_room (mr, mr->w / 2 + r, mr->h / 2 - i);
      if (room) return room;
      room = mr_coord_room (mr, mr->w / 2 - r, mr->h / 2 + i);
      if (room) return room;
      room = mr_coord_room (mr, mr->w / 2 + r, mr->h / 2 + i);
      if (room) return room;
    }
  }
}

int
mr_count_rooms (struct mr *mr)
{
  int x, y, c = 0;
  for (x = 0; x < mr->w; x++)
    for (y = 0; y < mr->h; y++)
      c += mr_coord_room (mr, x, y) ? 1 : 0;
  return c;
}

bool
mr_topmost_cell (struct mr *mr, int *rx, int *ry)
{
  int x, y;
  for (y = 0; y < mr->h; y++)
    for (x = 0; x < mr->w; x++)
      if (mr_coord_room (mr, x, y)) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

bool
mr_leftmost_cell (struct mr *mr, int *rx, int *ry)
{
  int x, y;
  for (x = 0; x < mr->w; x++)
    for (y = 0; y < mr->h; y++)
      if (mr_coord_room (mr, x, y)) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

bool
mr_bottommost_cell (struct mr *mr, int *rx, int *ry)
{
  int x, y;
  for (y = mr->h - 1; y >= 0; y--)
    for (x = mr->w - 1; x >= 0; x--)
      if (mr_coord_room (mr, x, y)) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}

bool
mr_rightmost_cell (struct mr *mr, int *rx, int *ry)
{
  int x, y;
  for (x = mr->w - 1; x >= 0; x--)
    for (y = mr->h - 1; y >= 0; y--)
      if (mr_coord_room (mr, x, y)) {
        *rx = x, *ry = y;
        return true;
      }

  return false;
}






/*******************************************************************
 * view
 *******************************************************************/

bool
is_valid_mr_coord (int x, int y)
{
  return x != INT_MIN && y != INT_MIN;
}

void
mr_set_origin (struct mr *mr, int room, int x, int y,
               struct room_linking *rlink, size_t room_nmemb)
{
  if (! room) return;

  mr->room = room;
  mr->x = x;
  mr->y = y;
  mr->min_x = INT_MAX;
  mr->max_x = INT_MIN;
  mr->min_y = INT_MAX;
  mr->max_y = INT_MIN;

  if (rlink && room_nmemb && mr->rlink != rlink) {
    al_free (mr->rlink);
    mr->rlink = copy_array
      (rlink, room_nmemb, &mr->room_nmemb, sizeof (* mr->rlink));

    al_free (mr->coord);
    mr->coord = xcalloc (mr->room_nmemb, sizeof (* mr->coord));
  }

  for (int r = 0; r < mr->room_nmemb; r++)
    mr->coord[r].x = INT_MIN, mr->coord[r].y = INT_MIN;

  for (int x = 0; x < mr->w; x++)
    for (int y = 0; y < mr->h; y++)
      mr->cell[x][y].room = 0;

  mr_map_rooms (mr, room, x, y);
}

void
mr_map_rooms (struct mr *mr, int room, int x, int y)
{
  if (is_valid_mr_coord (mr->coord[room].x, mr->coord[room].y))
    return;

  if (mr_is_coord_visible (mr, x, y)) {
    if (mr->cell[x][y].room) return;
    else mr->cell[x][y].room = room;
  }

  mr->coord[room].x = x;
  mr->coord[room].y = y;

  mr->min_x = min_int (mr->min_x, x);
  mr->max_x = max_int (mr->max_x, x);
  mr->min_y = min_int (mr->min_y, y);
  mr->max_y = max_int (mr->max_y, y);

  for (enum dir d = FIRST_DIR; d <= LAST_DIR; d++) {
    int dx, dy; dir_dx_dy (d, &dx, &dy);
    int r = roomd (mr->rlink, mr->room_nmemb, room, d);
    if (r) mr_map_rooms (mr, r, x + dx, y + dy);
  }
}

void
mr_set_room (struct mr *mr, int room, int x, int y)
{
  int rx, ry;
  if (mr_room_coord (mr, room, &rx, &ry))
    mr_set_origin (mr, mr->room, mr->x + (x - rx),
                   mr->y + (y - ry), NULL, 0);
  else mr_set_origin (mr, room, x, y, NULL, 0);
}

void
mr_best_view (struct mr *mr, int room)
{
  int x, y, lc = 0, c = 0, ld = INT_MAX;
  float ldc = INFINITY;
  int lx = mr->x;
  int ly = mr->y;
  for (y = mr->h - 1; y >= 0; y--)
    for (x = 0; x < mr->w; x++) {
      mr_set_room (mr, room, x, y);
      c = mr_count_rooms (mr);
      int cx, cy;
      mr_topmost_cell (mr, &cx, &cy);
      int ca = cy;
      mr_bottommost_cell (mr, &cx, &cy);
      int cb = (mr->h - 1) - cy;
      mr_leftmost_cell (mr, &cx, &cy);
      int cl = cx;
      mr_rightmost_cell (mr, &cx, &cy);
      int cr = (mr->w - 1) - cx;
      int d = abs (ca - cb) + abs (cl - cr);
      float dc = dist_cart (x, y, (mr->w - 1) / 2, (mr->h - 1) / 2);
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

  mr_set_room (mr, room, lx, ly);
  mr->select_cycles = SELECT_CYCLES;
}

void
mr_center_room (struct mr *mr, int room)
{
  mr_set_room (mr, room, mr->w / 2, mr->h / 2);
}

void
mr_focus_room (struct mr *mr, int room)
{
  mr_scroll_into_view (mr, room);
  int x, y; mr_room_coord (mr, room, &x, &y);
  mr_set_origin (mr, room, x, y, NULL, 0);
  mr->select_cycles = SELECT_CYCLES;
}

void
mr_focus_cell (struct mr *mr, int x, int y)
{
  if (x < 0 || y < 0) return;
  int room = mr_coord_room (mr, x, y);
  if (! room) return;
  mr_focus_room (mr, room);
  mr->select_cycles = SELECT_CYCLES;
}

void
mr_focus_mouse (struct mr *mr)
{
  struct mouse_coord m;
  get_mouse_coord (mr, &m);
  mr_focus_cell (mr, m.x, m.y);
}

void
mr_scroll_into_view (struct mr *mr, int room)
{
  /* optimization */
  if (is_room_visible (mr, room)) return;

  int x, y;
  if (mr_room_coord (mr, room, &x, &y)) {
    if (x < 0) x = 0;
    if (x > mr->w - 1) x = mr->w - 1;
    if (y < 0) y = 0;
    if (y > mr->h - 1) y = mr->h - 1;
    mr_set_room (mr, room, x, y);
  } else mr_best_view (mr, room);
}






struct mr_origin *
mr_save_origin (struct mr *mr, struct mr_origin *o)
{
  o->w = mr->w;
  o->h = mr->h;
  o->x = mr->x;
  o->y = mr->y;
  o->room = mr->room;
  return o;
}

bool
mr_origin_eq (struct mr_origin *a, struct mr_origin *b)
{
  return a->w == b->w && a->h == b->h
    && a->x == b->x && a->y == b->y
    && a->room == b->room;
}

void
mr_restore_origin (struct mr *mr, struct mr_origin *o)
{
  if (o->w == mr->w && o->h == mr->h)
    mr_set_origin (mr, o->room, o->x, o->y, NULL, 0);
  else mr_best_view (mr, o->room);
}

void
mr_room_trans (struct mr *mr, enum dir d)
{
  int dx, dy; dir_dx_dy (d, &dx, &dy);
  int room = roomd (mr->rlink, mr->room_nmemb, mr->room, d);
  if (room && ! is_room_visible (mr, room)) mr_page_trans (mr, d);
  mr_focus_room (mr, room);
  mr->select_cycles = SELECT_CYCLES;
}

void
mr_row_trans (struct mr *mr, enum dir d)
{
  int dx, dy; dir_dx_dy (d, &dx, &dy);

  struct mr_origin o;
  mr_save_origin (mr, &o);
  mr_set_origin (mr, mr->room, mr->x - dx, mr->y - dy, NULL, 0);

  if (mr_is_view_within_bounds (mr)) return;
  else mr_restore_origin (mr, &o);

  mr->select_cycles = SELECT_CYCLES;
}

void
mr_page_trans (struct mr *mr, enum dir d)
{
  int n = (d == LEFT || d == RIGHT) ? mr->w : mr->h;
  for (int i = 0; i < n; i++) mr_row_trans (mr, d);
}

bool
mr_is_view_within_bounds (struct mr *mr)
{
  return mr->min_x < mr->w
    && mr->max_x >= 0
    && mr->min_y < mr->h
    && mr->max_y >= 0;
}

bool
has_mr_view_changed (struct mr *mr)
{
  if (mr->w != mr->last.w || mr->h != mr->last.h) return true;

  int x, y;
  for (y = mr->h - 1; y >= 0; y--)
    for (x = 0; x < mr->w; x++)
      if (mr->last.cell[x][y].room != mr_coord_room (mr, x, y))
        return true;

  return false;
}

void
mr_update_last_settings (struct mr *mr)
{
  int x, y;
  for (y = mr->h - 1; y >= 0; y--)
    for (x = 0; x < mr->w; x++)
      mr->last.cell[x][y].room = mr_coord_room (mr, x, y);

  mr->last.w = mr->w;
  mr->last.h = mr->h;
  mr->last.x = mr->x;
  mr->last.y = mr->y;
  mr->last.room = mr->room;

  mr->last.level = global_level.n;
  mr->last.em = em;
  /* mr->last.vm = vm; */
  /* mr->last.hgc = hgc; */
  mr->last.hue = hue;
  mr->last.mouse_pos = mouse_pos;
  mr->last.selection_pos = selection_pos;
  mr->last.display_width = al_get_display_width (display);
  mr->last.display_height = al_get_display_height (display);
  mr->last.edit = edit;

  mr->full_update = false;
  if (mr->busy && ! is_dedicatedly_replaying ()) {
    mr->busy = false;
    al_set_system_mouse_cursor
      (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
  }
}

void
mr_busy (struct mr *mr)
{
  if (mr->busy) return;
  mr->busy = true;
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);
}

bool mr_is_coord_visible (struct mr *mr, int x, int y)
{
  return 0 <= x && x < mr->w && 0 <= y && y < mr->h;
}

bool
mr_room_coord (struct mr *mr, int room, int *rx, int *ry)
{
  if (! is_valid_mr_coord (mr->coord[room].x, mr->coord[room].y))
    return false;
  if (rx) *rx = mr->coord[room].x;
  if (ry) *ry = mr->coord[room].y;
  return true;
}

int
mr_coord_room (struct mr *mr, int x, int y)
{
  /* optimization */
  if (mr_is_coord_visible (mr, x, y))
    return mr->cell[x][y].room;

  for (int r = 1; r < mr->room_nmemb; r++)
    if (mr->coord[r].x == x && mr->coord[r].y == y)
      return r;

  return 0;
}

bool
mr_room_list_has_room (struct mr_room_list *l, int room)
{
  return bsearch (&room, l->room, l->nmemb, sizeof (room),
                  (m_comparison_fn_t) cint);
}

struct mr_room_list *
mr_get_room_list (struct mr *mr, struct mr_room_list *l)
{
  l->room = NULL;
  l->nmemb = 0;

  int x, y;
  for (x = 0; x < mr->w; x++)
    for (y = 0; y < mr->h; y++) {
      int room = mr_coord_room (mr, x, y);
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
mr_count_uniq_rooms (struct mr *mr)
{
  struct mr_room_list l;
  int c = mr_get_room_list (mr, &l)->nmemb;
  mr_destroy_room_list (&l);
  return c;
}

bool
is_room_visible (struct mr *mr, int room)
{
  for (int x = 0; x < mr->w; x++)
    for (int y = 0; y < mr->h; y++)
      if (room && mr_coord_room (mr, x, y) == room)
        return true;
  return false;
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
is_frame_visible (struct mr *mr, struct frame *f)
{
  int x, y;
  for (y = mr->h - 1; y >= 0; y--)
    for (x = 0; x < mr->w; x++) {
      int room = mr_coord_room (mr, x, y);
      if (room && is_frame_visible_at_room (f, room))
        return true;
    }
  return false;
}

bool
is_pos_visible (struct mr *mr, struct pos *p)
{
  struct pos np; npos (p, &np);

  if (is_room_visible (mr, np.room)) return true;

  int y;
  for (y = mr->h - 1; y >= 0; y--) {
    int room = mr_coord_room (mr, 0, y);
    if (room) {
      struct pos p1;
      new_pos (&p1, p->l, room, -1, -1);
      p1.place = -1;
      for (p1.floor = 0; p1.floor < FLOORS; p1.floor++)
        if (peq (p, &p1)) return true;
    }
  }
  return false;
}
