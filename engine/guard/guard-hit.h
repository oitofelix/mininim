/*
  guard-hit.h -- guard hit module;

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

#ifndef MININIM_GUARD_HIT_H
#define MININIM_GUARD_HIT_H

/* bitmaps */
#define GUARD_HIT_01 "data/guard/hit/01.png"
#define GUARD_HIT_02 "data/guard/hit/02.png"
#define GUARD_HIT_03 "data/guard/hit/03.png"
#define GUARD_HIT_04 "data/guard/hit/04.png"
#define GUARD_HIT_05 "data/guard/hit/05.png"

#define GUARD_HIT_FRAMESET_NMEMB 5
extern struct frameset guard_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];

void load_guard_hit (void);
void unload_guard_hit (void);
void guard_hit (struct anim *k);

extern ALLEGRO_BITMAP *guard_hit_01, *guard_hit_02,
  *guard_hit_03, *guard_hit_04, *guard_hit_05;

#endif	/* MININIM_GUARD_HIT_H */
