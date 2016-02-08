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

#include "mininim.h"

void *
xmalloc (size_t n)
{
  if (n == 0) return NULL;

  void *ptr = al_malloc (n);
  if (! ptr)
    error (-1, 0, "%s (%u): cannot allocate memory", __func__,
           (unsigned int) n);
  return ptr;
}

void *
xrealloc (void *ptr, size_t n)
{
  if (ptr == NULL) return xmalloc (n);
  if (n == 0) {
    al_free (ptr);
    return NULL;
  }

  void *_ptr = al_realloc (ptr, n);
  if (! _ptr)
    error (-1, 0, "%s (%p, %u): cannot reallocate memory", __func__, ptr,
           (unsigned int) n);
  return _ptr;
}

void *
xcalloc (size_t count, size_t n)
{
  if (n * count == 0) return NULL;

  void *ptr = al_calloc (count, n);
  if (! ptr)
    error (-1, 0, "%s (%u, %u): cannot allocat memory", __func__,
           (unsigned int) count,
           (unsigned int) n);
  return ptr;
}
