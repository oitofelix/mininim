--[[
   ega.lua -- EGA video mode;

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
local P = {package_type = "VIDEO MODE", package_name = "EGA",
           package_file = mininim.debugger.src}

-- imports
local M = mininim
local mod = M.math.mod
local rshift = M.math.rshift
local uint32 = M.math.uint32
local color = M.video.color
local coord = M.video.coordinate
local common = require "script/common"
local to_color_range = common.to_color_range
local _debug = _debug
local os = os







-- body
setfenv (1, P)

local PLACE_WIDTH = 32
local PLACE_HEIGHT = 63
local PLACES = 10

local asset = {}

local load_bitmap
local selection_palette
local apply_palettes
local draw


function load_bitmap (filename)
   return common.load_bitmap (P, filename)
end

function selection_palette (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

function apply_palettes (b, p)
   if p == M.mouse.position then
      b = b.apply_palette (selection_palette) end
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




-- TORCH
local draw_torch
local torch_coord

function draw_torch (p)
   local em = M.video.env_mode, b
   draw (asset[em].torch, torch_coord, p)
end

function torch_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1), PLACE_HEIGHT * p.floor + 23)
end



-- WALL
local draw_wall, wall_left_coord, wall_right_coord

local draw_wall_base
local draw_wall_left
local draw_wall_right
local draw_wall_top
local wall_top_coord
local draw_wall_base_randomization
local draw_wall_left_randomization
local draw_left_mark
local draw_right_mark
local draw_random_block
local wall_divider_00_coord
local draw_divider_00
local wall_divider_01_coord
local draw_divider_01
local wall_divider_00, wall_divider_01
local r0, r1, r2, r3

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
   local t
      = {BASE = draw_wall_base,
         LEFT = draw_wall_left,
         RIGHT = draw_wall_right,
         TOP = draw_wall_top}
   t[part] (p, w)
end

function draw_wall_right (p, w)
   local em = M.video.env_mode
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset[em].wall_right, wall_right_coord, p, 0, 0, w)
   end
end

function wall_top_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10)
end

function draw_wall_top (p, w)
   local em = M.video.env_mode
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset[em].wall_top, wall_top_coord, p, 0, 0, w)
   end
end

function draw_wall_base (p)
   local em = M.video.env_mode
   local t = {SWS = asset[em].wall_base_sws,
              SWW = asset[em].wall_base_sww,
              WWS = asset[em].wall_base_wws,
              WWW = asset[em].wall_base_www}
   local b = t[p.wall_correlation]
   draw (b, wall_base_coord, p)
   draw_wall_base_randomization (p)
end

function draw_wall_left (p)
   local em = M.video.env_mode
   local t = {SWS = asset[em].wall_left_sws,
              SWW = asset[em].wall_left_sww,
              WWS = asset[em].wall_left_wws,
              WWW = asset[em].wall_left_www}
   local b = t[p.wall_correlation]
   draw (b, wall_left_coord, p)
   draw_wall_left_randomization (p)
end

function draw_random_block (p)
   local em = M.video.env_mode
   draw (asset[em].wall_random_block, wall_left_coord, p)
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

function draw_wall_base_randomization (p)
   local em = M.video.env_mode
   local wall_narrow_divider = asset[em].wall_narrow_divider
   local wall_wide_divider = asset[em].wall_wide_divider

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

function draw_wall_left_randomization (p)
   local em = M.video.env_mode
   local wall_narrow_divider = asset[em].wall_narrow_divider
   local wall_wide_divider = asset[em].wall_wide_divider

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
      if em == "DUNGEON" then
         if prandom (4) == 0 then draw_right_mark (p, prandom (3)) end
         if prandom (4) == 0 then draw_left_mark (p, prandom (4)) end
      end
   elseif wc == "SWS" then
      if em == "DUNGEON" and prandom (6) == 0 then
         draw_left_mark (p, prandom (1)) end
   elseif wc == "SWW" then
      if prandom (4) == 0 then draw_random_block (p) end
      draw_divider_01 (p)
      if em == "DUNGEON" then
         if prandom (4) == 0 then draw_right_mark (p, prandom (3)) end
         if prandom (4) == 0 then draw_left_mark (p, prandom (3)) end
      end
   elseif wc == "WWS" then
      draw_divider_01 (p)
      draw_divider_00 (p)
      if em == "DUNGEON" then
         if prandom (4) == 0 then draw_right_mark (p, prandom (1) + 2) end
         if prandom (4) == 0 then draw_left_mark (p, prandom (4)) end
      end
   end

   unseedp ()
end





-- EGA video mode interface
function M.video.EGA (...)
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

   -- WALL (DUNGEON)
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

   -- WALL (PALACE)
   asset.PALACE.wall_base_sws = load_bitmap ("wall/palace/base/sws.png")
   asset.PALACE.wall_base_sww = load_bitmap ("wall/palace/base/sww.png")
   asset.PALACE.wall_base_wws = load_bitmap ("wall/palace/base/wws.png")
   asset.PALACE.wall_base_www = load_bitmap ("wall/palace/base/www.png")

   asset.PALACE.wall_left_sws = load_bitmap ("wall/palace/left/sws.png")
   asset.PALACE.wall_left_sww = load_bitmap ("wall/palace/left/sww.png")
   asset.PALACE.wall_left_wws = load_bitmap ("wall/palace/left/wws.png")
   asset.PALACE.wall_left_www = load_bitmap ("wall/palace/left/www.png")

   asset.PALACE.wall_narrow_divider =
      load_bitmap ("wall/palace/narrow-divider.png")
   asset.PALACE.wall_wide_divider =
      load_bitmap ("wall/palace/wide-divider.png")
   asset.PALACE.wall_random_block =
      load_bitmap ("wall/palace/random-block.png")

   asset.PALACE.wall_right =
      load_bitmap ("wall/palace/right.png")
   asset.PALACE.wall_top =
      load_bitmap ("wall/palace/top.png")

   return P
end

-- end
return P
