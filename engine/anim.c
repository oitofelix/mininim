/*
  anim.c -- animation module;

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

#include <stdio.h>
#include "kernel/event.h"
#include "kernel/timer.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "level.h"
#include "pos.h"
#include "physics.h"
#include "level.h"
#include "room.h"
#include "kid/kid.h"
#include "anim.h"

/* set to true to quit animation */
enum quit_anim quit_anim = false;
bool pause_anim = false;
bool cutscene = false;
bool next_frame_inv = false; /* invert next_anim offset interpretation  */

void
play_anim (void (*draw_callback) (void),
           void (*sample_callback) (void),
           void (*compute_callback) (void),
           int freq)
{
  quit_anim = NO_QUIT;

  acknowledge_resize ();
  ALLEGRO_EVENT event;
  ALLEGRO_TIMER *timer = create_timer (1.0 / freq);
  ALLEGRO_EVENT_QUEUE *event_queue = create_event_queue ();
  al_register_event_source (event_queue, get_display_event_source (display));
  al_register_event_source (event_queue, get_keyboard_event_source ());
  al_register_event_source (event_queue, get_timer_event_source (video_timer));
  al_register_event_source (event_queue, get_timer_event_source (timer));
  al_start_timer (timer);

  while (! quit_anim) {
    al_wait_for_event (event_queue, &event);
    switch (event.type) {
    case ALLEGRO_EVENT_TIMER:
      if (event.timer.source == timer) {
        get_keyboard_state ();
        if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, 0, false))
          pause_anim = true;
        if (was_key_pressed (ALLEGRO_KEY_P, 0, 0, true))
          pause_anim = false;

        /* begin kid hack */
        if (! cutscene) {
          /* static int px = 0; */
          /* static int py = 0; */
          /* if (a_key) px--; */
          /* if (d_key) px++; */
          /* if (w_key) py--; */
          /* if (s_key) py++; */
          /* al_set_target_bitmap (screen); */
          /* al_put_pixel (px, py, al_map_rgb (0, 255, 255)); */

          /* printf ("x = %i, y = %i, floor = %i, place = %i\n", px, py, (py -3) / 63, (px - 15) / 32); */

          struct coord bf; struct pos pbf, npbf;
          survey (_bf, pos, &current_kid->f, &bf, &pbf, &npbf);
          if (delete_key) current_kid->f.c.x--;
          if (page_down_key) current_kid->f.c.x++;
          int dn = dist_next_place (&current_kid->f, _bf, pos, 0, false);
          int dp = dist_next_place (&current_kid->f, _bf, pos, 0, true);
          int dc = dist_collision (&current_kid->f, false, &kid->ci) + 4;
          int df = dist_fall (&current_kid->f, false);
          int dl = dist_con (&current_kid->f, _bf, pos, -4, false, LOOSE_FLOOR);
          int dcl = dist_con (&current_kid->f, _bf, pos, -4, false, CLOSER_FLOOR);
          int dch = dist_chopper (&current_kid->f, false);
          if (delete_key || page_down_key || enter_key)
            printf ("\
f = %i, p = %i, dn = %i, dp = %i, dc = %i, df = %i, dl = %i, dcl = %i, dch = %i\n",
                    pbf.floor, pbf.place, dn, dp, dc, df, dl, dcl, dch);
        }
        /* end kid hack */

        if (! is_video_effect_started ()) show ();
        if (! pause_anim
            || (pause_anim &&
                was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, 0, true))) {
          if (compute_callback) compute_callback ();
          draw_callback ();
          if (sample_callback) sample_callback ();
        }
        drop_all_events_from_source
          (event_queue, get_timer_event_source (timer));
        al_set_timer_count (timer, 0);
      } else if (event.timer.source == video_timer) {
        show ();
        drop_all_events_from_source
          (event_queue, get_timer_event_source (video_timer));
        al_set_timer_count (video_timer, 0);
      }
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      acknowledge_resize ();
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_EXPOSE:
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      quit_anim = QUIT_GAME;
      break;
    case ALLEGRO_EVENT_KEY_CHAR:
      key = event;
      if (was_key_pressed (ALLEGRO_KEY_Q, 0, ALLEGRO_KEYMOD_CTRL, true))
        quit_anim = QUIT_GAME;
      break;
    }
  }

  al_stop_timer (timer);
}

void
draw_frame (ALLEGRO_BITMAP *bitmap, struct frame *f)
{
  if (! f->b) return;

  draw_bitmapc (f->b, bitmap, &f->c, f->flip);
}

void
draw_xframe (ALLEGRO_BITMAP *bitmap, struct frame *f, struct frame_offset *xf)
{
  struct coord c;
  if (! xf->b) return;
  draw_bitmapc (xf->b, bitmap, xframe_coord (f, xf, &c), f->flip);
}

struct coord *
xframe_coord (struct frame *f, struct frame_offset *xf, struct coord *c)
{
  int w = al_get_bitmap_width (xf->b);
  _tf (f, c);
  c->x += (f->dir == LEFT) ? xf->dx : -xf->dx - w + 1;
  c->y += xf->dy;
  c->room = f->c.room;
  return c;
}

struct frame *
xframe_frame (struct frame *f, struct frame_offset *xf, struct frame *nf)
{
  *nf = *f;
  nf->b = xf->b;
  xframe_coord (f, xf, &nf->c);
  return nf;
}

void
select_frame (struct anim *a, struct frameset *fs, int i)
{
  a->fo.b = fs[i].frame;
  a->fo.dx = fs[i].dx;
  a->fo.dy = fs[i].dy;
  a->i = i;
}

void
select_xframe (struct frame_offset *xf, struct frameset *fs, int j)
{
  xf->b = fs[j].frame;
  xf->dx = fs[j].dx;
  xf->dy = fs[j].dy;
  /* a->j = j; */
}

struct frame *
next_frame (struct frame *f, struct frame *nf, struct frame_offset *fo)
{
  *nf = *f;

  int ow = al_get_bitmap_width (nf->b);
  int oh = al_get_bitmap_height (nf->b);
  int w = al_get_bitmap_width (fo->b);
  int h = al_get_bitmap_height (fo->b);

  if (next_frame_inv) nf->c.x += (nf->dir == LEFT) ? ow - w - fo->dx : fo->dx;
  else nf->c.x += (nf->dir == LEFT) ? fo->dx : ow - w - fo->dx;
  nf->c.y += oh - h + fo->dy;

  nf->b = fo->b;
  if (! cutscene) nframe (nf, &nf->c);
  return nf;
}

bool
wait_anim (int cycles)
{
  static int i = 0;
  if (i == 0) i = cycles + 1;
  return --i;
}
