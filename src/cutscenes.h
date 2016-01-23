/*
  cutscenes.h -- cutscenes module;

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

#ifndef MININIM_CUTSCENES_H
#define MININIM_CUTSCENES_H

/* cga */
#define C_MAIN_BACKGROUND "data/title/c-main-background.png"
#define C_TEXT_BACKGROUND "data/title/c-text-background.png"
#define C_PRINCESS_ROOM "data/title/c-princess-room.png"
#define C_PRINCESS_ROOM_PILLAR "data/title/c-princess-room-pillar.png"

/* ega */
#define E_MAIN_BACKGROUND "data/title/e-main-background.png"
#define E_TEXT_BACKGROUND "data/title/e-text-background.png"
#define E_PRINCESS_ROOM "data/title/e-princess-room.png"
#define E_PRINCESS_ROOM_PILLAR "data/title/e-princess-room-pillar.png"

/* vga */
#define V_MAIN_BACKGROUND "data/title/v-main-background.png"
#define V_TEXT_BACKGROUND "data/title/v-text-background.png"
#define V_PRINCESS_ROOM "data/title/v-princess-room.png"
#define V_PRINCESS_ROOM_PILLAR "data/title/v-princess-room-pillar.png"

/* bitmaps */
#define MESSAGE_PRESENTS "data/title/message-presents.png"
#define MESSAGE_AUTHOR "data/title/message-author.png"
#define MESSAGE_GAME_NAME "data/title/message-game-name.png"
#define MESSAGE_COPYRIGHT "data/title/message-copyright.png"
#define TEXT_IN_THE_ABSENCE "data/title/text-in-the-absence.png"
#define TEXT_MARRY_JAFFAR "data/title/text-marry-jaffar.png"
#define TEXT_CREDITS "data/title/text-credits.png"
#define TEXT_THE_TYRANT "data/title/text-the-tyrant.png"

/* functions */
void load_cutscenes (void);
void unload_cutscenes (void);
void play_title (void);
void cutscene_01_05_11_anim (void);
void cutscene_03_anim (void);
void cutscene_07_anim (void);
void cutscene_08_anim (void);
void cutscene_11_little_time_left_anim (void);
void cutscene_11_anim (void);
void cutscene_14_anim (void);
void cutscene_out_of_time_anim (void);

/* variables */
extern bool cutscene_started;

#endif	/* MININIM_CUTSCENES_H */
