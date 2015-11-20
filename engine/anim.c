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
#include "physics.h"
#include "kid.h"
#include "level.h"
#include "room.h"
#include "anim.h"

/* set to true to quit animation */
bool quit_anim;
bool pause_anim = false;
bool cutscene = false;

void
play_anim (void (*callback) (void), int freq)
{
  quit_anim = false;

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
        if (was_key_pressed (ALLEGRO_KEY_ESCAPE, false))
          pause_anim = true;
        if (was_key_pressed (ALLEGRO_KEY_P, true))
          pause_anim = false;

        /* begin kid hack */
        if (! cutscene) {
          if (a_key) kid.c.x--;
          if (d_key) kid.c.x++;
          if (w_key) kid.c.y--;
          if (s_key) kid.c.y++;
          /* int dc = dist_collision (kid); */
          /* int dbc = dist_back_collision (kid); */
          /* int df = dist_fall (kid); */
          int dn = dist_next_place (kid, coord_bf, pos, 0, false);
          int dp = dist_next_place (kid, coord_bf, pos, 0, true);
          int dc = dist_collision (kid, coord_bf, pos, 0, false);
          int df = dist_con (kid, coord_bf, pos, -4, false, NO_FLOOR);
          int dl = dist_con (kid, coord_bf, pos, -4, false, LOOSE_FLOOR);
          if (a_key || d_key || w_key || s_key || enter_key)
            printf ("\
f = %i, p = %i, dn = %i, dp = %i, dc = %i, df = %i, dl = %i\n",
                    kids.pbf.floor, kids.pbf.place, dn, dp, dc, df, dl);
            /* printf ("floor = %i, place = %i, dc = %i, dbc = %i, df = %i, dn = %i, dp = %i\n", kids.ptf.floor, kids.ptf.place, dc, dbc, df, dn, dp); */
        }
        /* end kid hack */

        if (! pause_anim
            || (pause_anim &&
                was_key_pressed (ALLEGRO_KEY_ESCAPE, true)))
          (*callback) ();
        if (! is_video_effect_started ()) show ();
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
      exit (0);
      break;
    case ALLEGRO_EVENT_KEY_DOWN:
      key = event.keyboard.keycode;
      break;
    }
  }

  al_stop_timer (timer);
}

bool draw_anim_inv; /* invert draw_anim offset interpretation  */

void
draw_anim (struct anim *a, ALLEGRO_BITMAP *frame,
             int dx, int dy)
{
  struct anim na = next_anim (*a, frame, dx, dy);
  if (! cutscene) na.c = nanim (na);
  (*a) = na;
  draw_bitmapc (a->frame, screen, a->c, a->flip);
}

void
redraw_anim (struct anim a)
{
  draw_bitmapc (a.frame, screen, a.c, a.flip);
}

struct anim
next_anim (struct anim a, ALLEGRO_BITMAP* frame, int dx, int dy)
{
  struct anim na = a;

  int ow = al_get_bitmap_width (a.frame);
  int oh = al_get_bitmap_height (a.frame);
  int w = al_get_bitmap_width (frame);
  int h = al_get_bitmap_height (frame);

  if (draw_anim_inv) na.c.x += (a.dir == LEFT) ? ow - w - dx : dx;
  else na.c.x += (a.dir == LEFT) ? dx : ow - w - dx;
  na.c.y += oh - h + dy;

  na.frame = frame;

  return na;
}

bool
wait_anim (int cycles)
{
  static int i = 0;
  if (i == 0) i = cycles + 1;
  return --i;
}
