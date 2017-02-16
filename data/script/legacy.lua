--[[
   legacy.lua -- legacy script module;

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

-- package header
local P = {packge_type = "SPECIAL EVENTS", package_name = "LEGACY",
           package_file = debug.getinfo (1, "S").short_src}

-- imports
local M = mininim
local common = require "script/common"
local pos = M.level.position

-- body
setfenv (1, P)

-- level 1
local function level_01_cycle_hook ()
   local kid = M.actor (0)

   if (M.cycle == 12 and not M.level.retry) then
      M.audio.current["SUSPENSE"].play ()
   end
end

-- executed before the first animation cycle of each level
function start_hook ()
   local level = M.level.number
   local kid = M.actor (0)

   -- make kid turn appropriately
   if level == 13 then
      kid.direction = common.opposite_direction (kid.direction)
      kid.action = "RUN"
   elseif level == 1 then
      kid.action = "TURN"
   else
      kid.direction = common.opposite_direction (kid.direction)
      kid.action = "TURN"
   end

   -- for the first level...
   if level == 1 then
      -- activate tile (originally closes opened door in starting
      -- room)
      pos (5, 0, 2).activate ()

      -- if not retrying the level, get up slowly
      if not M.level.retry then
         kid.delayed_stand_up = true
      end
   end
end

-- executed for each animation cycle of each level
function cycle_hook ()
   local cycle_hooks
      = {level_01_cycle_hook, level_02_cycle_hook, level_03_cycle_hook,
         level_04_cycle_hook, level_05_cycle_hook, level_06_cycle_hook,
         level_07_cycle_hook, level_08_cycle_hook, level_09_cycle_hook,
         level_10_cycle_hook, level_11_cycle_hook, level_12_cycle_hook,
         level_13_cycle_hook, level_14_cycle_hook}
   local hook = cycle_hooks[M.level.number]
   if hook then hook () end
end

-- load package interface
function load ()
   M.level.start_hook = start_hook
   M.level.cycle_hook = cycle_hook
   return P
end

-- end
return P
