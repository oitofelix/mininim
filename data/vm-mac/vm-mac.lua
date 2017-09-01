--[[
   vm-mac.lua -- Macintosh video mode;

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
local P = {package_type = "VIDEO MODE", package_name = "MAC",
           package_file = MININIM.debugger.src}

-- imports
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

local MININIM = MININIM
local div = MININIM.math.div
local mod = MININIM.math.mod
local rshift = MININIM.math.rshift
local uint32 = MININIM.math.uint32
local C = MININIM.video.color
local rect = MININIM.video.rectangle
local pos = MININIM.level.position
local bitmap = MININIM.video.bitmap
local font = MININIM.video.font
local shader = MININIM.video.shader

local TRANSPARENT = C (0, 0, 0, 0)

local common = require "script/common"
local new = common.new
local instanceof = common.instanceof
local meta = common.meta
local to_color_range = common.to_color_range
local palette_table_color = common.palette_table_color
local palette_table_to_shader = common.palette_table_to_shader

-- body
setfenv (1, P)

local REAL_WIDTH = 640
local REAL_HEIGHT = 400

local PLACE_WIDTH = 32
local PLACE_HEIGHT = 63
local PLACES = 10

local asset = {}
local hue = {}
local selection = {}
local COORD = {}

local load_bitmap
local load_font
local coord
local offset
local apply_palettes_to_color
local apply_palettes
local draw

function load_shader (...)
   return common.load_shader (P, unpack (arg))
end

function load_bitmap (filename, ...)
   return common.load_bitmap (P, filename, unpack (arg))
end

function load_font (filename, ...)
   return common.load_font (P, filename, unpack (arg))
end

function coord (x, y, room)
   return rect (x, y, 0, 0, room)
end

function offset (dx, dy)
   return rect (dx, dy)
end

function apply_palettes_to_color (c, p)
   local em = MININIM.video.env_mode
   local hm = MININIM.video.hue_mode

   if hm ~= "NONE" then c = hue[hm] (c) end

   if MININIM.editing and p == MININIM.mouse.selection_position then
      c = selection[em] (c) end

   return c
end

function apply_palettes (b, p)
   local em = MININIM.video.env_mode
   local hm = MININIM.video.hue_mode

   if hm ~= "NONE" then b = b.apply_palette (hue[hm], true) end

   if MININIM.editing and p == MININIM.mouse.selection_position then
      b = b.apply_palette (selection[em], true) end

   return b
end

function draw (self, b, c, p, x, y, w, h)
   b = apply_palettes (b, p)
   b.draw (c (self, p), x, y, w, h)
end

local function IW (w)
   return (w * MININIM.video.original_width)
      / REAL_WIDTH
end

local function IH (h)
   return (h * MININIM.video.original_height)
      / REAL_HEIGHT
end

local function OW (w)
   return (w * REAL_WIDTH)
      / MININIM.video.original_width
end

local function OH (h)
   return (h * REAL_HEIGHT)
      / MININIM.video.original_height
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
      if mod (i, p) == 0 then r0 = nil end
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
   return C (to_color_range (c.r - 12, c.g + 16, c.b - 29))
end

function hue.GRAY (c)
   if c.a == 0 then return c end
   return C (to_color_range (c.r + 13, c.g - 7, c.b - 52))
end

function hue.YELLOW (c)
   if c.a == 0 then return c end
   return C (to_color_range (c.r + 78, c.g + 25, c.b - 64))
end

function hue.BLUE (c)
   if c.a == 0 then return c end
   return C (to_color_range (c.r - 96, c.g - 80, c.b + 64))
end

function selection.DUNGEON (c)
   if c.a == 0 then return c end
   return C (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

function selection.PALACE (c)
   if c.a == 0 then return c end
   return C (to_color_range (c.r + 32, c.g + 32, c.b + 32))
end

-- Video
local video = new ()

-- VALUE
video.VALUE = {
   WIDTH = REAL_WIDTH,
   HEIGHT = REAL_HEIGHT}

-- Palette tables
video.PT = {}

video.PT.kid_hp = {
   [C (0, 0, 160)] = C (228, 0, 0),
   [C (0, 167, 0)] = C (184, 0, 0)}

video.PT.shadow_hp = {
   [C (0, 0, 160)] = C (64, 64, 64),
   [C (0, 167, 0)] = C (32, 32, 32)}

video.PT.skeleton_hp = {
   [C (0, 0, 160)] = C ("white"),
   [C (0, 167, 0)] = C ("white")}

video.PT.kid_splash = {
   [C (0, 0, 160)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (184, 0, 0)}

-- Styles
video.PT.style = {}

-- Salmon
video.PT.style[0] = nil

-- Light blue
video.PT.style[1] = nil

-- Red
video.PT.style[2] = {
   -- Guard's turban and pants
   [C (171, 207, 89)] = C (208, 60, 0),
   [C (127, 127, 0)] = C (168, 48, 0),
   [C (121, 70, 0)] = C (128, 36, 0),

   -- -- Guard's coat
   [C (92, 255, 120)] = C (220, 168, 255),
   [C (0, 220, 74)] = C (176, 136, 252),
   [C (0, 120, 0)] = C (132, 104, 192),

   -- Guard's belt
   [C (255, 0, 255)] = C (225, 82, 230),
   [C (128, 42, 122)] = C (184, 40, 188),

   -- -- Guard's sleeves and shoes
   [C (5, 167, 232)] = C (255, 144, 144),
   [C (2, 124, 244)] = C (252, 120, 120),
   [C (0, 81, 255)] = C (200, 96, 96),

   -- -- Skeleton
   -- [C (250, 250, 250)] = C (),
   -- [C (255, 197, 120)] = C (),
   -- [C (174, 102, 0)] = C (),
   -- [C (110, 48, 0)] = C (),

   -- Shadow's turban
   [C (110, 184, 255)] = C (208, 60, 0),
   [C (38, 94, 255)] = C (168, 48, 0),
   [C (0, 0, 135)] = C (128, 36, 0),

   -- -- Shadow's shirt
   [C (255, 16, 0)] = C (176, 136, 252),
   [C (189, 0, 4)] = C (132, 104, 192),

   -- -- Shadow's pants
   [C (248, 248, 248)] = C (208, 60, 0),
   [C (189, 194, 202)] = C (168, 48, 0),
   [C (120, 125, 125)] = C (128, 36, 0),

   -- Shadow's shoes
   [C (255, 139, 70)] = C (252, 120, 120),
   [C (220, 74, 0)] = C (200, 96, 96),
}

-- Orange
video.PT.style[3] = nil

-- Green
video.PT.style[4] = nil

-- Dark blue
video.PT.style[5] = nil

-- Purple
video.PT.style[6] = nil

-- Yellow
video.PT.style[7] = nil

-- Build style shaders
video.SHADER = {style = {}}
for i = 0, 7 do
   video.SHADER.style[i] = palette_table_to_shader (video.PT.style[i])
end

-- OBJECT
video.OBJECT = new ()

function video.OBJECT:_new (o)
   for k, v in pairs (o) do
      if type (v) == "table"
      and not string.find (k, "^_") then
         local m = meta (video.OBJECT, self[k])
         new (m, v)
      end
   end
end

function video.OBJECT:DRAW (p, width, ...)
   if type (p) == "userdata" then
      if not self.ignore then
         local b = apply_palettes (self.bitmap, p)
         b.draw (self:rect (p), 0, 0, width)
      end
   else return self[p]:DRAW (width, unpack (arg)) end
end

function video.OBJECT:RECTANGLE (p, ...)
   if type (p) == "userdata" then return self:rect (p)
   else return self[p]:RECTANGLE (unpack (arg)) end
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

   local cb = self.BASE:rect (p)
   cb.y = cb.y + hm
   self.BASE.bitmap.draw (b, cb.x, cb.y)

   local cl = self.LEFT:rect (p)
   cl.y = cl.y + hm
   self.LEFT.bitmap.draw (b, cl.x, cl.y)

   local cr = self.RIGHT:rect (p)
   cr.y = cr.y + hm
   self.RIGHT.bitmap.draw (b, cr.x, cr.y)

   return b
end

-- BOX
video.BOX = new (video.OBJECT, {{}, {}, {}, {}, {}, {}})

function video.BOX.lock_palette (c)
   if c == C ("white") then return C ("red") end
   return c
end

function video.BOX:DRAW (p)
   local i = mod (MININIM.cycle, 3) + 1
   if MININIM.mouse.selection_locked then i = i + 3 end
   self[i]:DRAW (p)
end

video.BOX[1].rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 13,
                 p.room)
end

for i = 2, 6 do video.BOX[i].rect = video.BOX[1].rect end

-- SWORD ITEM
video.SWORD_ITEM = new (video.OBJECT, {normal = {}, shiny = {}})

function video.SWORD_ITEM:DRAW (p)
   if mod (MININIM.cycle, 60) == 0
   then self.shiny:DRAW (p)
   else self.normal:DRAW (p)
   end
end

function video.SWORD_ITEM.normal:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 54,
                 p.room)
end

function video.SWORD_ITEM.shiny:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 52.5,
                 p.room)
end

-- POTION
video.POTION = new (video.OBJECT)

function video.POTION:DRAW (p)
   local bottle = self.bitmap
   bottle.draw (self:rect (p))

   local i = mod (MININIM.cycle, 6) + 1
   if self[i] and self[i].bitmap then
      local bubble = self[i].bitmap
      if self.palette then
         bubble = bubble.apply_palette (self.palette, true) end
      bubble.DRAW (self[i]:rect (p))
   end
end

-- SMALL POTION
video.SMALL_POTION = new (video.POTION)

for i = 1, 6 do
   video.SMALL_POTION[i] = new (video.OBJECT)
end

function video.SMALL_POTION:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) - 10,
                 PLACE_HEIGHT * p.floor + 47,
                 p.room)
end

video.SMALL_POTION[1].rect = function (self, p)
   local c = self._parent:rect (p)
   return c + offset (5, -2)
end

video.SMALL_POTION[2].rect = function (self, p)
   local c = self._parent:rect (p)
   return c + offset (2, -5)
end

video.SMALL_POTION[3].rect = function (self, p)
   local c = self._parent:rect (p)
   return c + offset (0, -9)
end

video.SMALL_POTION[4].rect = function (self, p)
   local c = self._parent:rect (p)
   return c + offset (2, -9)
end

video.SMALL_POTION[5].rect = function (self, p)
   local c = self._parent:rect (p)
   return c + offset (0.5, -9.5)
end

video.SMALL_POTION[6].rect = function (self, p)
   local c = self._parent:rect (p)
   return c + offset (1, -9)
end

-- BIG POTION
video.BIG_POTION = new (video.POTION)

function video.BIG_POTION:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) - 9,
                 PLACE_HEIGHT * p.floor + 39,
                 p.room)
end

-- BRICKS (DUNGEON)
video.BRICKS_DUNGEON = new (video.OBJECT, {{}, {}, {}, {}, {}})

function video.BRICKS_DUNGEON.palette (c)
   -- just a remainder for other video modes
   if c == C (48, 68, 88) then return C (48, 68, 88)
   else return c end
end

video.BRICKS_DUNGEON[1].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1) + 1,
                 PLACE_HEIGHT * p.floor + 16,
                 p.room)
end

video.BRICKS_DUNGEON[2].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1) + 2,
                 PLACE_HEIGHT * p.floor + 21,
                 p.room)
end

video.BRICKS_DUNGEON[3].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1) + 4,
                 PLACE_HEIGHT * p.floor + 11,
                 p.room)
end

video.BRICKS_DUNGEON[4].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1) + 4,
                 PLACE_HEIGHT * p.floor + 10,
                 p.room)
end

video.BRICKS_DUNGEON[5].ignore = true
video.BRICKS_DUNGEON[5].rect = video.BRICKS_DUNGEON[1].rect

-- BRICKS (PALACE)
video.BRICKS_PALACE = new (video.OBJECT, {{}, {}, {}, {}, {}})

function video.BRICKS_PALACE.palette (c)
   -- just a remainder for other video modes
   if c == C (12, 56, 88) then return C (12, 56, 88)
   else return c end
end

video.BRICKS_PALACE[1].ignore = true
video.BRICKS_PALACE[1].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 22,
                 p.room)
end

video.BRICKS_PALACE[2].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 18,
                 p.room)
end

video.BRICKS_PALACE[3].rect = video.BRICKS_PALACE[1].rect
video.BRICKS_PALACE[4].rect = video.BRICKS_PALACE[2].rect
video.BRICKS_PALACE[5].rect = video.BRICKS_PALACE[1].rect

-- TORCH (DUNGEON)
video.TORCH_DUNGEON = new (video.OBJECT)

function video.TORCH_DUNGEON:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) + 11.5,
                 PLACE_HEIGHT * p.floor + 20.5,
                 p.room)
end

-- TORCH (PALACE)
video.TORCH_PALACE = new (video.OBJECT)

function video.TORCH_PALACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 20.5,
                 p.room)
end

-- FIRE
video.FIRE = new (video.OBJECT)

function video.FIRE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) + 9,
                 PLACE_HEIGHT * p.floor + 3.5,
                 p.room)
end

function video.FIRE:DRAW (p)
   local em = MININIM.video.env_mode
   local b = self[mod (MININIM.cycle, 9) + 1].bitmap
   if MININIM.editing and p == MININIM.mouse.selection_position then
      b = b.apply_palette (selection[em], true) end
   b.draw (self:rect (p))
end

-- WINDOW (DUNGEON)
video.WINDOW_DUNGEON = new (video.OBJECT)

function video.WINDOW_DUNGEON:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) + 6,
                 PLACE_HEIGHT * p.floor + 11,
                 p.room)
end

-- WINDOW (PALACE)
video.WINDOW_PALACE = new (video.OBJECT)

function video.WINDOW_PALACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 13,
                 p.room)
end

-- BALCONY
video.BALCONY = new (video.OBJECT)

function video.BALCONY:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 17,
                 p.room)
end

-- STARS
video.STARS = new (video.OBJECT)

video.STARS.color = {C (80, 84, 80), C (168, 168, 168), C (255, 255, 255)}
video.STARS.constellation = {}
video.STARS.n = 7

function video.STARS:rect (p)
   local r = video.BALCONY:rect (p)
   r.x = r.x + 20
   r.y = r.y + 12
   r.width = OW (23)
   r.height = OH (28)
   return r
end

function video.STARS:next_color (i)
   if i == 1 or i == 3 then return 2
   else return prandom (1) == 0 and 1 or 3 end
end

function video.STARS:draw_star (c, r, i)
   c.b.set_pixel (c[i].x, c[i].y, self.color [c[i].color])
end

function video.STARS:star_xy (c, r, j)
   local x, y, unique

   repeat
      unique = true
      x = prandom (c.n - 1) * div (r.width - 1, c.n - 1)
      for i = 1, j - 1 do
         if c[i].x == x then
            unique = false
            break
         end
      end
   until unique

   repeat
      unique = true
      y = prandom (c.n - 1) * div (r.height - 1, c.n - 1)
      for i = 1, j - 1 do
         if c[i].y == y then
            unique = false
            break
         end
      end
   until unique

   return x, y
end

function video.STARS:DRAW (p)
   local r = self:rect (p)
   local np = p.normal
   local k = np.room..","..np.floor..","..np.place
   local c = self.constellation[k]

   if not c then
      self.constellation[k] = {}
      c = self.constellation[k]
      c.b = bitmap (r.width, r.height)
      c.n = self.n
      seedp (p)
      for i = 1, c.n do
         c[i] = {}
         c[i].x, c[i].y = self:star_xy (c, r, i)
         c[i].color = prandom (2) + 1
         self:draw_star (c, r, i)
      end
      unseedp ()
   end

   if (mod (MININIM.cycle, 4) == 0 and not MININIM.paused) then
      local i = prandom (c.n - 1) + 1
      c[i].color = self:next_color (c[i].color)
      self:draw_star (c, r, i)
   end

   c.b.draw (r)
end

-- FLOOR
video.FLOOR = new (video.OBJECT)

function video.FLOOR:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

-- SKELETON FLOOR
video.SKELETON_FLOOR = new (video.OBJECT)

function video.SKELETON_FLOOR:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 44.5,
                 p.room)
end

-- BROKEN FLOOR (DUNGEON)
video.BROKEN_FLOOR_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BROKEN_FLOOR_DUNGEON.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 48.5,
                 p.room)
end

function video.BROKEN_FLOOR_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 0.5,
                 PLACE_HEIGHT * p.floor + 53.5,
                 p.room)
end

-- BROKEN FLOOR (PALACE)
video.BROKEN_FLOOR_PALACE = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BROKEN_FLOOR_PALACE.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49,
                 p.room)
end

function video.BROKEN_FLOOR_PALACE.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 54,
                 p.room)
end

-- LOOSE FLOOR
local o = new ({new (video.OBJECT), new (video.OBJECT)})
video.LOOSE_FLOOR = new (video.OBJECT, o)

video.LOOSE_FLOOR[1].rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49,
                 p.room)
end

video.LOOSE_FLOOR[2].rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49,
                 p.room)
end


-- OPENER FLOOR
video.OPENER_FLOOR = new (video.OBJECT)

function video.OPENER_FLOOR:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49,
                 p.room)
end

-- CLOSER FLOOR
local o = {UNPRESSED = new (video.OBJECT),
           PRESSED = new (video.OBJECT)}
video.CLOSER_FLOOR = new (video.OBJECT, o)

function video.CLOSER_FLOOR.UNPRESSED:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

function video.CLOSER_FLOOR.PRESSED:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 51,
                 p.room)
end

-- SPIKES FLOOR (DUNGEON)
local o = new ()
for i = 1, 6 do
   o[i] = {MAIN = {}}
   if i > 1 then o[i].FRONT = {} end
end
video.SPIKES_FLOOR_DUNGEON = new (video.OBJECT, o)

video.SPIKES_FLOOR_DUNGEON[1].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[2].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[2].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 9.5,
                 PLACE_HEIGHT * p.floor + 55,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[3].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 40,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[3].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 8.5,
                 PLACE_HEIGHT * p.floor + 47,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[4].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 33,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[4].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 36,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[5].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[5].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 37,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[6].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[6].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 39,
                 p.room)
end

-- SPIKES FLOOR (PALACE)
local o = new ()
for i = 1, 6 do
   o[i] = {MAIN = {}}
   if i > 1 then o[i].FRONT = {} end
end
video.SPIKES_FLOOR_PALACE = new (video.OBJECT, o)

video.SPIKES_FLOOR_PALACE[1].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[2].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[2].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 9.5,
                 PLACE_HEIGHT * p.floor + 55,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[3].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 40,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[3].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 8.5,
                 PLACE_HEIGHT * p.floor + 47,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[4].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 33,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[4].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 36,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[5].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[5].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 37,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[6].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[6].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 39,
                 p.room)
end

-- PILLAR (DUNGEON)
video.PILLAR_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.PILLAR_DUNGEON.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 3.5,
                 p.room)
end

function video.PILLAR_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 7.5,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- PILLAR (PALACE)
video.PILLAR_PALACE = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.PILLAR_PALACE.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 7,
                 p.room)
end

function video.PILLAR_PALACE.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 12.5,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- BIG PILLAR BOTTOM
video.BIG_PILLAR_BOTTOM = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_BOTTOM.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor,
                 p.room)
end

function video.BIG_PILLAR_BOTTOM.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor,
                 p.room)
end

-- BIG PILLAR TOP (DUNGEON)
video.BIG_PILLAR_TOP_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_TOP_DUNGEON.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place + 8.5,
                 PLACE_HEIGHT * p.floor - 4.5,
                 p.room)
end

function video.BIG_PILLAR_TOP_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 8.5,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- BIG PILLAR TOP (PALACE)
video.BIG_PILLAR_TOP_PALACE = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_TOP_PALACE.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor - 4.5,
                 p.room)
end

function video.BIG_PILLAR_TOP_PALACE.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- DOOR
video.DOOR = new (
   video.OBJECT,
   {MAIN = {}, FRONT = {}, GRID = {}})

local DOOR_STEPS = 48

function video.DOOR.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 9,
                 p.room)
end

function video.DOOR.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

function video.DOOR.GRID:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 6,
                 p.room)
end

function video.DOOR.GRID:create ()
   local parent = self._parent
   for i = 1, DOOR_STEPS do

      parent[i] = new ()
      parent[i].rect = self.rect
      parent[i].bitmap = self:create_bitmap (i)

      local j = i
      local SELF = self
      local function tip_rect (self, p)
         local c = SELF:rect (p)
         c.y = c.y + IH (parent[j].bitmap.height - 1)
         return c
      end

      parent[i].TIP = {rect = tip_rect}

      new (video.OBJECT, parent[i])
   end
end

function video.DOOR.GRID:create_bitmap (i)
   local f = OW (1)

   local h = self.slice.height

   local q = div (i * f - 1, h)
   local r = mod (i * f - 1, h)

   local w = self.slice.width
   local b = bitmap (w, self.slice.height + self.tip.height + i * f - 1)

   self.slice.draw (b, 0, 0, 0, (h - 1) - r, w, r + 1)
   for j = 0, q do self.slice.draw (b, 0, j * h + r + 1) end
   self.tip.draw (b, 0, (q + 1) * h + r + 1)

   local t = {40, 40, 35, 33, 31, 31, 27, 25, 23, 21, 19, 19, 15, 13,
              11, 9, 7, 7}

   local function eraser (row, max_col)
      for x = 0, max_col - 7 do b.set_pixel (x, row - 1, TRANSPARENT) end
   end

   table.foreachi (t, eraser)

   return b
end

-- LEVEL DOOR
video.LEVEL_DOOR = new (
   video.OBJECT, {ENTER = {}, EXIT = {}, FRONT = {}, SLIDE = {}})

local LEVEL_DOOR_STEPS = 44

function video.LEVEL_DOOR.SLIDE:create ()
   local parent = self._parent
   for i = 1, LEVEL_DOOR_STEPS do

      parent[i] = new ()
      parent[i].bitmap = self:create_bitmap (i)
      parent[i].rect = self.rect

      local j = i
      local SELF = self
      local function tip_rect  (self, p)
         local c = SELF:rect (p)
         c.y = c.y + parent[j].height - 1
         return c
      end

      parent[i].TIP = {rect = tip_rect}

      new (video.OBJECT, parent[i])
   end
end

function video.LEVEL_DOOR.SLIDE:create_bitmap (i)
   return self.slice.sub (0, 86 - 2 * (i - 1))
end

-- LEVEL DOOR (DUNGEON)
video.LEVEL_DOOR_DUNGEON = new (video.LEVEL_DOOR)

function video.LEVEL_DOOR_DUNGEON.ENTER:rect (p)
   return coord (PLACE_WIDTH * p.place + 1,
                 PLACE_HEIGHT * p.floor - 20,
                 p.room)
end

function video.LEVEL_DOOR_DUNGEON.EXIT:rect (p)
   return coord (PLACE_WIDTH * p.place + 1,
                 PLACE_HEIGHT * p.floor - 20,
                 p.room)
end

function video.LEVEL_DOOR_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) + 4,
                 PLACE_HEIGHT * p.floor - 20,
                 p.room)
end

function video.LEVEL_DOOR_DUNGEON.SLIDE:rect (p)
   return coord (PLACE_WIDTH * p.place + 17,
                 PLACE_HEIGHT * p.floor - 3.5,
                 p.room)
end


-- LEVEL DOOR (PALACE)
video.LEVEL_DOOR_PALACE = new (video.LEVEL_DOOR)

function video.LEVEL_DOOR_PALACE.ENTER:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 17,
                 p.room)
end

function video.LEVEL_DOOR_PALACE.EXIT:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 17,
                 p.room)
end

function video.LEVEL_DOOR_PALACE.FRONT:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 17,
                 p.room)
end

function video.LEVEL_DOOR_PALACE.SLIDE:rect (p)
   return coord (PLACE_WIDTH * p.place + 13,
                 PLACE_HEIGHT * p.floor - 5,
                 p.room)
end

-- CHOMPER
local o = new ()
for i = 1, 5 do
   o[i] = {MAIN = {}, BLOOD = {}}
   o[i].MAIN.rect = function (self, p)
      return coord (PLACE_WIDTH * p.place,
                    PLACE_HEIGHT * p.floor  + 3,
                    p.room)
   end
end
video.CHOMPER = new (video.OBJECT, o)

video.CHOMPER[1].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 6,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

video.CHOMPER[2].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 6,
                 PLACE_HEIGHT * p.floor + 9.5,
                 p.room)
end

video.CHOMPER[3].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 6.5,
                 PLACE_HEIGHT * p.floor + 20.5,
                 p.room)
end

video.CHOMPER[4].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 6,
                 PLACE_HEIGHT * p.floor + 15,
                 p.room)
end

video.CHOMPER[5].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 6,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- ARCH BOTTOM
video.ARCH_BOTTOM = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.ARCH_BOTTOM.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor,
                 p.room)
end

function video.ARCH_BOTTOM.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 8.5,
                 PLACE_HEIGHT * p.floor,
                 p.room)
end

-- ARCH TOP (abstract)
video.ARCH_TOP = new (video.OBJECT)

function video.ARCH_TOP:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- CARPET
local carpet = new (video.OBJECT, {MAIN = {}, MAIN_TRAVERSABLE = {},
                                   FACE = {}, FRONT = {}})

function carpet.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 9,
                 p.room)
end

function carpet.MAIN_TRAVERSABLE:rect (p)
   return coord (PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor - 9,
                 p.room)
end

function carpet.FACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 6.5,
                 p.room)
end

function carpet.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

video.CARPET = new (video.OBJECT)
for i = 1, 6 do
   video.CARPET[i] = new (carpet)
end

video.CARPET[3].FACE.rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

video.CARPET[4].FACE.rect = video.CARPET[3].FACE.rect

video.CARPET[5].MAIN_TRAVERSABLE.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 9,
                 p.room)
end

video.CARPET[5].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

video.CARPET[6].MAIN_TRAVERSABLE.rect =
   video.CARPET[5].MAIN_TRAVERSABLE.rect

video.CARPET[6].FRONT.rect =
   video.CARPET[5].FRONT.rect

-- MIRROR
video.MIRROR = new (video.OBJECT, {MAIN = {}, FACE = {}})

function video.MIRROR.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

function video.MIRROR.FACE:rect (p)
   return coord (PLACE_WIDTH * p.place + .5,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- WALL
video.WALL = new (video.OBJECT, {FACE = {}, FRONT = {}})

function video.WALL.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- WALL (DUNGEON)
video.WALL_DUNGEON = new (video.WALL)

function video.WALL_DUNGEON.FRONT:DRAW (p, width)
   draw (self, self[p.wall_correlation], self.rect, p)
end

function video.WALL_DUNGEON.FACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 9.5,
                 p.room)
end

-- WALL (PALACE)
video.WALL_PALACE = new (video.WALL, {mark = {}})

video.WALL_PALACE.color =
   {C (222, 148, 79),
    C (235, 151, 71),
    C (220, 138, 66),
    C (225, 145, 74),
    C (232, 158, 76),
    C (222, 151, 66), -- made up color
    C (230, 138, 58),
    C (204, 125, 56)}

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

   return rect (x + t[row + 1].x,
                y + t[row + 1].y,
                t[row + 1].w + .5,
                t[row + 1].h + .5,
                p.room)
end

function video.WALL_PALACE:mark_frame (p, i)
   local r = prandom_seq_pos (p, i, 1, 2) + 1
   local t = {
      {self.mark[r],
       function (self, p) return coord (PLACE_WIDTH * (p.place + 1) - .5,
                                        PLACE_HEIGHT * p.floor + 3,
                                        p.room) end},
      {self.mark[r + 3],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 16,
                                        p.room) end},
      {self.mark[r + 6],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 37,
                                        p.room) end},
      {self.mark[r + 9],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 56,
                                        p.room) end},
      {self.mark[r + 12],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 63,
                                        p.room) end}}
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
   if b0 then draw (self, b0, c0, p) end
   local b1, c1 = self:mark_frame (p, row + 1)
   if b1 then draw (self, b1, c1, p) end
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

function video.WALL_PALACE.FACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10,
                 p.room)
end

-- FRAME
video.FRAME = new (video.OBJECT)

function video.FRAME:BITMAP (part, ...)
   if part then return self[part]:BITMAP (unpack (arg)) end
   return self.bitmap
end

function video.FRAME:_new (o, i)
   o.index = i
end

function video.FRAME:rect (a)
   local t = self._parent.offset
   return offset (t[self.index][1], t[self.index][2])
end

-- FRAMESET
video.FRAMESET = new (video.OBJECT)

function video.FRAMESET:_new (o, offset)
   if offset then
      for k in pairs (offset) do
         local m = meta (video.FRAME, self[k])
         o[k] = new (m, {}, k)
      end
      o.offset = offset
   end
   local m = meta (video.OBJECT, self)
   new (m, o)
end

-- KID
video.KID = new (video.OBJECT)

-- KID MAIN
video.KID.MAIN = new (video.FRAMESET, {}, {{}})

function video.KID.MAIN:DRAW (a)
   if not a.should_draw then return end

   local bitmap = a.bitmap
   local xbitmap = a.xbitmap

   -- local shadow_palette
   -- local shadow_pt
   -- if a.is_shadow then
   --    shadow_pt = video.PT.style[a.style]
   --    function shadow_palette (c)
   --       return palette_table_color (shadow_pt, c)
   --    end

   --    bitmap = bitmap.apply_palette (shadow_palette, shadow_pt)
   --    if xbitmap then
   --       xbitmap = xbitmap.apply_palette (shadow_palette, shadow_pt) end
   -- end

   local splash
   if a.should_draw_splash then
      splash = self[1].bitmap
      -- if shadow_palette then
      --    splash = splash.apply_palette (shadow_palette, shadow_pt)
      -- else
      --    local pt = video.PT.kid_splash
      --    local function palette (c)
      --       return palette_table_color (pt, c)
      --    end
      --    splash = splash.apply_palette (palette, pt)
      -- end
   end

   a.draw (bitmap, xbitmap, splash)
end

-- KID HP
video.KID.HP = new (video.FRAMESET, {}, {{}, {}})

function video.KID.HP:DRAW (a)
   if not a.should_draw_hp then return end
   if a.current_hp == 0 then return end

   local current_hp = math.max (0, a.current_hp)
   current_hp = math.min (10, current_hp)
   local total_hp = math.max (0, a.total_hp)
   total_hp = math.min (10, total_hp)

   local full = self[1].bitmap
   local empty = self[2].bitmap

   local x = 0
   local y = REAL_HEIGHT - full.height - 1
   local w = (full.width + OW (1)) * total_hp
   local h = full.height + 1

   local bg = rect (x, y, w, h).draw (C (0, 0, 0))

   -- local pt
   -- if a.is_shadow then
   --    if a.style > 0 then
   --       pt = video.PT.style[a.style]
   --    else pt = video.PT.shadow_hp end
   -- else pt = video.PT.kid_hp end

   -- local function palette (c)
   --    return palette_table_color (pt, c)
   -- end

   -- full = full.apply_palette (palette, pt)
   -- empty = empty.apply_palette (palette, pt)

   local function x (i)
      return (full.width + OW (1)) * (i - 1)
   end

   local y = REAL_HEIGHT - full.height

   for i = 1, total_hp do
      empty.draw (x (i), y);
   end

   if current_hp <= 1 and mod (MININIM.cycle, 2) == 0
   then return end

   for i = 1, current_hp do
      full.draw (x (i), y);
   end
end

-- KID START RUN
video.KID.START_RUN = new (
   video.FRAMESET, {},
   {{1, 0}, {3.5, 0}, {2.5, 0}, {3, 0}, {5.5, 0}, {6, 0}})

video.KID.START_RUN[1].rect = function (self, a)
   if a.previous_action == "TURN" then
      return offset ()
   elseif a.previous_action == "STABILIZE" then
      local t = {-5, -6.5, -4, -2.5}
      return offset (t[a.previous_index])
   else return video.FRAME.rect (self, a) end
end

-- KID RUN
video.KID.RUN = new (
   video.FRAMESET, {},
   {{8, 0}, {6, 0}, {7, 0}, {3, 0}, {9.5, 0}, {7.5, 0}, {7, 0}, {7, 0};
      EDGE_DETECTION_THRESHOLD = {},
      CONSTRAINED_TURN_RUN = {-6, 0},
      CONSTRAINED_RUN_JUMP = {-7, 0}})

function video.KID.RUN.EDGE_DETECTION_THRESHOLD:rect (a)
   if a.direction == "LEFT" then return offset (67)
   else return offset (52) end
end

video.KID.RUN[1].rect = function (self, a)
   if a.previous_action == "START_RUN" then
      return offset (6)
   elseif a.previous_action == "RUN_JUMP" then
      return offset (13)
   else return video.FRAME.rect (self, a) end
end

video.KID.RUN[7].rect = function (self, a)
   if a.previous_action == "TURN_RUN" then
      return offset (2)
   else return video.FRAME.rect (self, a) end
end

-- KID STOP RUN
video.KID.STOP_RUN = new (
   video.FRAMESET, {},
   {{0, 0}, {0.5, 0}, {21, 0}, {2.5, 0};
      CONSTRAINED_TURN_RUN = {-14, 0}})

-- KID STABILIZE
video.KID.STABILIZE = new (
   video.FRAMESET, {},
   {{3, 0}, {1.5, 0}, {-2.5, 0}, {-1.5, 0}})

video.KID.STABILIZE[1].rect = function (self, a)
   if a.previous_action == "STOP_RUN" then
      return offset (5)
   else return video.FRAME.rect (self, a) end
end

-- KID TURN
video.KID.TURN = new (
   video.FRAMESET, {},
   {{1, 0}, {1, 0}, {1, 0}, {1, 0}})

video.KID.TURN[1].rect = function (self, a)
   if a.previous_action == "KEEP_SWORD" then
      return offset (2)
   elseif a.previous_action == "STABILIZE" then
      local t = {-5, -6.5, -4, -2.5}
      return offset (t[a.previous_index])
   elseif a.previous_action == "TURN" then
      return offset (-2)
   else return video.FRAME.rect (self, a) end
end

-- KID RUN JUMP
video.KID.RUN_JUMP = new (
   video.FRAMESET, {}, {
      {12, 0}, {11, 0}, {13, 0}, {10, 0}, {6, 0},
      {19, 0}, {26, 3}, {24, 9}, {14, 2}, {10, -11},
      {8, -3}})

-- KID TURN RUN
video.KID.TURN_RUN = new (
   video.FRAMESET, {}, {
      {18, 0}, {6, 0}, {4, 0}, {-2, 0}, {6, 0},
      {-1, 0}, {1, 0}, {1, 0}, {-0.5, 0}})

-- KID JUMP
video.KID.JUMP = new (
   video.FRAMESET, {}, {
      {1, 0}, {1.5, 0}, {2.5, 0}, {4, 0}, {4, 0},
      {3, 0}, {1.5, 0}, {13, 0}, {16, 0}, {20, 4},
      {6, -2}, {6, 0}, {-2, 0}, {13, 0}, {1, 0},
      {0, 0}, {0, 0}, {0, 0}})

video.KID.JUMP[1].rect = function (self, a)
   if a.previous_action == "STABILIZE" then
      local t = {-2, -6, -4, 0}
      return offset (t[a.previous_index])
   else return video.FRAME.rect (self, a) end
end

-- KID MISSTEP
video.KID.MISSTEP = new (
   video.FRAMESET, {}, {
      {1.5, 0}, {-1, 0}, {2.5, 0}, {7, 0}, {6.5, 0}, {2, 0},
      {-4.5, 0}, {-10, 0}, {-2, 0}, {-0.5, 0}, {-3, 0}})

-- KID CROUCH
video.KID.CROUCH = new (
   video.FRAMESET, {}, {
      {4, 0}, {4, 0}, {0, 0}, {2, 0}, {1, 0}, {5, 0},
      {0, 0}, {2, 0}, {1, 0}, {-1.5, 0}, {-1, 0}, {-0.5, 0},
      {-3, 0}})

video.KID.CROUCH[3].rect = function (self, a)
   if a.previous_index == 2 then
      return offset (1.5)
   else return video.FRAME.rect (self, a) end
end

video.KID.CROUCH[12].rect = function (self, a)
   if a.previous_action == "CLIMB" then
      return offset (-11, 0)
   else return video.FRAME.rect (self, a) end
end

-- KID FALL
video.KID.FALL = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -5}, {0, -10}, {0, -11}, {0, -20}})

video.KID.FALL[1].rect = function (self, a)
   if a.previous_action == "JUMP"
      and (a.previous_index == 10 or a.previous_index == 11)
   then return offset (10, -6)
   elseif a.previous_action == "RUN_JUMP"
   and a.previous_index == 10 then
      return offset (8, -4)
   elseif a.previous_action == "TURN_RUN" then
      return offset (a.float_timer > 0 and -26 or -20)
   elseif a.previous_action == "CROUCH" then
      return offset (a.has_collided and 12 or 12)
   else return video.FRAME.rect (self, a) end
end

-- KID NORMAL
video.KID.NORMAL = new (
   video.FRAMESET, {}, {
      {0, 0}})

video.KID.NORMAL[1].rect = function (self, a)
   local t = {STABILIZE = -3.5, JUMP = 1.5,
              CROUCH = 1.5, VJUMP = -2, KEEP_SWORD = -2,
              MISSTEP = 1.5, DRINK = -2.5}

   if t[a.previous_action] then
      return offset (t[a.previous_action]) end

   if a.previous_action == "WALK"
   and a.previous_index == 12 then
      return offset (0.5) end

   return offset ()
end

-- KID VJUMP
video.KID.VJUMP = new (
   video.FRAMESET, {}, {
      {-1.5, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      {2, 0}, {0, 0}, {2.5, 0}, {0.5, 0}, {6.5, 0},
      {-2.5, 0}, {-0.5, 3}, {-4, 7}, {-0.5, -8}, {-1.5, -2},
      {0, 0}, {1.5, 0}, {-0.5, 0};
      HANG_FRONT = {}, HANG_BACK = {}})

video.KID.VJUMP[14].rect = function (self, a)
   if a.previous_action == "HANG_FREE" and a.hang_position.is_critical
   then return offset (a.direction == "LEFT" and 3 or 8, -8)
   else return video.FRAME.rect (self, a) end
end

function video.KID.VJUMP.HANG_FRONT:rect (a)
   if not a.hang_position.is_free_at (a.direction)
   then return offset (a.direction == "LEFT" and 12 or 38)
   else return offset (a.direction == "LEFT" and 10 or 38) end
end

function video.KID.VJUMP.HANG_BACK:rect (a)
   if a.hang_position.is_critical
   then return offset (a.direction == "LEFT" and 10 or 42)
   else return offset (a.direction == "LEFT" and 10 or 38) end
end

-- KID WALK
video.KID.WALK = new (
   video.FRAMESET, {}, {
      {1.5, 0}, {-1, 0}, {2.5, 0}, {7, 0}, {6.5, 0}, {2, 0},
      {1.5, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 0}})

-- KID CLIMB
video.KID.CLIMB = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 9}, {1, 3}, {2, 5.5}, {5, 5}, {3, 5},
      {0, 6}, {6, 8}, {3, 4.5}, {0, -1}, {1, 5.5}, {2, 0},
      {-3, 0}, {2.5, 0}, {2, 0};
      CLIMB = {}, UNCLIMB = {}})

function video.KID.CLIMB.CLIMB:rect (a)
   local dx = a.direction == "LEFT" and 11 or 37
   local dy = -10
   return offset (dx, dy)
end

function video.KID.CLIMB.UNCLIMB:rect (a)
   local dx = a.direction == "LEFT" and 18.5 or 14
   local dy = 25
   return offset (dx, dy)
end

-- KID HANG
video.KID.HANG = new (
   video.FRAMESET, {}, {
      {0, 0}, {0.5, -2}, {-4.5, 0}, {-3, -2}, {-2.5, 0},
      {-2.5, 0}, {-0.5, 1.5}, {0, 1}, {0, 2.5}, {0, -0.5},
      {0, 0.5}, {0, 0}, {0, 1}, {0, 0};
      HANG = {}, NON_FREE_FALL = {}, NON_FREE_VJUMP = {},
      FREE_VJUMP = {}, FREE_VJUMP_FRONT = {}, FREE_FALL = {},
      FREE_FALL_FRONT = {}})

video.KID.HANG[5].rect = function (self, a)
   -- This is for HANG_NON_FREE action
   if a.previous_action == "HANG" then return offset (-0.5, -1)
   else return video.FRAME.rect (self, a) end
end

video.KID.HANG[6].rect = function (self, a)
   -- when hanging free the movement is initially reversed so
   -- coordinate of frame n + 1 corresponds to frame number n;
   -- furthermore their offsets are taken with opposite sign
   if a.previous_action == "HANG" then return offset (0.5, 1)
   else return video.FRAME.rect (self, a) end
end

function video.KID.HANG.HANG:rect (a)
   local dx = a.direction == "LEFT" and 7.5 or 35
   local dy = -10
   return offset (dx, dy)
end

function video.KID.HANG.NON_FREE_FALL:rect (a)
   local dx = a.direction == "LEFT" and 6 or 16
   local dy = 4
   return offset (dx, dy)
end

function video.KID.HANG.NON_FREE_VJUMP:rect (a)
   local dx = a.direction == "LEFT" and 10 or 34
   local dy = -8
   return offset (dx, dy)
end

function video.KID.HANG.FREE_VJUMP:rect (a)
   local dx = a.direction == "LEFT" and 7 or 41
   local dy = -8
   return offset (dx, dy)
end

function video.KID.HANG.FREE_VJUMP_FRONT:rect (a)
   local dx = a.direction == "LEFT" and 7 or 40
   local dy = -8
   return offset (dx, dy)
end

function video.KID.HANG.FREE_FALL:rect (a)
   local dx = a.direction == "LEFT" and 10 or 18
   local dy = 4
   return offset (dx, dy)
end

function video.KID.HANG.FREE_FALL_FRONT:rect (a)
   local dx = a.direction == "LEFT" and -6 or 32
   local dy = 12
   return offset (dx, dy)
end

-- KID DIE
video.KID.DIE = new (
   video.FRAMESET, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, -3},
      {-2, 0}, {0, 0}, {0, 0};
      CHOMPED = {}})

function video.KID.DIE.CHOMPED:rect (a)
   return offset (a.direction == "LEFT" and -9.5 or -9.5, 44)
end

-- KID DRINK
video.KID.DRINK = new (
   video.FRAMESET, {}, {
      {7, 0}, {-1, 0}, {-1, 0}, {0, 0}, {-1.5, 0}, {0, 0},
      {-2, 0}, {-2.5, 0}, {-2, 0}, {-1.5, 0}, {2, 0}, {-1, 0},
      {0.5, 0}, {-2, 0}, {-0.5, 0}})

video.KID.DRINK[8].rect = function (self, a)
   if a.is_reversed then return offset (1.5)
   else return video.FRAME.rect (self, a) end
end

video.KID.DRINK[11].rect = function (self, a)
   if a.is_reversed then return offset (3)
   else return video.FRAME.rect (self, a) end
end

-- KID STAIRS
video.KID.STAIRS = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {0, 0}, {-2, 0}, {-10, 3},
      {-10, 3}, {-4, 4.5}, {-6, 6.5}, {-3.5, 2.5}, {-5, 4},
      {-3, 6}, {-4, 0};
      STAIRS = {}})

function video.KID.STAIRS.STAIRS:rect (a)
   return offset (3, 16)
end

-- KID RAISE SWORD
video.KID.RAISE_SWORD = new (
   video.FRAMESET, {}, {
      {4, 0}, {2, 0}, {0.5, 0}, {3, 0}})

-- KID KEEP SWORD
video.KID.KEEP_SWORD = new (
   video.FRAMESET, {}, {
      {-0.5, 0}, {-1, 0}, {1.5, 0}, {-1.5, 0}, {-2, 0}, {-1.5, 0},
      {-2, 0}, {-0.5, 0}, {-2, 0}, {-1, 0}})

video.KID.KEEP_SWORD[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL" then return offset (-3)
   else return video.FRAME.rect (self, a) end
end

-- KID TAKE SWORD
video.KID.TAKE_SWORD = new (
   video.FRAMESET, {}, {
      {5.5, 0}, {0, 0}, {4, 0}, {6, 0}})

-- KID SWORD NORMAL
video.KID.SWORD_NORMAL = new (
   video.FRAMESET, {}, {
      {0, 0}})

video.KID.SWORD_NORMAL[1].rect = function (self, a)
   local t = {TAKE_SWORD = 4, SWORD_WALKF = 2, SWORD_WALKB = 2,
              SWORD_ATTACK = 2, SWORD_DEFENSE = 3.5}

   if t[a.previous_action] then
      return offset (t[a.previous_action]) end

   return offset ()
end

-- KID SWORD WALKF
video.KID.SWORD_WALKF = new (
   video.FRAMESET, {}, {
      {6, 0}, {6, 0}})

-- KID SWORD WALKB
video.KID.SWORD_WALKB = new (
   video.FRAMESET, {}, {
      {-8, 0}, {-4, 0}})

video.KID.SWORD_WALKB[1].rect = function (self, a)
   if a.action == "SWORD_DEFENSE"
   then return offset (-12, 0)
   else return video.FRAME.rect (self, a) end
end

-- KID SWORD HIT
video.KID.SWORD_HIT = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {-4, 0}, {-8, 0}, {-8,0}})

-- KID SWORD DEFENSE
video.KID.SWORD_DEFENSE = new (
   video.FRAMESET, {}, {
      {-0.5, 0}, {-3, 0}, {2,0}})

video.KID.SWORD_DEFENSE[2].rect = function (self, a)
   if a.xindex == 11 then return offset (5, 0)
   else return video.FRAME.rect (self, a) end
end

-- KID SWORD ATTACK
video.KID.SWORD_ATTACK = new (
   video.FRAMESET, {}, {
      {-1, 0}, {8.5, 0}, {9, 0}, {8.5, 0}, {-13.5, 0},
      {-6, 0}, {-7.5, 0}, {8.5, 0}, {1, 0}})

video.KID.SWORD_ATTACK[2].rect = function (self, a)
   if a.previous_action == "SWORD_DEFENSE" then return offset (9, 0)
   else return video.FRAME.rect (self, a) end
end

-- SWORD
video.KID.SWORD = new (
   video.FRAMESET, {}, {
      {0, -18}, {10.5, -6.5}, {17, -4}, {18, -20}, {14.5, -11},
      {-9, -1}, {7, -18}, {1, -20}, {12, 1}, {7, 8},
      {18.5, -14}, {13, -5}, {22, -12}, {18.5, -7.5},
      {10, 9}, {-1.5, 16}, {-0.5, -4.5}, {16, -14.5}, {11.5, -1.5},
      {17.5, -10.5}, {4, 11}, {5, 10}, {4, 10}, {7, 5},
      {16, -2}, {17, -6}, {15, -11}, {3, -10}, {12.5, -2},
      {-4, -12}, {12, -23}, {0, 0}, {10, -6}, {5, -12}})

video.KID.SWORD[14].rect = function (self, a)
   if a.action == "SWORD_WALKF" then
      if a.index == 1 then return offset (16.5, -6.5)
      elseif a.index == 2 then return offset (18.5, -7)
      end
   else return video.FRAME.rect (self, a) end
end

video.KID.SWORD[18].rect = function (self, a)
   if a.action == "SWORD_ATTACK" and a.index == 5
   then return offset (14, -14)
   elseif a.action == "SWORD_ATTACK" and a.index == 9
   then return offset (18.5, -9.5)
   else return video.FRAME.rect (self, a) end
end

video.KID.SWORD[20].rect = function (self, a)
   if a.action == "SWORD_ATTACK" and a.index == 4
   then return offset (17.5, -8.5)
   else return video.FRAME.rect (self, a) end
end

-- GUARD
video.GUARD = new (video.OBJECT)

-- GUARD MAIN
video.GUARD.MAIN = new (video.FRAMESET, {}, {{}})

function video.GUARD.MAIN:DRAW (a)
   if not a.should_draw then return end

   local bitmap = a.bitmap
   local xbitmap = a.xbitmap

   local style_shader = video.SHADER.style[2]

   -- local pt = video.PT.style[a.style]
   -- function palette (c)
   --    return palette_table_color (pt, c)
   -- end

   -- bitmap = bitmap.apply_palette (palette, pt)
   -- if xbitmap then
   --    xbitmap = xbitmap.apply_palette (palette, pt) end

   local splash
   if a.should_draw_splash and a.type ~= "SKELETON" then
      splash = self[1].bitmap
      -- splash = splash.apply_palette (palette, pt)
   end

   a.draw (bitmap, xbitmap, splash, style_shader)
end

-- GUARD HP
video.GUARD.HP = new (video.FRAMESET, {}, {{}})

function video.GUARD.HP:DRAW (a)
   if not a.should_draw_hp then return end
   if a.current_hp == 0 then return end

   local current_hp = math.max (0, a.current_hp)
   current_hp = math.min (10, current_hp)
   local total_hp = math.max (0, a.total_hp)
   total_hp = math.min (10, total_hp)

   local full = self[1].bitmap

   local x = REAL_WIDTH - (full.width + OW (1)) * current_hp
   local y = REAL_HEIGHT - full.height - 1
   local w = (full.width + OW (1)) * current_hp
   local h = full.height + 1

   local bg = rect (x, y, w, h).draw (C (0, 0, 0))

   -- local pt
   -- if (a.type == "SHADOW" and a.style == 0)
   -- or a.type == "KID" then
   --    pt = video.PT.shadow_hp
   -- elseif a.type == "SKELETON" and a.style == 0 then
   --    pt = video.PT.skeleton_hp
   -- elseif a.is_guard then
   --    pt = video.PT.style[a.style]
   -- end

   -- local function palette (c)
   --    return palette_table_color (pt, c)
   -- end

   -- full = full.apply_palette (palette, pt)

   local function x (i)
      return REAL_WIDTH - (full.width + OW (1)) * i + OW (1)
   end

   local y = REAL_HEIGHT - full.height

   if current_hp <= 1 and mod (MININIM.cycle, 2) == 0
   then return end

   for i = 1, current_hp do
      full.draw (x (i), y);
   end
end

-- GUARD NORMAL
video.GUARD.NORMAL = new (
   video.FRAMESET, {}, {{0, 0}})

video.GUARD.NORMAL[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL"
   then return offset (-6.5)
   else return video.FRAME.rect (self, a) end
end

-- GUARD SWORD NORMAL
video.GUARD.SWORD_NORMAL = new (
   video.FRAMESET, {}, {{0, 0}, {0, 0}, {0, 0}})

video.GUARD.SWORD_NORMAL[1].rect = function (self, a)
   if a.previous_action == "NORMAL" then return offset (6.5)
   elseif a.previous_action == "SWORD_WALKF" then return offset ()
   else return video.FRAME.rect (self, a) end
end

-- GUARD SWORD WALKF
video.GUARD.SWORD_WALKF = new (
   video.FRAMESET, {}, {{3.5, 0}, {6, 0}, {5, 0}})

-- GUARD SWORD WALKB
video.GUARD.SWORD_WALKB = new (
   video.FRAMESET, {}, {{-4, 0}, {-8, 0}})

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
      {0, 0}, {0, 0}, {-8, 0}, {-8, 0}, {-8, 0}})

-- GUARD FALL
video.GUARD.FALL = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -5}, {0, -10}, {0, -11}, {0, -20}})

-- GUARD DIE
video.GUARD.DIE = new (
   video.FRAMESET, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, 0},
      {2, 0}, {0, 0}, {0, 0}})

-- GUARD SWORD
video.GUARD.SWORD = new (video.KID.SWORD)

video.GUARD.SWORD.offset[1] = {2.5, -17}
video.GUARD.SWORD.offset[4] = {20, -16}
video.GUARD.SWORD.offset[5] = {14.5, -10}
video.GUARD.SWORD.offset[6] = {-4, 0}
video.GUARD.SWORD.offset[7] = {8, -16}
video.GUARD.SWORD.offset[8] = {-1, -21}

video.GUARD.SWORD[5].rect = function (self, a)
   if a.action == "SWORD_WALKF" then
      if a.index == 2 then return offset (9, -4.5)
      elseif a.index == 3 then return offset (11, -8.5)
      end
   else return video.FRAME.rect (self, a) end
end

video.GUARD.SWORD[9].rect = function (self, a)
   if a.action == "SWORD_ATTACK" and a.index == 4
   then return offset (13, 10)
   else return video.FRAME.rect (self, a) end
end

video.GUARD.SWORD.offset[10] = {6, 7}
video.GUARD.SWORD.offset[11] = {7, -16.5}
video.GUARD.SWORD.offset[12] = {8, -12}
video.GUARD.SWORD.offset[13] = {18, -8}
video.GUARD.SWORD.offset[30] = {14, -16}

-- FAT
video.FAT = new (video.GUARD)

-- FAT NORMAL
video.FAT.NORMAL = new (
   video.GUARD.NORMAL, {}, {{0, 0}})

video.FAT.NORMAL[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL"
   then return offset (-5.5)
   else return video.FRAME.rect (self, a) end
end

-- FAT SWORD NORMAL
video.FAT.SWORD_NORMAL = new (
   video.GUARD.SWORD_NORMAL, {}, {{0, 0}, {0, 0}, {0, 0}})

video.FAT.SWORD_NORMAL[1].rect = function (self, a)
   if a.previous_action == "NORMAL" then return offset (5.5)
   elseif a.previous_action == "SWORD_WALKF" then return offset ()
   else return video.FRAME.rect (self, a) end
end

-- FAT SWORD WALKF
video.FAT.SWORD_WALKF = new (
   video.GUARD.SWORD_WALKF, {}, {{3, 0}, {7, 0}, {4, 0}})

video.FAT.SWORD_WALKF[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL" then return offset (5)
   else return video.FRAME.rect (self, a) end
end

-- FAT DIE
video.FAT.DIE = new (
   video.GUARD.DIE, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}})

-- FAT SWORD
for i, v in ipairs (video.FAT.SWORD) do
   local rect = v.rect
   v.rect = function (self, a)
      return rect (self, a) + offset (-.5, -1)
   end
end

-- VIZIER
video.VIZIER = new (video.GUARD)

-- VIZIER NORMAL
video.VIZIER.NORMAL = new (
   video.GUARD.NORMAL, {}, {{0, 0}})

video.VIZIER.NORMAL[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL"
   then return offset (-11.5)
   else return video.FRAME.rect (self, a) end
end

-- VIZIER SWORD NORMAL
video.VIZIER.SWORD_NORMAL = new (
   video.GUARD.SWORD_NORMAL, {}, {{0, 0}, {0, 0}, {0, 0}})

video.VIZIER.SWORD_NORMAL[1].rect = function (self, a)
   if a.previous_action == "NORMAL" then return offset (11.5)
   elseif a.previous_action == "SWORD_WALKF" then return offset ()
   else return video.FRAME.rect (self, a) end
end

-- VIZIER SWORD WALKF
video.VIZIER.SWORD_WALKF = new (
   video.GUARD.SWORD_WALKF, {}, {{3.5, 0}, {8, 0}, {4, 0}})

-- VIZIER SWORD
for i, v in ipairs (video.VIZIER.SWORD) do
   local rect = v.rect
   v.rect = function (self, a)
      return rect (self, a) + offset (0, -1)
   end
end

video.VIZIER.SWORD.offset[29] = {12.5, 2}

-- SKELETON
video.SKELETON = new (video.GUARD)

-- SKELETON NORMAL
video.SKELETON.NORMAL = new (
   video.GUARD.NORMAL, {}, {{0, 0}})

video.SKELETON.NORMAL[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL"
   then return offset (0.5)
   else return video.FRAME.rect (self, a) end
end

-- SKELETON SWORD NORMAL
video.SKELETON.SWORD_NORMAL = new (
   video.GUARD.SWORD_NORMAL, {}, {{0, 0}, {0, 0}, {0, 0}})

video.SKELETON.SWORD_NORMAL[1].rect = function (self, a)
   if a.previous_action == "NORMAL" then return offset (-0.5)
   elseif a.previous_action == "SWORD_WALKF" then return offset ()
   else return video.FRAME.rect (self, a) end
end

-- SKELETON SWORD WALKF
video.SKELETON.SWORD_WALKF = new (
   video.GUARD.SWORD_WALKF, {}, {{3, 0}, {7, 0}, {5, 0}})

-- SKELETON DIE
video.SKELETON.DIE = new (
   video.GUARD.DIE, {}, {
      {8, 0}, {0, -2}, {0, -2}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}})

-- SKELETON SWORD
for i, v in ipairs (video.SKELETON.SWORD) do
   local rect = v.rect
   v.rect = function (self, a)
      return rect (self, a) + offset (1.5, 2)
   end
end

-- SHADOW
video.SHADOW = new (video.GUARD)

-- SHADOW NORMAL
video.SHADOW.NORMAL = new (
   video.GUARD.NORMAL, {}, {{0, 0}})

video.SHADOW.NORMAL[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL"
   then return offset (-17.5)
   else return video.FRAME.rect (self, a) end
end

-- SHADOW SWORD NORMAL
video.SHADOW.SWORD_NORMAL = new (
   video.GUARD.SWORD_NORMAL, {}, {{0, 0}, {0, 0}, {0, 0}})

video.SHADOW.SWORD_NORMAL[1].rect = function (self, a)
   if a.previous_action == "NORMAL" then return offset (17.5)
   elseif a.previous_action == "SWORD_WALKF" then return offset ()
   else return video.FRAME.rect (self, a) end
end

-- SHADOW SWORD WALKF
video.SHADOW.SWORD_WALKF = new (
   video.GUARD.SWORD_WALKF, {}, {{2, 0}, {8, 0}, {5, 0}})

-- SHADOW DIE
video.SHADOW.DIE = new (
   video.GUARD.DIE, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -1}, {0, -2},
      {-2, -1}, {0, 0}, {0, 0}})

-- SHADOW SWORD
for i, v in ipairs (video.SHADOW.SWORD) do
   local rect = v.rect
   v.rect = function (self, a)
      return rect (self, a) + offset (-1, .5)
   end
end

-- ASSET
local ASSET = new ()

function ASSET:_new (o, em)
   o.em = string.lower (em)
end

function ASSET:load ()
   self:load_box ()
   self:load_sword_item ()
   self:load_potions ()
   self:load_bricks ()
   self:load_torch ()
   self:load_fire ()
   self:load_window ()
   self:load_balcony ()
   self:load_stars ()
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
   self:load_actor ("KID", "MAIN")
   self:load_actor ("KID", "HP")
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
   self:load_actor ("GUARD", "MAIN")
   self:load_actor ("GUARD", "HP")
   self:load_actor ("GUARD", "NORMAL")
   self:load_actor ("GUARD", "SWORD_NORMAL")
   self:load_actor ("GUARD", "SWORD_WALKF")
   self:load_actor ("GUARD", "SWORD_WALKB")
   self:load_actor ("GUARD", "SWORD_ATTACK")
   self:load_actor ("GUARD", "SWORD_DEFENSE")
   self:load_actor ("GUARD", "SWORD_HIT")
   self:load_actor ("GUARD", "FALL")
   self:load_actor ("GUARD", "DIE")
   self:load_actor ("GUARD", "SWORD")

   self.FAT = new (video.FAT)
   self:load_actor ("FAT", "MAIN")
   self:load_actor ("FAT", "HP")
   self:load_actor ("FAT", "NORMAL")
   self:load_actor ("FAT", "SWORD_NORMAL")
   self:load_actor ("FAT", "SWORD_WALKF")
   self:load_actor ("FAT", "SWORD_WALKB")
   self:load_actor ("FAT", "SWORD_ATTACK")
   self:load_actor ("FAT", "SWORD_DEFENSE")
   self:load_actor ("FAT", "SWORD_HIT")
   self:load_actor ("FAT", "FALL")
   self:load_actor ("FAT", "DIE")
   self:load_actor ("FAT", "SWORD")

   self.VIZIER = new (video.VIZIER)
   self:load_actor ("VIZIER", "MAIN")
   self:load_actor ("VIZIER", "HP")
   self:load_actor ("VIZIER", "NORMAL")
   self:load_actor ("VIZIER", "SWORD_NORMAL")
   self:load_actor ("VIZIER", "SWORD_WALKF")
   self:load_actor ("VIZIER", "SWORD_WALKB")
   self:load_actor ("VIZIER", "SWORD_ATTACK")
   self:load_actor ("VIZIER", "SWORD_DEFENSE")
   self:load_actor ("VIZIER", "SWORD_HIT")
   self:load_actor ("VIZIER", "FALL")
   self:load_actor ("VIZIER", "DIE")
   self:load_actor ("VIZIER", "SWORD")

   self.SKELETON = new (video.SKELETON)
   self:load_actor ("SKELETON", "MAIN")
   self:load_actor ("SKELETON", "HP")
   self:load_actor ("SKELETON", "NORMAL")
   self:load_actor ("SKELETON", "SWORD_NORMAL")
   self:load_actor ("SKELETON", "SWORD_WALKF")
   self:load_actor ("SKELETON", "SWORD_WALKB")
   self:load_actor ("SKELETON", "SWORD_ATTACK")
   self:load_actor ("SKELETON", "SWORD_DEFENSE")
   self:load_actor ("SKELETON", "SWORD_HIT")
   self:load_actor ("SKELETON", "FALL")
   self:load_actor ("SKELETON", "DIE")
   self:load_actor ("SKELETON", "SWORD")

   self.SHADOW = new (video.SHADOW)
   self:load_actor ("SHADOW", "MAIN")
   self:load_actor ("SHADOW", "HP")
   self:load_actor ("SHADOW", "NORMAL")
   self:load_actor ("SHADOW", "SWORD_NORMAL")
   self:load_actor ("SHADOW", "SWORD_WALKF")
   self:load_actor ("SHADOW", "SWORD_WALKB")
   self:load_actor ("SHADOW", "SWORD_ATTACK")
   self:load_actor ("SHADOW", "SWORD_DEFENSE")
   self:load_actor ("SHADOW", "SWORD_HIT")
   self:load_actor ("SHADOW", "FALL")
   self:load_actor ("SHADOW", "DIE")
   self:load_actor ("SHADOW", "SWORD")

   return self
end

function ASSET:load_box ()
   local o = new (video.BOX)
   for i = 1, 3 do
      o[i].bitmap = load_bitmap ("box/%i.png", i)
   end
   for i = 4, 6 do
      o[i].bitmap = o[i - 3].bitmap
         .apply_palette (video.BOX.lock_palette, true)
   end
   self.BOX = o
end

function ASSET:load_sword_item ()
   local o = new (video.SWORD_ITEM)
   o.normal.bitmap = load_bitmap ("%s/sword-item/normal.png", self.em)
   o.shiny.bitmap = load_bitmap ("%s/sword-item/shiny.png", self.em)
   self.SWORD_ITEM = o
end

function ASSET:load_potions ()
   local small_potion = {}
   for i = 1, 4 do
      small_potion[i] = new (video.SMALL_POTION)
      small_potion[i].bitmap = load_bitmap ("%s/potion/small.png", self.em)
      if i > 1 then
         for j = 1, 6 do
            small_potion[i][j].bitmap
               = load_bitmap ("%s/potion/%i/%i.png", self.em, i, j)
         end
      end
   end

   local big_potion = new (video.BIG_POTION)
   big_potion.bitmap = load_bitmap ("%s/potion/big.png", self.em)

   self.EMPTY_POTION = new (small_potion[1])
   self.SMALL_HP_POTION = new (small_potion[2])
   self.SMALL_POISON_POTION = new (small_potion[4])
   self.ACTIVATION_POTION = new (small_potion[4])
   self.BIG_HP_POTION = new (big_potion)
   self.BIG_POISON_POTION = new (big_potion)
   self.FLOAT_POTION = new (small_potion[3])
   self.FLIP_POTION = new (small_potion[3])
end

function ASSET:load_bricks ()
   local o = new (video["BRICKS_" .. string.upper (self.em)])
   for i = 1, 5 do
      o[i].bitmap = load_bitmap ("%s/bricks/%i.png", self.em, i)
      o[i].bitmap = o[i].bitmap.apply_palette (o.palette, true)
   end
   self.BRICKS = o
end

function ASSET:load_torch ()
   local o = new (video["TORCH_" .. string.upper (self.em)])
   o.bitmap = load_bitmap ("%s/torch.png", self.em)
   self.TORCH = o
end

function ASSET:load_fire ()
   local o = new (video.FIRE)
   for i = 1, 9 do
      o[i] = new (video.OBJECT)
      o[i].bitmap = load_bitmap ("%s/fire/%i.png", self.em, i)
   end
   self.FIRE = o
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

function ASSET:load_stars ()
   local o = new (video.STARS)
   self.STARS = o
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
   local o = new (video["BROKEN_FLOOR_" .. string.upper (self.em)])
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
   self.CLOSER_FLOOR = o
end

function ASSET:load_spikes_floor ()
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
   local o = new (video.DOOR)
   o.MAIN.bitmap = load_bitmap ("%s/door/main.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/door/front.png", self.em)
   o.GRID.slice = load_bitmap ("%s/door/slice.png", self.em)
   o.GRID.tip = load_bitmap ("%s/door/tip.png", self.em)
   o.GRID:create ()
   self.DOOR = o
end

function ASSET:load_level_door ()
   local o = new (video["LEVEL_DOOR_" .. string.upper (self.em)])
   o.ENTER.bitmap = load_bitmap ("%s/level-door/enter.png", self.em)
   o.EXIT.bitmap = load_bitmap ("%s/level-door/exit.png", self.em)
   o.FRONT.bitmap = load_bitmap ("%s/level-door/front.png", self.em)
   o.SLIDE.slice = load_bitmap ("%s/level-door/slice.png", self.em)
   o.SLIDE:create ()
   self.LEVEL_DOOR = o
end

function ASSET:load_chomper ()
   local chomper = new (video.CHOMPER)
   for i = 1, 5 do
      chomper[i].MAIN.bitmap =
         load_bitmap ("%s/chomper/%i/main.png", self.em, i)
      chomper[i].BLOOD.bitmap =
         load_bitmap ("%s/chomper/%i/blood.png", self.em, i)
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
   asset.PALACE = new (ASSET, {}, "PALACE"):load ()
   local font_bmp = load_bitmap ("font.png")
   asset.font = font (font_bmp, 32, 95, 97, 122)
   return P
end

-- MAC video mode interface
MININIM.lua.video_mode["Macintosh Color"] = function (command, object, ...)
   if command == "DEBUG" then
      local t = {video = video, asset = asset}
      return t[object]
   end

   if command == "VALUE" then
      return video.VALUE[object] end

   if command == "DRAW" and object == "TEXT" then
      local x = OW (0)
      local y = OH (MININIM.video.original_height - 8)
      local w = REAL_WIDTH
      local h = OH (8)
      rect (x, y, w, h).draw (C (0, 0, 0))
      asset.font.draw (arg[1], REAL_WIDTH / 2,
                       REAL_HEIGHT - asset.font.height + 3)
      return
   end

   if command == "DRAW" and object == "BACKGROUND_SELECTION" then
      local p = arg[1]
      local x = OW (p.place * PLACE_WIDTH + 25)
      local y = OH (p.floor * PLACE_HEIGHT - 13)
      local w = OW (PLACE_WIDTH)
      local h = OH (PLACE_HEIGHT)
      rect (x, y, w, h).draw (C (64, 64, 64))
      return
   end

   local em = MININIM.video.env_mode
   local o = asset[em][object]
   return o[command](o, unpack (arg))
end

-- end
return P
