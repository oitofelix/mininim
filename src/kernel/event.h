/*
  event.h -- event module;

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

#ifndef MININIM_EVENT_H
#define MININIM_EVENT_H

/* Functions */
ALLEGRO_EVENT_QUEUE *create_event_queue (void);
void drop_all_events_from_source  (ALLEGRO_EVENT_QUEUE *queue,
                                   ALLEGRO_EVENT_SOURCE *src);
void emit_user_event (ALLEGRO_EVENT_SOURCE *src, ALLEGRO_EVENT *event);

#endif	/* MININIM_EVENT_H */
