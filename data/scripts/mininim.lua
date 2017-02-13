--[[
  mininim.lua -- main script;

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

-- imports
local M = mininim
local legacy = require "legacy"

-- called during loading screen for loading assets
function M.load_assets_hook ()
   local sblast = require "sblast"
   sblast.load ()
   M.setting.audio_mode = sblast.name
end

-- special events
M.level.start_hook = legacy.start_hook
M.level.cycle_hook = legacy.cycle_hook
