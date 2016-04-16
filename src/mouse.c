/*
  mouse.c -- mouse module;

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

#define RUN_FRAMESET_NMEMB 2
static struct frameset run_frameset[RUN_FRAMESET_NMEMB];
static void init_run_frameset (void);

/* variables */
ALLEGRO_BITMAP *mouse_normal_00, *mouse_run_00, *mouse_run_01;

static ALLEGRO_COLOR v_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_palette (ALLEGRO_COLOR c);

void
load_mouse (void)
{
  mouse_normal_00 = load_bitmap (MOUSE_NORMAL_00);
  mouse_run_00 = load_bitmap (MOUSE_RUN_00);
  mouse_run_01 = load_bitmap (MOUSE_RUN_01);

  /* framesets */
  init_run_frameset ();
}

void
unload_mouse (void)
{
  destroy_bitmap (mouse_normal_00);
  destroy_bitmap (mouse_run_00);
  destroy_bitmap (mouse_run_01);
}



struct anim *
create_mouse (struct anim *m0, struct anim *m1, struct pos *p, enum dir dir)
{
  if (! m0) {
    m1->f.b = mouse_normal_00;
    m1->fo.b = mouse_normal_00;
    m1->fight = false;
    m1->action = mouse_normal;

    place_frame (&m1->f, &m1->f, mouse_normal_00, p,
                 m1->f.dir == LEFT ? +22 : +31, +48);
  }

  return m1;
}

static ALLEGRO_COLOR
v_palette (ALLEGRO_COLOR c)
{
  return c;
}

static ALLEGRO_COLOR
e_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_MOUSE_FUR_COLOR)) return E_MOUSE_FUR_COLOR;
  if (color_eq (c, V_MOUSE_SKIN_COLOR_01)) return E_MOUSE_SKIN_COLOR_01;
  if (color_eq (c, V_MOUSE_SKIN_COLOR_02)) return E_MOUSE_SKIN_COLOR_02;
  if (color_eq (c, V_MOUSE_SKIN_COLOR_03)) return E_MOUSE_SKIN_COLOR_03;
  return c;
}

static ALLEGRO_COLOR
c_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_MOUSE_FUR_COLOR)) return C_MOUSE_FUR_COLOR;
  if (color_eq (c, V_MOUSE_SKIN_COLOR_01)) return C_MOUSE_SKIN_COLOR_01;
  if (color_eq (c, V_MOUSE_SKIN_COLOR_02)) return C_MOUSE_SKIN_COLOR_02;
  if (color_eq (c, V_MOUSE_SKIN_COLOR_03)) return C_MOUSE_SKIN_COLOR_03;
  return c;
}

static palette
get_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_palette;
  case EGA: return e_palette;
  case VGA: return v_palette;
  }
  return NULL;
}

void
draw_mouse_frame (ALLEGRO_BITMAP *bitmap, struct anim *m, enum vm vm)
{
  if (m->invisible) return;
  struct frame f = m->f;
  palette pal = get_palette (vm);
  f.b = apply_palette (f.b, pal);
  if (hgc) f.b = apply_palette (f.b, hgc_palette);
  draw_frame (bitmap, &f);
}



void
init_run_frameset (void)
{
  struct frameset frameset[RUN_FRAMESET_NMEMB] =
    {{mouse_run_00,-8,+0},{mouse_run_01,-8,+0}};

  memcpy (&run_frameset, &frameset,
          RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}



void
mouse_normal (struct anim *m)
{
  m->oaction = m->action;
  m->action = mouse_normal;
  m->f.flip = (m->f.dir == LEFT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (m->oaction != mouse_normal) m->i = 0;

  if (m->i == 12) {
    m->f.dir = (m->f.dir == LEFT) ? RIGHT : LEFT;
    mouse_run (m);
    return;
  }

  m->fo.b = mouse_normal_00;
  m->fo.dx = m->fo.dy = +0;

  if (m->oaction == mouse_run) m->fo.dy = +2;

  next_frame (&m->f, &m->f, &m->fo);

  update_depressible_floor (m, -2, -5);

  m->i++;
}

void
mouse_run (struct anim *m)
{
  m->oaction = m->action;
  m->action = mouse_run;
  m->f.flip = (m->f.dir == LEFT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  m->dc = dist_collision (&m->f, false, &m->ci) + 4;
  m->df = dist_fall (&m->f, false);
  m->dl = dist_con (&m->f, _bf, pos, -4, false, LOOSE_FLOOR);

  if (is_colliding (&m->f, &m->fo, m->dc, false, &m->ci)
      && m->ci.t == DOOR)
    m->dc = PLACE_WIDTH + 1;
  if (! cutscene && (m->dc < 12 || m->df < 12 || m->dl < 12)) {
    mouse_normal (m);
    return;
  }

  if (m->oaction != mouse_run) m->i = -1;

  if (m->i == 1) m->i = 0;
  else m->i++;

  select_frame (m, run_frameset, m->i);

  if (m->oaction == mouse_normal) m->fo.dy = -2;

  next_frame (&m->f, &m->f, &m->fo);

  if (m->i == 0) update_depressible_floor (m, -1, -9);
  else if (m->i == 1) update_depressible_floor (m, -2, -11);
}
