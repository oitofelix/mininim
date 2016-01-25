/*
  event.c -- event module;

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

ALLEGRO_EVENT_QUEUE *
create_event_queue (void)
{
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
  if (! queue) error (-1, 0, "%s: failed to create queue", __func__);
  return queue;
}

void
drop_all_events_from_source  (ALLEGRO_EVENT_QUEUE *queue,
                              ALLEGRO_EVENT_SOURCE *src)
{
  al_unregister_event_source (queue, src);
  al_register_event_source (queue, src);
}

void
emit_user_event (ALLEGRO_EVENT_SOURCE *src, ALLEGRO_EVENT *event)
{
  if (! al_emit_user_event (src, event, NULL))
    error (-1, 0, "%s (%p, %p): event source not registered with any queue",
            __func__, src, event);
}
