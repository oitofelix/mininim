/*
  mirror.h -- mirror module;

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

#ifndef MININIM_MIRROR_H
#define MININIM_MIRROR_H

/* dungeon cga */
#define DC_MIRROR "data/mirror/dc.png"

/* palace cga */
#define PC_MIRROR "data/mirror/pc.png"

/* dungeon ega */
#define DE_MIRROR "data/mirror/de.png"

/* palace ega */
#define PE_MIRROR "data/mirror/pe.png"

/* dungeon vga */
#define DV_MIRROR "data/mirror/dv.png"

/* palace vga */
#define PV_MIRROR "data/mirror/pv.png"

void load_mirror (void);
void unload_mirror (void);
void register_mirror (struct pos *p);
int compare_mirrors (const void *o0, const void *o1);
struct mirror *mirror_at_pos (struct pos *p);
void remove_mirror (struct mirror *m);
void uncross_mirrors (void);
void draw_mirror (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm);
void draw_mirror_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     struct frame *f, enum em em, enum vm vm);
void draw_floor_reflex (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
struct coord *floor_reflex_coord (struct pos *p, struct coord *c);
struct coord *mirror_coord (struct pos *p, struct coord *c);
struct coord *mirror_reflex_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_MIRROR_H */
