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
#if defined ALLEGRO_UNIX || defined ALLEGRO_LINUX
#define GNU_PORT true
#else
#define GNU_PORT false
#endif

#if defined ALLEGRO_MINGW32 || defined ALLEGRO_MSVC || defined ALLEGRO_BCC32
#define WINDOWS_PORT true
#else
#define WINDOWS_PORT false
#endif

#if defined ALLEGRO_MACOSX || defined ALLEGRO_DARWIN
#define MACOSX_PORT true
#else
#define MACOSX_PORT false
#endif

/* environ */
#if MACOSX_PORT
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

/* Non-debug build runtime checking */
#ifdef NDEBUG
#define DEBUG false
#else
#define DEBUG true
#endif

/* Menu */
#ifndef ALLEGRO_GTK_TOPLEVEL
#define ALLEGRO_GTK_TOPLEVEL 0
#endif

/* Haptics */
#if defined __al_included_allegro5_haptic_h && ! MACOSX_PORT
#define HAPTIC_FEATURE true
#else
#define HAPTIC_FEATURE false
#endif

#endif	/* MININIM_COMPATIBILITY_H */
