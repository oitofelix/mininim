/*
  memory.c -- memory module;

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

#include <error.h>
#include <allegro5/allegro.h>

void *
xmalloc (size_t n)
{
  void *ptr = al_malloc (n);
  if (! ptr)
    error (-1, 0, "%s (%zu): cannot allocate memory", __func__, n);
  return ptr;
}

void *
xrealloc (void *ptr, size_t n)
{
  void *_ptr = al_realloc (ptr, n);
  if (! ptr)
    error (-1, 0, "%s (%p, %zu): cannot reallocate memory", __func__, ptr, n);
  return _ptr;
}

void *
xcalloc (size_t count, size_t n)
{
  void *ptr = al_calloc (count, n);
  if (! ptr)
    error (-1, 0, "%s (%zu, %zu): cannot allocat memory", __func__, count, n);
  return ptr;
}
