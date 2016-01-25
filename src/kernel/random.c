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

#include "mininim.h"

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
prandom_pos_uniq (struct pos *p, int i, int period, int max)
{
  struct pos np; npos (p, &np);
  return
    prandom_uniq (np.room + np.floor * PLACES + np.place + i,
                  period, max);
}

int
prandom_pos (struct pos *p, int max)
{
  int r;
  seedp (p);
  r = prandom (max);
  unseedp ();
  return r;
}

void
seedp (struct pos *p)
{
  struct pos np; npos (p, &np);
  random_seedb = random_seed;
  random_seed = np.room + np.floor * PLACES + np.place;
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

int
prandom_seq (uint32_t seed, int n, int p, int max)
{
  uint32_t orand_seed;
  int i, r0 = -1, r1 = -1;

	orand_seed = random_seed;
	random_seed = seed;
  prandom (1);

  for (i = 0; i <= n; i++) {
    if (i % p == 0) r0 = -1;
    do {
      r1 = prandom (max);
    } while (r1 == r0);
    r0 = r1;
  }

  random_seed = orand_seed;
  return r1;
}

int
prandom_seq_pos (struct pos *p, int n, int pr, int max)
{
  struct pos np; npos (p, &np);
  return prandom_seq (np.room + np.floor * PLACES + np.place,
                      n, pr, max);
}
