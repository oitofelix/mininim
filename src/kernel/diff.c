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
add_diffset_diff (struct diffset *diffset, uint8_t *ptr0, uint8_t *ptr1,
                  size_t size, char *desc)
{
  if (! diffset->diff || ! diffset->count) diffset->current = -1;

  size_t i;
  for (i = diffset->current + 1; i < diffset->count; i++)
    free_diff (&diffset->diff[i]);

  diffset->count = diffset->current + 2;
  diffset->diff = xrealloc (diffset->diff, diffset->count * sizeof (* diffset->diff));
  diffset->current++;

  diff (ptr0, ptr1, size, &diffset->diff[diffset->current], desc);

  if (! diffset->diff[diffset->current].line) {
    diffset->count--;
    diffset->diff = xrealloc (diffset->diff, diffset->count * sizeof (* diffset->diff));
    diffset->current--;
  }
}

bool
apply_diffset_diff (struct diffset *diffset, uint8_t *base, size_t size,
                    int dir, char **desc)
{
  if ((diffset->current == -1 && dir < 0)
      || (diffset->current == diffset->count - 1 && dir >= 0)
      || ! diffset->diff || ! diffset->count) {
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
apply_diff (struct diff *d, uint8_t *base, size_t size, int dir)
{
  size_t i;
  for (i = 0; i < d->count; i++) {
    if (d->line[i].offset >= size) return;
    uint8_t *src = (dir >= 0) ? d->line[i].forward : d->line[i].backward;
    memcpy (base + d->line[i].offset, src, d->line[i].size);
  }
}

struct diff *
diff (uint8_t *ptr0, uint8_t *ptr1, size_t size, struct diff *d, char *desc)
{
  d->line = NULL;
  d->count = 0;
  d->desc = desc;

  bool prev_diff = false;
  size_t i;
  for (i = 0; i < size; i++) {
    if (ptr0[i] == ptr1[i]) {
      prev_diff = false;
      continue;
    }

    if (! prev_diff) {
      d->count++;
      d->line = xrealloc (d->line, d->count * sizeof (* d->line));
      memset (&d->line[d->count - 1], 0, sizeof (* d->line));
      d->line[d->count - 1].offset = i;
    }

    add_diff_line_byte (&d->line[d->count - 1], ptr0[i], ptr1[i]);

    prev_diff = true;
  }

  return d;
}

void
add_diff_line_byte (struct diff_line *l, uint8_t b0, uint8_t b1)
{
  l->size++;
  l->forward = xrealloc (l->forward, l->size * sizeof (* l->forward));
  l->backward = xrealloc (l->backward, l->size * sizeof (* l->backward));
  l->forward[l->size - 1] = b1;
  l->backward[l->size - 1] = b0;
}
