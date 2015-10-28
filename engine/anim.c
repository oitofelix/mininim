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

#include "kernel/event.h"
#include "kernel/timer.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "anim.h"
#include "physics.h"

/* set to true to quit animation */
bool quit_anim;
bool pause_anim = false;

void
play_anim (void (*callback) (void), unsigned int freq)
{
  quit_anim = false;

  acknowledge_resize ();
  ALLEGRO_EVENT event;
  ALLEGRO_TIMER *timer = create_timer (1.0 / freq);
  ALLEGRO_EVENT_QUEUE *event_queue = create_event_queue ();
  al_register_event_source (event_queue, al_get_display_event_source (display));
  al_register_event_source (event_queue, get_timer_event_source (video_timer));
  al_register_event_source (event_queue, get_timer_event_source (timer));
  al_start_timer (timer);

  while (! quit_anim) {
    al_wait_for_event (event_queue, &event);
    switch (event.type) {
    case ALLEGRO_EVENT_TIMER:
      if (event.timer.source == timer) {
        get_keyboard_state ();
        if (esc_key) pause_anim = true;
        if (esc_key && shift_key) pause_anim = false;
        if (! pause_anim || (pause_anim && esc_key)) (*callback) ();
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
    }
  }

  al_stop_timer (timer);
}

bool draw_anim_inv; /* invert draw_anim offset interpretation  */

void
draw_anim (struct anim *anim, ALLEGRO_BITMAP *frame,
             int dx, int dy)
{
  int ow = al_get_bitmap_width (anim->frame);
  int oh = al_get_bitmap_height (anim->frame);
  int w = al_get_bitmap_width (frame);
  int h = al_get_bitmap_height (frame);
  int x = anim->x;
  int y = anim->y;
  struct anim new_anim;

  if (draw_anim_inv) x += (anim->dir == LEFT) ? ow - w - dx : dx;
  else x += (anim->dir == LEFT) ? dx : ow - w - dx;
  y += oh - h + dy;

  new_anim.room = anim->room;
  new_anim.frame = frame;
  new_anim.dir = anim->dir;
  new_anim.x = x;
  new_anim.y = y;
  new_anim.collision = anim->collision;

  if (is_colliding (new_anim)) {
    anim->odraw = anim->draw;
    anim->draw = anim->collision;

    do {
      if (anim->dir == LEFT) new_anim.x++;
      else if (anim->dir == RIGHT) new_anim.x--;
      else if (anim->dir == TOP) new_anim.y++;
      else if (anim->dir == BOTTOM) new_anim.y--;
      x = new_anim.x;
      y = new_anim.y;
    } while (is_colliding (new_anim));
  }

  draw_bitmap (frame, screen, x, y, anim->flip);
  anim->frame = frame;
  anim->x = x;
  anim->y = y;
}

bool
wait_anim (unsigned int cycles)
{
  static unsigned int i = 0;
  if (i == 0) i = cycles + 1;
  return --i;
}
