/*
  room.c -- room module;

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

bool no_recursive_links_continuity;

void
load_room (void)
{
  load_floor ();
  load_broken_floor ();
  load_skeleton_floor ();
  load_loose_floor ();
  load_opener_floor ();
  load_closer_floor ();
  load_spikes_floor ();
  load_wall ();
  load_pillar ();
  load_big_pillar ();
  load_door ();
  load_level_door ();
  load_chopper ();
  load_bricks ();
  load_torch ();
  load_window ();
  load_balcony ();
  load_arch ();
  load_carpet ();
  load_stars ();
  load_mirror ();
}

void
unload_room (void)
{
  /* modules */
  unload_floor ();
  unload_broken_floor ();
  unload_skeleton_floor ();
  unload_loose_floor ();
  unload_opener_floor ();
  unload_closer_floor ();
  unload_spikes_floor ();
  unload_wall ();
  unload_pillar ();
  unload_big_pillar ();
  unload_door ();
  unload_level_door ();
  unload_chopper ();
  unload_bricks ();
  unload_torch ();
  unload_window ();
  unload_balcony ();
  unload_arch ();
  unload_carpet ();
  unload_stars ();
  unload_mirror ();
}

struct rect *
new_rect (struct rect *r, int room, int x, int y, int w, int h)
{
  new_coord (&r->c, &global_level, room, x, y);
  r->w = w;
  r->h = h;
  return r;
}

void
draw_filled_rect (ALLEGRO_BITMAP *to, struct rect *r,
                  ALLEGRO_COLOR color)
{
  struct coord nc = r->c;

  if (! cutscene && nc.room != room_view) {
    rect2room (r, room_view, &nc);
    if (nc.room != room_view) return;
  }

  draw_filled_rectangle (to, nc.x, nc.y, nc.x + r->w - 1,
                         nc.y + r->h - 1, color);
}

void
clear_rect_to_color (ALLEGRO_BITMAP *to, struct rect *r,
                     ALLEGRO_COLOR color)
{
  int op, src, dst;
  al_get_blender (&op, &src, &dst);
  al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
  draw_filled_rect (to, r, color);
  al_set_blender (op, src, dst);
}

void
draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
              struct coord *c, int flags)
{
  int w = al_get_bitmap_width (from);
  int h = al_get_bitmap_height (from);
  draw_bitmap_regionc (from, to, 0, 0, w, h, c, flags);
}

void
draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                     float sx, float sy, float sw, float sh,
                     struct coord *c, int flags)
{
  if (! from) return;

  struct coord nc = *c;

  if (! cutscene && nc.room != room_view) {
    struct frame f;
    f.b = from;
    f.c = *c;
    frame2room (&f, room_view, &nc);
    if (nc.room != room_view) return;
  }

  draw_bitmap_region (from, to, sx, sy, sw, sh, nc.x, nc.y, flags);

  if (cutscene || con_caching
      || no_recursive_links_continuity) return;

  int x = nc.x;
  int y = nc.y;
  int room = nc.room;
  int w = sw;
  int h = sh;

  struct coord mc;

  push_reset_clipping_rectangle (to);

  if (room == roomd (nc.l, room, LEFT)
      && x < 0) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, ABOVE)
      && y < 3) {
    mc = nc;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, LEFT) && x < 0
      && room == roomd (nc.l, room, ABOVE) && y < 3) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (nc.l, room, ABOVE) && y < 3) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, LEFT)
      && x < 0
      && room == roomd (nc.l, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (nc.l, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  pop_clipping_rectangle ();
}

void
redraw_drawn_rectangle (struct drawn_rectangle *dr, struct pos *p,
                        enum em em, enum vm vm)
{
  if (dr->w <= 0 || dr->h <= 0) return;

  push_clipping_rectangle (dr->bitmap, dr->x, dr->y, dr->w, dr->h);

  struct coord tl, br;
  new_coord (&tl, &global_level, room_view, dr->x, dr->y);
  new_coord (&br, &global_level, room_view, dr->x + dr->w - 1,
             dr->y + dr->h - 1);

  struct pos ptl, pbr;
  posf (&tl, &ptl);
  posf (&br, &pbr);

  pos2room (&ptl, room_view, &ptl);
  pos2room (&pbr, room_view, &pbr);
  struct pos p0 = ptl;
  for (p0.floor = pbr.floor; p0.floor >= ptl.floor; p0.floor--)
    for (p0.place = ptl.place; p0.place <= pbr.place; p0.place++)
      if (! peq (&p0, p)) draw_confg (dr->bitmap, &p0, em, vm);

  pop_clipping_rectangle ();
}

void
draw_room (ALLEGRO_BITMAP *bitmap, int room,
           enum em em, enum vm vm)
{
  struct pos p; new_pos (&p, &global_level, room, -1, -1);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_conbg (bitmap, &p, em, vm);
      free_stars (&p);
    }

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place <= PLACES; p.place++)
      draw_confg (bitmap, &p, em, vm);
}

void
draw_con (ALLEGRO_BITMAP *bitmap, struct pos *p,
          enum em em, enum vm vm)
{
  draw_conbg (bitmap, p, em, vm);
  draw_confg (bitmap, p, em, vm);
}

void
draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm)
{
  switch (bg (p)) {
  case NO_BG:
    if (em == PALACE) draw_bricks_02 (bitmap, p, em, vm);
    break;
  case BRICKS_00: draw_bricks_00 (bitmap, p, em, vm); break;
  case BRICKS_01: draw_bricks_01 (bitmap, p, em, vm); break;
  case BRICKS_02: draw_bricks_02 (bitmap, p, em, vm); break;
  case BRICKS_03: draw_bricks_03 (bitmap, p, em, vm); break;
  case NO_BRICKS: break;
  case TORCH: draw_torch (bitmap, p, em, vm); break;
  case WINDOW: draw_window (bitmap, p, em, vm); break;
  case BALCONY: draw_balcony (bitmap, p, em, vm); break;
  default: assert (false); break;
  }

  if (fake (p) == LEVEL_DOOR)
    draw_level_door (bitmap, p, em, vm);
}

void
draw_confg_no_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  draw_confg_base (bitmap, p, em, vm);
  draw_confg_left (bitmap, p, em, vm);
  draw_confg_right (bitmap, p, em, vm);
}

void
draw_confg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm)
{
  draw_confg_no_top (bitmap, p, em, vm);
  draw_confg_top (bitmap, p, em, vm);
}

void
draw_confg_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_base (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_base (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_base (bitmap, p, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_base (bitmap, p, em, vm); break;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_floor_base (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM: draw_floor_base (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP: return;
  case WALL:
    if (con_caching) draw_wall_base (bitmap, p, em, vm);
    else draw_wall_base_cache (bitmap, p);
    break;
  case DOOR: draw_floor_base (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_floor_base (bitmap, p, em, vm); break;
  case CHOPPER: draw_floor_base (bitmap, p, em, vm); break;
  case ARCH_BOTTOM: draw_floor_base (bitmap, p, em, vm); break;
  case ARCH_TOP_MID: return;
  case ARCH_TOP_SMALL: return;
  case ARCH_TOP_LEFT: return;
  case ARCH_TOP_RIGHT: return;
  case CARPET: draw_floor_base (bitmap, p, em, vm); break;
  case TCARPET: draw_door_pole_base (bitmap, p, em, vm); break;
  case MIRROR: draw_floor_base (bitmap, p, em, vm); break;
  default: assert (false); break;
  }
}

void
draw_confg_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  enum carpet_design d;

  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR: draw_floor_left (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_left (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_left (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_left (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_left (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_left (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_left (bitmap, p, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_left (bitmap, p, em, vm); break;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_left (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_left (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, p, em, vm); break;
  case WALL:
    if (con_caching) draw_wall_left (bitmap, p, em, vm);
    else draw_wall_left_cache (bitmap, p);
    break;
  case DOOR: draw_door_left (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_level_door_left (bitmap, p, em, vm); break;
  case CHOPPER: draw_chopper_left (bitmap, p, em, vm); break;
  case ARCH_BOTTOM:
    draw_floor_left (bitmap, p, em, vm);
    draw_arch_bottom (bitmap, p, em, vm); break;
  case ARCH_TOP_MID: draw_arch_top_mid (bitmap, p, em, vm); break;
  case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, p, em, vm); break;
  case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, p, em, vm); break;
  case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, p, em, vm); break;
  case CARPET:
    draw_floor_left (bitmap, p, em, vm);
    draw_door_pole (bitmap, p, em, vm);
    break;
  case TCARPET:
    d = ext (p);
    if (d == ARCH_CARPET_RIGHT_00 || d == ARCH_CARPET_RIGHT_01)
      draw_arch_top_right_end (bitmap, p, em, vm);
    else draw_door_pole (bitmap, p, em, vm);
    break;
  case MIRROR:
    draw_floor_left (bitmap, p, em, vm);
    draw_mirror (bitmap, p, em, vm);
    break;
  default: assert (false); break;
  }
}

void
draw_confg_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR: draw_floor_right (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_right (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_right (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_right (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_right (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_right (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_right (bitmap, p, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_right (bitmap, p, em, vm); break;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_right (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_right (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_right (bitmap, p, em, vm); break;
  case WALL: draw_wall_right (bitmap, p, em, vm); break;
  case DOOR: draw_door_right (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_level_door_right (bitmap, p, em, vm); break;
  case CHOPPER: draw_floor_right (bitmap, p, em, vm); break;
  case ARCH_BOTTOM: draw_floor_right (bitmap, p, em, vm); break;
  case ARCH_TOP_MID: return;
  case ARCH_TOP_SMALL: return;
  case ARCH_TOP_LEFT: return;
  case ARCH_TOP_RIGHT: return;
  case CARPET: draw_carpet_right (bitmap, p, FULL_WIDTH, em, vm); break;
  case TCARPET: draw_carpet_right (bitmap, p, FULL_WIDTH, em, vm); break;
  case MIRROR: draw_floor_right (bitmap, p, em, vm); break;
  default: assert (false); break;
  }
}

void
draw_confg_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR: return;
  case BROKEN_FLOOR: return;
  case SKELETON_FLOOR: return;
  case LOOSE_FLOOR: return;
  case SPIKES_FLOOR: return;
  case OPENER_FLOOR: return;
  case CLOSER_FLOOR: return;
  case STUCK_FLOOR: return;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_top (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM: return;
  case BIG_PILLAR_TOP: draw_big_pillar_top_top (bitmap, p, em, vm); break;
  case WALL: draw_wall_top (bitmap, p, em, vm); break;
  case DOOR: draw_door_top (bitmap, p, em, vm); break;
  case LEVEL_DOOR: return;
  case CHOPPER: return;
  case ARCH_BOTTOM: return;
  case ARCH_TOP_MID: return;
  case ARCH_TOP_SMALL: return;
  case ARCH_TOP_LEFT: return;
  case ARCH_TOP_RIGHT: return;
  case CARPET: draw_carpet_top (bitmap, p, FULL_WIDTH, em, vm); break;
  case TCARPET: draw_carpet_top (bitmap, p, FULL_WIDTH, em, vm); break;
  case MIRROR: return;
  default: assert (false); break;
  }
}

void
draw_room_frame_fg (ALLEGRO_BITMAP *bitmap, enum em em, enum vm vm,
                    struct frame *f)
{
  struct pos ptl, pbr, ptl2, pbr2;

  survey (_tl, posf, f, NULL, &ptl, NULL);
  survey (_br, posf, f, NULL, &pbr, NULL);
  pos2room (&ptl, room_view, &ptl);
  pos2room (&pbr, room_view, &pbr);

  survey (_tl, pos, f, NULL, &ptl2, NULL);
  survey (_br, pos, f, NULL, &pbr2, NULL);
  pos2room (&ptl2, room_view, &ptl2);
  pos2room (&pbr2, room_view, &pbr2);

  struct coord c; frame2room (f, room_view, &c);

  int w = al_get_bitmap_width (f->b);
  int h = al_get_bitmap_height (f->b);

  push_clipping_rectangle (bitmap, c.x, c.y, w, h);

  /* FALLING LOOSE FLOOR */
  struct pos ptr, ptra;
  survey (_tr, posf, f, NULL, &ptr, NULL);
  prel (&ptr, &ptra, -1, +0);
  draw_falling_loose_floor (bitmap, &ptr, em, vm);
  draw_falling_loose_floor (bitmap, &ptra, em, vm);

  struct pos p = ptl;
  for (p.floor = pbr.floor; p.floor >= ptl.floor; p.floor--)
    for (p.place = ptl.place; p.place <= pbr.place; p.place++)
      switch (fake (&p)) {
      case SPIKES_FLOOR: draw_spikes_fg (bitmap, &p, em, vm); break;
      case BROKEN_FLOOR: draw_broken_floor_fg (bitmap, &p, em, vm); break;
      case OPENER_FLOOR: draw_opener_floor_fg (bitmap, &p, em, vm); break;
      case CLOSER_FLOOR: draw_closer_floor_fg (bitmap, &p, em, vm); break;
      case PILLAR: draw_pillar_fg (bitmap, &p, em, vm); break;
      case BIG_PILLAR_BOTTOM:
        draw_big_pillar_bottom_fg (bitmap, &p, em, vm); break;
      case BIG_PILLAR_TOP:
        draw_big_pillar_top_left (bitmap, &p, em, vm); break;
      case ARCH_BOTTOM: draw_arch_bottom_fg (bitmap, &p, em, vm); break;
      case WALL: draw_wall_fg (bitmap, &p, f, em, vm); break;
      case CHOPPER: draw_chopper_fg (bitmap, &p, em, vm); break;
      case MIRROR: draw_mirror_fg (bitmap, &p, f, em, vm); break;
      case ARCH_TOP_MID: draw_arch_top_mid (bitmap, &p, em, vm); break;
      case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, &p, em, vm); break;
      case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, &p, em, vm); break;
      case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, &p, em, vm); break;
      default: break;
      }

  p = ptl2;
  for (p.floor = pbr2.floor; p.floor >= ptl2.floor; p.floor--)
    for (p.place = ptl2.place; p.place <= pbr2.place; p.place++)
      switch (fake (&p)) {
      case WALL: draw_wall_fg (bitmap, &p, f, em, vm); break;
      case DOOR: draw_door_fg (bitmap, &p, f, em, vm); break;
      case CARPET: draw_carpet_fg (bitmap, &p, f, em, vm); break;
      case TCARPET: draw_carpet_fg (bitmap, &p, f, em, vm); break;
      default: break;
      }

  pop_clipping_rectangle ();
}

void
draw_room_anim_fg_sub (ALLEGRO_BITMAP *bitmap,
                       enum em em, enum vm vm, struct anim *a)
{
  struct pos pm, ptl, ptr, ptl2, ptr2;

  struct frame *f = &a->f;
  struct coord c; frame2room (f, room_view, &c);

  int w = al_get_bitmap_width (f->b);
  int h = al_get_bitmap_height (f->b);

  survey (_m, posf, f, NULL, &pm, NULL);
  survey (_tl, posf, f, NULL, &ptl, NULL);
  survey (_tr, posf, f, NULL, &ptr, NULL);

  pos2room (&pm, room_view, &pm);
  pos2room (&ptl, room_view, &ptl);
  pos2room (&ptr, room_view, &ptr);

  survey (_tl, pos, f, NULL, &ptl2, NULL);
  survey (_tr, pos, f, NULL, &ptr2, NULL);

  pos2room (&ptl2, room_view, &ptl2);
  pos2room (&ptr2, room_view, &ptr2);

  push_clipping_rectangle (bitmap, c.x, c.y, w, h);

  /* SWORD */
  if (fake (&pm) == CARPET
      && a->f.dir == RIGHT
      && a->xf.b) {
    struct coord c2;
    int w2 = al_get_bitmap_width (a->xf.b);
    int h2 = al_get_bitmap_height (a->xf.b);
    xframe_coord (&a->f, &a->xf, &c2);
    ignore_clipping_rectangle_intersection = true;
    push_clipping_rectangle (bitmap, c2.x, c2.y, w2, h2);
    push_drawn_rectangle (bitmap);
    draw_confg_right (bitmap, &pm, em, vm);
    redraw_drawn_rectangle (pop_drawn_rectangle (), &pm, em, vm);
    pop_clipping_rectangle ();
    ignore_clipping_rectangle_intersection = false;
  }

  /* CLIMBING STAIRS */
  if (is_kid_stairs (f)) draw_level_door_fg (bitmap, &a->p, f, em, vm);

  /* FALLING */
  if (is_anim_fall (f)) {
    int dy;
    if (is_strictly_traversable (&ptl2)) dy = 49;
    else dy = 56;

    push_clipping_rectangle (bitmap,
                             PLACE_WIDTH * ptr2.place,
                             PLACE_HEIGHT * ptr2.floor + dy,
                             PLACE_WIDTH + 16,
                             PLACE_HEIGHT - dy + 3);

    push_drawn_rectangle (bitmap);

    if (! is_strictly_traversable_fake (&ptr2))
      draw_confg (bitmap, &ptr2, em, vm);
    if (! is_strictly_traversable_fake (&ptr))
      draw_confg (bitmap, &ptr, em, vm);

    redraw_drawn_rectangle (pop_drawn_rectangle (), &ptr2, em, vm);

    pop_clipping_rectangle ();
  }

  /* VERTICAL JUMP */
  if (is_kid_vjump_touching_above (f)) {
    int dx;
    if (f->dir == LEFT && is_strictly_traversable (&ptr2)) dx = PLACE_WIDTH;
    else dx = 2 * PLACE_WIDTH;

    push_clipping_rectangle (bitmap,
                             PLACE_WIDTH * ptl2.place,
                             PLACE_HEIGHT * ptl2.floor + 56,
                             dx,
                             PLACE_HEIGHT - 56 + 3);

    push_drawn_rectangle (bitmap);

    draw_confg (bitmap, &ptl2, em, vm);
    draw_confg (bitmap, &ptr, em, vm);

    redraw_drawn_rectangle (pop_drawn_rectangle (), &ptl2, em, vm);

    pop_clipping_rectangle ();
  }

  /* HANGING */
  if (is_kid_hang (f)) {
    struct pos p; get_hanged_pos (&a->hang_pos, f->dir, &p);
    struct pos pb; prel (&p, &pb, +0, f->dir == LEFT ? +1 : -1);
    pos2room (&p, room_view, &p);
    pos2room (&pb, room_view, &pb);

    if (f->dir == RIGHT) {
      push_clipping_rectangle (bitmap,
                               PLACE_WIDTH * p.place,
                               PLACE_HEIGHT * p.floor + 56,
                               PLACE_WIDTH,
                               PLACE_HEIGHT - 53);
      push_drawn_rectangle (bitmap);
      draw_confg (bitmap, &p, em, vm);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &p, em, vm);
    } else {
      push_clipping_rectangle (bitmap,
                               PLACE_WIDTH * pb.place,
                               PLACE_HEIGHT * pb.floor + 56,
                               PLACE_WIDTH,
                               PLACE_HEIGHT - 53);
      push_drawn_rectangle (bitmap);
      draw_confg (bitmap, &pb, em, vm);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &pb, em, vm);
    }

    pop_clipping_rectangle ();
  }

  /* CLIMBING FLOOR */
  if (is_kid_climb (f)) {
    struct pos p; get_hanged_pos (&a->hang_pos, f->dir, &p);
    struct pos pb; prel (&p, &pb, +0, f->dir == LEFT ? +1 : -1);
    pos2room (&p, room_view, &p);
    pos2room (&pb, room_view, &pb);

    if (f->dir == RIGHT) {
      int dy, w;

      if (f->b == kid_climb_00
          || f->b == kid_climb_01
          || f->b == kid_climb_02
          || f->b == kid_climb_08
          || f->b == kid_climb_09) {
        dy = 55;
        w = 18;
      } else if (f->b == kid_climb_05
                 || f->b == kid_climb_06) {
        dy = 53;
        w = 22;
      } else if (f->b == kid_climb_03
                 || f->b == kid_climb_07
                 || f->b == kid_climb_04) {
        dy = 53;
        w = 21;
      } else {
        /* shouldn't get here */
        assert (false);
        dy = 53;
      }

      push_clipping_rectangle (bitmap,
                               PLACE_WIDTH * p.place,
                               PLACE_HEIGHT * p.floor + dy,
                               w,
                               PLACE_HEIGHT - dy + 3);
      push_drawn_rectangle (bitmap);
      draw_confg (bitmap, &p, em, vm);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &p, em, vm);
    } else {
      push_clipping_rectangle (bitmap,
                               PLACE_WIDTH * pb.place,
                               PLACE_HEIGHT * pb.floor + 54,
                               PLACE_WIDTH,
                               PLACE_HEIGHT - 54 + 3);
      push_drawn_rectangle (bitmap);
      draw_confg (bitmap, &pb, em, vm);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &pb, em, vm);
    }

    pop_clipping_rectangle ();
  }

  pop_clipping_rectangle ();

  draw_room_frame_fg (bitmap, em, vm, f);

  /* xframe */
  if (a->xf.b) {
    struct frame xf; xframe_frame (&a->f, &a->xf, &xf);
    draw_room_frame_fg (bitmap, em, vm, &xf);
  }

  /* splash */
  if (a->splash) {
    struct frame sf; splash_frame (&a->f, &sf);
    draw_room_frame_fg (bitmap, em, vm, &sf);
  }
}

void
draw_room_anim_fg (ALLEGRO_BITMAP *bitmap,
                   enum em em, enum vm vm, struct anim *a0)
{
  draw_room_anim_fg_sub (bitmap, em, vm, a0);

  int x = a0->f.c.x;
  int y = a0->f.c.y;
  int room = a0->f.c.room;
  int w = al_get_bitmap_width (a0->f.b);
  int h = al_get_bitmap_height (a0->f.b);

  struct anim a = *a0;

  if (room == roomd (a.f.c.l, room, LEFT)
      && x < 0) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, ABOVE)
      && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, LEFT) && x < 0
      && room == roomd (a.f.c.l, room, ABOVE) && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (a.f.c.l, room, ABOVE) && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, LEFT)
      && x < 0
      && room == roomd (a.f.c.l, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (a.f.c.l, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (a.f.c.l, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }
}

void
draw_confg_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm, struct frame *f)
{
  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR: return;
  case BROKEN_FLOOR: draw_broken_floor_fg (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: return;
  case LOOSE_FLOOR: return;
  case SPIKES_FLOOR: draw_spikes_fg (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_fg (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_fg (bitmap, p, em, vm); break;
  case STUCK_FLOOR: return;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_fg (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_fg (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, p, em, vm); break;
  case WALL: draw_wall_fg (bitmap, p, f, em, vm); break;
  case DOOR: draw_door_fg (bitmap, p, f, em, vm); break;
  case LEVEL_DOOR: break;
  case CHOPPER: draw_chopper_fg (bitmap, p, em, vm); break;
  case ARCH_BOTTOM: draw_arch_bottom_fg (bitmap, p, em, vm);
    break;
  case ARCH_TOP_MID: draw_arch_top_mid (bitmap, p, em, vm); break;
  case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, p, em, vm); break;
  case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, p, em, vm); break;
  case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, p, em, vm); break;
  case CARPET: draw_carpet_fg (bitmap, p, f, em, vm); break;
  case TCARPET:
    draw_door_pole_base (bitmap, p, em, vm);
    draw_carpet_fg (bitmap, p, f, em, vm); break;
  case MIRROR: draw_mirror_fg (bitmap, p, f, em, vm); break;
  default: assert (false); break;
  }
}

ALLEGRO_COLOR
green_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -12);
  g = add_char (g, +16);
  b = add_char (b, -29);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
gray_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, +13);
  g = add_char (g, -7);
  b = add_char (b, -52);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
yellow_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, +78);
  g = add_char (g, +25);
  b = add_char (b, -64);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
blue_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -96);
  g = add_char (g, -80);
  b = add_char (b, +64);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
selection_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  signed char d;
  if (vm == CGA && ! hgc) d = -128;
  else if (vm == CGA && hgc && em == DUNGEON) d = -96;
  else if (vm == VGA && em == PALACE) d = +32;
  else d = +64;

  r = add_char (r, d);
  g = add_char (g, d);
  b = add_char (b, d);
  return al_map_rgb (r, g, b);
}

ALLEGRO_BITMAP *
apply_hue_palette (ALLEGRO_BITMAP *bitmap)
{
  switch (hue) {
  case HUE_NONE: default: return bitmap;
  case HUE_GREEN:
    return apply_palette (bitmap, green_hue_palette);
  case HUE_GRAY:
    return apply_palette (bitmap, gray_hue_palette);
  case HUE_YELLOW:
    return apply_palette (bitmap, yellow_hue_palette);
  case HUE_BLUE:
    return apply_palette (bitmap, blue_hue_palette);
  }
}

ALLEGRO_COLOR
apply_hue_color (ALLEGRO_COLOR c)
{
  switch (hue) {
  case HUE_NONE: default: return c;
  case HUE_GREEN: return green_hue_palette (c);
  case HUE_GRAY: return gray_hue_palette (c);
  case HUE_YELLOW: return yellow_hue_palette (c);
  case HUE_BLUE: return blue_hue_palette (c);
  }
}

void
draw_no_floor_selection (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (peq (p, &mouse_pos)) {
    struct rect r;
    new_rect (&r, p->room, p->place * PLACE_WIDTH + 25,
              p->floor * PLACE_HEIGHT - 13,
              PLACE_WIDTH, PLACE_HEIGHT);
    draw_filled_rect (bitmap, &r, NO_FLOOR_SELECTION_COLOR);
  }
}
