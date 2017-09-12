/*
  colors.h -- colors definitions;

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

#ifndef MININIM_COLORS_H
#define MININIM_COLORS_H

#define BLACK (al_map_rgb (0, 0, 0))
#define RED (al_map_rgb (255, 0, 0))
#define GREEN (al_map_rgb (0, 255, 0))
#define BLUE (al_map_rgb (0, 0, 255))
#define YELLOW (al_map_rgb (255, 255, 0))
#define MAGENTA (al_map_rgb (255, 0, 255))
#define CYAN (al_map_rgb (0, 255, 255))
#define WHITE (al_map_rgb (255, 255, 255))

#define NO_FLOOR_SELECTION_COLOR (al_map_rgb (64, 64, 64))

#define V_KID_HAIR_COLOR_01 (al_map_rgb (184, 144, 0))
#define V_KID_HAIR_COLOR_02 (al_map_rgb (120, 92, 56))
#define V_KID_SKIN_COLOR_01 (al_map_rgb (220, 128, 108))
#define V_KID_SKIN_COLOR_02 (al_map_rgb (176, 112, 96))
#define V_KID_CLOTHES_COLOR_01 (al_map_rgb (252, 252, 216))
#define V_KID_CLOTHES_COLOR_02 (al_map_rgb (216, 184, 160))
#define V_KID_EYE_COLOR (al_map_rgb (136, 84, 76))
#define V_KID_NOSE_COLOR (al_map_rgb (176, 112, 96))

#define E_KID_HAIR_COLOR (al_map_rgb (170, 0, 0))
#define E_KID_SKIN_COLOR (al_map_rgb (170, 85, 0))
#define E_KID_EYE_COLOR (al_map_rgb (0, 0, 0))
#define E_KID_CLOTHES_COLOR_01 (al_map_rgb (255, 255, 255))
#define E_KID_CLOTHES_COLOR_02 (al_map_rgb (170, 170, 170))

#define C_KID_HAIR_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_KID_HAIR_COLOR_02 (al_map_rgb (85, 255, 255))
#define C_KID_SKIN_COLOR (al_map_rgb (255, 85, 255))
#define C_KID_EYE_COLOR (al_map_rgb (0, 0, 0))
#define C_KID_CLOTHES_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_KID_CLOTHES_COLOR_02 (al_map_rgb (85, 255, 255))

#define V_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (64, 64 , 64))
#define V_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (32, 32, 32))
#define V_KID_SHADOW_EYE_COLOR (al_map_rgb (184, 0, 0))

#define E_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (170, 170 , 170))
#define E_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (85, 85, 85))
#define E_KID_SHADOW_SKIN_COLOR (al_map_rgb (255, 255, 255))
#define E_KID_SHADOW_HAIR_COLOR (al_map_rgb (85, 85, 85))
#define E_KID_SHADOW_EYE_COLOR (al_map_rgb (170, 0, 0))

#define C_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (85, 255, 255))
#define C_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (0, 0, 0))
#define C_KID_SHADOW_SKIN_COLOR (al_map_rgb (255, 255, 255))
#define C_KID_SHADOW_EYE_COLOR (al_map_rgb (255, 85, 255))

#define V_HP_POTION_BUBBLE_COLOR (al_map_rgb (224, 0, 48))
#define V_POISON_POTION_BUBBLE_COLOR (al_map_rgb (80, 84, 248))
#define V_FLOAT_POTION_BUBBLE_COLOR (al_map_rgb (84, 252, 84))
#define V_FLIP_POTION_BUBBLE_COLOR (al_map_rgb (255, 255, 0))

#define E_HP_POTION_BUBBLE_COLOR (al_map_rgb (255, 85, 85))
#define E_POISON_POTION_BUBBLE_COLOR (al_map_rgb (85, 85, 255))
#define E_FLOAT_POTION_BUBBLE_COLOR (al_map_rgb (85, 255, 85))
#define E_FLIP_POTION_BUBBLE_COLOR (al_map_rgb (255, 255, 85))

#define C_HP_POTION_BUBBLE_COLOR (al_map_rgb (255, 85, 255))
#define C_POISON_POTION_BUBBLE_COLOR (al_map_rgb (85, 255, 255))
#define C_FLOAT_POTION_BUBBLE_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_FLOAT_POTION_BUBBLE_COLOR_02 (al_map_rgb (255, 85, 255))
#define C_FLIP_POTION_BUBBLE_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_FLIP_POTION_BUBBLE_COLOR_02 (al_map_rgb (85, 255, 255))

#define H_BLOOD_COLOR (al_map_rgb (85, 85, 85))
#define C_BLOOD_COLOR (al_map_rgb (255, 85, 255))
#define E_BLOOD_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_BLOOD_COLOR_02 (al_map_rgb (170, 0, 0))
#define V_BLOOD_COLOR_01 (al_map_rgb (228, 0, 0))
#define V_BLOOD_COLOR_02 (al_map_rgb (184, 0, 0))

#define RRED_COLOR (al_map_rgb (prandom (255), 0, 0))
#define RGREEN_COLOR (al_map_rgb (0, prandom (255), 0))
#define RBLUE_COLOR (al_map_rgb (0, 0, prandom (255)))

#define TRANSPARENT_COLOR (al_map_rgba (0, 0, 0, 0))
#define TRED_COLOR (al_map_rgba (255, 0, 0, 0))
#define TGREEN_COLOR (al_map_rgba (0, 255, 0, 0))
#define TBLUE_COLOR (al_map_rgba (0, 0, 255, 0))

#define PALACE_WALL_COLOR_00 (al_map_rgb (216, 168, 88))
#define PALACE_WALL_COLOR_01 (al_map_rgb (224, 164, 92))
#define PALACE_WALL_COLOR_02 (al_map_rgb (224, 168, 96))
#define PALACE_WALL_COLOR_03 (al_map_rgb (216, 160, 84))
#define PALACE_WALL_COLOR_04 (al_map_rgb (224, 164, 92))
#define PALACE_WALL_COLOR_05 (al_map_rgb (216, 164, 88))
#define PALACE_WALL_COLOR_06 (al_map_rgb (224, 168, 88))
#define PALACE_WALL_COLOR_07 (al_map_rgb (216, 168, 96))

#define C_FIRE_COLOR_01 (al_map_rgb (255, 85, 255))
#define C_FIRE_COLOR_02 (al_map_rgb (255, 255, 255))
#define E_FIRE_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_FIRE_COLOR_02 (al_map_rgb (255, 255, 85))
#define V_FIRE_COLOR_01 (al_map_rgb (252, 132, 0))
#define V_FIRE_COLOR_02 (al_map_rgb (252, 252, 0))

#define DV_BRICKS_COLOR (al_map_rgb (48, 68, 88))
#define DE_BRICKS_COLOR (al_map_rgb (80, 80, 80))
#define DC_BRICKS_COLOR (al_map_rgb (85, 255, 255))

#define PV_BRICKS_COLOR (al_map_rgb (12, 56, 88))
#define PE_BRICKS_COLOR (al_map_rgb (0, 0, 160))
#define PC_BRICKS_COLOR (al_map_rgb (85, 255, 255))

#define C_HP_COLOR (al_map_rgb (255, 85, 255))
#define E_HP_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_HP_COLOR_02 (al_map_rgb (170, 0, 0))
#define V_HP_COLOR_01 (al_map_rgb (228, 0, 0))
#define V_HP_COLOR_02 (al_map_rgb (184, 0, 0))

#define C_TEXT_LINE_COLOR (al_map_rgb (0, 0, 0))
#define V_TEXT_LINE_COLOR (al_map_rgb (0, 0, 0))

#define V_HP_RECT_COLOR (al_map_rgb (0, 0, 0))
#define C_HP_RECT_COLOR (al_map_rgb (0, 0, 0))
#define E_HP_RECT_COLOR (al_map_rgb (0, 0, 0))

#define V_MSG_LINE_COLOR (al_map_rgb (0, 0, 0))
#define E_MSG_LINE_COLOR (al_map_rgb (0, 0, 0))
#define C_MSG_LINE_COLOR (al_map_rgb (0, 0, 0))

#define H_FLICKER_RAISE_SWORD_COLOR (al_map_rgb (170, 170, 170))
#define C_FLICKER_RAISE_SWORD_COLOR (al_map_rgb (85, 255, 255))
#define E_FLICKER_RAISE_SWORD_COLOR (al_map_rgb (255, 255, 85))
#define V_FLICKER_RAISE_SWORD_COLOR (YELLOW)

#define H_FLICKER_FLOAT_COLOR (al_map_rgb (255, 255, 255))
#define C_FLICKER_FLOAT_COLOR (al_map_rgb (255, 255, 255))
#define E_FLICKER_FLOAT_COLOR (al_map_rgb (85, 255, 85))
#define V_FLICKER_FLOAT_COLOR (al_map_rgb (84, 252, 84))

#define C_STAR_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_STAR_COLOR_02 (al_map_rgb (255, 255, 255))
#define C_STAR_COLOR_03 (al_map_rgb (255, 255, 255))
#define E_STAR_COLOR_01 (al_map_rgb (80, 84, 80))
#define E_STAR_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_STAR_COLOR_03 (al_map_rgb (248, 252, 248))
#define V_STAR_COLOR_01 (al_map_rgb (80, 84, 80))
#define V_STAR_COLOR_02 (al_map_rgb (168, 168, 168))
#define V_STAR_COLOR_03 (al_map_rgb (255, 255, 255))

#define C_MOUSE_FUR_COLOR (al_map_rgb (255, 255, 255))
#define C_MOUSE_SKIN_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_MOUSE_SKIN_COLOR_02 (al_map_rgb (85, 255, 255))
#define C_MOUSE_SKIN_COLOR_03 (al_map_rgb (85, 255, 255))
#define E_MOUSE_FUR_COLOR (al_map_rgb (248, 252, 248))
#define E_MOUSE_SKIN_COLOR_01 (al_map_rgb (248, 252, 248))
#define E_MOUSE_SKIN_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_MOUSE_SKIN_COLOR_03 (al_map_rgb (168, 0, 0))
#define V_MOUSE_FUR_COLOR (al_map_rgb (252, 252, 216))
#define V_MOUSE_SKIN_COLOR_01 (al_map_rgb (252, 200, 184))
#define V_MOUSE_SKIN_COLOR_02 (al_map_rgb (216, 184, 160))
#define V_MOUSE_SKIN_COLOR_03 (al_map_rgb (120, 92, 56))

#define GUARD_SHOES_COLOR (al_map_rgb (80, 248, 255))
#define GUARD_PANTS_COLOR_01 (al_map_rgb (0, 0, 160))
#define GUARD_PANTS_COLOR_02 (al_map_rgb (0, 167, 0))
#define GUARD_CAPE_COLOR_01 (al_map_rgb (80, 80, 255))
#define GUARD_CAPE_COLOR_02 (al_map_rgb (160, 167, 160))
#define GUARD_CAPE_COLOR_03 (al_map_rgb (80, 80, 80))
#define GUARD_BELT_COLOR (al_map_rgb (255, 80, 80))
#define GUARD_HAT_COLOR_01 (al_map_rgb (0, 167, 160))
#define GUARD_HAT_COLOR_02 (al_map_rgb (255, 248, 80))
#define GUARD_SKIN_COLOR_01 (al_map_rgb (160, 80, 0))
#define GUARD_SKIN_COLOR_02 (al_map_rgb (160, 0, 160))
#define GUARD_EYES_COLOR (al_map_rgb (160, 0, 0))
#define GUARD_CLOSED_EYES_COLOR (al_map_rgb (80, 248, 80))
#define GUARD_BLOOD_COLOR (al_map_rgb (255, 255, 255))
#define GUARD_SPLASH_COLOR (al_map_rgb (255, 80, 255))

#define V_SALMON_GUARD_SHOES_COLOR (al_map_rgb (232, 184, 112))
#define V_SALMON_GUARD_PANTS_COLOR_01 (al_map_rgb (252, 104, 120))
#define V_SALMON_GUARD_PANTS_COLOR_02 (al_map_rgb (188, 76, 124))
#define V_SALMON_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 216, 252))
#define V_SALMON_GUARD_CAPE_COLOR_02 (al_map_rgb (208, 180, 208))
#define V_SALMON_GUARD_CAPE_COLOR_03 (al_map_rgb (144, 124, 144))
#define V_SALMON_GUARD_BELT_COLOR (al_map_rgb (176, 68, 232))
#define V_SALMON_GUARD_HAT_COLOR_01 (al_map_rgb (176, 68, 232))
#define V_SALMON_GUARD_HAT_COLOR_02 (al_map_rgb (116, 44, 152))
#define V_SALMON_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_SALMON_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_SALMON_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_SALMON_GUARD_CLOSED_EYES_COLOR (al_map_rgb (160, 128, 76))
#define V_SALMON_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_SALMON_GUARD_SPLASH_COLOR (V_SALMON_GUARD_PANTS_COLOR_02)

#define V_LIGHT_BLUE_GUARD_SHOES_COLOR (al_map_rgb (232, 184, 112))
#define V_LIGHT_BLUE_GUARD_PANTS_COLOR_01 (al_map_rgb (72, 144, 252))
#define V_LIGHT_BLUE_GUARD_PANTS_COLOR_02 (al_map_rgb (104, 128, 176))
#define V_LIGHT_BLUE_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 216, 252))
#define V_LIGHT_BLUE_GUARD_CAPE_COLOR_02 (al_map_rgb (208, 180, 208))
#define V_LIGHT_BLUE_GUARD_CAPE_COLOR_03 (al_map_rgb (144, 124, 144))
#define V_LIGHT_BLUE_GUARD_BELT_COLOR (al_map_rgb (128, 40, 120))
#define V_LIGHT_BLUE_GUARD_HAT_COLOR_01 (al_map_rgb (252, 252, 252))
#define V_LIGHT_BLUE_GUARD_HAT_COLOR_02 (al_map_rgb (192, 192, 192))
#define V_LIGHT_BLUE_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_LIGHT_BLUE_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_LIGHT_BLUE_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_LIGHT_BLUE_GUARD_CLOSED_EYES_COLOR (al_map_rgb (160, 128, 72))
#define V_LIGHT_BLUE_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_LIGHT_BLUE_GUARD_SPLASH_COLOR (V_LIGHT_BLUE_GUARD_PANTS_COLOR_02)

#define V_RED_GUARD_SHOES_COLOR (al_map_rgb (168, 112, 112))
#define V_RED_GUARD_PANTS_COLOR_01 (al_map_rgb (168, 48, 0))
#define V_RED_GUARD_PANTS_COLOR_02 (al_map_rgb (128, 36, 0))
#define V_RED_GUARD_CAPE_COLOR_01 (al_map_rgb (176, 136, 252))
#define V_RED_GUARD_CAPE_COLOR_02 (al_map_rgb (132, 104, 192))
#define V_RED_GUARD_CAPE_COLOR_03 (al_map_rgb (116, 92, 168))
#define V_RED_GUARD_BELT_COLOR (al_map_rgb (184, 40, 188))
#define V_RED_GUARD_HAT_COLOR_01 (al_map_rgb (252, 120, 120))
#define V_RED_GUARD_HAT_COLOR_02 (al_map_rgb (200, 96, 96))
#define V_RED_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_RED_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_RED_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_RED_GUARD_CLOSED_EYES_COLOR (al_map_rgb (120, 80, 80))
#define V_RED_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_RED_GUARD_SPLASH_COLOR (V_RED_GUARD_PANTS_COLOR_02)

#define V_ORANGE_GUARD_SHOES_COLOR (al_map_rgb (232, 184, 112))
#define V_ORANGE_GUARD_PANTS_COLOR_01 (al_map_rgb (252, 80, 0))
#define V_ORANGE_GUARD_PANTS_COLOR_02 (al_map_rgb (188, 56, 0))
#define V_ORANGE_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 184, 0))
#define V_ORANGE_GUARD_CAPE_COLOR_02 (al_map_rgb (208, 152, 0))
#define V_ORANGE_GUARD_CAPE_COLOR_03 (al_map_rgb (168, 124, 0))
#define V_ORANGE_GUARD_BELT_COLOR (al_map_rgb (108, 40, 152))
#define V_ORANGE_GUARD_HAT_COLOR_01 (al_map_rgb (252, 252, 252))
#define V_ORANGE_GUARD_HAT_COLOR_02 (al_map_rgb (200, 200, 208))
#define V_ORANGE_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_ORANGE_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_ORANGE_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_ORANGE_GUARD_CLOSED_EYES_COLOR (al_map_rgb (160, 128, 72))
#define V_ORANGE_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_ORANGE_GUARD_SPLASH_COLOR (V_ORANGE_GUARD_PANTS_COLOR_02)

#define V_GREEN_GUARD_SHOES_COLOR (al_map_rgb (232, 184, 112))
#define V_GREEN_GUARD_PANTS_COLOR_01 (al_map_rgb (12, 144, 0))
#define V_GREEN_GUARD_PANTS_COLOR_02 (al_map_rgb (12, 112, 0))
#define V_GREEN_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 216, 96))
#define V_GREEN_GUARD_CAPE_COLOR_02 (al_map_rgb (200, 172, 76))
#define V_GREEN_GUARD_CAPE_COLOR_03 (al_map_rgb (144, 124, 56))
#define V_GREEN_GUARD_BELT_COLOR (al_map_rgb (152, 0, 32))
#define V_GREEN_GUARD_HAT_COLOR_01 (al_map_rgb (252, 208, 252))
#define V_GREEN_GUARD_HAT_COLOR_02 (al_map_rgb (176, 144, 176))
#define V_GREEN_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_GREEN_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_GREEN_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_GREEN_GUARD_CLOSED_EYES_COLOR (al_map_rgb (160, 128, 72))
#define V_GREEN_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_GREEN_GUARD_SPLASH_COLOR (V_GREEN_GUARD_PANTS_COLOR_02)

#define V_DARK_BLUE_GUARD_SHOES_COLOR (al_map_rgb (184, 136, 64))
#define V_DARK_BLUE_GUARD_PANTS_COLOR_01 (al_map_rgb (96, 0, 252))
#define V_DARK_BLUE_GUARD_PANTS_COLOR_02 (al_map_rgb (96, 0, 168))
#define V_DARK_BLUE_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 232, 176))
#define V_DARK_BLUE_GUARD_CAPE_COLOR_02 (al_map_rgb (200, 184, 140))
#define V_DARK_BLUE_GUARD_CAPE_COLOR_03 (al_map_rgb (152, 140, 108))
#define V_DARK_BLUE_GUARD_BELT_COLOR (al_map_rgb (128, 40, 120))
#define V_DARK_BLUE_GUARD_HAT_COLOR_01 (al_map_rgb (188, 60, 176))
#define V_DARK_BLUE_GUARD_HAT_COLOR_02 (al_map_rgb (136, 44, 128))
#define V_DARK_BLUE_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_DARK_BLUE_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_DARK_BLUE_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_DARK_BLUE_GUARD_CLOSED_EYES_COLOR (al_map_rgb (144, 108, 48))
#define V_DARK_BLUE_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_DARK_BLUE_GUARD_SPLASH_COLOR (V_DARK_BLUE_GUARD_PANTS_COLOR_02)

#define V_PURPLE_GUARD_SHOES_COLOR (al_map_rgb (232, 184, 112))
#define V_PURPLE_GUARD_PANTS_COLOR_01 (al_map_rgb (200, 88, 252))
#define V_PURPLE_GUARD_PANTS_COLOR_02 (al_map_rgb (152, 68, 192))
#define V_PURPLE_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 252, 216))
#define V_PURPLE_GUARD_CAPE_COLOR_02 (al_map_rgb (208, 180, 208))
#define V_PURPLE_GUARD_CAPE_COLOR_03 (al_map_rgb (176, 176, 152))
#define V_PURPLE_GUARD_BELT_COLOR (al_map_rgb (92, 108, 160))
#define V_PURPLE_GUARD_HAT_COLOR_01 (al_map_rgb (144, 168, 252))
#define V_PURPLE_GUARD_HAT_COLOR_02 (al_map_rgb (120, 140, 208))
#define V_PURPLE_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_PURPLE_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_PURPLE_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_PURPLE_GUARD_CLOSED_EYES_COLOR (al_map_rgb (160, 128, 72))
#define V_PURPLE_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_PURPLE_GUARD_SPLASH_COLOR (V_PURPLE_GUARD_PANTS_COLOR_02)

#define V_YELLOW_GUARD_SHOES_COLOR (al_map_rgb (184, 120, 252))
#define V_YELLOW_GUARD_PANTS_COLOR_01 (al_map_rgb (252, 252, 0))
#define V_YELLOW_GUARD_PANTS_COLOR_02 (al_map_rgb (200, 216, 0))
#define V_YELLOW_GUARD_CAPE_COLOR_01 (al_map_rgb (252, 192, 0))
#define V_YELLOW_GUARD_CAPE_COLOR_02 (al_map_rgb (208, 156, 0))
#define V_YELLOW_GUARD_CAPE_COLOR_03 (al_map_rgb (240, 156, 0))
#define V_YELLOW_GUARD_BELT_COLOR (al_map_rgb (96, 0, 176))
#define V_YELLOW_GUARD_HAT_COLOR_01 (al_map_rgb (176, 48, 224))
#define V_YELLOW_GUARD_HAT_COLOR_02 (al_map_rgb (132, 36, 168))
#define V_YELLOW_GUARD_SKIN_COLOR_01 (al_map_rgb (216, 136, 104))
#define V_YELLOW_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 108, 80))
#define V_YELLOW_GUARD_EYES_COLOR (al_map_rgb (120, 92, 56))
#define V_YELLOW_GUARD_CLOSED_EYES_COLOR (al_map_rgb (112, 76, 160))
#define V_YELLOW_GUARD_BLOOD_COLOR (V_BLOOD_COLOR_01)
#define V_YELLOW_GUARD_SPLASH_COLOR (V_YELLOW_GUARD_PANTS_COLOR_02)

#define E_DARK_RED_GUARD_SHOES_COLOR (al_map_rgb (168, 64, 0))
#define E_DARK_RED_GUARD_PANTS_COLOR_01 (al_map_rgb (168, 0, 0))
#define E_DARK_RED_GUARD_PANTS_COLOR_02 (al_map_rgb (168, 0, 0))
#define E_DARK_RED_GUARD_CAPE_COLOR_01 (al_map_rgb (248, 252, 248))
#define E_DARK_RED_GUARD_CAPE_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_DARK_RED_GUARD_CAPE_COLOR_03 (al_map_rgb (80, 84, 80))
#define E_DARK_RED_GUARD_BELT_COLOR (al_map_rgb (168, 0, 168))
#define E_DARK_RED_GUARD_HAT_COLOR_01 (al_map_rgb (168, 0, 168))
#define E_DARK_RED_GUARD_HAT_COLOR_02 (al_map_rgb (168, 0, 168))
#define E_DARK_RED_GUARD_SKIN_COLOR_01 (al_map_rgb (168, 84, 0))
#define E_DARK_RED_GUARD_SKIN_COLOR_02 (al_map_rgb (168, 84, 0))
#define E_DARK_RED_GUARD_EYES_COLOR (al_map_rgb (0, 0, 0))
#define E_DARK_RED_GUARD_CLOSED_EYES_COLOR (al_map_rgb (168, 84, 0))
#define E_DARK_RED_GUARD_BLOOD_COLOR (E_BLOOD_COLOR_01)
#define E_DARK_RED_GUARD_SPLASH_COLOR (E_BLOOD_COLOR_01)

#define E_RED_GUARD_SHOES_COLOR (al_map_rgb (168, 64, 0))
#define E_RED_GUARD_PANTS_COLOR_01 (al_map_rgb (248, 84, 80))
#define E_RED_GUARD_PANTS_COLOR_02 (al_map_rgb (248, 84, 80))
#define E_RED_GUARD_CAPE_COLOR_01 (al_map_rgb (248, 252, 248))
#define E_RED_GUARD_CAPE_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_RED_GUARD_CAPE_COLOR_03 (al_map_rgb (80, 84, 80))
#define E_RED_GUARD_BELT_COLOR (al_map_rgb (168, 0, 168))
#define E_RED_GUARD_HAT_COLOR_01 (al_map_rgb (248, 252, 248))
#define E_RED_GUARD_HAT_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_RED_GUARD_SKIN_COLOR_01 (al_map_rgb (168, 84, 0))
#define E_RED_GUARD_SKIN_COLOR_02 (al_map_rgb (0, 0, 0))
#define E_RED_GUARD_EYES_COLOR (al_map_rgb (0, 0, 0))
#define E_RED_GUARD_CLOSED_EYES_COLOR (al_map_rgb (168, 84, 0))
#define E_RED_GUARD_BLOOD_COLOR (E_BLOOD_COLOR_01)
#define E_RED_GUARD_SPLASH_COLOR (E_BLOOD_COLOR_01)

#define E_GREEN_GUARD_SHOES_COLOR (al_map_rgb (248, 252, 248))
#define E_GREEN_GUARD_PANTS_COLOR_01 (al_map_rgb (0, 168, 168))
#define E_GREEN_GUARD_PANTS_COLOR_02 (al_map_rgb (0, 168, 168))
#define E_GREEN_GUARD_CAPE_COLOR_01 (al_map_rgb (248, 252, 248))
#define E_GREEN_GUARD_CAPE_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_GREEN_GUARD_CAPE_COLOR_03 (al_map_rgb (80, 84, 80))
#define E_GREEN_GUARD_BELT_COLOR (al_map_rgb (168, 0, 168))
#define E_GREEN_GUARD_HAT_COLOR_01 (al_map_rgb (248, 252, 248))
#define E_GREEN_GUARD_HAT_COLOR_02 (al_map_rgb (168, 168, 168))
#define E_GREEN_GUARD_SKIN_COLOR_01 (al_map_rgb (168, 84, 0))
#define E_GREEN_GUARD_SKIN_COLOR_02 (al_map_rgb (0, 0, 0))
#define E_GREEN_GUARD_EYES_COLOR (al_map_rgb (0, 0, 0))
#define E_GREEN_GUARD_CLOSED_EYES_COLOR (al_map_rgb (168, 168, 168))
#define E_GREEN_GUARD_BLOOD_COLOR (E_BLOOD_COLOR_01)
#define E_GREEN_GUARD_SPLASH_COLOR (E_BLOOD_COLOR_01)

#define C_GREEN_GUARD_SHOES_COLOR (al_map_rgb (255, 255, 255))
#define C_GREEN_GUARD_PANTS_COLOR_01 (al_map_rgb (85, 255, 255))
#define C_GREEN_GUARD_PANTS_COLOR_02 (al_map_rgb (85, 255, 255))
#define C_GREEN_GUARD_CAPE_COLOR_01 (al_map_rgb (255, 85, 255))
#define C_GREEN_GUARD_CAPE_COLOR_02 (al_map_rgb (255, 85, 255))
#define C_GREEN_GUARD_CAPE_COLOR_03 (al_map_rgb (255, 85, 255))
#define C_GREEN_GUARD_BELT_COLOR (al_map_rgb (255, 85, 255))
#define C_GREEN_GUARD_HAT_COLOR_01 (al_map_rgb (255, 85, 255))
#define C_GREEN_GUARD_HAT_COLOR_02 (al_map_rgb (255, 85, 255))
#define C_GREEN_GUARD_SKIN_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_GREEN_GUARD_SKIN_COLOR_02 (al_map_rgb (85, 255, 255))
#define C_GREEN_GUARD_EYES_COLOR (al_map_rgb (0, 0, 0))
#define C_GREEN_GUARD_CLOSED_EYES_COLOR (al_map_rgb (85, 255, 255))
#define C_GREEN_GUARD_BLOOD_COLOR (C_BLOOD_COLOR)
#define C_GREEN_GUARD_SPLASH_COLOR (C_BLOOD_COLOR)

#define C_RED_GUARD_SHOES_COLOR (al_map_rgb (255, 255, 255))
#define C_RED_GUARD_PANTS_COLOR_01 (al_map_rgb (255, 85, 255))
#define C_RED_GUARD_PANTS_COLOR_02 (al_map_rgb (255, 85, 255))
#define C_RED_GUARD_CAPE_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_RED_GUARD_CAPE_COLOR_02 (al_map_rgb (255, 255, 255))
#define C_RED_GUARD_CAPE_COLOR_03 (al_map_rgb (85, 255, 255))
#define C_RED_GUARD_BELT_COLOR (al_map_rgb (85, 255, 255))
#define C_RED_GUARD_HAT_COLOR_01 (al_map_rgb (85, 255, 255))
#define C_RED_GUARD_HAT_COLOR_02 (al_map_rgb (255, 255, 255))
#define C_RED_GUARD_SKIN_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_RED_GUARD_SKIN_COLOR_02 (al_map_rgb (255, 85, 255))
#define C_RED_GUARD_EYES_COLOR (al_map_rgb (0, 0, 0))
#define C_RED_GUARD_CLOSED_EYES_COLOR (al_map_rgb (255, 85, 255))
#define C_RED_GUARD_BLOOD_COLOR (C_BLOOD_COLOR)
#define C_RED_GUARD_SPLASH_COLOR (C_BLOOD_COLOR)

#define HP_COLOR_01  (al_map_rgb (0, 0, 160))
#define HP_COLOR_02  (al_map_rgb (0, 167, 0))

#define PRINCESS_SKIN_COLOR_01 (al_map_rgb (160, 80, 0))
#define PRINCESS_SKIN_COLOR_02 (al_map_rgb (160, 0, 160))
#define PRINCESS_SKIN_COLOR_03 (al_map_rgb (80, 248, 255))
#define PRINCESS_DRESS_COLOR_01 (al_map_rgb (0, 0, 160))
#define PRINCESS_DRESS_COLOR_02 (al_map_rgb (0, 167, 0))
#define PRINCESS_DRESS_COLOR_03 (al_map_rgb (80, 80, 255))
#define PRINCESS_HAIR_COLOR_01 (al_map_rgb (0, 167, 160))
#define PRINCESS_HAIR_COLOR_02 (al_map_rgb (255, 248, 80))
#define PRINCESS_EYES_COLOR (al_map_rgb (160, 0, 0))

#define V_PRINCESS_SKIN_COLOR_01 (al_map_rgb (252, 156, 128))
#define V_PRINCESS_SKIN_COLOR_02 (al_map_rgb (220, 132, 112))
#define V_PRINCESS_SKIN_COLOR_03 (al_map_rgb (252, 172, 144))
#define V_PRINCESS_DRESS_COLOR_01 (al_map_rgb (252, 56, 252))
#define V_PRINCESS_DRESS_COLOR_02 (al_map_rgb (208, 48, 208))
#define V_PRINCESS_DRESS_COLOR_03 (al_map_rgb (252, 216, 252))
#define V_PRINCESS_HAIR_COLOR_01 (al_map_rgb (144, 92, 0))
#define V_PRINCESS_HAIR_COLOR_02 (al_map_rgb (112, 72, 0))
#define V_PRINCESS_EYES_COLOR (al_map_rgb (112, 64, 0))

#define E_PRINCESS_SKIN_COLOR_01 (al_map_rgb (168, 84, 0))
#define E_PRINCESS_SKIN_COLOR_02 (E_PRINCESS_SKIN_COLOR_01)
#define E_PRINCESS_SKIN_COLOR_03 (E_PRINCESS_SKIN_COLOR_01)
#define E_PRINCESS_DRESS_COLOR_01 (al_map_rgb (0, 168, 168))
#define E_PRINCESS_DRESS_COLOR_02 (E_PRINCESS_DRESS_COLOR_01)
#define E_PRINCESS_DRESS_COLOR_03 (al_map_rgb (248, 252, 248))
#define E_PRINCESS_HAIR_COLOR_01 (al_map_rgb (168, 0, 0))
#define E_PRINCESS_HAIR_COLOR_02 (E_PRINCESS_HAIR_COLOR_01)
#define E_PRINCESS_EYES_COLOR (al_map_rgb (0, 0, 0))

#define C_PRINCESS_SKIN_COLOR_01 (al_map_rgb (255, 85, 255))
#define C_PRINCESS_SKIN_COLOR_02 (C_PRINCESS_SKIN_COLOR_01)
#define C_PRINCESS_SKIN_COLOR_03 (C_PRINCESS_SKIN_COLOR_01)
#define C_PRINCESS_DRESS_COLOR_01 (al_map_rgb (85, 255, 255))
#define C_PRINCESS_DRESS_COLOR_02 (C_PRINCESS_DRESS_COLOR_01)
#define C_PRINCESS_DRESS_COLOR_03 (al_map_rgb (255, 255, 255))
#define C_PRINCESS_HAIR_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_PRINCESS_HAIR_COLOR_02 (C_PRINCESS_HAIR_COLOR_01)
#define C_PRINCESS_EYES_COLOR (al_map_rgb (0, 0, 0))

#define CLOCK_SAND_COLOR (al_map_rgb (255, 255, 255))
#define V_CLOCK_SAND_COLOR (al_map_rgb (252, 232, 168))
#define E_CLOCK_SAND_COLOR (al_map_rgb (248, 252, 248))
#define C_CLOCK_SAND_COLOR (al_map_rgb (255, 255, 255))

#define C_TITLE_BACKGROUND_PATTERN_COLOR (al_map_rgb (0, 0, 0))
#define C_ENDING_BACKGROUND_PATTERN_COLOR (al_map_rgb (0, 0, 0))

#define E_TITLE_BACKGROUND_PATTERN_COLOR (al_map_rgb (0, 0, 168))
#define E_ENDING_BACKGROUND_PATTERN_COLOR (al_map_rgb (168, 0, 0))

#define V_TITLE_BACKGROUND_PATTERN_COLOR (al_map_rgb (16, 0, 97))
#define V_ENDING_BACKGROUND_PATTERN_COLOR (al_map_rgb (128, 0, 0))

#define SKELETON_COLOR (al_map_rgb (252, 252, 240))
#define V_SKELETON_COLOR (al_map_rgb (252, 252, 240))
#define E_SKELETON_COLOR (al_map_rgb (248, 252, 248))
#define C_SKELETON_COLOR (al_map_rgb (255, 255, 255))

#define V_BOX_COLOR (al_map_rgb (255, 255, 255))
#define E_BOX_COLOR (al_map_rgb (255, 255, 255))
#define C_BOX_COLOR (al_map_rgb (255, 255, 255))

#endif	/* MININIM_COLORS_H */
