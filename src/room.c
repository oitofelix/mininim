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

static void draw_arch_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p);
static void draw_big_pillar_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p);

void
draw_filled_rect (ALLEGRO_BITMAP *to, struct rect *r,
                  ALLEGRO_COLOR color)
{
  struct coord nc = r->c;

  if (! cutscene && nc.room > 0 && nc.room != room_view) {
    rect2room (r, room_view, &nc);
    if (nc.room != room_view) return;
  }

  lua_Number x = nc.x;
  lua_Number y = nc.y;
  lua_Number w = nc.x + r->w - 1;
  lua_Number h = nc.y + r->h - 1;

  if (nc.room >= 0) {
    x = OW (x);
    y = OH (y);
    w = OW (w);
    h = OH (h);
  }

  draw_filled_rectangle (to, x, y, w, h, color);
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
  lua_Number w = IW (get_bitmap_width (from));
  lua_Number h = IH (get_bitmap_height (from));
  draw_bitmap_regionc (from, to, 0, 0, w, h, c, flags);
}

void
draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                     lua_Number sx, lua_Number sy,
                     lua_Number sw, lua_Number sh,
                     struct coord *c, int flags)
{
  if (! from) return;

  struct coord nc = *c;

  if (! cutscene && nc.room > 0 && nc.room != room_view) {
    struct frame f;
    f.b = from;
    f.c = *c;
    frame2room (&f, room_view, &nc);
    if (nc.room != room_view) return;
  }

  draw_bitmap_region_o (from, to, sx, sy, sw, sh, nc.x, nc.y, flags);

  if (cutscene || tile_caching
      || nc.room < 0
      || no_recursive_links_continuity) return;

  int room = nc.room;
  lua_Number x = nc.x;
  lua_Number y = nc.y;
  lua_Number w = sw;
  lua_Number h = sh;

  struct coord mc;

  push_reset_clipping_rectangle (to);

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, LEFT)
      && x < 0) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, ABOVE)
      && y < 3) {
    mc = nc;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, LEFT) && x < 0
      && room == roomd (nc.l->rlink, nc.l->room_nmemb, room, ABOVE) && y < 3) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (nc.l->rlink, nc.l->room_nmemb, room, ABOVE) && y < 3) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, LEFT)
      && x < 0
      && room == roomd (nc.l->rlink, nc.l->room_nmemb, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (nc.l->rlink, nc.l->room_nmemb, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (nc.l->rlink, nc.l->room_nmemb, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region_o (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  pop_clipping_rectangle ();
}

void
draw_bitmap_region_o (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                      lua_Number sx, lua_Number sy,
                      lua_Number sw, lua_Number sh,
                      lua_Number dx, lua_Number dy, int flags)
{
  sx = OW (sx);
  sy = OH (sy);
  sw = OW (sw);
  sh = OH (sh);
  dx = OW (dx);
  dy = OW (dy);

  draw_bitmap_region (from, to, sx, sy, sw, sh, dx, dy, flags);
}

void
redraw_drawn_rectangle (struct drawn_rectangle *dr, struct pos *p)
{
  if (dr->w <= 0 || dr->h <= 0) return;

  push_clipping_rectangle (dr->bitmap, dr->x, dr->y, dr->w, dr->h);

  struct coord tl, br;
  new_coord (&tl, &global_level, room_view, IW (dr->x), IH (dr->y));
  new_coord (&br, &global_level, room_view, IW (dr->x + dr->w - 1),
             IH (dr->y + dr->h - 1));

  struct pos ptl, pbr;
  posf (&tl, &ptl);
  posf (&br, &pbr);

  pos2room (&ptl, room_view, &ptl);
  pos2room (&pbr, room_view, &pbr);
  struct pos p0 = ptl;
  for (p0.floor = pbr.floor; p0.floor >= ptl.floor; p0.floor--) {
    if (p0.floor > p->floor) continue;
    for (p0.place = ptl.place; p0.place <= pbr.place; p0.place++)
      if (! peq (&p0, p)) draw_tile_fg (dr->bitmap, &p0);
  }

  pop_clipping_rectangle ();
}

void
draw_room (ALLEGRO_BITMAP *bitmap, int room)
{
  struct pos p; new_pos (&p, &global_level, room, -1, -1);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_tile_bg (bitmap, &p);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place <= PLACES; p.place++)
      draw_tile_fg (bitmap, &p);
}

void
draw_tile (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_tile_bg (bitmap, p);
  draw_tile_fg (bitmap, p);
}

void
draw_tile_bg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  switch (bg (p)) {
  case NO_BRICKS: break;
  case BRICKS_1:
    draw_object_part (bitmap, "BRICKS", 1, p);
    break;
  case BRICKS_2:
    draw_object_part (bitmap, "BRICKS", 2, p);
    break;
  case BRICKS_3:
    draw_object_part (bitmap, "BRICKS", 3, p);
    break;
  case BRICKS_4:
    draw_object_part (bitmap, "BRICKS", 4, p);
    break;
  case BRICKS_5:
    draw_object_part (bitmap, "BRICKS", 5, p);
    break;
  case TORCH:
    draw_object (bitmap, "TORCH", p);
    break;
  case WINDOW:
    draw_object (bitmap, "WINDOW", p);
    break;
  case BALCONY:
    draw_object (bitmap, "BALCONY", p);
    break;
  default: assert (false); break;
  }

  if (fake (p) == LEVEL_DOOR) draw_level_door (bitmap, p);
}

void
draw_tile_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR:
    draw_object (bitmap, "FLOOR", p);
    break;
  case BROKEN_FLOOR:
    draw_object_part (bitmap, "BROKEN_FLOOR", "MAIN", p);
    break;
  case SKELETON_FLOOR:
    draw_object (bitmap, "SKELETON_FLOOR", p);
    break;
  case LOOSE_FLOOR:
    draw_loose_floor (bitmap, p);
    break;
  case SPIKES_FLOOR:
    draw_spikes_floor (bitmap, p);
    break;
  case OPENER_FLOOR:
    draw_opener_floor (bitmap, p);
    break;
  case CLOSER_FLOOR:
    draw_closer_floor (bitmap, p);
    break;
  case STUCK_FLOOR:
    draw_object_part (bitmap, "CLOSER_FLOOR", "PRESSED", p);
    break;
  case HIDDEN_FLOOR: return;
  case PILLAR:
    draw_object_part (bitmap, "PILLAR", "MAIN", p);
    break;
  case BIG_PILLAR_BOTTOM:
    draw_object_part (bitmap, "BIG_PILLAR_BOTTOM", "MAIN", p);
    break;
  case BIG_PILLAR_TOP:
    draw_object_part (bitmap, "BIG_PILLAR_TOP", "MAIN", p);
    break;
  case WALL:
    draw_wall (bitmap, p);
    break;
  case DOOR:
    draw_door (bitmap, p);
    break;
  case LEVEL_DOOR:
    draw_level_door_floor (bitmap, p);
    break;
  case CHOMPER:
    draw_chomper (bitmap, p);
    break;
  case ARCH_BOTTOM:
    draw_object_part (bitmap, "ARCH_BOTTOM", "MAIN", p);
    break;
  case ARCH_TOP_MID:
    draw_object (bitmap, "ARCH_TOP_MID", p);
    break;
  case ARCH_TOP_SMALL:
    draw_object (bitmap, "ARCH_TOP_SMALL", p);
    break;
  case ARCH_TOP_LEFT:
    draw_object (bitmap, "ARCH_TOP_LEFT", p);
    break;
  case ARCH_TOP_RIGHT:
    draw_object (bitmap, "ARCH_TOP_RIGHT", p);
    break;
  case CARPET:
    draw_carpet (bitmap, p);
    break;
  case TCARPET:
    draw_carpet (bitmap, p);
    break;
  case MIRROR:
    draw_object_part (bitmap, "MIRROR", "MAIN", p);
    break;
  default: assert (false); break;
  }
}

void
draw_arch_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  push_drawn_rectangle (bitmap);
  draw_object_part (bitmap, "ARCH_BOTTOM", "FRONT", p);
  redraw_drawn_rectangle (pop_drawn_rectangle (), p);
}

void
draw_big_pillar_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  push_drawn_rectangle (bitmap);
  draw_object_part (bitmap, "BIG_PILLAR_BOTTOM", "FRONT", p);
  redraw_drawn_rectangle (pop_drawn_rectangle (), p);
}

void
draw_room_frame_front (ALLEGRO_BITMAP *bitmap, struct frame *f)
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

  int w = get_bitmap_width (f->b);
  int h = get_bitmap_height (f->b);

  push_clipping_rectangle (bitmap, OW (c.x), OH (c.y), w, h);

  /* FALLING LOOSE FLOOR */
  struct pos ptr, ptra;
  survey (_tr, posf, f, NULL, &ptr, NULL);
  prel (&ptr, &ptra, -1, +0);
  draw_falling_loose_floor (bitmap, &ptr);
  draw_falling_loose_floor (bitmap, &ptra);

  struct pos p = ptl;
  for (p.floor = pbr.floor; p.floor >= ptl.floor; p.floor--)
    for (p.place = ptl.place; p.place <= pbr.place; p.place++)
      switch (fake (&p)) {
      case SPIKES_FLOOR:
        draw_spikes_floor_fg (bitmap, &p);
        break;
      case BROKEN_FLOOR:
        draw_object_part (bitmap, "BROKEN_FLOOR", "FRONT", &p);
        break;
      case OPENER_FLOOR: break;
      case CLOSER_FLOOR: break;
      case PILLAR:
        draw_object_part (bitmap, "PILLAR", "FRONT", &p);
        break;
      case BIG_PILLAR_BOTTOM:
        draw_big_pillar_bottom_fg (bitmap, &p);
        break;
      case BIG_PILLAR_TOP:
        draw_object_part (bitmap, "BIG_PILLAR_TOP", "FRONT", &p);
        break;
      case ARCH_BOTTOM:
        draw_arch_bottom_fg (bitmap, &p);
        break;
      case WALL:
        draw_wall_fg (bitmap, &p, f);
        break;
      case CHOMPER:
        draw_chomper_fg (bitmap, &p, f);
        break;
      case MIRROR: draw_mirror_fg (bitmap, &p, f); break;
      case ARCH_TOP_MID:
        draw_object (bitmap, "ARCH_TOP_MID", &p);
        break;
      case ARCH_TOP_SMALL:
        draw_object (bitmap, "ARCH_TOP_SMALL", &p);
        break;
      case ARCH_TOP_LEFT:
        draw_object (bitmap, "ARCH_TOP_LEFT", &p);
        break;
      case ARCH_TOP_RIGHT:
        draw_object (bitmap, "ARCH_TOP_RIGHT", &p);
        break;
      default: break;
      }

  p = ptl2;
  for (p.floor = pbr2.floor; p.floor >= ptl2.floor; p.floor--)
    for (p.place = ptl2.place; p.place <= pbr2.place; p.place++)
      switch (fake (&p)) {
      case WALL:
        draw_wall_fg (bitmap, &p, f);
        break;
      case DOOR:
        draw_door_fg (bitmap, &p, f);
        break;
      case CARPET:
        draw_carpet_fg (bitmap, &p, f);
        break;
      case TCARPET:
        draw_carpet_fg (bitmap, &p, f);
        break;
      default: break;
      }

  pop_clipping_rectangle ();
}

void
draw_room_actor_front_sub (ALLEGRO_BITMAP *bitmap, struct actor *a)
{
  struct pos pm, ptl, ptr, ptl2, ptr2;

  struct frame *f = &a->f;
  struct coord c; frame2room (f, room_view, &c);

  int w = get_bitmap_width (f->b);
  int h = get_bitmap_height (f->b);

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

  push_clipping_rectangle (bitmap, OW (c.x), OH (c.y), w, h);

  /* SWORD */
  if (fake (&pm) == CARPET
      && a->f.dir == RIGHT
      && a->xf.b) {
    struct coord c2;
    int w2 = get_bitmap_width (a->xf.b);
    int h2 = get_bitmap_height (a->xf.b);
    xframe_coord (&a->f, &a->xf, &c2);
    ignore_clipping_rectangle_intersection = true;
    push_clipping_rectangle (bitmap, OW (c2.x), OH (c2.y), w2, h2);
    push_drawn_rectangle (bitmap);
    draw_tile_fg (bitmap, &pm);
    redraw_drawn_rectangle (pop_drawn_rectangle (), &pm);
    pop_clipping_rectangle ();
    ignore_clipping_rectangle_intersection = false;
  }

  /* CLIMBING STAIRS */
  if (a->action == kid_stairs) draw_level_door_fg (bitmap, &a->p, a);

  /* FALLING */
  if (is_actor_fall (a)) {
    int dy;
    if (is_traversable (&ptl2)) dy = 49;
    else dy = 56;

    push_clipping_rectangle (bitmap,
                             OW (PLACE_WIDTH * ptr2.place),
                             OH (PLACE_HEIGHT * ptr2.floor + dy),
                             OW (PLACE_WIDTH + 16),
                             OH (PLACE_HEIGHT - dy + 3));

    push_drawn_rectangle (bitmap);

    if (! is_traversable_fake (&ptr2))
      draw_tile_fg (bitmap, &ptr2);
    if (! is_traversable_fake (&ptr))
      draw_tile_fg (bitmap, &ptr);

    redraw_drawn_rectangle (pop_drawn_rectangle (), &ptr2);

    pop_clipping_rectangle ();
  }

  /* VERTICAL JUMP */
  if (is_kid_vjump_touching_above (a)) {
    int dx;
    if (f->dir == LEFT && is_traversable (&ptr2)) dx = PLACE_WIDTH;
    else dx = 2 * PLACE_WIDTH;

    push_clipping_rectangle (bitmap,
                             OW (PLACE_WIDTH * ptl2.place),
                             OH (PLACE_HEIGHT * ptl2.floor + 54),
                             OW (dx),
                             OH (PLACE_HEIGHT - 54 + 3));

    push_drawn_rectangle (bitmap);

    tile_caching = true;
    draw_tile_fg (bitmap, &ptl2);
    draw_tile_fg (bitmap, &ptr);
    tile_caching = false;

    redraw_drawn_rectangle (pop_drawn_rectangle (), &ptl2);

    pop_clipping_rectangle ();
  }

  /* HANGING */
  if (is_kid_hang (a)) {
    struct pos p; get_hanged_pos (&a->hang_pos, f->dir, &p);
    struct pos pb; prel (&p, &pb, +0, f->dir == LEFT ? +1 : -1);
    pos2room (&p, room_view, &p);
    pos2room (&pb, room_view, &pb);

    if (f->dir == RIGHT) {
      push_clipping_rectangle (bitmap,
                               OW (PLACE_WIDTH * p.place),
                               OH (PLACE_HEIGHT * p.floor + 57),
                               OW (PLACE_WIDTH),
                               OH (PLACE_HEIGHT - 57 + 3));
      push_drawn_rectangle (bitmap);
      draw_tile_fg (bitmap, &p);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &p);
    } else {
      push_clipping_rectangle (bitmap,
                               OW (PLACE_WIDTH * pb.place),
                               OH (PLACE_HEIGHT * pb.floor + 56),
                               OW (PLACE_WIDTH),
                               OH (PLACE_HEIGHT - 53));
      push_drawn_rectangle (bitmap);
      draw_tile_fg (bitmap, &pb);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &pb);
    }

    pop_clipping_rectangle ();
  }

  /* CLIMBING FLOOR */
  if (is_kid_climb (a)) {
    struct pos p; get_hanged_pos (&a->hang_pos, f->dir, &p);
    struct pos pb; prel (&p, &pb, +0, f->dir == LEFT ? +1 : -1);
    pos2room (&p, room_view, &p);
    pos2room (&pb, room_view, &pb);

    if (f->dir == RIGHT) {
      lua_Number dy, w;

      dy = 57;
      w = 20;

      push_clipping_rectangle (bitmap,
                               OW (PLACE_WIDTH * p.place),
                               OH (PLACE_HEIGHT * p.floor + dy),
                               OW (w),
                               OH (PLACE_HEIGHT - dy + 3));
      /* push_drawn_rectangle (bitmap); */
      draw_tile_fg (bitmap, &p);
      /* redraw_drawn_rectangle (pop_drawn_rectangle (), &p); */
    } else {
      push_clipping_rectangle (bitmap,
                               OW (PLACE_WIDTH * pb.place),
                               OH (PLACE_HEIGHT * pb.floor + 54),
                               OW (PLACE_WIDTH),
                               OH (PLACE_HEIGHT - 54 + 3));
      push_drawn_rectangle (bitmap);
      draw_tile_fg (bitmap, &pb);
      redraw_drawn_rectangle (pop_drawn_rectangle (), &pb);
    }

    pop_clipping_rectangle ();
  }

  pop_clipping_rectangle ();

  draw_room_frame_front (bitmap, f);

  /* xframe */
  if (a->xf.b) {
    struct frame xf; xframe_frame (&a->f, &a->xf, &xf);
    draw_room_frame_front (bitmap, &xf);
  }

  /* splash */
  if (a->splash) {
    struct frame sf; splash_frame (a->splash_bitmap, &a->f, &sf);
    draw_room_frame_front (bitmap, &sf);
  }
}

void
draw_room_actor_front (ALLEGRO_BITMAP *bitmap, struct actor *a0)
{
  draw_room_actor_front_sub (bitmap, a0);

  int x = a0->f.c.x;
  int y = a0->f.c.y;
  int room = a0->f.c.room;
  int w = get_bitmap_width (a0->f.b);
  int h = get_bitmap_height (a0->f.b);

  struct actor a = *a0;

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, LEFT)
      && x < 0) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, ABOVE)
      && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, LEFT)
      && x < 0
      && room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, ABOVE)
      && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, ABOVE)
      && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, LEFT)
      && x < 0
      && room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_actor_front_sub (bitmap, &a);
  }

  if (room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (a.f.c.l->rlink, a.f.c.l->room_nmemb, room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_actor_front_sub (bitmap, &a);
  }
}

void
draw_tile_fg_front (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  switch (fake (p)) {
  case NO_FLOOR: return;
  case FLOOR: return;
  case BROKEN_FLOOR:
    draw_object_part (bitmap, "BROKEN_FLOOR", "FRONT", p); break;
  case SKELETON_FLOOR: return;
  case LOOSE_FLOOR: return;
  case SPIKES_FLOOR:
    draw_spikes_floor_fg (bitmap, p);
    break;
  case OPENER_FLOOR: return;
  case CLOSER_FLOOR: return;
  case STUCK_FLOOR: return;
  case HIDDEN_FLOOR: return;
  case PILLAR:
    draw_object_part (bitmap, "PILLAR", "FRONT", p);
    break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_fg (bitmap, p);
    break;
  case BIG_PILLAR_TOP:
    draw_object_part (bitmap, "BIG_PILLAR_TOP", "FRONT", p);
    break;
  case WALL:
    draw_wall_fg (bitmap, p, f);
    break;
  case DOOR:
    draw_door_fg (bitmap, p, f);
    break;
  case LEVEL_DOOR: break;
  case CHOMPER:
    draw_chomper_fg (bitmap, p, f);
    break;
  case ARCH_BOTTOM:
    draw_arch_bottom_fg (bitmap, p);
    break;
  case ARCH_TOP_MID:
    draw_object (bitmap, "ARCH_TOP_MID", p);
    break;
  case ARCH_TOP_SMALL:
    draw_object (bitmap, "ARCH_TOP_SMALL", p);
    break;
  case ARCH_TOP_LEFT:
    draw_object (bitmap, "ARCH_TOP_LEFT", p);
    break;
  case ARCH_TOP_RIGHT:
    draw_object (bitmap, "ARCH_TOP_RIGHT", p);
    break;
  case CARPET:
    draw_carpet_fg (bitmap, p, f);
    break;
  case TCARPET:
    draw_carpet_fg (bitmap, p, f);
    break;
  case MIRROR: draw_mirror_fg (bitmap, p, f); break;
  default: assert (false); break;
  }
}

ALLEGRO_BITMAP *
get_tile_bitmap (struct tile *tile_ref, union tile_state *ts,
                 float scale, enum tile_part part)
{
  float sx = (scale * ORIGINAL_WIDTH) / REAL_WIDTH;
  float sy = (scale * ORIGINAL_HEIGHT) / REAL_HEIGHT;
  int w = OW (2 * PLACE_WIDTH) * sx;
  int h_ex = 20;
  int h = OH (PLACE_HEIGHT + 3 + h_ex) * sy;
  ALLEGRO_BITMAP *b = create_bitmap (w, h);
  clear_bitmap (b, TRANSPARENT_COLOR);
  ALLEGRO_TRANSFORM trans;
  al_build_transform (&trans, 0, OH(h_ex) * sx, sx, sy, 0);
  al_set_target_bitmap (b);
  al_use_transform (&trans);
  struct pos p; new_pos (&p, &global_level, 0, 0, 0);
  struct tile *t = tile (&p);
  struct tile t_bkp = *t;
  *t = (struct tile)
    {.fg = NO_FLOOR, .bg = NO_BRICKS, .ext = 0, .fake = -1};

  switch (part) {
  case TILE_ALL:
    *t = *tile_ref;
    break;
  case TILE_FG:
  case TILE_FAKE:
    t->fg = (part == TILE_FG)
      ? fg_tile (tile_ref) : fake_tile (tile_ref);

    switch (fg_tile (t)) {
    case LOOSE_FLOOR:
      t->fg = NO_FLOOR;
      t->fake = LOOSE_FLOOR;
      break;
    case CHOMPER: t->ext = 2; break;
    case SPIKES_FLOOR: t->ext = 5; break;
    case DOOR: t->ext = DOOR_STEPS - 1; break;
    case LEVEL_DOOR: t->ext = LEVEL_DOOR_STEPS - 1; break;
    default: break;
    }

    break;
  case TILE_BG:
    t->bg = tile_ref->bg;
    break;
  case TILE_EXT_ITEM:
    t->ext = ext_val (p.l, FLOOR, tile_ref->ext);
    break;
  case TILE_EXT_DESIGN:
    t->fg = TCARPET;
    t->ext = tile_ref->ext;
    break;
  case TILE_EXT_STEP_SPIKES_FLOOR:
    t->fg = SPIKES_FLOOR;
    t->ext = tile_ref->ext;
    break;
  case TILE_EXT_STEP_CHOMPER:
    t->fg = CHOMPER;
    t->ext = tile_ref->ext;
    break;
  default: assert (false); return NULL;
  }

  if (ts) copy_from_tile_state (&p, ts);
  else init_tile_at_pos (&p);

  tile_caching = true;

  if (t->bg == TORCH) {
    uint16_t anim_cycle_bkp = anim_cycle;
    anim_cycle = 0;
    draw_object (b, "FIRE", &p);
    anim_cycle = anim_cycle_bkp;
  }

  draw_tile_bg (b, &p);
  draw_tile_fg (b, &p);

  if (part == TILE_EXT_ITEM || part == TILE_ALL) {
    if (potion_ext_cs (t->ext)) {
      uint16_t anim_cycle_bkp = anim_cycle;
      anim_cycle = 3;
      const char *potion = tile_item_str[t->ext];
      draw_object (b, potion, &p);
      anim_cycle = anim_cycle_bkp;
    } else if (t->ext == SWORD) {
      uint16_t anim_cycle_bkp = anim_cycle;
      anim_cycle = 1;
      draw_object (b, "SWORD_ITEM", &p);
      anim_cycle = anim_cycle_bkp;
    }
  }

  destroy_tile_at_pos (&p);

  tile_caching = false;
  *t = t_bkp;

  return b;
}

ALLEGRO_COLOR
green_hue_palette (ALLEGRO_COLOR c, void *data)
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
gray_hue_palette (ALLEGRO_COLOR c, void *data)
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
yellow_hue_palette (ALLEGRO_COLOR c, void* data)
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
blue_hue_palette (ALLEGRO_COLOR c, void *data)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -96);
  g = add_char (g, -80);
  b = add_char (b, +64);
  return al_map_rgb (r, g, b);
}

/* ALLEGRO_COLOR */
/* selection_palette (ALLEGRO_COLOR c) */
/* { */
/*   unsigned char r, g, b, a; */
/*   al_unmap_rgba (c, &r, &g, &b, &a); */
/*   if (a == 0) return c; */

/*   signed char d; */
/*   if (vm == CGA && ! hgc) d = -128; */
/*   else if (vm == CGA && hgc && em == DUNGEON) d = -96; */
/*   else if (vm == VGA && em == PALACE) d = +32; */
/*   else d = +64; */

/*   r = add_char (r, d); */
/*   g = add_char (g, d); */
/*   b = add_char (b, d); */
/*   return al_map_rgb (r, g, b); */
/* } */

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
  case HUE_GREEN: return green_hue_palette (c, NULL);
  case HUE_GRAY: return gray_hue_palette (c, NULL);
  case HUE_YELLOW: return yellow_hue_palette (c, NULL);
  case HUE_BLUE: return blue_hue_palette (c, NULL);
  }
}
