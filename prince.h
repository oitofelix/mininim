/*
  prince.h -- MININIM main module;

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

#ifndef MININIM_PRINCE_H
#define MININIM_PRINCE_H

#include <allegro5/allegro.h>
#include "kernel/keyboard.h"
#include "kernel/audio.h"

#define ROOMS 25
#define FLOORS 3
#define PLACES 10
#define EVENTS 256
#define GUARDS 25

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 400
#define ORIGINAL_WIDTH 320
#define ORIGINAL_HEIGHT 200

#define PLACE_WIDTH (ORIGINAL_WIDTH / PLACES)
#define PLACE_HEIGHT ((ORIGINAL_HEIGHT - 11) / FLOORS)

#define KID_INITIAL_TOTAL_LIVES 3
#define KID_INITIAL_CURRENT_LIVES 3
#define KID_MINIMUM_LIVES_TO_BLINK 1
#define BOTTOM_TEXT_DURATION 3

#define EFFECT_HZ 30
#define SECS_TO_VCYCLES(x) ((x) * EFFECT_HZ)

/* colors */
#define BLACK (al_map_rgb (0, 0, 0))
#define RED (al_map_rgb (255, 0, 0))
#define GREEN (al_map_rgb (0, 255, 0))
#define BLUE (al_map_rgb (0, 0, 255))
#define YELLOW (al_map_rgb (255, 255, 0))
#define WHITE (al_map_rgb (255, 255, 255))

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

#define V_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgba (64, 64 , 64, 0))
#define V_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgba (32, 32, 32, 0))
#define V_KID_SHADOW_EYE_COLOR (al_map_rgba (184, 0, 0, 0))

#define E_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (170, 170 , 170))
#define E_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (85, 85, 85))
#define E_KID_SHADOW_SKIN_COLOR (al_map_rgb (255, 255, 255))
#define E_KID_SHADOW_HAIR_COLOR (al_map_rgb (85, 85, 85))
#define E_KID_SHADOW_EYE_COLOR (al_map_rgb (170, 0, 0))

#define C_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (85, 255, 255))
#define C_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (0, 0, 0))
#define C_KID_SHADOW_SKIN_COLOR (al_map_rgb (255, 255, 255))
#define C_KID_SHADOW_EYE_COLOR (al_map_rgb (255, 85, 255))

#define V_LIFE_POTION_BUBBLE_COLOR (al_map_rgb (224, 0, 48))
#define V_POISON_POTION_BUBBLE_COLOR (al_map_rgb (80, 84, 248))
#define V_FLOAT_POTION_BUBBLE_COLOR (al_map_rgb (84, 252, 84))
#define V_FLIP_POTION_BUBBLE_COLOR (al_map_rgb (255, 255, 0))

#define E_LIFE_POTION_BUBBLE_COLOR (al_map_rgb (255, 85, 85))
#define E_POISON_POTION_BUBBLE_COLOR (al_map_rgb (85, 85, 255))
#define E_FLOAT_POTION_BUBBLE_COLOR (al_map_rgb (85, 255, 85))
#define E_FLIP_POTION_BUBBLE_COLOR (al_map_rgb (255, 255, 85))

#define C_LIFE_POTION_BUBBLE_COLOR (al_map_rgb (255, 85, 255))
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

#define C_LIVE_COLOR (al_map_rgb (255, 85, 255))
#define E_LIVE_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_LIVE_COLOR_02 (al_map_rgb (170, 0, 0))
#define V_LIVE_COLOR_01 (al_map_rgb (228, 0, 0))
#define V_LIVE_COLOR_02 (al_map_rgb (184, 0, 0))

#define C_TEXT_LINE_COLOR (al_map_rgb (0, 0, 0))
#define V_TEXT_LINE_COLOR (al_map_rgb (0, 0, 0))

#define V_LIVES_RECT_COLOR (al_map_rgba (0, 0, 0, 170))
#define C_LIVES_RECT_COLOR (al_map_rgb (0, 0, 0))
#define E_LIVES_RECT_COLOR (al_map_rgb (0, 0, 0))

#define V_MSG_LINE_COLOR (al_map_rgba (0, 0, 0, 192))
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
#define V_STAR_COLOR_03 (al_map_rgb (248, 252, 248))

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
#define GUARD_SKIN_COLOR_01 (al_map_rgb (160, 80, 0))
#define GUARD_SKIN_COLOR_02 (al_map_rgb (160, 0, 160))
#define GUARD_EYES_COLOR (al_map_rgb (160, 0, 0))
#define GUARD_CLOSED_EYES_COLOR (al_map_rgb (80, 248, 80))
#define GUARD_HAT_COLOR_01 (al_map_rgb (0, 167, 160))
#define GUARD_HAT_COLOR_02 (al_map_rgb (255, 248, 80))
#define GUARD_BLOOD_COLOR (al_map_rgb (255, 255, 255))
#define GUARD_SPLASH_COLOR (al_map_rgb (255, 80, 255))

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

#define GUARD_LIFE_COLOR_01  (al_map_rgb (0, 0, 160))
#define GUARD_LIFE_COLOR_02  (al_map_rgb (0, 167, 0))

/* environment mode */
enum em {
  DUNGEON, PALACE,
};

/* video mode */
enum vm {
  CGA, EGA, VGA,
};

struct pos {
  int room, floor, place;
};

struct frameset {
  ALLEGRO_BITMAP *frame;
  int dx, dy;
};

struct coord {
  int room, x, y;
};

struct rect {
  struct coord c;
  int w, h;
};

struct dim {
  int x, y, w, h, fx, bx;
};

enum dir {
  LEFT, RIGHT, ABOVE, BELOW
};

enum carpet_design {
  CARPET_01,
  CARPET_02,
  ARCH_CARPET_LEFT,
  ARCH_CARPET_RIGHT_01,
  ARCH_CARPET_RIGHT_02,
};

enum anim_type {
  NO_ANIM, KID, SHADOW, GUARD, FAT_GUARD, SKELETON, VIZIER, PRINCESS, MOUSE
};

struct skill {
  int attack_prob;
  int counter_attack_prob;
  int defense_prob;
  int counter_defense_prob;
  int advance_prob;
  int return_prob;
  int refraction;
  int extra_life;
};

struct level {
  void (*start) (void);
  void (*special_events) (void);
  void (*end) (struct pos *p);
  void (*next_level) (int);
  int number;
  int nominal_number;

  struct con {
    enum confg {
      NO_FLOOR,
      FLOOR,
      BROKEN_FLOOR,
      SKELETON_FLOOR,
      LOOSE_FLOOR,
      SPIKES_FLOOR,
      OPENER_FLOOR,
      CLOSER_FLOOR,
      STUCK_FLOOR,
      HIDDEN_FLOOR,
      PILLAR,
      BIG_PILLAR_BOTTOM,
      BIG_PILLAR_TOP,
      WALL,
      DOOR,
      LEVEL_DOOR,
      CHOPPER,
      MIRROR,
      CARPET,
      TCARPET,
      ARCH_BOTTOM,
      ARCH_TOP_LEFT,
      ARCH_TOP_RIGHT,
      ARCH_TOP_MID,
      ARCH_TOP_SMALL,
    } fg;
    enum conbg {
      NO_BG,
      BRICKS_01,
      BRICKS_02,
      BRICKS_03,
      BRICKS_04,
      NO_BRICKS,
      TORCH,
      WINDOW,
      BALCONY,
    } bg;
    union conext {
      int event;
      int design;
      int step;
      bool bloody;
      bool cant_fall;
      enum item {
        NO_ITEM,
        EMPTY_POTION,
        SMALL_LIFE_POTION,
        BIG_LIFE_POTION,
        SMALL_POISON_POTION,
        BIG_POISON_POTION,
        FLOAT_POTION,
        FLIP_POTION,
        ACTIVATION_POTION,
        SWORD,
      } item;
    } ext;
  } con[ROOMS][FLOORS][PLACES];

  struct {
    int l, r, a, b;
  } link[ROOMS];

  struct level_event {
    struct pos p;
    bool next;
  } event[EVENTS];

  struct pos start_pos;
  enum dir start_dir;

  struct guard {
    enum anim_type type;
    struct pos p;
    enum dir dir;
    struct skill skill;
    int total_lives;
    int style;
  } guard[GUARDS];
};

/* avoid "'struct' declared inside parameter list" error for the
   ACTION definition */
struct anim *_action;
typedef void (*ACTION) (struct anim *a);

struct anim {
  enum anim_type type;

  int id;
  int shadow_of;

  struct frame {
    void *id;
    struct coord c;
    ALLEGRO_BITMAP *b;
    struct coord oc;
    ALLEGRO_BITMAP *ob;
    enum dir dir;
    int flip;
  } f;

  struct frame of;

  struct frame_offset {
    ALLEGRO_BITMAP *b;
    int dx, dy;
  } fo;

  struct frame_offset xf;

  struct collision_info {
    enum confg t;
    struct pos p;
  } ci;

  struct keyboard_state key;

  ACTION oaction;
  ACTION action;
  ACTION hang_caller;
  int i, j, wait, repeat, cinertia, inertia, walk,
    total_lives, current_lives;
  bool reverse, collision, fall, hit_ceiling,
    just_hanged, hang, hang_limit, misstep, uncouch_slowly,
    keep_sword_fast, turn, shadow, splash, hit_by_loose_floor,
    invisible, has_sword, hurt, controllable, fight;

  int attack_defended, counter_attacked, counter_defense;
  bool hurt_enemy_in_counter_attack;

  struct skill skill;
  int refraction;

  ALLEGRO_TIMER *floating;
  ALLEGRO_SAMPLE_INSTANCE *sample;

  int dc, df, dl, dcl, dch, dcd;

  bool immortal, poison_immune, loose_floor_immune, fall_immune,
    spikes_immune, chopper_immune, sword_immune;

  int enemy_id, oenemy_id;
  bool enemy_aware;

  bool dont_draw_lives;

  int style;

  enum confg confg;

  enum item item;

  struct pos p, item_pos, hang_pos;

  enum death_reason {
    NO_DEATH, FALL_DEATH, LOOSE_FLOOR_DEATH, POTION_DEATH,
    SPIKES_DEATH, CHOPPER_DEATH, FIGHT_DEATH
  } death_reason;

  /* depressible floor */
  struct pos df_pos[2];
  struct pos df_posb[2];
};

/* functions */
int max_int (int a, int b);
int min_int (int a, int b);

/* variables */
extern ALLEGRO_TIMER *play_time;
extern enum em em;
extern enum vm vm;
extern char *program_name;

#endif	/* MININIM_PRINCE_H */
