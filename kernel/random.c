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

/* random number generator seed */
uint32_t random_seed = 0;

unsigned int
prandom (unsigned int max)
{
	if (! random_seed) random_seed = time (NULL);
	random_seed = random_seed * 214013 + 2531011;
	return (random_seed >> 16) % (max + 1);
}

unsigned int
prandom_uniq (uint32_t seed, unsigned int max)
{
  uint32_t random_seed_backup = random_seed;

  random_seed = seed - 1;
  unsigned int prev_random = prandom (max);

  random_seed = seed;
  unsigned int next_random = prandom (max);

  if (prev_random == next_random)
    next_random = (next_random + 1) % (max + 1);

  random_seed = random_seed_backup;

  return next_random;
}
