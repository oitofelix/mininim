/*
  random.c -- random module;

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

#include <stdint.h>
#include <time.h>
#include "engine/pos.h"

/* random number generator seed */
uint32_t random_seed = 0;
static uint32_t random_seedb = 0;

int
prandom (int max)
{
	if (! random_seed) random_seed = time (NULL);
	random_seed = random_seed * 214013 + 2531011;
	return (random_seed >> 16) % (max + 1);
}

int
prandom_uniq (uint32_t seed, int period, int max)
{
  uint32_t random_seed_backup = random_seed;

  random_seed = seed - seed % period;
  int prev_random = prandom (max);

  if (seed % period) return prev_random;

  random_seed = (seed - 1) - (seed - 1) % period;
  prev_random = prandom (max);

  random_seed = seed;
  int next_random = prandom (max);

  if (prev_random == next_random)
    next_random = (next_random + 1) % (max + 1);

  random_seed = random_seed_backup;

  return next_random;
}

int
prandom_pos (struct pos p, int i, int period, int max)
{
  return
    prandom_uniq (p.room + p.floor * PLACES + p.place + i, period, max);
}

void
seedp (struct pos p)
{
  random_seedb = random_seed;
  random_seed = p.room + p.floor * PLACES + p.place;
  /* a null random seed makes the random number generator get a
     non-null seed based on the current time, but we avoid this
     non-deterministic behavior because it affects the position
     (0,0,0) odly */
  random_seed = random_seed ? random_seed : UINT32_MAX;
}

void
unseedp (void)
{
  random_seed = random_seedb;
}
