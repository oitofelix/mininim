/*
  timer.c -- timer handling routines;

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

ALLEGRO_TIMER *
create_timer (double speed_secs)
{
  ALLEGRO_TIMER *timer =  al_create_timer (speed_secs);
  if (! timer) error (-1, 0, "%s: failed to create timer (%f)", __func__, speed_secs);
  return timer;
}

ALLEGRO_EVENT_SOURCE *
get_timer_event_source (ALLEGRO_TIMER *timer)
{
  ALLEGRO_EVENT_SOURCE *event_source = al_get_timer_event_source (timer);
  if (! event_source)
    error (-1, 0, "%s: failed to get timer event source (%p)", __func__,  timer);
  return event_source;
}
