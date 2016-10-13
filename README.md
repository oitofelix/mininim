# MININIM
**The Advanced Prince of Persia Engine (a childhood dream)**

![MININIM](http://oitofelix.github.io/mininim/mininim.png)

- [Homepage](http://oitofelix.github.io/mininim/)
- [Forum](http://forum.princed.org/viewforum.php?f=127)
- **IRC:** irc://irc.freenode.net/mininim
- [VCS](http://github.com/oitofelix/mininim/)

## Game play demo

[![GAME PLAY DEMO](https://img.youtube.com/vi/i8kXFLuZ8Io/0.jpg)](https://www.youtube.com/watch?v=i8kXFLuZ8Io)

## Building

If you are getting the sources from the VCS (or change
`configure.ac`), you'll need to have **Automake** and **Autoconf** to
(re)build the configuration script.  To build the binary **GCC**,
**GNU Make**, **gnulib**, and **Allegro 5** (>= 5.0.9) development
library are necessary as well.  For building the documentation you'll
need **Texinfo**, **TeX Live** and **help2man**.  All these should be
available from your distribution's package manager.  If you use an apt
based distribution (Debian, Ubuntu, ...) you can do this by invoking
the command:

```
sudo apt-get install automake autoconf gcc make gnulib liballegro5-dev \
liballegro-image5-dev liballegro-audio5-dev liballegro-acodec5-dev \
liballegro-dialog5-dev texinfo texlive texlive-generic-recommended help2man
```

When building the sources, the usual incantation `./bootstrap &&
./configure && make` is often enough. The installation step (`make
install`) is optional as MININIM is able to run directly from its
source directory (provided you don’t make a `VPATH` build, in which
case you need to specify the *data path*. See the `--data-path`
option).  After that first time, running `make` should suffice to
rebuild the program with your changes.  See file `INSTALL`.
