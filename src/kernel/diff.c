/*
  diff.c -- diff module;

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

void
add_diffset_diff (struct diffset *diffset, void *ptr0, void *ptr1,
                  size_t size, size_t unit_size, char *desc)
{
  if (! diffset->diff || ! diffset->count) diffset->current = -1;

  size_t i;
  for (i = diffset->current + 1; i < diffset->count; i++)
    free_diff (&diffset->diff[i]);

  diffset->count = diffset->current + 2;
  diffset->diff = xrealloc (diffset->diff, diffset->count * sizeof (* diffset->diff));
  diffset->current++;

  diff (ptr0, ptr1, size, unit_size, &diffset->diff[diffset->current], desc);

  if (! diffset->diff[diffset->current].line && ! desc) {
    diffset->count--;
    diffset->diff = xrealloc (diffset->diff, diffset->count * sizeof (* diffset->diff));
    diffset->current--;
  }
}

bool
can_apply_diffset (struct diffset *diffset, int dir)
{
  return ! (diffset->current == -1 && dir < 0)
    && ! (diffset->current == diffset->count - 1 && dir >= 0)
    && diffset->diff && diffset->count;
}

bool
apply_diffset_diff (struct diffset *diffset, void *base, size_t size,
                    int dir, char **desc)
{
  if (! can_apply_diffset (diffset, dir)) {
    desc = NULL;
    return false;
  }
  if (dir >= 0) diffset->current++;
  apply_diff (&diffset->diff[diffset->current], base, size, dir);
  *desc = diffset->diff[diffset->current].desc;
  if (dir < 0) diffset->current--;
  return true;
}

void
free_diffset (struct diffset *diffset)
{
  size_t i;
  for (i = 0; i < diffset->count; i++)
    free_diff (&diffset->diff[i]);

  diffset->count = 0;
  diffset->current = -1;
}

void
free_diff (struct diff *d)
{
  size_t i;
  for (i = 0; i < d->count; i++) {
    al_free (d->line[i].forward);
    al_free (d->line[i].backward);
  }
  al_free (d->line);
}

void
apply_diff (struct diff *d, void *base, size_t size, int dir)
{
  size_t i;
  for (i = 0; i < d->count; i++) {
    if (d->line[i].offset >= size) return;
    void *src = (dir >= 0) ? d->line[i].forward : d->line[i].backward;
    memcpy (base + d->line[i].offset, src, d->line[i].count * d->unit_size);
  }
}

struct diff *
diff (void *ptr0, void *ptr1, size_t size, size_t unit_size,
      struct diff *d, char *desc)
{
  d->line = NULL;
  d->count = 0;
  d->desc = desc;
  d->unit_size = unit_size;

  uint8_t *_ptr0 = ptr0;
  uint8_t *_ptr1 = ptr1;

  bool prev_diff = false;
  size_t i;
  for (i = 0; i < size; i += unit_size) {
    if (! memcmp (&_ptr0[i], &_ptr1[i], unit_size)) {
      prev_diff = false;
      continue;
    }

    if (! prev_diff) {
      d->count++;
      d->line = xrealloc (d->line, d->count * sizeof (* d->line));
      memset (&d->line[d->count - 1], 0, sizeof (* d->line));
      d->line[d->count - 1].offset = i;
    }

    add_diff_line_unit (&d->line[d->count - 1], &_ptr0[i], &_ptr1[i], unit_size);

    prev_diff = true;
  }

  return d;
}

void
add_diff_line_unit (struct diff_line *l, void *d0, void *d1, size_t unit_size)
{
  l->count++;
  l->forward = xrealloc (l->forward, l->count * unit_size);
  l->backward = xrealloc (l->backward, l->count * unit_size);
  uint8_t *f = l->forward;
  uint8_t *b = l->backward;
  memcpy (&f[(l->count - 1) * unit_size], d1, unit_size);
  memcpy (&b[(l->count - 1) * unit_size], d0, unit_size);
}
