/*
  torch.h -- torch module;

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

#ifndef MININIM_TORCH_H
#define MININIM_TORCH_H

/* dungeon cga */
#define DC_TORCH "data/torch/dc.png"

/* palace cga */
#define PC_TORCH "data/torch/pc.png"

/* dungeon ega */
#define DE_TORCH "data/torch/de.png"

/* palace ega */
#define PE_TORCH "data/torch/pe.png"

/* dungeon vga */
#define DV_TORCH "data/torch/dv.png"

/* palace vga */
#define PV_TORCH "data/torch/pv.png"

void load_torch (void);
void unload_torch (void);
void draw_torch (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm);
struct coord *torch_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_TORCH_H */
