/*
  threads.c -- threads module;

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

ALLEGRO_THREAD *
create_thread (thread_f thread_func, void *arg)
{
  ALLEGRO_THREAD *thread = al_create_thread (thread_func, arg);
  if (! thread)
    error (-1, 0, "%s (%p, %p): failed to create thread",
           __func__, thread_func, arg);
  return thread;
}

ALLEGRO_MUTEX *
create_mutex (void)
{
  ALLEGRO_MUTEX *mutex = al_create_mutex ();
  if (! mutex)
    error (-1, 0, "%s (void): failed to create mutex",
           __func__);
  return mutex;
}

ALLEGRO_MUTEX *
create_mutex_recursive (void)
{
  ALLEGRO_MUTEX *mutex = al_create_mutex_recursive ();
  if (! mutex)
    error (-1, 0, "%s (void): failed to create recursive mutex",
           __func__);
  return mutex;
}

ALLEGRO_COND *
create_cond (void)
{
  ALLEGRO_COND *cond = al_create_cond ();
  if (! cond)
    error (-1, 0, "%s (void): failed to create condition variable",
           __func__);
  return cond;
}
