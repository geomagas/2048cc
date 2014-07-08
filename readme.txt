==========================================================================
                   2048 Console Clone v0.3a1 (c) 2014 migf1
==========================================================================

  Author:       migf1 <mig_f1@hotmail.com>
  Version:      0.3a1
  Date:         July 7, 2014
  License:      Free Software (see the Licence section for limitations)

A console clone of the game 2048 ( http://gabrielecirulli.github.io/2048/ )
written in ISO C99. It is meant to be cross-platform across Windows, Unix,
Linux, and MacOSX (for the latter 3, you should enable ANSI-colors support
on your terminal emulation).

Compared to the original game, this version additionally supports:
- skins (color themes)
- undo/redo (it disables best-score tracking)
- replays
- load/save games (via replays)

Moreover, this version of the game is also cloning 3 unofficial variants
of the original game, namely:

- 5x5 board ( http://2048game.com/variations/5x5.html )
- 6x6 board ( http://2048game.com/variations/6x6.html )
- 8x8 board ( http://2048game.com/variations/8x8.html )


Pre-compiled Binaries
---------------------

The zip file contains a pre-compiled 32bit executable file for Windows (XP or
newer). It is called: 2048cc_win_x86.exe On other platforms, you can compile
the sources. See the next section.


How to compile the game 
------------------------

To compile with gcc follow these simple steps:

1. Extract the zip file anywhere you want in your hard-drive
2. From the command-line, navigate into the src/ folder
3. On Windows (e.g. with mingw-gcc) type:
   gcc -std=c99 -s -O3 -D_BSD_SOURCE.c -o 2048cc.exe

   On Unix/Linux/MacOSX other platforms, type:
   gcc -std=c99 -s -O3 -D_BSD_SOURCE.c -o 2048cc.out

4. See the next section on how to run the executable file.

To compile from within an IDE (with any compiler) follow these steps:

1. Extract the zip file anywhere you want in your hard-drive
2. Create a new console/terminal project (you may call it: 2048cc)
3. Add to the project all the .c files found inside the src/ folder
4. Enable C99 support on your compiler
5. Predefine the directive _BSD_SOURCE (only tested with gcc)
6. Build a Release version of the project.
7. See the next section on how to run the executable file.


How to run the game
-------------------

After the compilation is done, move the executable file (either 2048cc.exe
or 2048cc.out... see above) out of the src/ folder.

Actually the executable file should be in the same path with the replays/
folder, otherwise you will not be able to load/save replays.

Run the executable, enjoy the game and please report any bugs you may find.


License
-------

The game is open-source, free software with only 3 limitations:

1. Keep it free and open-source.
2. Do not try to make any kind of profit from it or from any
    derivatives of it, unless you have contacted me for special
    arrangements ( mig_f1@hotmail.com ).
 3. Always re-distribute the original package, along with any
    software you distribute that is based on this game.


 Disclaimer
 ----------

 Use the program at your own risk! I take no responsibility for
 any damage it may cause to your system.
