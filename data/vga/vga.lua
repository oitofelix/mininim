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
           package_file = MININIM.debugger.src}

-- imports
local common = require "script/common"
local new = common.new
local to_color_range = common.to_color_range
local set_subtables_metatable = common.set_subtables_metatable

local MININIM = MININIM
local div = MININIM.math.div
local mod = MININIM.math.mod
local rshift = MININIM.math.rshift
local uint32 = MININIM.math.uint32
local color = MININIM.video.color
local TRANSPARENT = color (0, 0, 0, 0)
local coord = MININIM.video.coordinate
local rect = MININIM.video.rectangle
local pos = MININIM.level.position
local bitmap = MININIM.video.bitmap

local _debug = _debug
local os = os
local unpack = unpack
local math = math
local assert = assert
local getmetatable = getmetatable
local pairs = pairs
local ipairs = ipairs
local type = type
local print = print
local require = require
local string = string
local table = table


-- body
setfenv (1, P)

local PLACE_WIDTH = 32
local PLACE_HEIGHT = 63
local PLACES = 10

local asset = {}
local hue = {}
local selection = {}
local COORD = {}

local load_bitmap
local apply_palettes_to_color
local apply_palettes
local draw

function load_bitmap (filename, ...)
   return common.load_bitmap (P, filename, unpack (arg))
end

function apply_palettes_to_color (c, p)
   local em = MININIM.video.env_mode
   local hm = MININIM.video.hue_mode

   if hm ~= "NONE" then c = hue[hm] (c) end

   if p == MININIM.mouse.position then c = selection[em] (c) end

   return c
end

function apply_palettes (b, p)
   local em = MININIM.video.env_mode
   local hm = MININIM.video.hue_mode

   if hm ~= "NONE" then b = b.apply_palette (hue[hm]) end

   if p == MININIM.mouse.position then
      b = b.apply_palette (selection[em]) end

   return b
end

function draw (self, b, c, p, x, y, w, h)
   b = apply_palettes (b, p)
   b.draw (c (self, p), x, y, w, h)
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
local function blood_palette (c)
   if (c == color ("white")) then return color (228, 0, 0)
   else return c end
end

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

-- video
local video = new ()

-- OBJECT
video.OBJECT = new ()

function video.OBJECT:_new ()
   for k, v in pairs (self) do
      if type (v) == "table"
      and not string.find (k, "^_") then
         new (video.OBJECT, v)
      end
   end
end

function video.OBJECT:DRAW (p, width, ...)
   if type (p) == "userdata" then
      if not self.ignore then
         local b = apply_palettes (self.bitmap, p)
         b.draw (self:coord (p), 0, 0, width)
      end
   else return self[p]:DRAW (width, unpack (arg)) end
end

function video.OBJECT:COORDINATE (p, ...)
   if type (p) == "userdata" then return self:coord (p)
   else return self[p]:COORDINATE (unpack (arg)) end
end

function video.OBJECT:BITMAP (part, ...)
   if part then return self[part]:BITMAP (unpack (arg))
   elseif not self.bitmap then
      self.bitmap = self:CREATE_BITMAP () end
   local b = apply_palettes (self.bitmap)
   return b
end

function video.OBJECT:CREATE_BITMAP ()
   local w = self.LEFT.bitmap.width + self.RIGHT.bitmap.width
   local hm = math.max (self.LEFT.bitmap.height, self.RIGHT.bitmap.height)
   local h = hm + self.BASE.bitmap.height
   local b = bitmap (w, h)
   local p = pos (0, -1, 0)

   local cb = self.BASE:coord (p)
   cb.y = cb.y + hm
   self.BASE.bitmap.draw (b, cb.x, cb.y)

   local cl = self.LEFT:coord (p)
   cl.y = cl.y + hm
   self.LEFT.bitmap.draw (b, cl.x, cl.y)

   local cr = self.RIGHT:coord (p)
   cr.y = cr.y + hm
   self.RIGHT.bitmap.draw (b, cr.x, cr.y)

   return b
end

-- BRICKS (DUNGEON)
video.BRICKS_DUNGEON = new (video.OBJECT, {{}, {}, {}, {}, {}})

function video.BRICKS_DUNGEON.palette (c)
   -- just a remainder for other video modes
   if (c == color (48, 68, 88)) then return color (48, 68, 88)
   else return c end
end

video.BRICKS_DUNGEON[1].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1) + 1,
                 PLACE_HEIGHT * p.floor + 16)
end

video.BRICKS_DUNGEON[2].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1) + 2,
                 PLACE_HEIGHT * p.floor + 21)
end

video.BRICKS_DUNGEON[3].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1) + 4,
                 PLACE_HEIGHT * p.floor + 11)
end

video.BRICKS_DUNGEON[4].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1) + 4,
                 PLACE_HEIGHT * p.floor + 10)
end

video.BRICKS_DUNGEON[5].ignore = true
video.BRICKS_DUNGEON[5].coord = video.BRICKS_DUNGEON[1].coord

-- BRICKS (PALACE)
video.BRICKS_PALACE = new (video.OBJECT, {{}, {}, {}, {}, {}})

function video.BRICKS_PALACE.palette (c)
   -- just a remainder for other video modes
   if (c == color (12, 56, 88)) then return color (12, 56, 88)
   else return c end
end

video.BRICKS_PALACE[1].ignore = true
video.BRICKS_PALACE[1].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 29)
end

video.BRICKS_PALACE[2].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 26)
end

video.BRICKS_PALACE[3].coord = video.BRICKS_PALACE[1].coord
video.BRICKS_PALACE[4].coord = video.BRICKS_PALACE[2].coord
video.BRICKS_PALACE[5].coord = video.BRICKS_PALACE[1].coord

-- TORCH (DUNGEON)
video.TORCH_DUNGEON = new (video.OBJECT)

function video.TORCH_DUNGEON:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1) + 12,
                 PLACE_HEIGHT * p.floor + 23)
end

-- TORCH (PALACE)
video.TORCH_PALACE = new (video.OBJECT)

function video.TORCH_PALACE:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 23)
end

-- WINDOW (DUNGEON)
video.WINDOW_DUNGEON = new (video.OBJECT)

function video.WINDOW_DUNGEON:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1) + 6,
                 PLACE_HEIGHT * p.floor + 11)
end

-- WINDOW (PALACE)
video.WINDOW_PALACE = new (video.OBJECT)

function video.WINDOW_PALACE:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 13)
end

-- BALCONY
video.BALCONY = new (video.OBJECT)

function video.BALCONY:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 17)
end

-- FLOOR
video.FLOOR = new (video.OBJECT)

function video.FLOOR:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50)
end

-- SKELETON FLOOR
video.SKELETON_FLOOR = new (video.OBJECT)

function video.SKELETON_FLOOR:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 44)
end

-- BROKEN FLOOR
video.BROKEN_FLOOR = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BROKEN_FLOOR.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49)
end

function video.BROKEN_FLOOR.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 54)
end

-- LOOSE FLOOR
local o = new ({new (video.OBJECT), new (video.OBJECT)})
video.LOOSE_FLOOR = new (video.OBJECT, o)

video.LOOSE_FLOOR[1].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49)
end

video.LOOSE_FLOOR[2].coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49)
end


-- OPENER FLOOR
video.OPENER_FLOOR = new (video.OBJECT)

function video.OPENER_FLOOR:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49)
end

-- CLOSER FLOOR
local o = {UNPRESSED = new (video.OBJECT),
           PRESSED = new (video.OBJECT),
           PRESSED_TRAVERSABLE = new (video.OBJECT)}
video.CLOSER_FLOOR = new (video.OBJECT, o)

function video.CLOSER_FLOOR.UNPRESSED:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50)
end

function video.CLOSER_FLOOR.PRESSED:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50)
end

function video.CLOSER_FLOOR.PRESSED_TRAVERSABLE:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 51)
end

-- SPIKES FLOOR (DUNGEON)
local o = new ()
for i = 1, 6 do
   o[i] = {MAIN = {}}
   if i > 1 then o[i].FRONT = {} end
end
video.SPIKES_FLOOR_DUNGEON = new (video.OBJECT, o)

video.SPIKES_FLOOR_DUNGEON[1].MAIN.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50)
end

video.SPIKES_FLOOR_DUNGEON[2].MAIN.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50)
end

video.SPIKES_FLOOR_DUNGEON[2].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor + 56)
end

video.SPIKES_FLOOR_DUNGEON[3].MAIN.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 41)
end

video.SPIKES_FLOOR_DUNGEON[3].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 48)
end

video.SPIKES_FLOOR_DUNGEON[4].MAIN.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34)
end

video.SPIKES_FLOOR_DUNGEON[4].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 37)
end

video.SPIKES_FLOOR_DUNGEON[5].MAIN.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35)
end

video.SPIKES_FLOOR_DUNGEON[5].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 38)
end

video.SPIKES_FLOOR_DUNGEON[6].MAIN.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35)
end

video.SPIKES_FLOOR_DUNGEON[6].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 40)
end

-- SPIKES FLOOR (PALACE)
local o = new ()
o.FLOOR = new (video.FLOOR)
for i = 1, 5 do o[i] = {LEFT = {}, RIGHT = {}, FRONT = {}} end
video.SPIKES_FLOOR_PALACE = new (video.OBJECT, o)

video.SPIKES_FLOOR_PALACE[1].LEFT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49)
end

video.SPIKES_FLOOR_PALACE[1].RIGHT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 49)
end

video.SPIKES_FLOOR_PALACE[1].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 55)
end

video.SPIKES_FLOOR_PALACE[2].LEFT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 42)
end

video.SPIKES_FLOOR_PALACE[2].RIGHT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 44)
end

video.SPIKES_FLOOR_PALACE[2].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 48)
end

video.SPIKES_FLOOR_PALACE[3].LEFT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34)
end

video.SPIKES_FLOOR_PALACE[3].RIGHT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 41)
end

video.SPIKES_FLOOR_PALACE[3].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 37)
end

video.SPIKES_FLOOR_PALACE[4].LEFT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35)
end

video.SPIKES_FLOOR_PALACE[4].RIGHT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 36)
end

video.SPIKES_FLOOR_PALACE[4].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 38)
end

video.SPIKES_FLOOR_PALACE[5].LEFT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35)
end

video.SPIKES_FLOOR_PALACE[5].RIGHT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 37)
end

video.SPIKES_FLOOR_PALACE[5].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 39)
end

-- PILLAR (DUNGEON)
video.PILLAR_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.PILLAR_DUNGEON.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 4)
end

function video.PILLAR_DUNGEON.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 3)
end

-- PILLAR (PALACE)
video.PILLAR_PALACE = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.PILLAR_PALACE.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 7)
end

function video.PILLAR_PALACE.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 3)
end

-- BIG PILLAR BOTTOM
video.BIG_PILLAR_BOTTOM = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_BOTTOM.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3)
end

function video.BIG_PILLAR_BOTTOM.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 3)
end

-- BIG PILLAR TOP (DUNGEON)
video.BIG_PILLAR_TOP_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_TOP_DUNGEON.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor - 3)
end

function video.BIG_PILLAR_TOP_DUNGEON.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor + 3)
end

-- BIG PILLAR TOP (PALACE)
video.BIG_PILLAR_TOP_PALACE = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_TOP_PALACE.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 10,
                 PLACE_HEIGHT * p.floor - 6)
end

function video.BIG_PILLAR_TOP_PALACE.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 10,
                 PLACE_HEIGHT * p.floor + 3)
end

-- DOOR
video.DOOR = new (
   video.OBJECT,
   {MAIN = {}, FRONT = {}, GRID = {}})

local DOOR_STEPS = 48

function video.DOOR.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10)
end

function video.DOOR.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor + 3)
end

function video.DOOR.GRID:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 6)
end

function video.DOOR.GRID:create ()
   local parent = self._parent
   for i = 1, DOOR_STEPS do

      parent[i] = new ()
      parent[i].coord = self.coord
      parent[i].bitmap = self:create_bitmap (i)

      local j = i
      local SELF = self
      local function tip_coord (self, p)
         local c = SELF:coord (p)
         c.y = c.y + parent[j].bitmap.height - 1
         return c
      end

      parent[i].TIP = {coord = tip_coord}

      new (video.OBJECT, parent[i])
   end
end

function video.DOOR.GRID:create_bitmap (i)
   local q = div (i - 1, 8)
   local r = mod (i - 1, 8)

   local w = self.slice.width
   local b = bitmap (w, self.slice.height + self.tip.height + i - 1)

   self.slice.draw (b, 0, 0, 0, 7 - r, w, r + 1)
   for j = 0, q do self.slice.draw (b, 0, j * 8 + r + 1) end
   self.tip.draw (b, 0, (q + 1) * 8 + r + 1)

   local y
   y = 0
   for x = 0, 17 do b.set_pixel (x, y, TRANSPARENT) end
   y = 1
   for x = 0, 13 do b.set_pixel (x, y, TRANSPARENT) end
   y = 2
   for x = 0, 13 do b.set_pixel (x, y, TRANSPARENT) end
   y = 3
   for x = 0, 11 do b.set_pixel (x, y, TRANSPARENT) end
   y = 4
   for x = 0, 7 do b.set_pixel (x, y, TRANSPARENT) end
   y = 5
   for x = 0, 7 do b.set_pixel (x, y, TRANSPARENT) end
   y = 6
   for x = 0, 5 do b.set_pixel (x, y, TRANSPARENT) end
   y = 7
   for x = 0, 1 do b.set_pixel (x, y, TRANSPARENT) end

   return b
end

-- LEVEL DOOR
video.LEVEL_DOOR = new (
   video.OBJECT, {ENTER = {}, EXIT = {}, FRONT = {}, SLIDE = {}})

local LEVEL_DOOR_STEPS = 44

function video.LEVEL_DOOR.SLIDE:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor - 1)
end

function video.LEVEL_DOOR.SLIDE:create ()
   local parent = self._parent
   for i = 1, LEVEL_DOOR_STEPS do

      parent[i] = new ()
      parent[i].bitmap = self:create_bitmap (i)
      parent[i].coord = self.coord

      local j = i
      local SELF = self
      local function tip_coord  (self, p)
         local c = SELF:coord (p)
         c.y = c.y + parent[j].height - 1
         return c
      end

      parent[i].TIP = {coord = tip_coord}

      new (video.OBJECT, parent[i])
   end
end

function video.LEVEL_DOOR.SLIDE:create_bitmap (i)
   local q = div (i, 4)
   local r = mod (i, 4)

   local w = self.slice.width
   local b = bitmap (w, r + 1 + (q + 2) * 4)

   self.slice.draw (b, 0, 0, 0, 3 - r, w, r + 1)
   for j = 0, q do self.slice.draw (b, 0, j * 4 + r + 1) end

   return b
end

-- LEVEL DOOR (DUNGEON)
video.LEVEL_DOOR_DUNGEON = new (video.LEVEL_DOOR)

function video.LEVEL_DOOR_DUNGEON.ENTER:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 1,
                 PLACE_HEIGHT * p.floor - 13)
end

function video.LEVEL_DOOR_DUNGEON.EXIT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 1,
                 PLACE_HEIGHT * p.floor - 13)
end

function video.LEVEL_DOOR_DUNGEON.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 13)
end

-- LEVEL DOOR (PALACE)
video.LEVEL_DOOR_PALACE = new (video.LEVEL_DOOR)

function video.LEVEL_DOOR_PALACE.ENTER:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10)
end

function video.LEVEL_DOOR_PALACE.EXIT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10)
end

function video.LEVEL_DOOR_PALACE.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10)
end

-- CHOMPER
local o = new ()
for i = 1, 5 do
   o[i] = {MAIN = {}, BLOOD = {}}
   o[i].MAIN.coord = function (self, p)
      return coord (p.room, PLACE_WIDTH * p.place,
                    PLACE_HEIGHT * p.floor  + 3)
   end
end
video.CHOMPER = new (video.OBJECT, o)

video.CHOMPER[1].BLOOD.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 48)
end

video.CHOMPER[2].BLOOD.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 39)
end

video.CHOMPER[3].BLOOD.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 28)
end

video.CHOMPER[4].BLOOD.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 32)
end

video.CHOMPER[5].BLOOD.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 52)
end

-- ARCH BOTTOM
video.ARCH_BOTTOM = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.ARCH_BOTTOM.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor)
end

function video.ARCH_BOTTOM.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor)
end

-- ARCH TOP (abstract)
video.ARCH_TOP = new (video.OBJECT)

function video.ARCH_TOP:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3)
end

-- CARPET
local carpet = new (video.OBJECT, {MAIN = {}, MAIN_TRAVERSABLE = {},
                                   FACE = {}, FRONT = {}})

function carpet.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10)
end

function carpet.MAIN_TRAVERSABLE:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor - 10)
end

function carpet.FACE:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 7)
end

function carpet.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor + 3)
end

video.CARPET = new (video.OBJECT)
for i = 1, 6 do
   video.CARPET[i] = new (carpet)
end

video.CARPET[3].FACE.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 3)
end

video.CARPET[4].FACE.coord = video.CARPET[3].FACE.coord

video.CARPET[5].MAIN_TRAVERSABLE.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10)
end

video.CARPET[5].FRONT.coord = function (self, p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3)
end

video.CARPET[6].MAIN_TRAVERSABLE.coord =
   video.CARPET[5].MAIN_TRAVERSABLE.coord

video.CARPET[6].FRONT.coord =
   video.CARPET[5].FRONT.coord

-- MIRROR
video.MIRROR = new (video.OBJECT, {MAIN = {}, FACE = {}})

function video.MIRROR.MAIN:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3)
end

function video.MIRROR.FACE:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3)
end

-- WALL
video.WALL = new (video.OBJECT, {FACE = {}, FRONT = {}})

function video.WALL.FRONT:coord (p)
   return coord (p.room, PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3)
end

-- WALL (DUNGEON)
video.WALL_DUNGEON = new (video.WALL)

function video.WALL_DUNGEON:draw_divider_00 (p)
   local function divider_00_coord (self, p)
      return coord (p.room, PLACE_WIDTH * p.place + self.r3,
                    PLACE_HEIGHT * p.floor + 45)
   end
   draw (self, self.divider_00, divider_00_coord, p)
end

function video.WALL_DUNGEON:draw_divider_01 (p)
   local function divider_01_coord (self, p)
      return coord (p.room, PLACE_WIDTH * p.place + 8 + self.r1,
                    PLACE_HEIGHT * p.floor + 24)
   end
   draw (self, self.divider_01, divider_01_coord, p)
end

function video.WALL_DUNGEON:draw_random_block (p)
   draw (self, self.random_block, self.FRONT.coord, p)
end

function video.WALL_DUNGEON:draw_left_mark (p, r)
   local mark = self.mark_top_left
   local floor_offset = {58, 41, 37, 20, 16}
   local place_offset = 0

   if mod (r, 2) ~= 0 then
      mark = self.mark_bottom_left end
   if r > 3 then place_offset = self.r3 - self.r2 + 6
   elseif r > 1 then place_offset = self.r1 - self.r0 + 6 end

   local function mark_left_coord (self, p)
      return coord (p.room, PLACE_WIDTH * p.place + place_offset
                       + 8 * (((r == 2) or (r == 3)) and 1 or 0),
                    PLACE_HEIGHT * p.floor + 61 - floor_offset[r + 1])
   end

   draw (self, mark, mark_left_coord, p)
end

function video.WALL_DUNGEON:draw_right_mark (p, r)
   local floor_offset, mark = {52, 42, 31, 21}

   if mod (r, 2) == 0 then mark = self.mark_top_right
   else mark = self.mark_bottom_right end

   local function mark_right_coord (self, p)
      return coord (p.room, PLACE_WIDTH * p.place + 8 * ((r > 1) and 1 or 0)
                       + ((r < 2) and 24 or self.r1 - 3),
                    PLACE_HEIGHT * p.floor + 56 - floor_offset[r + 1])
   end

   draw (self, mark, mark_right_coord, p)
end

function video.WALL_DUNGEON.FRONT:draw_randomization (p)
   local parent = self._parent
   local narrow_divider = parent.narrow_divider
   local wide_divider = parent.wide_divider

   seedp (p)
   prandom (1)
   parent.r0 = prandom (1)
   parent.r1 = prandom (4)
   parent.r2 = prandom (1)
   parent.r3 = prandom (4)

   if parent.r2 == 0 then parent.divider_00 = wide_divider
   else parent.divider_00 = narrow_divider end

   if parent.r0 == 0 then parent.divider_01 = wide_divider
   else parent.divider_01 = narrow_divider end

   local wc = p.wall_correlation
   if wc == "WWW" then
      if prandom (4) == 0 then parent:draw_random_block (p) end
      parent:draw_divider_01 (p)
      parent:draw_divider_00 (p)
      if prandom (4) == 0 then parent:draw_right_mark (p, prandom (3)) end
      if prandom (4) == 0 then parent:draw_left_mark (p, prandom (4)) end
   elseif wc == "SWS" then
      if prandom (6) == 0 then parent:draw_left_mark (p, prandom (1)) end
   elseif wc == "SWW" then
      if prandom (4) == 0 then parent:draw_random_block (p) end
      parent:draw_divider_01 (p)
      if prandom (4) == 0 then parent:draw_right_mark (p, prandom (3)) end
      if prandom (4) == 0 then parent:draw_left_mark (p, prandom (3)) end
   elseif wc == "WWS" then
      parent:draw_divider_01 (p)
      parent:draw_divider_00 (p)
      if prandom (4) == 0 then parent:draw_right_mark (p, prandom (1) + 2) end
      if prandom (4) == 0 then parent:draw_left_mark (p, prandom (4)) end
   end

   unseedp ()
end

function video.WALL_DUNGEON.FRONT:DRAW (p, width)
   draw (self, self[p.wall_correlation], self.coord, p)
   self:draw_randomization (p)
end

function video.WALL_DUNGEON.FACE:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 9)
end

-- WALL (PALACE)
video.WALL_PALACE = new (video.WALL, {mark = {}})

video.WALL_PALACE.color =
   {color (216, 168, 88),
    color (224, 164, 92),
    color (224, 168, 96),
    color (216, 160, 84),
    color (224, 164, 92),
    color (216, 164, 88),
    color (224, 168, 88),
    color (216, 168, 96)}

video.WALL_PALACE.color_cache = {}

function video.WALL_PALACE:compute_color (p, row, col)
   local np = p.normal
   local r = prandom_seq (np.room, np.floor * 4 * 11
                             + row * 11
                             + np.place + col, 11, 3)
   return self.color[(mod (row, 2) ~= 0 and 0 or 4) + r + 1]
end

function video.WALL_PALACE:generate_colors_for_room (room)
   local orand_seed

   orand_seed = random_seed
   random_seed = room
   prandom (1)

   self.color_cache[room] = {}

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
            self.color_cache[room][floor.."\0"..row.."\0"..col]
               = self.color[color + 1]
            ocolor = color
         end
      end
   end

   random_seed = orand_seed
end

function video.WALL_PALACE:cached_color (p, row, col)
   np = p.normal
   if not self.color_cache[np.room] then
      self:generate_colors_for_room (np.room) end
   return
      self.color_cache[np.room][np.floor.."\0"..row.."\0"..np.place + col]
end

function video.WALL_PALACE:brick_rect (p, row, col)
   local x = PLACE_WIDTH * p.place
   local y = PLACE_HEIGHT * p.floor + 3

   local t = {{x = 0, y = 0, w = PLACE_WIDTH, h = 20},
      {x = 16 * mod (col, 2), y = 20, w = 16, h = 21},
      {x = 8 * mod (col, 2), y = 41,
       w = mod (col, 2) ~= 0 and 24 or 8, h = 19},
      {x = 0, y = 60, w = PLACE_WIDTH, h = 3}}

   return rect (p.room, x + t[row + 1].x, y + t[row + 1].y,
                t[row + 1].w, t[row + 1].h)
end

function video.WALL_PALACE:mark_frame (p, i)
   local r = prandom_seq_pos (p, i, 1, 2) + 1
   local t = {
      {self.mark[r],
       function (p) return coord (p.room, PLACE_WIDTH * (p.place + 1) - 8,
                                  PLACE_HEIGHT * p.floor + 3) end},
      {self.mark[r + 3],
       function (p) return coord (p.room, PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 17) end},
      {self.mark[r + 6],
       function (p) return coord (p.room, PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 38) end},
      {self.mark[r + 9],
       function (p) return coord (p.room, PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 58) end},
      {self.mark[r + 12],
       function (p) return coord (p.room, PLACE_WIDTH * p.place,
                                  PLACE_HEIGHT * p.floor + 63) end}}
   return unpack (t[i + 1])
end

function video.WALL_PALACE:draw_brick (p, row, col)
   local np = p.normal
   local wc
   wc = self:cached_color (p, row, col)
   -- wc = self:compute_color (p, row, col)
   wc = apply_palettes_to_color (wc, p)
   self:brick_rect (p, row, col).draw (wc)
   local b0, c0 = self:mark_frame (p, row)
   draw (self, b0, c0, p)
   local b1, c1 = self:mark_frame (p, row + 1)
   draw (self, b1, c1, p)
end

function video.WALL_PALACE.FRONT:DRAW (p, width)
   local parent = self._parent
   parent:draw_brick (p, 0, 0)
   parent:draw_brick (p, 1, 0)
   parent:draw_brick (p, 1, 1)
   parent:draw_brick (p, 2, 0)
   parent:draw_brick (p, 2, 1)
   parent:draw_brick (p, 3, 0)
end

function video.WALL_PALACE.FACE:coord (p)
   return coord (p.room, PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10)
end

-- FRAME
video.FRAME = new (video.OBJECT)

function video.FRAME:BITMAP (part, ...)
   if part then return self[part]:BITMAP (unpack (arg)) end
   return self.bitmap
end

function video.FRAME:_new (i)
   self.index = i
end

function video.FRAME:coord (a)
   local offset = self._parent.offset
   return coord (a.room, offset[self.index][1],
                 offset[self.index][2])
end

-- FRAMESET
video.FRAMESET = new (video.OBJECT)

function video.FRAMESET:_new (offset)
   if offset then
      for i = 1, table.getn (offset) do
         self[i] = new (video.FRAME, {}, i)
      end
      self.offset = offset
   end
   new (video.OBJECT, self)
end

-- KID
video.KID = new (video.OBJECT)

-- KID START RUN
video.KID.START_RUN = new (
   video.FRAMESET, {},
   {{1, 0}, {2, 0}, {5, 0}, {1, 0}, {7, 0}, {6, 0}})

video.KID.START_RUN[1].coord = function (self, a)
   if a.previous_action == "TURN" then
      return coord (a.room, 0, 0)
   elseif a.previous_action == "STABILIZE" then
      local t = {-2, -6, -4, 0}
      return coord (a.room, t[a.previous_index], 0)
   else return video.FRAME.coord (self, a) end
end

-- KID RUN
video.KID.RUN = new (
   video.FRAMESET, {},
   {{10, 0}, {7, 0}, {6, 0}, {4, 0}, {8, 0}, {11, 0}, {4, 0}, {8, 0}})

video.KID.RUN[1].coord = function (self, a)
   if a.previous_action == "START_RUN" then
      return coord (a.room, 6, 0)
   elseif a.previous_action == "RUN_JUMP" then
      return coord (a.room, 15, 0)
   else return video.FRAME.coord (self, a) end
end

-- KID STOP RUN
video.KID.STOP_RUN = new (
   video.FRAMESET, {},
   {{0, 0}, {1, 0}, {21, 0}, {2, 0}})

-- KID STABILIZE
video.KID.STABILIZE = new (
   video.FRAMESET, {},
   {{3, 0}, {4, 0}, {-2, 0}, {-4, 0}})

video.KID.STABILIZE[1].coord = function (self, a)
   if a.previous_action == "STOP_RUN" then
      return coord (a.room, 5, 0)
   else return video.FRAME.coord (self, a) end
end

-- KID TURN
video.KID.TURN = new (
   video.FRAMESET, {},
   {{-2, 0}, {-1, 0}, {3, 0}, {0, 0}})

video.KID.TURN[1].coord = function (self, a)
   if a.previous_action == "KEEP_SWORD" then
      return coord (a.room, 2, 0)
   elseif a.previous_action == "STABILIZE" then
      local t = {-6, -10, -8, -4}
      return coord (a.room, t[a.previous_index], 0)
   else return video.FRAME.coord (self, a) end
end

-- KID RUN JUMP
video.KID.RUN_JUMP = new (
   video.FRAMESET, {}, {
      {10, 0}, {11, 0}, {13, 0}, {7, 0}, {12, 0},
      {15, 0}, {29, 3}, {17, 9}, {18, 2}, {10, -11},
      {8, -3}})

-- KID TURN RUN
video.KID.TURN_RUN = new (
   video.FRAMESET, {}, {
      {18, 0}, {6, 0}, {4, 0}, {-2, 0}, {6, 0},
      {-3, 0}, {1, 0}, {0, 0}, {-4, 0}})

-- KID JUMP
video.KID.JUMP = new (
   video.FRAMESET, {}, {
      {0, 0}, {2, 0}, {3, 0}, {6, 0}, {2, 0},
      {4, 0}, {1, 0}, {12, 0}, {19, 0}, {16, 6},
      {2, -6}, {11, 0}, {-5, 0}, {13, 0}, {0, 0},
      {1, 0}, {1, 0}, {0, 0}})

video.KID.JUMP[1].coord = function (self, a)
   if a.previous_action == "STABILIZE" then
      local t = {-2, -6, -4, 0}
      return coord (a.room, t[a.previous_index], 0)
   else return video.FRAME.coord (self, a) end
end

-- KID MISSTEP
video.KID.MISSTEP = new (
   video.FRAMESET, {}, {
      {1, 0}, {1, 0}, {0, 0}, {8, 0}, {7, 0}, {4, 0},
      {-8, 0}, {-8, 0}, {-3, 0}, {0, 0}, {-4, 0}})

-- KID CROUCH
video.KID.CROUCH = new (
   video.FRAMESET, {}, {
      {3, 0}, {4, 0}, {0, 0}, {4, 0}, {1, 0}, {4, 0},
      {-1, 0}, {2, 0}, {1, 0}, {0, 0}, {-3, 0}, {0, 0},
      {-4, 0}})

video.KID.CROUCH[12].coord = function (self, a)
   if a.previous_action == "CLIMB" then
      return video.FRAME.coord (self, a) + coord (a.room, -7, 0)
   else return video.FRAME.coord (self, a) end
end

-- KID FALL
video.KID.FALL = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -5}, {0, -10}, {0, -11}, {0, -20}})

-- KID NORMAL
video.KID.NORMAL = new (
   video.FRAMESET, {}, {
      {0, 0}})

-- KID VJUMP
video.KID.VJUMP = new (
   video.FRAMESET, {}, {
      {-3, 0}, {1, 0}, {1, 0}, {-2, 0}, {0, 0},
      {1, 0}, {1, 0}, {3, 0}, {1, 0}, {6, 0},
      {0, 0}, {-2, 3}, {-3, 7}, {0, -8}, {-3, -2},
      {1, 0}, {0, 0}, {0, 0}})

-- KID WALK
video.KID.WALK = new (
   video.FRAMESET, {}, {
      {1, 0}, {1, 0}, {0, 0}, {8, 0}, {7, 0}, {6, 0},
      {-3, 0}, {2, 0}, {1, 0}, {1, 0}, {2, 0}, {0, 0}})

-- KID CLIMB
video.KID.CLIMB = new (
   video.FRAMESET, {}, {
      {-1, 0}, {0, 9}, {4, 5}, {8, 6}, {5, 4}, {2, 5},
      {1, 5}, {4, 8}, {0, 4}, {0, 1}, {3, 4}, {-1, 0},
      {0, 0}, {1, 0}, {0, 0}})

-- KID HANG
video.KID.HANG = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -2}, {-4, 0}, {-3, -2}, {-3, 0},
      {-1, 0}, {-1, 1}, {-2, 0}, {0, 3}, {0, 0},
      {-1, 1}, {0, 0}, {3, 0}, {0, 0}})

-- KID DIE
video.KID.DIE = new (
   video.FRAMESET, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, -3},
      {-2, -1}, {0, 0}, {0, 0}})

-- KID DRINK
video.KID.DRINK = new (
   video.FRAMESET, {}, {
      {7, 0}, {-1, -1}, {-1, 1}, {0, 0}, {-2, 0}, {1, 0},
      {-1, 0}, {-6, 0}, {1, 0}, {-2, 1}, {2, -1}, {0, 1},
      {1, 0}, {-1, 0}, {-1, 0}})

-- KID STAIRS
video.KID.STAIRS = new (
   video.FRAMESET, {}, {
      {0, 0}, {1, 0}, {0, 0}, {2, 0}, {-10, 3},
      {-8, 2}, {-4, 3}, {-7, 8}, {-4, 1}, {-5, 4},
      {-3, 6}, {-4, 0}})

-- KID RAISE SWORD
video.KID.RAISE_SWORD = new (
   video.FRAMESET, {}, {
      {4, 0}, {0, 0}, {-1, 0}, {1, 0}})

-- KID KEEP SWORD
video.KID.KEEP_SWORD = new (
   video.FRAMESET, {}, {
      {-2, 0}, {2, 0}, {0, 0}, {-1, 0}, {6, 0}, {-2, 0},
      {-3, 0}, {0, 0}, {-3, 0}, {0, 0}})

-- KID TAKE SWORD
video.KID.TAKE_SWORD = new (
   video.FRAMESET, {}, {
      {6, 0}, {0, 0}, {4, 0}, {6, 0}})

-- KID SWORD NORMAL
video.KID.SWORD_NORMAL = new (
   video.FRAMESET, {}, {
      {0, 0}})

-- KID SWORD WALKF
video.KID.SWORD_WALKF = new (
   video.FRAMESET, {}, {
      {14, 0}, {4, 0}})

-- KID SWORD WALKB
video.KID.SWORD_WALKB = new (
   video.FRAMESET, {}, {
      {1, 0}, {-11, 0}})

-- KID SWORD HIT
video.KID.SWORD_HIT = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {-4, 0}, {-8, 0}, {-8,0}})

-- KID SWORD DEFENSE
video.KID.SWORD_DEFENSE = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {0,0}})

-- KID SWORD ATTACK
video.KID.SWORD_ATTACK = new (
   video.FRAMESET, {}, {
      {-1, 0}, {8, 0}, {8, 0}, {9, 0}, {-8, 0},
      {-8, 0}, {-8, 0}, {8, 0}, {0, 0}})

-- SWORD
video.KID.SWORD = new (
   video.FRAMESET, {}, {
      {0, -18}, {7, -5}, {17, -4}, {18, -20}, {16, -13},
      {-9, -1}, {7, -18}, {0, -17}, {7, 4}, {7, 8},
      {11, -13}, {10, -14}, {22, -12}, {28, -10},
      {13, 5}, {1, 11}, {0, 0}, {15, -15}, {14, -4},
      {22, -10}, {7, 11}, {14, 11}, {14, 11}, {14, 5},
      {28, -3}, {28, -8}, {21, -7}, {14, -5}, {14, -2},
      {0, -12}, {12, -23}, {0, 0}, {14, -9}, {5, -12}})

-- GUARD
video.GUARD = new (video.OBJECT)

-- GUARD NORMAL
video.GUARD.NORMAL = new (
   video.FRAMESET, {}, {{0, 0}})

-- GUARD SWORD NORMAL
video.GUARD.SWORD_NORMAL = new (
   video.FRAMESET, {}, {{0, 0}, {0, 0}, {0, 0}})

-- GUARD SWORD WALKF
video.GUARD.SWORD_WALKF = new (
   video.FRAMESET, {}, {{7, 0}, {8, 0}, {1, 0}})

-- GUARD SWORD WALKB
video.GUARD.SWORD_WALKB = new (
   video.FRAMESET, {}, {{-2, 0}, {-10, 0}})

-- GUARD SWORD ATTACK
video.GUARD.SWORD_ATTACK = new (
   video.FRAMESET, {}, {
      {6, 0}, {0, 0}, {3, 0}, {6, 0}, {7, 0}, {-6, 0},
      {-9, 0}, {-7, 0}, {0, 0}, {-6, 0}, {-1, 0}})

-- GUARD SWORD DEFENSE
video.GUARD.SWORD_DEFENSE = new (
   video.FRAMESET, {}, {{0, 0}, {0, 0}})

-- GUARD SWORD HIT
video.GUARD.SWORD_HIT = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {-4, 0}, {-8, 0}, {-8, 0}})

-- GUARD FALL
video.GUARD.FALL = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -5}, {0, -10}, {0, -11}, {0, -20}})

-- GUARD DIE
video.GUARD.DIE = new (
   video.FRAMESET, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, 0},
      {2, 0}, {0, 0}, {0, 0}})

-- FAT GUARD
video.FAT_GUARD = new (video.GUARD)

-- FAT GUARD DIE
video.FAT_GUARD.DIE = new (
   video.FRAMESET, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}})

-- VIZIER
video.VIZIER = new (video.GUARD)

-- SKELETON
video.SKELETON = new (video.GUARD)

-- SKELETON DIE
video.SKELETON.DIE = new (
   video.FRAMESET, {}, {
      {8, 0}, {0, -2}, {0, -2}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}})

-- SHADOW
video.SHADOW = new (video.GUARD)

-- SHADOW DIE
video.SHADOW.DIE = new (
   video.FRAMESET, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -1}, {0, -2},
      {-2, -1}, {0, 0}, {0, 0}})

-- ASSET
local ASSET = new ()

function ASSET:_new (em)
   self.em = string.lower (em)
end

function ASSET:load ()
   self:load_bricks ()
   self:load_torch ()
   self:load_window ()
   self:load_balcony ()
   self:load_floor ()
   self:load_skeleton_floor ()
   self:load_broken_floor ()
   self:load_loose_floor ()
   self:load_opener_floor ()
   self:load_closer_floor ()
   self:load_spikes_floor ()
   self:load_pillar ()
   self:load_big_pillar_bottom ()
   self:load_big_pillar_top ()
   self:load_door ()
   self:load_level_door ()
   self:load_chomper ()
   self:load_arch_bottom ()
   self:load_arch_top_mid ()
   self:load_arch_top_small ()
   self:load_arch_top_left ()
   self:load_arch_top_right ()
   self:load_carpet ()
   self:load_mirror ()
   if self.em == "dungeon" then self:load_dungeon_wall ()
   else self:load_palace_wall () end

   self.KID = new (video.KID)
   self:load_actor ("KID", "RUN")
   self:load_actor ("KID", "START_RUN")
   self:load_actor ("KID", "STOP_RUN")
   self:load_actor ("KID", "STABILIZE")
   self:load_actor ("KID", "TURN")
   self:load_actor ("KID", "RUN_JUMP")
   self:load_actor ("KID", "TURN_RUN")
   self:load_actor ("KID", "JUMP")
   self:load_actor ("KID", "MISSTEP")
   self:load_actor ("KID", "CROUCH")
   self:load_actor ("KID", "FALL")
   self:load_actor ("KID", "NORMAL")
   self:load_actor ("KID", "VJUMP")
   self:load_actor ("KID", "WALK")
   self:load_actor ("KID", "CLIMB")
   self:load_actor ("KID", "HANG")
   self:load_actor ("KID", "DIE")
   self:load_actor ("KID", "DRINK")
   self:load_actor ("KID", "STAIRS")
   self:load_actor ("KID", "RAISE_SWORD")
   self:load_actor ("KID", "KEEP_SWORD")
   self:load_actor ("KID", "TAKE_SWORD")
   self:load_actor ("KID", "SWORD_NORMAL")
   self:load_actor ("KID", "SWORD_WALKF")
   self:load_actor ("KID", "SWORD_WALKB")
   self:load_actor ("KID", "SWORD_HIT")
   self:load_actor ("KID", "SWORD_DEFENSE")
   self:load_actor ("KID", "SWORD_ATTACK")
   self:load_actor ("KID", "SWORD")

   self.GUARD = new (video.GUARD)
   self:load_actor ("GUARD", "NORMAL")
   self:load_actor ("GUARD", "SWORD_NORMAL")
   self:load_actor ("GUARD", "SWORD_WALKF")
   self:load_actor ("GUARD", "SWORD_WALKB")
   self:load_actor ("GUARD", "SWORD_ATTACK")
   self:load_actor ("GUARD", "SWORD_DEFENSE")
   self:load_actor ("GUARD", "SWORD_HIT")
   self:load_actor ("GUARD", "FALL")
   self:load_actor ("GUARD", "DIE")
   self.GUARD.SWORD = new (self.KID.SWORD)

   self.FAT_GUARD = new (video.FAT_GUARD)
   self:load_actor ("FAT_GUARD", "NORMAL")
   self:load_actor ("FAT_GUARD", "SWORD_NORMAL")
   self:load_actor ("FAT_GUARD", "SWORD_WALKF")
   self:load_actor ("FAT_GUARD", "SWORD_WALKB")
   self:load_actor ("FAT_GUARD", "SWORD_ATTACK")
   self:load_actor ("FAT_GUARD", "SWORD_DEFENSE")
   self:load_actor ("FAT_GUARD", "SWORD_HIT")
   self:load_actor ("FAT_GUARD", "FALL")
   self:load_actor ("FAT_GUARD", "DIE")
   self.FAT_GUARD.SWORD = new (self.KID.SWORD)

   self.VIZIER = new (video.VIZIER)
   self:load_actor ("VIZIER", "NORMAL")
   self:load_actor ("VIZIER", "SWORD_NORMAL")
   self:load_actor ("VIZIER", "SWORD_WALKF")
   self:load_actor ("VIZIER", "SWORD_WALKB")
   self:load_actor ("VIZIER", "SWORD_ATTACK")
   self:load_actor ("VIZIER", "SWORD_DEFENSE")
   self:load_actor ("VIZIER", "SWORD_HIT")
   self:load_actor ("VIZIER", "FALL")
   self:load_actor ("VIZIER", "DIE")
   self.VIZIER.SWORD = new (self.KID.SWORD)

   self.SKELETON = new (video.SKELETON)
   self:load_actor ("SKELETON", "NORMAL")
   self:load_actor ("SKELETON", "SWORD_NORMAL")
   self:load_actor ("SKELETON", "SWORD_WALKF")
   self:load_actor ("SKELETON", "SWORD_WALKB")
   self:load_actor ("SKELETON", "SWORD_ATTACK")
   self:load_actor ("SKELETON", "SWORD_DEFENSE")
   self:load_actor ("SKELETON", "SWORD_HIT")
   self:load_actor ("SKELETON", "FALL")
   self:load_actor ("SKELETON", "DIE")
   self.SKELETON.SWORD = new (self.KID.SWORD)

   self.SHADOW = new (video.SHADOW)
   self:load_actor ("SHADOW", "NORMAL")
   self:load_actor ("SHADOW", "SWORD_NORMAL")
   self:load_actor ("SHADOW", "SWORD_WALKF")
   self:load_actor ("SHADOW", "SWORD_WALKB")
   self:load_actor ("SHADOW", "SWORD_ATTACK")
   self:load_actor ("SHADOW", "SWORD_DEFENSE")
   self:load_actor ("SHADOW", "SWORD_HIT")
   self:load_actor ("SHADOW", "FALL")
   self:load_actor ("SHADOW", "DIE")
   self.SHADOW.SWORD = new (self.KID.SWORD)

   return self
end

function ASSET:load_bricks ()
   local o = new (video["BRICKS_" .. string.upper (self.em)])
   for i = 1, 5 do
      o[i].bitmap = load_bitmap ("%s/bricks/%i.png", self.em, i)
      o[i].bitmap = o[i].bitmap.apply_palette (o.palette)
   end
   self.BRICKS = o
end

function ASSET:load_torch ()
   local o = new (video["TORCH_" .. string.upper (self.em)])
   o.bitmap = load_bitmap ("%s/torch.png", self.em)
   self.TORCH = o
end

function ASSET:load_window ()
   local o = new (video["WINDOW_" .. string.upper (self.em)])
   o.bitmap = load_bitmap ("%s/window.png", self.em)
   self.WINDOW = o
end

function ASSET:load_balcony ()
   local o = new (video.BALCONY)
   o.bitmap = load_bitmap ("%s/balcony.png", self.em)
   self.BALCONY = o
end

function ASSET:load_floor ()
   local o = new (video.FLOOR)
   o.bitmap = load_bitmap ("%s/floor.png", self.em)
   self.FLOOR = o
end

function ASSET:load_skeleton_floor ()
   local o = new (video.SKELETON_FLOOR)
   o.bitmap = load_bitmap ("%s/skeleton-floor.png", self.em)
   self.SKELETON_FLOOR = o
end

function ASSET:load_broken_floor ()
   local o = new (video.BROKEN_FLOOR)
   o.MAIN.bitmap = load_bitmap ("%s/broken-floor/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/broken-floor/front.png", self.em)
   self.BROKEN_FLOOR = o
end

function ASSET:load_loose_floor ()
   local o = new (video.LOOSE_FLOOR)
   for i = 1, 2 do
      o[i].bitmap = load_bitmap ("%s/loose-floor/%i.png", self.em, i)
   end
   self.LOOSE_FLOOR = o
end

function ASSET:load_opener_floor ()
   local o = new (video.OPENER_FLOOR)
   o.bitmap = load_bitmap ("%s/opener-floor.png", self.em)
   self.OPENER_FLOOR = o
end

function ASSET:load_closer_floor ()
   local o = new (video.CLOSER_FLOOR)
   o.UNPRESSED.bitmap =
      load_bitmap ("%s/closer-floor/unpressed.png", self.em)
   o.PRESSED.bitmap =
      load_bitmap ("%s/closer-floor/pressed.png", self.em)
   o.PRESSED_TRAVERSABLE.bitmap =
      load_bitmap ("%s/closer-floor/pressed-traversable.png", self.em)
   self.CLOSER_FLOOR = o
end

function ASSET:load_spikes_floor ()
   -- temporary
   if (self.em == "palace") then return end

   local o = new (video["SPIKES_FLOOR_" .. string.upper (self.em)])
   for i = 1, 6 do
      o[i].MAIN.bitmap =
         load_bitmap ("%s/spikes-floor/%i/main.png", self.em, i)
      if i > 1 then o[i].FRONT.bitmap =
            load_bitmap ("%s/spikes-floor/%i/front.png", self.em, i)
      end
   end
   self.SPIKES_FLOOR = o
end

function ASSET:load_pillar ()
   local o = new (video["PILLAR_" .. string.upper (self.em)])
   o.MAIN.bitmap = load_bitmap ("%s/pillar/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/pillar/front.png", self.em)
   self.PILLAR = o
end

function ASSET:load_big_pillar_bottom ()
   local o = new (video.BIG_PILLAR_BOTTOM)
   o.MAIN.bitmap = load_bitmap ("%s/big-pillar-bottom/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/big-pillar-bottom/front.png", self.em)
   self.BIG_PILLAR_BOTTOM = o
end

function ASSET:load_big_pillar_top ()
   local o = new (video["BIG_PILLAR_TOP_" .. string.upper (self.em)])
   o.MAIN.bitmap = load_bitmap ("%s/big-pillar-top/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/big-pillar-top/front.png", self.em)
   self.BIG_PILLAR_TOP = o
end

function ASSET:load_door ()
   -- temporary
   if (self.em == "palace") then return end

   local o = new (video.DOOR)
   o.MAIN.bitmap = load_bitmap ("%s/door/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/door/front.png", self.em)
   o.GRID.slice = load_bitmap ("%s/door/slice.png", self.em)
   o.GRID.tip = load_bitmap ("%s/door/tip.png", self.em)
   o.GRID:create ()
   self.DOOR = o
end

function ASSET:load_level_door ()
   -- temporary
   if self.em == "palace" then return end

   local o = new (video["LEVEL_DOOR_" .. string.upper (self.em)])
   o.ENTER.bitmap = load_bitmap ("%s/level-door/enter.png", self.em)
   o.EXIT.bitmap = load_bitmap ("%s/level-door/exit.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/level-door/front.png", self.em)
   o.SLIDE.slice = load_bitmap ("%s/level-door/slice.png", self.em)
   o.SLIDE:create ()
   self.LEVEL_DOOR = o
end

function ASSET:load_chomper ()
   -- temporary
   if self.em == "palace" then return end

   local chomper = new (video.CHOMPER)
   for i = 1, 5 do
      chomper[i].MAIN.bitmap =
         load_bitmap ("%s/chomper/%i/main.png", self.em, i)
      chomper[i].BLOOD.bitmap =
         load_bitmap ("%s/chomper/%i/blood.png", self.em, i)
      chomper[i].BLOOD.bitmap =
         chomper[i].BLOOD.bitmap.apply_palette (blood_palette)
   end
   self.CHOMPER = chomper
end

function ASSET:load_arch_bottom ()
   local o = new (video.ARCH_BOTTOM)
   o.MAIN.bitmap = load_bitmap ("%s/arch-bottom/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/arch-bottom/front.png", self.em)
   self.ARCH_BOTTOM = o
end

function ASSET:load_arch_top_mid ()
   local o = new (video.ARCH_TOP)
   o.bitmap = load_bitmap ("%s/arch-top-mid.png", self.em)
   self.ARCH_TOP_MID = o
end

function ASSET:load_arch_top_small ()
   local o = new (video.ARCH_TOP)
   o.bitmap = load_bitmap ("%s/arch-top-small.png", self.em)
   self.ARCH_TOP_SMALL = o
end

function ASSET:load_arch_top_left ()
   local o = new (video.ARCH_TOP)
   o.bitmap = load_bitmap ("%s/arch-top-left.png", self.em)
   self.ARCH_TOP_LEFT = o
end

function ASSET:load_arch_top_right ()
   local o = new (video.ARCH_TOP)
   o.bitmap = load_bitmap ("%s/arch-top-right.png", self.em)
   self.ARCH_TOP_RIGHT = o
end

function ASSET:load_carpet ()
   local o = new (video.CARPET)

   for i = 1, 6 do
      o[i].MAIN.bitmap =
         load_bitmap ("%s/carpet/%i/main.png", self.em, i)
      o[i].MAIN_TRAVERSABLE.bitmap =
         load_bitmap ("%s/carpet/%i/main-traversable.png", self.em, i)
      o[i].FRONT.bitmap =
         load_bitmap ("%s/carpet/%i/front.png", self.em, i)
      o[i].FACE.bitmap =
         load_bitmap ("%s/carpet/%i/face.png", self.em, i)
   end

   self.CARPET = o
end

function ASSET:load_mirror ()
   local o = new (video.MIRROR)
   o.MAIN.bitmap = load_bitmap ("%s/mirror/main.png", self.em)
   o.FACE.bitmap = load_bitmap ("%s/mirror/face.png", self.em)
   self.MIRROR = o
end

function ASSET:load_dungeon_wall ()
      local o = new (video.WALL_DUNGEON)
      o.FRONT.SWS = load_bitmap ("%s/wall/front/sws.png", self.em)
      o.FRONT.SWW = load_bitmap ("%s/wall/front/sww.png", self.em)
      o.FRONT.WWS = load_bitmap ("%s/wall/front/wws.png", self.em)
      o.FRONT.WWW = load_bitmap ("%s/wall/front/www.png", self.em)
      o.FACE.bitmap = load_bitmap ("%s/wall/face.png", self.em)
      o.narrow_divider = load_bitmap ("%s/wall/narrow-divider.png", self.em)
      o.wide_divider = load_bitmap ("%s/wall/wide-divider.png", self.em)
      o.random_block = load_bitmap ("%s/wall/random-block.png", self.em)
      o.mark_top_right = load_bitmap ("%s/wall/mark-top-right.png", self.em)
      o.mark_bottom_right =
         load_bitmap ("%s/wall/mark-bottom-right.png", self.em)
      o.mark_top_left =
         load_bitmap ("%s/wall/mark-top-left.png", self.em)
      o.mark_bottom_left =
         load_bitmap ("%s/wall/mark-bottom-left.png", self.em)
      self.WALL = o
end

function ASSET:load_palace_wall ()
   local o = new (video.WALL_PALACE)
   o.FACE.bitmap = load_bitmap ("%s/wall/face.png", self.em)
   for i = 1, 15 do
      o.mark[i] = load_bitmap ("%s/wall/mark/%02d.png", self.em, i)
   end
   self.WALL = o
end

function ASSET:load_actor (atype, action)
   local o = new (video[atype][action])
   local type_dir = string.lower (atype)
   type_dir = string.gsub (type_dir, "_", "-")
   local action_dir = string.lower (action)
   action_dir = string.gsub (action_dir, "_", "-")
   local number_fmt = table.getn (o) >= 10 and "%02d" or "%i"
   for i, v in ipairs (o) do
      v.bitmap = load_bitmap ("%s/%s/" .. number_fmt .. ".png",
                              type_dir, action_dir, i, type_dir)
   end
   self[atype][action] = o
end

-- Load assets
function load ()
   asset.DUNGEON = new (ASSET, {}, "DUNGEON"):load ()
   -- asset.PALACE = new (ASSET, {}, "PALACE"):load ()
   return P
end

-- VGA video mode interface
function MININIM.lua.video_mode.VGA (command, object, ...)
   if command == "VALUE" then
      local t = {WIDTH = 320, HEIGHT = 200}
      return t[object]
   end
   local em = MININIM.video.env_mode
   local o = asset[em][object]
   return o[command](o, unpack (arg))
end

-- end
return P
