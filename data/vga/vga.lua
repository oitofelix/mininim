--[[
   vga.lua -- VGA video mode;

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
--]]




-- header
local P = {package_type = "VIDEO MODE", package_name = "VGA",
           package_file = mininim.debugger.src}

-- imports
local M = mininim
local mod = M.math.mod
local rshift = M.math.rshift
local uint32 = M.math.uint32
local color = M.video.color
local coord = M.video.coordinate
local rect = M.video.rectangle
local common = require "script/common"
local to_color_range = common.to_color_range
local _debug = _debug
local os = os
local unpack = unpack





-- body
setfenv (1, P)

local PLACE_WIDTH = 32
local PLACE_HEIGHT = 63
local PLACES = 10

local asset = {}
local hue = {}
local selection = {}

local load_bitmap
local apply_palettes_to_color
local apply_palettes
local draw

function load_bitmap (filename)
   return common.load_bitmap (P, filename)
end

function apply_palettes_to_color (c, p)
   local em = M.video.env_mode
   local hm = M.video.hue_mode

   if hm ~= "NONE" then c = hue[hm] (c) end

   if p == M.mouse.position then c = selection[em] (c) end

   return c
end

function apply_palettes (b, p)
   local em = M.video.env_mode
   local hm = M.video.hue_mode

   if hm ~= "NONE" then b = b.apply_palette (hue[hm]) end

   if p == M.mouse.position then
      b = b.apply_palette (selection[em]) end

   return b
end

function draw (b, c, p, x, y, w, h)
   b = apply_palettes (b, p)
   b.draw (c (p), x, y, w, h)
end


-- RNG
local random_seed = 0
local random_seedb = 0
local prandom
local seedp
local unseedp
local prandom_seq
local prandom_seq_pos

function prandom (max)
   if random_seed == 0 then random_seed = os.time () end
   random_seed = uint32 (random_seed * 214013 + 2531011);
   if random_seed == 0 then random_seed = 1 end
   local r = mod (rshift (random_seed, 16), max + 1)
   return r
end

function seedp (p)
   np = p.normal
   random_seedb = random_seed
   random_seed = mod (np.room + np.floor * PLACES + np.place, 2 ^ 32)
   if random_seed == 0 then random_seed = 1 end
end

function unseedp ()
   random_seed = random_seedb
end

function prandom_seq (seed, n, p, max)
   local orand_seed
   local r0, r1

   orand_seed = random_seed
   random_seed = seed
   prandom (1)

   for i = 0, n do
      if (mod (i, p) == 0) then r0 = nil end
      repeat r1 = prandom (max)
      until r1 ~= r0
      r0 = r1
   end

   random_seed = orand_seed
   return r1
end

function prandom_seq_pos (p, n, pr, max)
   local np = p.normal
   local r = prandom_seq (np.room + np.floor * PLACES + np.place,
                          n, pr, max)
   return r
end


-- Palettes

function hue.GREEN (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 12, c.g + 16, c.b - 29))
end

function hue.GRAY (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 13, c.g - 7, c.b - 52))
end

function hue.YELLOW (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 78, c.g + 25, c.b - 64))
end

function hue.BLUE (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 96, c.g - 80, c.b + 64))
end

function selection.DUNGEON (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

function selection.PALACE (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 32, c.g + 32, c.b + 32))
end



-- TORCH
local draw_torch
local torch_coord

function draw_torch (p)
   local em = M.video.env_mode
   draw (asset[em].torch, torch_coord, p)
end

function torch_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1), PLACE_HEIGHT * p.floor + 23)
end



-- WALL
local draw_wall, wall_left_coord, wall_right_coord

-- dungeon locals
local draw_wall_dungeon_base
local draw_wall_dungeon_left
local draw_wall_dungeon_right
local draw_wall_dungeon_top
local wall_dungeon_top_coord
local draw_dungeon_wall_base_randomization
local draw_dungeon_wall_left_randomization
local draw_left_mark
local draw_right_mark
local draw_random_block
local wall_divider_00_coord
local draw_divider_00
local wall_divider_01_coord
local draw_divider_01
local wall_divider_00, wall_divider_01
local r0, r1, r2, r3

-- palace locals
local draw_wall_palace_base
local draw_wall_palace_left
local draw_wall_palace_right
local draw_wall_palace_top
local wall_palace_top_coord
local wall_color
local wall_color_cache
local compute_wall_color
local generate_wall_colors_for_room
local cached_wall_color
local wall_brick_rect
local wall_mark_frame
local draw_wall_brick


function wall_left_coord (p)
   return coord (PLACE_WIDTH * p.place, PLACE_HEIGHT * p.floor + 3)
end

function wall_base_coord (p)
   return coord (PLACE_WIDTH * p.place, PLACE_HEIGHT * (p.floor + 1))
end

function wall_right_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 3)
end

function draw_wall (part, p, w)
   local em = M.video.env_mode
   local t
      = {DUNGEON
            = {BASE = draw_wall_dungeon_base,
               LEFT = draw_wall_dungeon_left,
               RIGHT = draw_wall_dungeon_right,
               TOP = draw_wall_dungeon_top},
         PALACE
            = {BASE = draw_wall_palace_base,
               LEFT = draw_wall_palace_left,
               RIGHT = draw_wall_palace_right,
               TOP = draw_wall_palace_top}}
   t[em][part] (p, w)
end



-- WALL DUNGEON

function draw_wall_dungeon_right (p, w)
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset.DUNGEON.wall_right, wall_right_coord, p, 0, 0, w)
   end
end

function wall_dungeon_top_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 9)
end

function draw_wall_dungeon_top (p, w)
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset.DUNGEON.wall_top, wall_dungeon_top_coord, p, 0, 0, w)
   end
end

function draw_wall_dungeon_base (p)
   local t = {SWS = asset.DUNGEON.wall_base_sws,
              SWW = asset.DUNGEON.wall_base_sww,
              WWS = asset.DUNGEON.wall_base_wws,
              WWW = asset.DUNGEON.wall_base_www}
   local b = t[p.wall_correlation]

   draw (b, wall_base_coord, p)
   draw_dungeon_wall_base_randomization (p)
end

function draw_wall_dungeon_left (p)
   local t = {SWS = asset.DUNGEON.wall_left_sws,
              SWW = asset.DUNGEON.wall_left_sww,
              WWS = asset.DUNGEON.wall_left_wws,
              WWW = asset.DUNGEON.wall_left_www}
   local b = t[p.wall_correlation]

   draw (b, wall_left_coord, p)
   draw_dungeon_wall_left_randomization (p)
end

function draw_random_block (p)
   draw (asset.DUNGEON.wall_random_block, wall_left_coord, p)
end

function wall_divider_00_coord (p)
   return coord (PLACE_WIDTH * p.place + r3,
                 PLACE_HEIGHT * p.floor + 45)
end

function draw_divider_00 (p)
   draw (wall_divider_00, wall_divider_00_coord, p)
end

function wall_divider_01_coord (p)
   return coord (PLACE_WIDTH * p.place + 8 + r1,
                 PLACE_HEIGHT * p.floor + 24)
end

function draw_divider_01 (p)
   draw (wall_divider_01, wall_divider_01_coord, p)
end

function draw_left_mark (p, r)
   local wall_mark = asset.DUNGEON.wall_mark_top_left
   local floor_offset = {58, 41, 37, 20, 16}
   local place_offset = 0

   if mod (r, 2) ~= 0 then
      wall_mark = asset.DUNGEON.wall_mark_bottom_left end
   if r > 3 then place_offset = r3 - r2 + 6
   elseif r > 1 then place_offset = r1 - r0 + 6 end

   local function wall_mark_left_coord (p)
      return coord (PLACE_WIDTH * p.place + place_offset
                       + 8 * (((r == 2) or (r == 3)) and 1 or 0),
                    PLACE_HEIGHT * p.floor + 61 - floor_offset[r + 1])
   end

   draw (wall_mark, wall_mark_left_coord, p)
end

function draw_right_mark (p, r)
   local floor_offset, wall_mark = {52, 42, 31, 21}

   if mod (r, 2) == 0 then wall_mark = asset.DUNGEON.wall_mark_top_right
   else wall_mark = asset.DUNGEON.wall_mark_bottom_right end

   local function wall_mark_right_coord (p)
      return coord (PLACE_WIDTH * p.place + 8 * ((r > 1) and 1 or 0)
                       + ((r < 2) and 24 or r1 - 3),
                    PLACE_HEIGHT * p.floor + 56 - floor_offset[r + 1])
   end

   draw (wall_mark, wall_mark_right_coord, p)
end

function draw_dungeon_wall_base_randomization (p)
   local wall_narrow_divider = asset.DUNGEON.wall_narrow_divider
   local wall_wide_divider = asset.DUNGEON.wall_wide_divider

   seedp (p)
   prandom (1)
   r0 = prandom (1)
   r1 = prandom (4)
   r2 = prandom (1)
   r3 = prandom (4)

   if r2 == 0 then wall_divider_00 = wall_wide_divider
   else wall_divider_00 = wall_narrow_divider end

   if r0 == 0 then wall_divider_01 = wall_wide_divider
   else wall_divider_01 = wall_narrow_divider end

   local wc = p.wall_correlation
   if wc == "WWW" then draw_divider_00 (p)
   elseif wc == "WWS" then draw_divider_00 (p) end

   unseedp ()
end

function draw_dungeon_wall_left_randomization (p)
   local wall_narrow_divider = asset.DUNGEON.wall_narrow_divider
   local wall_wide_divider = asset.DUNGEON.wall_wide_divider

   seedp (p)
   prandom (1)
   r0 = prandom (1)
   r1 = prandom (4)
   r2 = prandom (1)
   r3 = prandom (4)

   if r2 == 0 then wall_divider_00 = wall_wide_divider
   else wall_divider_00 = wall_narrow_divider end

   if r0 == 0 then wall_divider_01 = wall_wide_divider
   else wall_divider_01 = wall_narrow_divider end

   local wc = p.wall_correlation
   if wc == "WWW" then
      if prandom (4) == 0 then draw_random_block (p) end
      draw_divider_01 (p)
      draw_divider_00 (p)
      if prandom (4) == 0 then draw_right_mark (p, prandom (3)) end
      if prandom (4) == 0 then draw_left_mark (p, prandom (4)) end
   elseif wc == "SWS" then
      if prandom (6) == 0 then draw_left_mark (p, prandom (1)) end
   elseif wc == "SWW" then
      if prandom (4) == 0 then draw_random_block (p) end
      draw_divider_01 (p)
      if prandom (4) == 0 then draw_right_mark (p, prandom (3)) end
      if prandom (4) == 0 then draw_left_mark (p, prandom (3)) end
   elseif wc == "WWS" then
      draw_divider_01 (p)
      draw_divider_00 (p)
      if prandom (4) == 0 then draw_right_mark (p, prandom (1) + 2) end
      if prandom (4) == 0 then draw_left_mark (p, prandom (4)) end
   end

   unseedp ()
end



-- WALL PALACE
wall_color = {color (216, 168, 88),
              color (224, 164, 92),
              color (224, 168, 96),
              color (216, 160, 84),
              color (224, 164, 92),
              color (216, 164, 88),
              color (224, 168, 88),
              color (216, 168, 96)}

wall_color_cache = {}

function compute_wall_color (p, row, col)
   local np = p.normal
   local r = prandom_seq (np.room, np.floor * 4 * 11
                             + row * 11
                             + np.place + col, 11, 3)
   return wall_color[(mod (row, 2) ~= 0 and 0 or 4) + r + 1]
end

function generate_wall_colors_for_room (room)
   local orand_seed

   orand_seed = random_seed
   random_seed = room
   prandom (1)

   wall_color_cache[room] = {}

   for floor = 0, 2 do
      for row = 0, 3 do
         local bcolor
         if mod (row, 2) ~= 0 then bcolor = 0
         else bcolor = 4 end
         local ocolor
         for col = 0, 10 do
            local color
            repeat color = bcolor + prandom (3)
            until color ~= ocolor
            wall_color_cache[room][floor.."\0"..row.."\0"..col]
               = wall_color[color + 1]
            ocolor = color
         end
      end
   end

   random_seed = orand_seed
end

function cached_wall_color (p, row, col)
   np = p.normal
   if not wall_color_cache[np.room] then
      generate_wall_colors_for_room (np.room) end
   return
      wall_color_cache[np.room][np.floor.."\0"..row.."\0"..np.place + col]
end

function wall_brick_rect (p, row, col)
   local x = PLACE_WIDTH * p.place
   local y = PLACE_HEIGHT * p.floor + 3

   local t = {{x = 0, y = 0, w = PLACE_WIDTH, h = 20},
      {x = 16 * mod (col, 2), y = 20, w = 16, h = 21},
      {x = 8 * mod (col, 2), y = 41,
       w = mod (col, 2) ~= 0 and 24 or 8, h = 19},
      {x = 0, y = 60, w = PLACE_WIDTH, h = 3}}

   return rect (x + t[row + 1].x, y + t[row + 1].y,
                t[row + 1].w, t[row + 1].h)
end

function wall_mark_frame (p, i)
   local r = prandom_seq_pos (p, i, 1, 2) + 1
   local t = {
      {asset.PALACE.wall_mark[r],
       function (p) return coord (PLACE_WIDTH * (p.place + 1) - 8,
                                  PLACE_HEIGHT * p.floor + 3) end},
      {asset.PALACE.wall_mark[r + 3],
       function (p) return coord (PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 17) end},
      {asset.PALACE.wall_mark[r + 6],
       function (p) return coord (PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 38) end},
      {asset.PALACE.wall_mark[r + 9],
       function (p) return coord (PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 58) end},
      {asset.PALACE.wall_mark[r + 12],
       function (p) return coord (PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 63) end}}
   return unpack (t[i + 1])
end

function draw_wall_brick (p, row, col)
   local np = p.normal
   local wc
   wc = cached_wall_color (p, row, col)
   -- wc = compute_wall_color (p, row, col)
   wc = apply_palettes_to_color (wc, p)
   wall_brick_rect (p, row, col).draw (wc)
   local b0, c0 = wall_mark_frame (p, row)
   draw (b0, c0, p)
   local b1, c1 = wall_mark_frame (p, row + 1)
   draw (b1, c1, p)
end

function draw_wall_palace_base (p)
   draw_wall_brick (p, 3, 0)
end

function draw_wall_palace_left (p)
  draw_wall_brick (p, 0, 0)
  draw_wall_brick (p, 1, 0)
  draw_wall_brick (p, 1, 1)
  draw_wall_brick (p, 2, 0)
  draw_wall_brick (p, 2, 1)
end

function draw_wall_palace_right (p, w)
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset.PALACE.wall_right, wall_right_coord, p, 0, 0, w)
   end
end

function wall_palace_top_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10)
end

function draw_wall_palace_top (p, w)
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset.PALACE.wall_top, wall_palace_top_coord, p, 0, 0, w)
   end
end




-- VGA video mode interface
function M.video.VGA (...)
   if arg[1] == "DRAW" then
      if arg[2] == "TORCH" then draw_torch (arg[3])
      elseif arg[2] == "WALL" then draw_wall (arg[3], arg[4], arg[5])
      end
   end
end




-- Load assets
function load ()

   asset.DUNGEON = {}
   asset.PALACE = {}

   -- TORCH
   asset.DUNGEON.torch = load_bitmap ("torch/dungeon.png")
   asset.PALACE.torch = load_bitmap ("torch/palace.png")

   -- WALL
   asset.DUNGEON.wall_base_sws = load_bitmap ("wall/dungeon/base/sws.png")
   asset.DUNGEON.wall_base_sww = load_bitmap ("wall/dungeon/base/sww.png")
   asset.DUNGEON.wall_base_wws = load_bitmap ("wall/dungeon/base/wws.png")
   asset.DUNGEON.wall_base_www = load_bitmap ("wall/dungeon/base/www.png")

   asset.DUNGEON.wall_left_sws = load_bitmap ("wall/dungeon/left/sws.png")
   asset.DUNGEON.wall_left_sww = load_bitmap ("wall/dungeon/left/sww.png")
   asset.DUNGEON.wall_left_wws = load_bitmap ("wall/dungeon/left/wws.png")
   asset.DUNGEON.wall_left_www = load_bitmap ("wall/dungeon/left/www.png")

   asset.DUNGEON.wall_narrow_divider =
      load_bitmap ("wall/dungeon/narrow-divider.png")
   asset.DUNGEON.wall_wide_divider =
      load_bitmap ("wall/dungeon/wide-divider.png")
   asset.DUNGEON.wall_random_block =
      load_bitmap ("wall/dungeon/random-block.png")
   asset.DUNGEON.wall_mark_top_right =
      load_bitmap ("wall/dungeon/mark-top-right.png")
   asset.DUNGEON.wall_mark_bottom_right =
      load_bitmap ("wall/dungeon/mark-bottom-right.png")
   asset.DUNGEON.wall_mark_top_left =
      load_bitmap ("wall/dungeon/mark-top-left.png")
   asset.DUNGEON.wall_mark_bottom_left =
      load_bitmap ("wall/dungeon/mark-bottom-left.png")

   asset.DUNGEON.wall_right =
      load_bitmap ("wall/dungeon/right.png")
   asset.DUNGEON.wall_top =
      load_bitmap ("wall/dungeon/top.png")

   asset.PALACE.wall_mark
      = {load_bitmap ("wall/palace/mark/01.png"),
         load_bitmap ("wall/palace/mark/02.png"),
         load_bitmap ("wall/palace/mark/03.png"),
         load_bitmap ("wall/palace/mark/04.png"),
         load_bitmap ("wall/palace/mark/05.png"),
         load_bitmap ("wall/palace/mark/06.png"),
         load_bitmap ("wall/palace/mark/07.png"),
         load_bitmap ("wall/palace/mark/08.png"),
         load_bitmap ("wall/palace/mark/09.png"),
         load_bitmap ("wall/palace/mark/10.png"),
         load_bitmap ("wall/palace/mark/11.png"),
         load_bitmap ("wall/palace/mark/12.png"),
         load_bitmap ("wall/palace/mark/13.png"),
         load_bitmap ("wall/palace/mark/14.png"),
         load_bitmap ("wall/palace/mark/15.png")}

   asset.PALACE.wall_right =
      load_bitmap ("wall/palace/right.png")
   asset.PALACE.wall_top =
      load_bitmap ("wall/palace/top.png")

   return P
end

-- end
return P
