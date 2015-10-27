/*
  room.h -- room module;

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

#ifndef FREEPOP_ROOM_H
#define FREEPOP_ROOM_H

#define VDUNGEON_FLOOR_NORMAL_LEFT "dat/vdungeon/floor panels/normal left.png"
#define VDUNGEON_FLOOR_NORMAL_RIGHT "dat/vdungeon/floor panels/normal right.png"
#define VDUNGEON_FLOOR_NORMAL_BASE "dat/vdungeon/floor panels/normal base.png"
#define VDUNGEON_FLOOR_BROKEN_LEFT "dat/vdungeon/floor panels/broken left.png"
#define VDUNGEON_FLOOR_BROKEN_RIGHT "dat/vdungeon/floor panels/broken right.png"
#define VDUNGEON_WALL_LEFT "dat/vdungeon/walls/left stack main.png"
#define VDUNGEON_WALL_CENTER "dat/vdungeon/walls/centre stack main.png"
#define VDUNGEON_WALL_RIGHT "dat/vdungeon/walls/right stack main.png"
#define VDUNGEON_WALL_SINGLE "dat/vdungeon/walls/single stack main.png"
#define VDUNGEON_WALL_FACE "dat/vdungeon/walls/face stack main.png"
#define VDUNGEON_WALL_FACE_TOP "dat/vdungeon/walls/face stack top.png"
#define VDUNGEON_WALL_LEFT_BASE "dat/vdungeon/walls/left stack base.png"
#define VDUNGEON_WALL_CENTER_BASE "dat/vdungeon/walls/centre stack base.png"
#define VDUNGEON_WALL_RIGHT_BASE "dat/vdungeon/walls/right stack base.png"
#define VDUNGEON_WALL_SINGLE_BASE "dat/vdungeon/walls/single stack base.png"
#define VDUNGEON_WALL_GRAY_BLOCK "dat/vdungeon/walls/random block.png"
#define VDUNGEON_WALL_WIDE_DIVIDER "dat/vdungeon/walls/divider01.png"
#define VDUNGEON_WALL_NARROW_DIVIDER "dat/vdungeon/walls/divider02.png"
#define VDUNGEON_WALL_MARK_TOP_LEFT "dat/vdungeon/walls/mark01.png"
#define VDUNGEON_WALL_MARK_TOP_RIGHT "dat/vdungeon/walls/mark03.png"
#define VDUNGEON_WALL_MARK_BOTTOM_LEFT "dat/vdungeon/walls/mark02.png"
#define VDUNGEON_WALL_MARK_BOTTOM_RIGHT "dat/vdungeon/walls/mark04.png"
#define VDUNGEON_PILLAR_LEFT "dat/vdungeon/pillar/pillar left.png"
#define VDUNGEON_PILLAR_RIGHT "dat/vdungeon/pillar/pillar right main.png"
#define VDUNGEON_PILLAR_TOP "dat/vdungeon/pillar/pillar right top.png"
#define VDUNGEON_BRICKS_1 "dat/vdungeon/background/bricks01.png"
#define VDUNGEON_TORCH "dat/vdungeon/background/torch.png"

void load_room (void);
void unload_room (void);
void draw_room (int room);

enum floor_type {
  NO_FLOOR_TYPE, NORMAL, BROKEN, LOOSE,
};

enum bg_type {
  NO_BG, BRICKS_1, TORCH,
};

enum wall_correlation {
  SWS, SWW, WWS, WWW
};

#endif	/* FREEPOP_ROOM_H */
