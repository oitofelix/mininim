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

   MININIM features various of improvements over the original game, too
many to make an exhaustive list here.  Some of its noteworthy advantages
are:

- Portable to modern operating systems.
- Video, environment, hue and guard modes may be selected on-the-fly.
- Mirror mode (inverts screen and gamepad axis horizontally).
- Joystick mapping and threshold configuration.
- Improved battle system based on upgradable skills, arbitrary number
  of simultaneous fighters and smarter enemy AI.
- Real-time responsiveness, higher world interaction accuracy and
  more stable animations.
- New useful movements.
- Volume of sounds based on distance from the source.
- Integrated configuration system using multiple back-ends.
- Capability of loading level files in multiple formats.


### Game play

The image below links to a game play demonstration in the animated PNG
(APNG) format.  This format was chosen because it produces perfect
image quality with sizes __much smaller__ than the common general
video formats.  It may not work with all browsers.

<div>
<a href="{{ page.base_local }}{{ site.baseurl }}/game-play.html"
alt="Game play" title="Click here to watch a game play (APNG format).">
<img src="{{ page.base_local }}{{ site.baseurl }}/game-play-thumb.png">
</a>
</div>


### MININIM 0.10 (March 01, 2016)

**Features:**

- Joystick support.  To use a joystick just plug it in and press
‘CTRL+J’ to enable and auto-calibrate it on the fly.  See the manual
for the default mapping.  If the default mapping doesn’t suit you or
your joystick, you can use the options ‘--joystick-axis’,
‘-joystick-button’ and ‘--joystick-info’ to define a new mapping.  In
case your joystick is ill-behaved the ‘--joystick-axis-threshold’ and
‘--joystick-button-threshold’ options may help.  Using a joystick
doesn’t disable the keyboard — you can use both interchangeably.

- Classic numeric keypad control.  This is always enabled, and can be
used along the two other input methods (arrow keys and joystick).  The
original game manual used to refer to this as the standard input
method.

- Apoplexy level editor integration.  Just copy MININIM’s executable
along with any dlls (if under _Windows_) and its ‘data’ directory to
apoplexy’s ‘prince/’ directory, rename the executable to ‘prince.exe’
(‘prince’ if under _GNU/Linux_) and make sure there is a ‘LEVELS.DAT’
file there, and you are ready to go.  Apoplexy will invoke MININIM as
a drop-in replacement for the original engine.

- Hue coloring support.  This makes the engine able to render VGA
color schemes like the 1.3 and 1.4 versions of the original game.
That’s the default.  You can force particular hue modes by using the
‘--hue-mode’ option or the ‘F9’ key binding.  To revert to the
previous and classic behavior use ‘--hue-mode=NONE’.

- Support for reading legacy PoP 1 ‘LEVELS.DAT’ file using the option
‘--legacy-level=DAT’.  Put the ‘LEVELS.DAT’ file inside
‘data/dat-levels/’ directory and use that option.

- Support for reading legacy PoP 1 PLV extended level files using the
option ‘--legacy-level=PLV’.  Put the ‘00.plv’..‘15.plv’ PLV files
inside ‘data/plv-levels/’ and use that option.

- Command line legacy compatibility for the sake of applications which
use it.  Used by apoplexy.

- ‘LEVELS.DAT’ legacy loading behavior compatibility for the sake of
applications that rely on it.  Used by apoplexy.  Just put the
‘LEVELS.DAT’ file in the same directory as the ‘mininim’ executable
and preference will be given to it, over the legacy level files under
the ‘dat/legacy-levels/’ directory.

- Kid start position can be specified using the ‘--start-pos’ option.
Very useful for placing the kid in a specific place for tests.  To
start with the kid in level 12, room 15, floor 0 and place 7, invoke
MININIM with the options ‘--start-level=12 --start-pos=15,0,7’.

- Disable screensaver by default.

- Guards have infallible defense in refraction periods, for longer
and more dramatic battles.

- Shadow in legacy level 12 has the same total/current life points as
the kid, and life points are taken off one at a time, for a longer and
more dramatic battle.

- ‘--keyboard-flip-mode’ option renamed to ‘--gamepad-flip-mode’,
since now it applies to joysticks as well.

**Bug fixes:**

All the bugs the author knew about at the time of this release have
been fixed.  If you’ve found a bug or annoyance in the previous
version, chances are it has been fixed, as well.  ;-)

- Spurious wall collision occurs in kid’s stabilization movement.

- Kid can’t hang on a ledge after a long floating fall.

- Kid’s _run_ movement after a _running turn_ starts one frame off.

- In some circumstances the kid is unable to reach the other end
after a jump, as if a force field prevented him from landing there.

- Kid dies after just touching guards.

- Guards spuriously attack thin air while trying to prevent the kid
from bypassing them.

- While in counter attack and counter defense mode kid defends even
if too far from the guard, when he could otherwise let the guard
strike only thin air.

- Restarting the game while it’s paused causes it to get stuck in a
paused state after the title screen.

- It’s hard to perform 3-tile running jumps because MININIM is
lacking platform edge detection.

- Kid doesn’t stumble on guards, in case he manages to pass through
them without being hit.

- Guards continue to advance towards the kid, even when he is running
towards them.

- Guard mode changing doesn’t work for guards that died falling,
spiked or chopped.

- Kid can’t battle properly against more than one guard at once.

- Shadow death music in level 12 plays in a row if reviving kid with
R repeatedly in a short period of time.

- Legacy level rooms above room 0 may have traversable constructions
at bottom over room 0’s walls.

- In legacy level 12 shadow appears sooner than expected if the kid
manages to enter room 15 from below after exiting room 18 to the
right.

- Guards turn too quickly when their attention is grabbed from
opposite directions in a short period of time.

- Drawing glitches occur in several situations involving two
characters near each other.

- Fight mode collision detection is ill-behaved for kid and guards.

- Game doesn’t pause while saving.

- In some cases of death after fight, the normal death tune is played
along the fight one.

- Falling close to door makes kid collide and stabilize standing,
instead of crouching.

- Guard chasing logic has flaws and corner cases that result in
counter-intuitive guard behavior in certain situations.

- If the kid is exactly 27 pixels from falling, walking to the very
edge and then from there again doesn’t trigger a misstep.

- Turning close to door at its left causes a spurious collision.

- Game saves should store the remaining time of when kid first enters
the level, not of when the game is saved.

- Simple jump might fall earlier than expected.


### Links

- [Homepage](http://oitofelix.github.io/mininim/)
- [Atom feed](http://oitofelix.github.io/feed.xml)
- [Forum](http://forum.princed.org/viewtopic.php?f=69&t=3783)
- [IRC channel](irc://irc.freenode.net/mininim)
- [Bug reporting](https://github.com/oitofelix/mininim/issues)
- [VCS](https://github.com/oitofelix/mininim)


### Download


**User’s Manual**

   If you just want to have a look in the latest release documentation,
you can download the manual in PDF format separately.

   [mininim-0.10.pdf](https://github.com/oitofelix/mininim/releases/download/v0.10/mininim-0.10.pdf)


**Windows**

   If you are under Microsoft Windows a MinGW build is readily
available.  No installation is required, just extract the zip archive
and run ‘mininim.exe’.  The PDF manual is bundled as well.

   [mininim-0.10-win32.zip](https://github.com/oitofelix/mininim/releases/download/v0.10/mininim-0.10-win32.zip)


**GNU/Linux**

   If you use GNU/Linux, you have to build from source.  This should be
quite straightforward, provided you install the Allegro 5 development
libraries using your distribution’s package manager.  If you use an apt
based distribution (Debian, Ubuntu, ...)  you can do this by invoking
the command:

     sudo apt-get install '^liballegro.*5-dev$'

   When building the sources, the usual incantation ‘./configure &&
make’ is often enough.  The installation step (‘make install’) is
optional as MININIM is able to run directly from its source directory
(provided you don’t make a ‘VPATH’ build, in which case you need to
specify the _data path_.  See the ‘--data-path’ option).

Here are the compressed sources and a GPG detached signature:

- [mininim-0.10.tar.gz](https://github.com/oitofelix/mininim/releases/download/v0.10/mininim-0.10.tar.gz)
- [mininim-0.10.tar.gz.sig](https://github.com/oitofelix/mininim/releases/download/v0.10/mininim-0.10.tar.gz.sig)

   Use the ‘.sig’ file to verify that the corresponding file (without
the ‘.sig’ suffix) is intact.  First, be sure to download both the
‘.sig’ file and the corresponding tarball.  Then, run this command:

     gpg --verify mininim-0.10.tar.gz.sig

   If that command fails because you don’t have the required public key,
then run this command to import it:

     gpg --recv-keys 0x28D618AF --keyserver hkp://keys.gnupg.net

and rerun the ‘gpg --verify’ command.

   This release is signed by _Bruno Félix Rezende Ribeiro_.  His key
fingerprint is ‘7CB1 208C 7336 56B7 5962 2500 27B9 C6FD 28D6 18AF’.

This release was bootstrapped with the following tools:

- GNU Autoconf 2.69
- GNU Automake 1.14.1
- GNU Texinfo 5.2


### To do

The following additional features are planned to be implemented
eventually.

- Native level format
- Level editor
- Level generator module
- Guile scripting
- Network play


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
