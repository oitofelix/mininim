# MININIM
**The Advanced Prince of Persia Engine (a childhood dream)**

![MININIM](http://oitofelix.github.io/mininim/mininim.png)

- [Homepage](http://oitofelix.github.io/mininim/)
- [Forum](http://forum.princed.org/viewforum.php?f=127)
- **IRC:** irc://irc.freenode.net/mininim
- [VCS](http://github.com/oitofelix/mininim/)

## Building

To build MININIM from this Git repository you'll need to have
installed in your machine **Automake**, **Autoconf**, **GCC**, **GNU
Make**, **gnulib**, **CMake**, and
[MININIM's fork of **Allegro**](https://github.com/oitofelix/allegro5).
For apt based GNU/Linux distributions (Ubuntu, Debian, ...):

```
sudo apt-get remove liballegro5-dev liballegro-image5-dev \
  liballegro-audio5-dev liballegro-acodec5-dev liballegro-dialog5-dev
sudo apt-get install automake autoconf gcc make gnulib cmake
sudo apt-get build-dep allegro5
git clone https://github.com/oitofelix/allegro5.git
cd allegro5
cmake
make
sudo make install
cd ..
git clone https://github.com/oitofelix/mininim.git
cd mininim
./bootstrap
./configure
make
./mininim
```

To build MININIM's documentation you'll need **Texinfo**, **TeX Live**
and **help2man**.  For apt based distributions:

```
sudo apt-get install texinfo texlive texlive-generic-recommended \
  help2man
cd mininim/doc
make mininim.pdf
```
