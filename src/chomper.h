/*
  chomper.h -- chomper module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_CHOMPER_H
#define MININIM_CHOMPER_H

/* variables */
extern struct chomper *chomper;
extern size_t chomper_nmemb;

/* functions */
struct chomper *init_chomper (struct pos *p, struct chomper *c);
void register_chomper (struct pos *p);
int compare_chompers (const void *d0, const void *d1);
struct chomper *copy_chomper (struct chomper *to,
                              struct chomper *from);
struct chomper *chomper_at_pos (struct pos *p);
void remove_chomper (struct chomper *c);
bool should_chomp (struct pos *p);
void compute_chompers (void);
void draw_chomper (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_chomper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      struct frame *f);

#endif	/* MININIM_CHOMPER_H */
