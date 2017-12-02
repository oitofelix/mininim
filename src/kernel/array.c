/*
  array.c -- array module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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
add_to_array (void *s_base, size_t s_nmemb,
              void *d_base, size_t *d_nmemb, size_t d_index, size_t size)
{
  /* Sanity checks */
  assert (s_base != NULL && s_nmemb > 0);
  assert (d_nmemb != NULL);
  if (*d_nmemb == 0) assert (d_base == NULL);
  if (d_base == NULL) assert (*d_nmemb == 0);
  if (*d_nmemb > 0) assert (d_base != NULL);
  if (d_base != NULL) assert (*d_nmemb > 0);
  if (*d_nmemb > 0 ) assert (d_index <= *d_nmemb);
  assert (size > 0);

  /* copy source array to a temporary location in order to prevent
     data corruption in case it's a subarray of the destination one */
  void *src = copy_array (s_base, s_nmemb, NULL, size);

  /* Allocate space in the destination array */
  void *dst = xrealloc (d_base, (*d_nmemb + s_nmemb) * size);

  /* Move data within the destination array in order to make room for
     the source data */
  if (d_index < *d_nmemb)
    memmove ((char *) dst + (d_index + s_nmemb) * size,
             (char *) dst + d_index * size,
             (*d_nmemb - d_index) * size);

  /* Copy source array into destination */
  memcpy ((char *) dst + d_index * size, src, s_nmemb * size);

  /* Increment destination array counter */
  *d_nmemb += s_nmemb;

  /* Return the pointer to the new array */
  return dst;
}

void *
remove_from_array (void *base, size_t *nmemb, size_t index, size_t count,
                   size_t size)
{
  /* Sanity checks */
  assert (base != NULL && *nmemb != 0);
  assert (count > 0);
  assert (index + count <= *nmemb);
  assert (size > 0);

  /* Make array contiguous */
  memmove ((char *) base + index * size,
           (char *) base + (index + count) * size,
           (*nmemb - index - count) * size);

  /* Decrement array member counter */
  *nmemb -= count;

  /* Return the pointer to the new reallocated array */
  return xrealloc (base, *nmemb * size);
}

void *
copy_array (void *s_base, size_t s_nmemb, size_t *d_nmemb, size_t size)
{
  void *d_base = xcalloc (s_nmemb, size);
  if (d_nmemb) *d_nmemb = s_nmemb;
  return memcpy (d_base, s_base, s_nmemb * size);
}

void
destroy_array (void **base, size_t *nmemb)
{
  if (! nmemb || (nmemb && *nmemb > 0)) al_free (*base);
  *base = NULL;
  if (nmemb) *nmemb = 0;
}
