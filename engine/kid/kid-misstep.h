/*
  kid-misstep.h -- kid misstep module;

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

#ifndef MININIM_KID_MISSTEP_H
#define MININIM_KID_MISSTEP_H

#define KID_MISSTEP_FRAMESET_NMEMB 11
extern struct frameset kid_misstep_frameset[KID_MISSTEP_FRAMESET_NMEMB];

void load_kid_misstep (void);
void unload_kid_misstep (void);
void kid_misstep (struct anim *k);

#endif	/* MININIM_KID_MISSTEP_H */
