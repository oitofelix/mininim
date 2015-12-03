/*
  kid-couch.h -- kid module;

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

#ifndef FREEPOP_KID_COUCH_H
#define FREEPOP_KID_COUCH_H

void load_kid_couch (void);
void unload_kid_couch (void);
void kid_couch (void);
void kid_couch_collision (void);

#define COUCH_FRAMESET_NMEMB 13
extern struct frameset couch_frameset[COUCH_FRAMESET_NMEMB];

extern ALLEGRO_BITMAP *kid_couch_01, *kid_couch_02, *kid_couch_03, *kid_couch_04,
  *kid_couch_05, *kid_couch_06, *kid_couch_07, *kid_couch_08,
  *kid_couch_09, *kid_couch_10, *kid_couch_11, *kid_couch_12,
  *kid_couch_13;

#endif	/* FREEPOP_KID_COUCH_H */
