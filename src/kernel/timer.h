/*
  timer.h -- timer handling routines;

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

#ifndef MININIM_TIMER_H
#define MININIM_TIMER_H

/* functions */
ALLEGRO_TIMER *create_timer (double speed_secs);
ALLEGRO_EVENT_SOURCE *get_timer_event_source (ALLEGRO_TIMER *timer);

#endif	/* MININIM_TIMER_H */
