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
local _debug = _debug
local os = os
local unpack = table.unpack
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

local TRANSPARENT = C (0, 0, 0, 0)

local common = require "script/common"
local new = common.new
local instanceof = common.instanceof
local meta = common.meta
local to_color_range = common.to_color_range
local palette_table_color = common.palette_table_color

-- body
local _ENV = P

local REAL_WIDTH = 320
local REAL_HEIGHT = 200

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

function load_bitmap (filename, ...)
   return common.load_bitmap (P, filename, ...)
end

function load_font (filename, ...)
   return common.load_font (P, filename, ...)
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

   if MININIM.editing and
      (p == MININIM.mouse.position
       or p == MININIM.mouse.selection_position) then
      c = selection[em] (c) end

   return c
end

function apply_palettes (b, p)
   local em = MININIM.video.env_mode
   local hm = MININIM.video.hue_mode

   if hm ~= "NONE" then b = b.apply_palette (hue[hm], true) end

   if MININIM.editing and
      (p == MININIM.mouse.position
       or p == MININIM.mouse.selection_position) then
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
local function blood_palette (c)
   if c == C ("white") then return C (228, 0, 0)
   else return c end
end

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
local video = {}

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
video.PT.style[0] = {
   -- Kid
   [C (252, 252, 216)] = C (64, 64 , 64),
   [C (216, 184, 160)] = C (32, 32, 32),
   -- Skeleton
   [C (252, 252, 240)] = C (252, 252, 240),
   -- Guards
   [C (80, 248, 255)] = C (232, 184, 112),
   [C (0, 0, 160)] = C (252, 104, 120),
   [C (0, 167, 0)] = C (188, 76, 124),
   [C (80, 80, 255)] = C (252, 216, 252),
   [C (160, 167, 160)] = C (208, 180, 208),
   [C (80, 80, 80)] = C (144, 124, 144),
   [C (255, 80, 80)] = C (176, 68, 232),
   [C (0, 167, 160)] = C (176, 68, 232),
   [C (255, 248, 80)] = C (116, 44, 152),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (160, 128, 76),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (188, 76, 124)}

-- Light blue
video.PT.style[1] = {
   -- Kid
   [C (252, 252, 216)] = C (72, 144, 252),
   [C (216, 184, 160)] = C (104, 128, 176),
   -- Skeleton
   [C (252, 252, 240)] = C (72, 144, 252),
   -- Guards
   [C (80, 248, 255)] = C (232, 184, 112),
   [C (0, 0, 160)] = C (72, 144, 252),
   [C (0, 167, 0)] = C (104, 128, 176),
   [C (80, 80, 255)] = C (252, 216, 252),
   [C (160, 167, 160)] = C (208, 180, 208),
   [C (80, 80, 80)] = C (144, 124, 144),
   [C (255, 80, 80)] = C (128, 40, 120),
   [C (0, 167, 160)] = C (252, 252, 252),
   [C (255, 248, 80)] = C (192, 192, 192),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (160, 128, 72),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (104, 128, 176)}

-- Red
video.PT.style[2] = {
   -- Kid
   [C (252, 252, 216)] = C (168, 48, 0),
   [C (216, 184, 160)] = C (128, 36, 0),
   -- Skeleton
   [C (252, 252, 240)] = C (168, 48, 0),
   -- Guards
   [C (80, 248, 255)] = C (168, 112, 112),
   [C (0, 0, 160)] = C (168, 48, 0),
   [C (0, 167, 0)] = C (128, 36, 0),
   [C (80, 80, 255)] = C (176, 136, 252),
   [C (160, 167, 160)] = C (132, 104, 192),
   [C (80, 80, 80)] = C (116, 92, 168),
   [C (255, 80, 80)] = C (184, 40, 188),
   [C (0, 167, 160)] = C (252, 120, 120),
   [C (255, 248, 80)] = C (200, 96, 96),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (120, 80, 80),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (128, 36, 0)}

-- Orange
video.PT.style[3] = {
   -- Kid
   [C (252, 252, 216)] = C (252, 80, 0),
   [C (216, 184, 160)] = C (188, 56, 0),
   -- Skeleton
   [C (252, 252, 240)] = C (252, 80, 0),
   -- Guards
   [C (80, 248, 255)] = C (232, 184, 112),
   [C (0, 0, 160)] = C (252, 80, 0),
   [C (0, 167, 0)] = C (188, 56, 0),
   [C (80, 80, 255)] = C (252, 184, 0),
   [C (160, 167, 160)] = C (208, 152, 0),
   [C (80, 80, 80)] = C (168, 124, 0),
   [C (255, 80, 80)] = C (108, 40, 152),
   [C (0, 167, 160)] = C (252, 252, 252),
   [C (255, 248, 80)] = C (200, 200, 208),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (160, 128, 72),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (188, 56, 0)}

-- Green
video.PT.style[4] = {
   -- Kid
   [C (252, 252, 216)] = C (12, 144, 0),
   [C (216, 184, 160)] = C (12, 112, 0),
   -- Skeleton
   [C (252, 252, 240)] = C (12, 144, 0),
   -- Guards
   [C (80, 248, 255)] = C (232, 184, 112),
   [C (0, 0, 160)] = C (12, 144, 0),
   [C (0, 167, 0)] = C (12, 112, 0),
   [C (80, 80, 255)] = C (252, 216, 96),
   [C (160, 167, 160)] = C (200, 172, 76),
   [C (80, 80, 80)] = C (144, 124, 56),
   [C (255, 80, 80)] = C (152, 0, 32),
   [C (0, 167, 160)] = C (252, 208, 252),
   [C (255, 248, 80)] = C (176, 144, 176),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (160, 128, 72),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (12, 112, 0)}

-- Dark blue
video.PT.style[5] = {
   -- Kid
   [C (252, 252, 216)] = C (96, 0, 252),
   [C (216, 184, 160)] = C (96, 0, 168),
   -- Skeleton
   [C (252, 252, 240)] = C (96, 0, 252),
   -- Guards
   [C (80, 248, 255)] = C (184, 136, 64),
   [C (0, 0, 160)] = C (96, 0, 252),
   [C (0, 167, 0)] = C (96, 0, 168),
   [C (80, 80, 255)] = C (252, 232, 176),
   [C (160, 167, 160)] = C (200, 184, 140),
   [C (80, 80, 80)] = C (152, 140, 108),
   [C (255, 80, 80)] = C (128, 40, 120),
   [C (0, 167, 160)] = C (188, 60, 176),
   [C (255, 248, 80)] = C (136, 44, 128),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (144, 108, 48),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (96, 0, 168)}

-- Purple
video.PT.style[6] = {
   -- Kid
   [C (252, 252, 216)] = C (200, 88, 252),
   [C (216, 184, 160)] = C (152, 68, 192),
   -- Skeleton
   [C (252, 252, 240)] = C (200, 88, 252),
   -- Guards
   [C (80, 248, 255)] = C (232, 184, 112),
   [C (0, 0, 160)] = C (200, 88, 252),
   [C (0, 167, 0)] = C (152, 68, 192),
   [C (80, 80, 255)] = C (252, 252, 216),
   [C (160, 167, 160)] = C (208, 180, 208),
   [C (80, 80, 80)] = C (176, 176, 152),
   [C (255, 80, 80)] = C (92, 108, 160),
   [C (0, 167, 160)] = C (144, 168, 252),
   [C (255, 248, 80)] = C (120, 140, 208),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (160, 128, 72),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (152, 68, 192)}

-- Yellow
video.PT.style[7] = {
   -- Kid
   [C (252, 252, 216)] = C (252, 252, 0),
   [C (216, 184, 160)] = C (200, 216, 0),
   -- Skeleton
   [C (252, 252, 240)] = C (252, 252, 0),
   -- Guards
   [C (80, 248, 255)] = C (184, 120, 252),
   [C (0, 0, 160)] = C (252, 252, 0),
   [C (0, 167, 0)] = C (200, 216, 0),
   [C (80, 80, 255)] = C (252, 192, 0),
   [C (160, 167, 160)] = C (208, 156, 0),
   [C (80, 80, 80)] = C (240, 156, 0),
   [C (255, 80, 80)] = C (96, 0, 176),
   [C (0, 167, 160)] = C (176, 48, 224),
   [C (255, 248, 80)] = C (132, 36, 168),
   [C (160, 80, 0)] = C (216, 136, 104),
   [C (160, 0, 160)] = C (168, 108, 80),
   [C (160, 0, 0)] = C (120, 92, 56),
   [C (80, 248, 80)] = C (112, 76, 160),
   [C (255, 255, 255)] = C (228, 0, 0),
   [C (255, 80, 255)] = C (200, 216, 0)}

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
   else return self[p]:DRAW (width, ...) end
end

function video.OBJECT:RECTANGLE (p, ...)
   if type (p) == "userdata" then return self:rect (p)
   else return self[p]:RECTANGLE (...) end
end

function video.OBJECT:BITMAP (part, ...)
   if part then return self[part]:BITMAP (...)
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
video.BOX = new (video.OBJECT)

for i = 1, 9 do
   video.BOX[i] = {}
end

function video.BOX.selection_palette (c)
   if c == C ("white") then return C (0, 0, 255) end
   return c
end

function video.BOX.lock_palette (c)
   if c == C ("white") then return C (255, 0, 0) end
   return c
end

function video.BOX.rect_sel_palette (c)
   if c == C ("white") then return C (255, 255, 0) end
   return c
end

function video.BOX:DRAW (part, p)
   local i = (part - 1) * 3 + mod (MININIM.tick, 3) + 1
   self[i]:DRAW (p)
end

video.BOX[1].rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 13,
                 p.room)
end

for i = 2, 9 do video.BOX[i].rect = video.BOX[1].rect end

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
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

function video.SWORD_ITEM.shiny:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 50,
                 p.room)
end

-- POTION
video.POTION = new (video.OBJECT)

function video.POTION:DRAW (p)
   local bottle = self.bitmap
   bottle.draw (self:rect (p))

   local i = mod (MININIM.cycle, 7) + 1
   if self[i] and self[i].bitmap then
      local bubble = self[i].bitmap
      if self.palette then
         bubble = bubble.apply_palette (self.palette, true) end
      bubble.DRAW (self[i]:rect (p))
   end
end

-- SMALL POTION
video.SMALL_POTION = new (video.POTION)

for i = 1, 7 do
   video.SMALL_POTION[i] = new (video.OBJECT)
end

function video.SMALL_POTION:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) - 10,
                 PLACE_HEIGHT * p.floor + 47,
                 p.room)
end

video.SMALL_POTION[1].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1) - 7,
                 PLACE_HEIGHT * p.floor + 38,
                 p.room)
end

for i = 1, 7 do
   video.SMALL_POTION[i].rect = video.SMALL_POTION[1].rect end

-- BIG POTION
video.BIG_POTION = new (video.POTION)

for i = 1, 7 do
   video.BIG_POTION[i] = new (video.OBJECT)
end

function video.BIG_POTION:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) - 10,
                 PLACE_HEIGHT * p.floor + 43,
                 p.room)
end

video.BIG_POTION[1].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1) - 7,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

for i = 1, 7 do
   video.BIG_POTION[i].rect = video.BIG_POTION[1].rect end

-- EMPTY POTION
video.EMPTY_POTION = new (video.SMALL_POTION)

function video.EMPTY_POTION.palette (c)
   if c == C ("white") then return TRANSPARENT
   else return c end
end

-- SMALL HP POTION
video.SMALL_HP_POTION = new (video.SMALL_POTION)

function video.SMALL_HP_POTION.palette (c)
   if c == C ("white") then return C (224, 0, 48)
   else return c end
end

-- SMALL POISON POTION
video.SMALL_POISON_POTION = new (video.SMALL_POTION)

function video.SMALL_POISON_POTION.palette (c)
   if c == C ("white") then return C (80, 84, 248)
   else return c end
end

-- ACTIVATION POTION
video.ACTIVATION_POTION = new (video.SMALL_POTION)

function video.ACTIVATION_POTION.palette (c)
   if c == C ("white") then return C (80, 84, 248)
   else return c end
end

-- BIG HP POTION
video.BIG_HP_POTION = new (video.BIG_POTION)

function video.BIG_HP_POTION.palette (c)
   if c == C ("white") then return C (224, 0, 48)
   else return c end
end

-- BIG POISON POTION
video.BIG_POISON_POTION = new (video.BIG_POTION)

function video.BIG_POISON_POTION.palette (c)
   if c == C ("white") then return C (80, 84, 248)
   else return c end
end

-- FLOAT POTION
video.FLOAT_POTION = new (video.BIG_POTION)

function video.FLOAT_POTION.palette (c)
   if c == C ("white") then return C (84, 252, 84)
   else return c end
end

-- FLIP POTION
video.FLIP_POTION = new (video.BIG_POTION)

function video.FLIP_POTION.palette (c)
   if c == C ("white") then return C (255, 255, 0)
   else return c end
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
                 PLACE_HEIGHT * p.floor + 29,
                 p.room)
end

video.BRICKS_PALACE[2].rect = function (self, p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 26,
                 p.room)
end

video.BRICKS_PALACE[3].rect = video.BRICKS_PALACE[1].rect
video.BRICKS_PALACE[4].rect = video.BRICKS_PALACE[2].rect
video.BRICKS_PALACE[5].rect = video.BRICKS_PALACE[1].rect

-- TORCH (DUNGEON)
video.TORCH_DUNGEON = new (video.OBJECT)

function video.TORCH_DUNGEON:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) + 12,
                 PLACE_HEIGHT * p.floor + 23,
                 p.room)
end

-- TORCH (PALACE)
video.TORCH_PALACE = new (video.OBJECT)

function video.TORCH_PALACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 23,
                 p.room)
end

-- FIRE
video.FIRE = new (video.OBJECT)

function video.FIRE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1) + 8,
                 PLACE_HEIGHT * p.floor + 5,
                 p.room)
end

function video.FIRE:DRAW (p)
   local em = MININIM.video.env_mode
   local b = self[mod (MININIM.cycle, 9) + 1].bitmap
   if MININIM.editing and
      (p == MININIM.mouse.position
       or p == MININIM.mouse.selection_position) then
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

-- Princess' room star coordinates
--    static struct star s[6] = {
--     {20,97,0}, {16,104,1}, {23,110,0},
--      {17,116,1}, {24,120,2}, {18,128,2}};

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
                 PLACE_HEIGHT * p.floor + 44,
                 p.room)
end

-- BROKEN FLOOR
video.BROKEN_FLOOR = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BROKEN_FLOOR.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 49,
                 p.room)
end

function video.BROKEN_FLOOR.FRONT:rect (p)
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
   return coord (PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor + 56,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[3].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 41,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[3].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 48,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[4].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[4].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 37,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[5].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[5].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 38,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[6].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35,
                 p.room)
end

video.SPIKES_FLOOR_DUNGEON[6].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 40,
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
                 PLACE_HEIGHT * p.floor + 49,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[2].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor + 55,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[3].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 42,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[3].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 48,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[4].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 34,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[4].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 37,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[5].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[5].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 38,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[6].MAIN.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 35,
                 p.room)
end

video.SPIKES_FLOOR_PALACE[6].FRONT.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 7,
                 PLACE_HEIGHT * p.floor + 39,
                 p.room)
end

-- PILLAR (DUNGEON)
video.PILLAR_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.PILLAR_DUNGEON.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 4,
                 p.room)
end

function video.PILLAR_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 8,
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
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- BIG PILLAR BOTTOM
video.BIG_PILLAR_BOTTOM = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_BOTTOM.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

function video.BIG_PILLAR_BOTTOM.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- BIG PILLAR TOP (DUNGEON)
video.BIG_PILLAR_TOP_DUNGEON = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_TOP_DUNGEON.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor - 3,
                 p.room)
end

function video.BIG_PILLAR_TOP_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 9,
                 PLACE_HEIGHT * p.floor + 3,
                 p.room)
end

-- BIG PILLAR TOP (PALACE)
video.BIG_PILLAR_TOP_PALACE = new (video.OBJECT, {MAIN = {}, FRONT = {}})

function video.BIG_PILLAR_TOP_PALACE.MAIN:rect (p)
   return coord (PLACE_WIDTH * p.place + 10,
                 PLACE_HEIGHT * p.floor - 6,
                 p.room)
end

function video.BIG_PILLAR_TOP_PALACE.FRONT:rect (p)
   return coord (PLACE_WIDTH * p.place + 10,
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
                 PLACE_HEIGHT * p.floor - 10,
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

   local t = {17, 13, 13, 11, 7, 7, 5, 1}

   local function eraser (row, max_col)

   end

   for row, max_col in ipairs (t)
   do for x = 0, max_col do b.set_pixel (x, row - 1, TRANSPARENT) end
   end

   return b
end

-- LEVEL DOOR
video.LEVEL_DOOR = new (
   video.OBJECT, {ENTER = {}, EXIT = {}, FRONT = {}, SLIDE = {}})

local LEVEL_DOOR_STEPS = 44

function video.LEVEL_DOOR.SLIDE:rect (p)
   return coord (PLACE_WIDTH * p.place + 8,
                 PLACE_HEIGHT * p.floor - 1,
                 p.room)
end

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

function video.LEVEL_DOOR_DUNGEON.ENTER:rect (p)
   return coord (PLACE_WIDTH * p.place + 1,
                 PLACE_HEIGHT * p.floor - 13,
                 p.room)
end

function video.LEVEL_DOOR_DUNGEON.EXIT:rect (p)
   return coord (PLACE_WIDTH * p.place + 1,
                 PLACE_HEIGHT * p.floor - 13,
                 p.room)
end

function video.LEVEL_DOOR_DUNGEON.FRONT:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 13,
                 p.room)
end

-- LEVEL DOOR (PALACE)
video.LEVEL_DOOR_PALACE = new (video.LEVEL_DOOR)

function video.LEVEL_DOOR_PALACE.ENTER:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10,
                 p.room)
end

function video.LEVEL_DOOR_PALACE.EXIT:rect (p)
   return coord (PLACE_WIDTH * p.place,
                 PLACE_HEIGHT * p.floor - 10,
                 p.room)
end

function video.LEVEL_DOOR_PALACE.FRONT:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10,
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
   return coord (PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 48,
                 p.room)
end

video.CHOMPER[2].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 39,
                 p.room)
end

video.CHOMPER[3].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 28,
                 p.room)
end

video.CHOMPER[4].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 32,
                 p.room)
end

video.CHOMPER[5].BLOOD.rect = function (self, p)
   return coord (PLACE_WIDTH * p.place + 12,
                 PLACE_HEIGHT * p.floor  + 52,
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
   return coord (PLACE_WIDTH * p.place + 9,
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
                 PLACE_HEIGHT * p.floor - 10,
                 p.room)
end

function carpet.MAIN_TRAVERSABLE:rect (p)
   return coord (PLACE_WIDTH * p.place + 25,
                 PLACE_HEIGHT * p.floor - 10,
                 p.room)
end

function carpet.FACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 7,
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
                 PLACE_HEIGHT * p.floor - 10,
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
   return coord (PLACE_WIDTH * p.place,
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

function video.WALL_DUNGEON:draw_divider_00 (p)
   local function divider_00_coord (self, p)
      return coord (PLACE_WIDTH * p.place + self.r3,
                    PLACE_HEIGHT * p.floor + 45,
                    p.room)
   end
   draw (self, self.divider_00, divider_00_coord, p)
end

function video.WALL_DUNGEON:draw_divider_01 (p)
   local function divider_01_coord (self, p)
      return coord (PLACE_WIDTH * p.place + 8 + self.r1,
                    PLACE_HEIGHT * p.floor + 24,
                    p.room)
   end
   draw (self, self.divider_01, divider_01_coord, p)
end

function video.WALL_DUNGEON:draw_random_block (p)
   draw (self, self.random_block, self.FRONT.rect, p)
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
      return coord (PLACE_WIDTH * p.place + place_offset
                       + 8 * (((r == 2) or (r == 3)) and 1 or 0),
                    PLACE_HEIGHT * p.floor + 61 - floor_offset[r + 1],
                    p.room)
   end

   draw (self, mark, mark_left_coord, p)
end

function video.WALL_DUNGEON:draw_right_mark (p, r)
   local floor_offset, mark = {52, 42, 31, 21}

   if mod (r, 2) == 0 then mark = self.mark_top_right
   else mark = self.mark_bottom_right end

   local function mark_right_coord (self, p)
      return coord (PLACE_WIDTH * p.place + 8 * ((r > 1) and 1 or 0)
                       + ((r < 2) and 24 or self.r1 - 3),
                    PLACE_HEIGHT * p.floor + 56 - floor_offset[r + 1],
                    p.room)
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
   draw (self, self[p.wall_correlation], self.rect, p)
   self:draw_randomization (p)
end

function video.WALL_DUNGEON.FACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 9,
                 p.room)
end

-- WALL (PALACE)
video.WALL_PALACE = new (video.WALL, {mark = {}})

video.WALL_PALACE.color =
   {C (216, 168, 88),
    C (224, 164, 92),
    C (224, 168, 96),
    C (216, 160, 84),
    C (224, 164, 92),
    C (216, 164, 88),
    C (224, 168, 88),
    C (216, 168, 96)}

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
            self.color_cache[room][floor..","..row..","..col]
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
      self.color_cache[np.room][np.floor..","..row..","..np.place + col]
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
                t[row + 1].w,
                t[row + 1].h,
                p.room)
end

function video.WALL_PALACE:mark_frame (p, i)
   local r = prandom_seq_pos (p, i, 1, 2) + 1
   local t = {
      {self.mark[r],
       function (self, p) return coord (PLACE_WIDTH * (p.place + 1) - 8,
                                        PLACE_HEIGHT * p.floor + 3,
                                        p.room) end},
      {self.mark[r + 3],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 17,
                                        p.room) end},
      {self.mark[r + 6],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 38,
                                        p.room) end},
      {self.mark[r + 9],
       function (self, p) return coord (PLACE_WIDTH * p.place,
                                        PLACE_HEIGHT * p.floor + 58,
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

function video.WALL_PALACE.FACE:rect (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10,
                 p.room)
end

-- FRAME
video.FRAME = new (video.OBJECT)

function video.FRAME:BITMAP (part, ...)
   if part then return self[part]:BITMAP (...) end
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

   local shadow_palette
   local shadow_pt
   if a.is_shadow then
      shadow_pt = video.PT.style[a.style]
      function shadow_palette (c)
         return palette_table_color (shadow_pt, c)
      end

      bitmap = bitmap.apply_palette (shadow_palette, shadow_pt)
      if xbitmap then
         xbitmap = xbitmap.apply_palette (shadow_palette, shadow_pt) end
   end

   local splash
   if a.should_draw_splash then
      splash = self[1].bitmap
      if shadow_palette then
         splash = splash.apply_palette (shadow_palette, shadow_pt)
      else
         local pt = video.PT.kid_splash
         local function palette (c)
            return palette_table_color (pt, c)
         end
         splash = splash.apply_palette (palette, pt)
      end
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

   local bg = rect (0, REAL_HEIGHT - 8, 7 * total_hp, 8)
   bg.draw (C (0, 0, 0))

   local full = self[1].bitmap
   local empty = self[2].bitmap

   local pt
   if a.is_shadow then
      if a.style > 0 then
         pt = video.PT.style[a.style]
      else pt = video.PT.shadow_hp end
   else pt = video.PT.kid_hp end

   local function palette (c)
      return palette_table_color (pt, c)
   end

   full = full.apply_palette (palette, pt)
   empty = empty.apply_palette (palette, pt)

   for i = 1, total_hp do
      empty.draw (7 * (i - 1), REAL_HEIGHT - 6);
   end

   if current_hp <= 1 and mod (MININIM.cycle, 2) == 0
   then return end

   for i = 1, current_hp do
      full.draw (7 * (i - 1), REAL_HEIGHT - 6);
   end
end

-- KID START RUN
video.KID.START_RUN = new (
   video.FRAMESET, {},
   {{1, 0}, {2, 0}, {5, 0}, {1, 0}, {7, 0}, {6, 0}})

video.KID.START_RUN[1].rect = function (self, a)
   if a.previous_action == "TURN" then
      return offset ()
   elseif a.previous_action == "STABILIZE" then
      local t = {-2, -6, -4, 0}
      return offset (t[a.previous_index])
   else return video.FRAME.rect (self, a) end
end

-- KID RUN
video.KID.RUN = new (
   video.FRAMESET, {},
   {{10, 0}, {7, 0}, {6, 0}, {4, 0}, {8, 0}, {11, 0}, {4, 0}, {8, 0};
      EDGE_DETECTION_THRESHOLD = {60, 0},
      CONSTRAINED_TURN_RUN = {-4, 0},
      CONSTRAINED_RUN_JUMP = {-4, 0}})

video.KID.RUN[1].rect = function (self, a)
   if a.previous_action == "START_RUN" then
      return offset (6)
   elseif a.previous_action == "RUN_JUMP" then
      return offset (15)
   else return video.FRAME.rect (self, a) end
end

-- KID STOP RUN
video.KID.STOP_RUN = new (
   video.FRAMESET, {},
   {{0, 0}, {1, 0}, {21, 0}, {2, 0};
      CONSTRAINED_TURN_RUN = {-12, 0}})

-- KID STABILIZE
video.KID.STABILIZE = new (
   video.FRAMESET, {},
   {{3, 0}, {4, 0}, {-2, 0}, {-4, 0}})

video.KID.STABILIZE[1].rect = function (self, a)
   if a.previous_action == "STOP_RUN" then
      return offset (5)
   else return video.FRAME.rect (self, a) end
end

-- KID TURN
video.KID.TURN = new (
   video.FRAMESET, {},
   {{-2, 0}, {-1, 0}, {3, 0}, {0, 0}})

video.KID.TURN[1].rect = function (self, a)
   if a.previous_action == "KEEP_SWORD" then
      return offset (2)
   elseif a.previous_action == "STABILIZE" then
      local t = {-6, -10, -8, -4}
      return offset (t[a.previous_index])
   elseif a.previous_action == "TURN" then
      return offset (-2)
   else return video.FRAME.rect (self, a) end
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

video.KID.JUMP[1].rect = function (self, a)
   if a.previous_action == "STABILIZE" then
      local t = {-2, -6, -4, 0}
      return offset (t[a.previous_index])
   else return video.FRAME.rect (self, a) end
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

video.KID.CROUCH[12].rect = function (self, a)
   if a.previous_action == "CLIMB" then
      return offset (-10)
   else return video.FRAME.rect (self, a) end
end

-- KID FALL
video.KID.FALL = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -5}, {0, -10}, {0, -11}, {0, -20}})

video.KID.FALL[1].rect = function (self, a)
   if a.previous_action == "JUMP"
      and (a.previous_index == 10 or a.previous_index == 11)
   then return offset (8, -8)
   elseif a.previous_action == "RUN_JUMP"
   and a.previous_index == 10 then
      return offset (8, -4)
   elseif a.previous_action == "TURN_RUN" then
      return offset (a.float_timer > 0 and -26 or -20)
   elseif a.previous_action == "CROUCH" then
      return offset (a.has_collided and 10 or 12)
   else return video.FRAME.rect (self, a) end
end

-- KID NORMAL
video.KID.NORMAL = new (
   video.FRAMESET, {}, {
      {0, 0}})

video.KID.NORMAL[1].rect = function (self, a)
   local t = {STABILIZE = -2, JUMP = 2,
              CROUCH = 2, VJUMP = -2, KEEP_SWORD = -2}

   if t[a.previous_action] then
      return offset (t[a.previous_action]) end

   if a.previous_action == "WALK"
   and a.previous_index == 12 then
      return offset (1) end

   return video.FRAME.rect (self, a)
end

-- KID VJUMP
video.KID.VJUMP = new (
   video.FRAMESET, {}, {
      {-3, 0}, {1, 0}, {1, 0}, {-2, 0}, {0, 0},
      {1, 0}, {1, 0}, {3, 0}, {1, 0}, {6, 0},
      {0, 0}, {-2, 3}, {-3, 7}, {0, -8}, {-3, -2},
      {1, 0}, {0, 0}, {0, 0};
      HANG_FRONT = {}, HANG_BACK = {}})

video.KID.VJUMP[14].rect = function (self, a)
   if a.previous_action == "HANG_FREE" and a.hang_position.is_critical
   then return offset (a.direction == "LEFT" and 10 or 14, -8)
   else return video.FRAME.rect (self, a) end
end

function video.KID.VJUMP.HANG_FRONT:rect (a)
   return offset (a.direction == "LEFT" and 11 or 38)
end

function video.KID.VJUMP.HANG_BACK:rect (a)
   if a.hang_position.is_critical
   then return offset (a.direction == "LEFT" and 8 or 44)
   else return offset (a.direction == "LEFT" and 11 or 38) end
end

-- KID WALK
video.KID.WALK = new (
   video.FRAMESET, {}, {
      {1, 0}, {1, 0}, {0, 0}, {8, 0}, {7, 0}, {6, 0},
      {-3, 0}, {2, 0}, {1, 0}, {1, 0}, {2, 0}, {0, 0}})

-- KID CLIMB
video.KID.CLIMB = new (
   video.FRAMESET, {}, {
      {-1, 0}, {0, 9}, {2, 3}, {4, 6}, {4, 4}, {1, 6},
      {0, 5}, {4, 8}, {3, 4}, {0, 1}, {2, 5}, {1, 0},
      {0, 0}, {1, 0}, {1, 0};
      CLIMB = {}, UNCLIMB = {}})

function video.KID.CLIMB.CLIMB:rect (a)
   local dx = a.direction == "LEFT" and 9 or 41
   local dy = -9
   return offset (dx, dy)
end

function video.KID.CLIMB.UNCLIMB:rect (a)
   local dx = a.direction == "LEFT" and 19 or 14
   local dy = 25
   return offset (dx, dy)
end

-- KID HANG
video.KID.HANG = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, -2}, {-4, 0}, {-3, -2}, {-3, 0},
      {-1, 0}, {-1, 1}, {-2, 0}, {0, 3}, {0, 0},
      {-1, 1}, {0, 0}, {3, 0}, {0, 0};
      HANG = {}, NON_FREE_FALL = {}, NON_FREE_VJUMP = {},
      FREE_VJUMP = {}, FREE_VJUMP_FRONT = {}, FREE_FALL = {},
      FREE_FALL_FRONT = {}})

video.KID.HANG[5].rect = function (self, a)
   -- This is for HANG_NON_FREE action
   if a.previous_action == "HANG" then return offset (0, -1)
   else return video.FRAME.rect (self, a) end
end

video.KID.HANG[6].rect = function (self, a)
   -- when hanging free the movement is initially reversed so
   -- coordinate of frame n + 1 corresponds to frame number n;
   -- furthermore their offsets are taken with opposite sign
   if a.previous_action == "HANG" then return offset (0, 1)
   else return video.FRAME.rect (self, a) end
end

function video.KID.HANG.HANG:rect (a)
   local dx = a.direction == "LEFT" and 7 or 38
   local dy = -9
   return offset (dx, dy)
end

function video.KID.HANG.NON_FREE_FALL:rect (a)
   local dx = a.direction == "LEFT" and 10 or 22
   local dy = 4
   return offset (dx, dy)
end

function video.KID.HANG.NON_FREE_VJUMP:rect (a)
   local dx = a.direction == "LEFT" and 12 or 30
   local dy = -8
   return offset (dx, dy)
end

function video.KID.HANG.FREE_VJUMP:rect (a)
   local dx = a.direction == "LEFT" and 7 or 41
   local dy = -8
   return offset (dx, dy)
end

function video.KID.HANG.FREE_VJUMP_FRONT:rect (a)
   local dx = a.direction == "LEFT" and 7 or 37
   local dy = -8
   return offset (dx, dy)
end

function video.KID.HANG.FREE_FALL:rect (a)
   local dx = a.direction == "LEFT" and 10 or 22
   local dy = 4
   return offset (dx, dy)
end

function video.KID.HANG.FREE_FALL_FRONT:rect (a)
   local dx = a.direction == "LEFT" and -10 or 42
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
   return offset (a.direction == "LEFT" and -8 or -7, 47)
end

-- KID DRINK
video.KID.DRINK = new (
   video.FRAMESET, {}, {
      {7, 0}, {-1, -1}, {-1, 1}, {0, 0}, {-2, 0}, {1, 0},
      {-1, 0}, {-6, 0}, {1, 0}, {-2, 1}, {2, -1}, {0, 1},
      {1, 0}, {-1, 0}, {-1, 0}})

video.KID.DRINK[8].rect = function (self, a)
   if a.is_reversed then return offset (-1)
   else return video.FRAME.rect (self, a) end
end

video.KID.DRINK[11].rect = function (self, a)
   if a.is_reversed then return offset (2, -1)
   else return video.FRAME.rect (self, a) end
end

-- KID STAIRS
video.KID.STAIRS = new (
   video.FRAMESET, {}, {
      {0, 0}, {1, 0}, {0, 0}, {2, 0}, {-10, 3},
      {-8, 2}, {-4, 3}, {-7, 8}, {-4, 1}, {-5, 4},
      {-3, 6}, {-4, 0};
      STAIRS = {}})

function video.KID.STAIRS.STAIRS:rect (a)
   return offset (3, 16)
end

-- KID RAISE SWORD
video.KID.RAISE_SWORD = new (
   video.FRAMESET, {}, {
      {4, 0}, {0, 0}, {-1, 0}, {1, 0}})

-- KID KEEP SWORD
video.KID.KEEP_SWORD = new (
   video.FRAMESET, {}, {
      {-2, 0}, {2, 0}, {0, 0}, {-1, 0}, {6, 0}, {-2, 0},
      {-3, 0}, {0, 0}, {-3, 0}, {0, 0}})

video.KID.KEEP_SWORD[1].rect = function (self, a)
   if a.previous_action == "SWORD_NORMAL" then return offset (-10)
   else return video.FRAME.rect (self, a) end
end

-- KID TAKE SWORD
video.KID.TAKE_SWORD = new (
   video.FRAMESET, {}, {
      {6, 0}, {0, 0}, {4, 0}, {6, 0}})

-- KID SWORD NORMAL
video.KID.SWORD_NORMAL = new (
   video.FRAMESET, {}, {
      {0, 0}})

video.KID.SWORD_NORMAL[1].rect = function (self, a)
   local t = {TAKE_SWORD = 4, SWORD_WALKF = -5, SWORD_WALKB = -2}

   if t[a.previous_action] then
      return offset (t[a.previous_action]) end

   return offset ()
end

-- KID SWORD WALKF
video.KID.SWORD_WALKF = new (
   video.FRAMESET, {}, {
      {14, 0}, {6, 0}})

-- KID SWORD WALKB
video.KID.SWORD_WALKB = new (
   video.FRAMESET, {}, {
      {1, 0}, {-11, 0}})

video.KID.SWORD_WALKB[1].rect = function (self, a)
   if a.action == "SWORD_DEFENSE"
   then return offset (-1, 0)
   else return video.FRAME.rect (self, a) end
end

-- KID SWORD HIT
video.KID.SWORD_HIT = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {-4, 0}, {-8, 0}, {-8,0}})

-- KID SWORD DEFENSE
video.KID.SWORD_DEFENSE = new (
   video.FRAMESET, {}, {
      {0, 0}, {0, 0}, {0,0}})

video.KID.SWORD_DEFENSE[2].rect = function (self, a)
   if a.xindex == 11 then return offset (-7, 0)
   else return video.FRAME.rect (self, a) end
end

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
      {11, -13}, {13, -5}, {22, -12}, {28, -10},
      {13, 5}, {1, 11}, {0, -5}, {15, -15}, {14, -4},
      {22, -10}, {7, 11}, {14, 11}, {14, 11}, {14, 5},
      {28, -3}, {28, -8}, {21, -7}, {14, -5}, {14, -2},
      {0, -12}, {12, -23}, {0, 0}, {13, -10}, {5, -12}})

video.KID.SWORD[14].rect = function (self, a)
   if a.action == "SWORD_WALKF" then
      if a.index == 1 then return offset (19, -10)
      elseif a.index == 2 then return offset (21, -10)
      end
   else return video.FRAME.rect (self, a) end
end

video.KID.SWORD[18].rect = function (self, a)
   local em = MININIM.video.env_mode
   if a.action == "SWORD_ATTACK" and a.index == 5
   then return offset (7, -17)
   elseif a.action == "SWORD_ATTACK" and a.index == 9
   then return offset (21, -11)
   else return video.FRAME.rect (self, a) end
end

video.KID.SWORD[20].rect = function (self, a)
   if a.action == "SWORD_ATTACK" and a.index == 4
   then return offset (21, -7)
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

   local pt = video.PT.style[a.style]
   function palette (c)
      return palette_table_color (pt, c)
   end

   bitmap = bitmap.apply_palette (palette, pt)
   if xbitmap then
      xbitmap = xbitmap.apply_palette (palette, pt) end

   local splash
   if a.should_draw_splash and a.type ~= "SKELETON" then
      splash = self[1].bitmap
      splash = splash.apply_palette (palette, pt)
   end

   a.draw (bitmap, xbitmap, splash)
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

   local bg = rect (REAL_WIDTH - 7 * current_hp, REAL_HEIGHT - 8,
                    7 * current_hp, 8)
   bg.draw (C (0, 0, 0))

   local full = self[1].bitmap

   local pt
   if (a.type == "SHADOW" and a.style == 0)
   or a.type == "KID" then
      pt = video.PT.shadow_hp
   elseif a.type == "SKELETON" and a.style == 0 then
      pt = video.PT.skeleton_hp
   elseif a.is_guard then
      pt = video.PT.style[a.style]
   end

   local function palette (c)
      return palette_table_color (pt, c)
   end

   full = full.apply_palette (palette, pt)

   if current_hp <= 1 and mod (MININIM.cycle, 2) == 0
   then return end

   for i = 1, current_hp do
      full.draw (REAL_WIDTH - 7 * i + 1, REAL_HEIGHT - 6);
   end
end

-- GUARD NORMAL
video.GUARD.NORMAL = new (
   video.FRAMESET, {}, {{0, 0}})

-- GUARD SWORD NORMAL
video.GUARD.SWORD_NORMAL = new (
   video.FRAMESET, {}, {{0, 0}, {0, 0}, {0, 0}})

video.GUARD.SWORD_NORMAL[1].rect = function (self, a)
   if a.previous_action == "NORMAL" then return offset (1)
   elseif a.previous_action == "SWORD_WALKF" then return offset (-2)
   else return video.FRAME.rect (self, a) end
end

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

video.GUARD.SWORD.offset[2] = {7, -9}

video.GUARD.SWORD[5].rect = function (self, a)
   if a.action == "SWORD_WALKF" then
      if a.index == 2 then return offset (6, -8)
      elseif a.index == 3 then return offset (9, -11)
      end
   else return video.FRAME.rect (self, a) end
end

video.GUARD.SWORD.offset[8] = {0, -23}

video.GUARD.SWORD[9].rect = function (self, a)
   if a.action == "SWORD_ATTACK" and a.index == 4
   then return offset (13, 14)
   else return video.FRAME.rect (self, a) end
end

video.GUARD.SWORD.offset[11] = {10, -16}
video.GUARD.SWORD.offset[12] = {10, -14}
video.GUARD.SWORD.offset[29] = {11, -5}
video.GUARD.SWORD.offset[30] = {11, -15}

-- FAT
video.FAT = new (video.GUARD)

-- FAT DIE
video.FAT.DIE = new (
   video.GUARD.DIE, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -2}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}})

-- VIZIER
video.VIZIER = new (video.GUARD)

video.VIZIER.SWORD.offset[31] = {12, -27}

-- SKELETON
video.SKELETON = new (video.GUARD)

-- SKELETON DIE
video.SKELETON.DIE = new (
   video.GUARD.DIE, {}, {
      {8, 0}, {0, -2}, {0, -2}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}})

-- SKELETON SWORD
for i, v in ipairs (video.SKELETON.SWORD) do
   local rect = v.rect
   v.rect = function (self, a)
      return rect (self, a) + offset (0, 3)
   end
end

video.SKELETON.SWORD.offset[31] = {17, -20}

-- SHADOW
video.SHADOW = new (video.GUARD)

-- SHADOW DIE
video.SHADOW.DIE = new (
   video.GUARD.DIE, {}, {
      {1, 0}, {0, 0}, {3, -1}, {2, -1}, {0, -2},
      {-2, -1}, {0, 0}, {0, 0}})

-- SHADOW SWORD
for i, v in ipairs (video.SHADOW.SWORD) do
   local rect = v.rect
   v.rect = function (self, a)
      return rect (self, a) + offset (0, 2)
   end
end

video.SHADOW.SWORD[9].rect = function (self, a)
   if a.action == "SWORD_ATTACK" and a.index == 4
   then return offset (13, 14)
   else return video.FRAME.rect (self, a) + offset (0, 2) end
end

video.SHADOW.SWORD.offset[30] = {0, -12}

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
      .apply_palette (video.BOX.selection_palette, true)
   end
   for i = 4, 6 do
      o[i].bitmap = load_bitmap ("box/%i.png", i - 3)
         .apply_palette (video.BOX.lock_palette, true)
   end
   for i = 7, 9 do
      o[i].bitmap = load_bitmap ("box/%i.png", i - 6)
         .apply_palette (video.BOX.rect_sel_palette, true)
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
   local small_potion = new (video.SMALL_POTION)
   small_potion.bitmap = load_bitmap ("%s/potion/small.png", self.em)
   for i = 1, 7 do
      small_potion[i].bitmap = load_bitmap ("%s/potion/%i.png", self.em, i)
   end

   local big_potion = new (video.BIG_POTION)
   big_potion.bitmap = load_bitmap ("%s/potion/big.png", self.em)
   for i = 1, 7 do
      big_potion[i].bitmap = small_potion[i].bitmap
   end

   self.EMPTY_POTION = new (video.EMPTY_POTION)
   new (small_potion, self.EMPTY_POTION)

   self.SMALL_HP_POTION = new (video.SMALL_HP_POTION)
   new (small_potion, self.SMALL_HP_POTION)

   self.SMALL_POISON_POTION = new (video.SMALL_POISON_POTION)
   new (small_potion, self.SMALL_POISON_POTION)

   self.ACTIVATION_POTION = new (video.ACTIVATION_POTION)
   new (small_potion, self.ACTIVATION_POTION)

   self.BIG_HP_POTION = new (video.BIG_HP_POTION)
   new (big_potion, self.BIG_HP_POTION)

   self.BIG_POISON_POTION = new (video.BIG_POISON_POTION)
   new (big_potion, self.BIG_POISON_POTION)

   self.FLOAT_POTION = new (video.FLOAT_POTION)
   new (big_potion, self.FLOAT_POTION)

   self.FLIP_POTION = new (video.FLIP_POTION)
   new (big_potion, self.FLIP_POTION)
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
      chomper[i].BLOOD.bitmap =
         chomper[i].BLOOD.bitmap.apply_palette (blood_palette, true)
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
   local number_fmt = #o >= 10 and "%02d" or "%i"
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
   asset.font = load_font ("font.png")
   return P
end

-- VGA video mode interface
MININIM.lua.video_mode["DOS VGA"] = function (command, object, ...)
   local arg = {...}

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
                       REAL_HEIGHT - asset.font.height + 2)
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
