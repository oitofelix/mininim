/*
  title.h -- title module;

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

#ifndef MININIM_TITLE_H
#define MININIM_TITLE_H

/* bitmaps */
#define MAIN_BACKGROUND "dat/title/main titles/main background.png"
#define PRESENTS "dat/title/main titles/presents.png"
#define AUTHOR "dat/title/main titles/author.png"
#define GAME_NAME "dat/title/main titles/game name.png"
#define COPYRIGHT "dat/title/main titles/copyright.png"
#define TEXT_BACKGROUND "dat/title/main titles/text background.png"
#define IN_THE_ABSENCE "dat/title/texts/in the absence.png"
#define PRINCESS_ROOM "dat/pv/objects/room.png"
#define PRINCESS_ROOM_BED "dat/pv/objects/room bed.png"
#define PRINCESS_ROOM_PILLAR "dat/pv/objects/room pillar.png"
#define MARRY_JAFFAR "dat/title/texts/marry jaffar.png"
#define CREDITS "dat/title/texts/credits.png"

/* music */
#define MAIN_THEME "dat/midisnd2/main theme.ogg"
#define STORY_1 "dat/midisnd2/story1.ogg"
#define PRINCESS_WAITING_3 "dat/midisnd2/princess waiting 3.ogg"
#define JAFFAR_APPEARING_3 "dat/midisnd2/story2 jaffar and clock apprearing.ogg"
#define STORY_3 "dat/midisnd2/story3.ogg"
#define DOOR_OPENING "dat/digisnd2/res10051.ogg"
#define DOOR_GATE_OPENING_1 "dat/digisnd1/door gate opening 1.ogg"

/* functions */
void play_title (void);
void load_title (void);
void unload_title (void);

/* variables */
extern bool title_started;

#endif	/* MININIM_TITLE_H */
