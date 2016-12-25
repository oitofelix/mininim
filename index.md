---
title: oitofelix - MININIM
description: >
  MININIM: The Advanced Prince of Persia Engine (a childhood dream)
tags: >
  C, Allegro, Prince of Persia
license: CC BY-SA 4.0
layout: oitofelix-homepage
base: http://oitofelix.github.io
#base_local: http://localhost:4000
---
<div id="markdown" markdown="1">
## MININIM

<img style="border: 0px;" src="{{ page.base_local }}{{ site.baseurl }}/mininim.png"
alt="MININIM logo">

<blockquote markdown="1">
_Menininho_ is the Brazilian Portuguese word for _male kid_, and
_mininim_ is how it's informally pronounced throughout the country.
_Kid_ is the term used by _Jordan Mechner_<sup>1</sup> when referring
to the main character of his most famous game --- __Prince of Persia__
--- in its source code and documentation, because the character's
movements were derived from rotoscoping footage of Mechner's little
brother.  __MININIM__ is the _Advanced Prince of Persia Engine_, written
from scratch by computer programmer and free software activist _Bruno
Félix Rezende Ribeiro_ (`oitofelix`).  Bruno had a childhood dream of
deciphering the secrets of his favorite game that introduced him to
computing --- a dream dreamt when he was just a kid, a dream realized
now that he's still a _mininim_ deep in his heart.<sup>2</sup>
</blockquote>

__MININIM__ is the _Advanced Prince of Persia Engine_ --- a childhood
dream, the free software implementation of _Jordan Mechner_'s
masterpiece game, developed from scratch by _Bruno Félix Rezende
Ribeiro_ (`oitofelix`).  _MININIM is free software_; you can
redistribute it and/or modify it under the terms of the
[GNU General Public License](https://www.gnu.org/licenses/gpl.html) as
published by the Free Software Foundation; either version 3, or (at
your option) any later version.

In addition to being already a complete replacement for the original
game with several improvements, MININIM aims to have detailed
documentation on all matters concerning the original Prince of Persia
world, ranging from its design concepts to the engine’s practical use
and development.  _MININIM documentation is free_; you can
redistribute it and/or modify it under the terms of the
[GNU FDL](http://www.gnu.org/licenses/fdl.html) (Free Documentation
Licence) as published by the FSF — with no Invariant Sections; either
version 1.3, or (at your option) any later version.

MININIM features various improvements over the original game, too many
to make an exhaustive list here.  Some of its noteworthy features are:

- Portable to modern operating systems
- Generalized multi-room support
- Video, environment, hue and guard modes may be selected on-the-fly
- Screen mirroring mode (inverts screen and gamepad axis horizontally)
- Level mirroring mode (completely inverts level constructions
  horizontally on-the-fly)
- Joystick mapping and threshold configuration
- Improved battle system based on upgradable skills, arbitrary number
  of simultaneous fighters and smarter enemy AI
- Any type of guard per level, styles applicable to any guard type and
  guard skills fine-tuned per guard.
- Sword can be drawn at will, but it draws and keeps it automatically
  when appropriate.
- Real-time responsiveness, higher world interaction accuracy, more
  stable animations, smoother collision and robust physics
- Single pressing transition commands
- New useful movements: normal jump hang, normal jump turn, running
  jump hang, running jump turn, hanging fall, hanging turn,
- Complete level editor based on non-intrusive mode-line menus and
  mouse selection
- Unlimited undo/redo for level changes
- Integrated configuration system using multiple back-ends
- Native (fully expressive) level format
- Capability of loading level files in multiple formats: native,
  legacy, PLV and DAT
- Consistency level generator module
- Arbitrary combinations of foreground and background for
  constructions
- Generalized fake constructions
- Rich set of itemmizable constructions and items
- Support for triggering of any tile
- Screenshot capturing

The following additional features are planned to be implemented soon.

- Save state
- Replay recording
- Dynamic number of levels, rooms, guards and events
- Guile scripting
- Network play
- Back in time
- Arbitrary resolution graphics set support
- Level generator
- Android, iOS, Mac OS X ports


### Links

- [Homepage](http://oitofelix.github.io/mininim/)
- [Forum](http://forum.princed.org/viewforum.php?f=127)
- [IRC channel](irc://irc.freenode.net/mininim)
- [Bug reporting](https://github.com/oitofelix/mininim/issues)
- [VCS](https://github.com/oitofelix/mininim)


### Game play

[![MININIM multi-room features](https://img.youtube.com/vi/i8kXFLuZ8Io/0.jpg)](https://www.youtube.com/watch?v=i8kXFLuZ8Io)

[![MININIM fake constructions - fake wall](https://img.youtube.com/vi/UFBaBVS7Kc8/0.jpg)](https://www.youtube.com/watch?v=UFBaBVS7Kc8)


### Download

MININIM is under constant and fast development.  It uses a rolling
release scheme, meaning that updated versions are often released as
soon as new features have been implemented or bugs have been fixed.
This way the latest developments are always available to an wider
audience.

Its user's manual (in PDF format) is bundled in the hope that it might
be useful, but keep in mind that it lags far behind the current
development stage of MININIM and thus doesn't describe many features
thereof accurately or at all.  The author plans to update MININIM's
documentation in a definitive manner when MININIM reaches reasonable
feature-set stability.  In case you'd like to help in maintaining the
documentation, you're very welcome!


**Windows**

   If you are under Microsoft Windows a MinGW build is readily
available.  No installation is required, just extract the zip archive
and run ‘mininim.exe’.

[mininim-201612251709.zip](https://github.com/oitofelix/mininim/files/672293/mininim-201612251709.zip)


**GNU/Linux**

  If you use GNU/Linux, you have to clone its
[VCS repository](https://github.com/oitofelix/mininim) and build it
yourself.  It should be quite straightforward --- just follow the
advice given at its GitHub page.


### Using MININIM

This briefly describes how to access most currently implemented
MININIM features.  You can have access to other features using the
command-line.  Use `mininim --help` for a complete list of the
available options.  In case you have any question about MININIM, the
best place to obtain help is at its
[forum](http://forum.princed.org/viewforum.php?f=127).


**Key bindings**

- `CTRL+R`: restart game
- `CTRL+A`: restart level
- `CTRL+G`: save game
- `CTRL+Q`: quit game
- `CTRL+V`: show MININIM version
- `CTRL+S`: enable/disable sound
- `CTRL+J`: calibrate joystick
- `CTRL+L`: load configuration
- `CTRL+P`: save picture

- `F9`: change hue palette
- `F10`: change guard mode
- `F11`: change environment mode
- `F12`: change video mdoe

- `D`: change display mode
- `F`: enable/disable fullscreen (desktop mode)
- `SHIFT+I`: flip screen
- `SHIFT+K`: flip gamepad
- `SHIFT+B`: enable/disable room drawing

- `ESC`: pause game
- `SPACE`: display remaining time
- `+`: increment remaining time
- `-`: decrement remaining time
- `(`: decrease time frequency
- `)`: increase time frequency

- `R`: resurrect kid
- `A`: alternate between kid and its shadows
- `K`: kill current enemy
- `I`: enable/disable immortal mode
- `SHIFT+S`: increment kid's current lives
- `SHIFT+T`: increment kid's total lives
- `SHIFT+W`: float kid

- `TAB`: display skill
- `CTRL+=`: increment counter attack skill
- `CTRL+-`: decrement counter attack skill
- `ALT+=`: increment counter defense skill
- `ALT+-`: decrement counter defense skill

- `F8`: enable/disable level editor
- `CTRL+Z`: undo level modification
- `CTRL+Y`: redo level modification
- `SHIFT+L`: warp to next level
- `SHIFT+M`: warp to previous level
- `C`: show direct coordinates
- `SHIFT+C`: show indirect coordinates

- `HOME`: focus multi-room view on kid
- `SHIFT+HOME`: center multi-room view
- `M`: change multi-room fit mode
- `[`: decrease multi-room resolution
- `]`: increase multi-room resolution
- `CTRL+[`: decrease multi-room width resolution
- `CTRL+]`: increase multi-room width resolution
- `ALT+[`: decrease multi-room height resolution
- `ALT+]`: increase multi-room height resolution
- `H`: select room at left (right if flipped horizontally)
- `J`: select room at right (left if flipped horizontally)
- `U`: select room above (below if flipped vertically)
- `N`: select room below (above if flipped vertically)
- `SHIFT+H`: move multi-room view one cell to left (right if flipped
  horizontally)
- `SHIFT+J`: move multi-room view one cell to right (left if flipped
  horizontally)
- `SHIFT+U`: move multi-room view one cell above (below if flipped
  vertically)
- `SHIFT+N`: move multi-room view one cell below (above if flipped
  vertically)
- `ALT+H`: move multi-room view one page to left (right if flipped
  horizontally)
- `ALT+J`: move multi-room view one page to right (left if flipped
  horizontally)
- `ALT+U`: move multi-room view one page above (below if flipped
  vertically)
- `ALT+N`: move multi-room view one page below (above if flipped
  vertically)



### Support MININIM development

__MININIM__ development is voluntary.  If you like this game and want
to thank the author for the work he has done so far, or want to enable
him to spend more time improving it, please
[make a donation!](/funding.html) For values equal or greater than 50
US$ or 0.12 BTC your name will be acknowledged in the "Donors" section
of the manual and in the [donors page](/funding.html#donors).  If you
decide to make recurrent donations, the total amount of your donations
will be updated every month for all records.  Thank you for your
support.

[__PLEASE, SUPPORT MININIM DEVELOPMENT!__](/funding.html)



<hr>

<p markdown="1">
<small><sup>1</sup> Jordan Mechner (born June 4, 1964) is an American
video game designer, author, screenwriter, and filmmaker, best known
for creating the Prince of Persia video game franchise and the 8-bit
computer game Karateka.</small>
</p>

<p markdown="1">
<small><sup>2</sup> Of course, none of this is reason enough to name a
game engine "MININIM", if not by the fact that it's a really cool
palindrome!</small>
</p>

</div>
