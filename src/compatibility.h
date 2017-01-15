/*
  compatibility.h -- compatibility with different Allegro versions;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

#ifndef MININIM_COMPATIBILITY_H
#define MININIM_COMPATIBILITY_H

/* Platform */
#ifdef ALLEGRO_MINGW32
#define MINGW_BUILD true
#else
#define MINGW_BUILD false
#endif

/* Non-debug build runtime checking */
#ifdef NDEBUG
#define DEBUG false
#else
#define DEBUG true
#endif

/* Menu */
#ifdef ALLEGRO_START_OF_MENU
#define MENU_FEATURE true
#else
#define MENU_FEATURE false
#endif

#ifndef ALLEGRO_GTK_TOPLEVEL
#define ALLEGRO_GTK_TOPLEVEL 0
#endif

/* Haptics */
#ifdef __al_included_allegro5_haptic_h
#define HAPTIC_FEATURE true
#else
#define HAPTIC_FEATURE false
#endif

#endif	/* MININIM_COMPATIBILITY_H */
