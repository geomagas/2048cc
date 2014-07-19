2048cc
======

**Author** : migf1 <mig_f1@hotmail.com>
**Version**: 0.3a3
**Date**   : July 18, 2014

Console clone of the game 2048 ( http://gabrielecirulli.github.io/2048/ )

2048 is written in ISO C99 by migf1 (http://goo.gl/b0AZG5). It is meant
to be cross-platform across Windows, Unix, Linux and MacOSX (for the last 3
you should enable ANSI-colors support on your terminal emulator).

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

The included zip-file contains a pre-compiled 32bit executable file
for Windows (XP or newer). It is called: *2048cc_win_x86.exe*.

On other platforms, you can compile the sources. See the next section.

How to compile the game
------------------------

To compile with the *gcc tool-chain* follow these simple steps:

1. Extract the zip file anywhere you want in your hard-drive.
2. From the command-line, navigate into the *src/* folder.
3. On Windows (e.g. with the *mingw gcc tool-chain*) type:
   `gcc -std=c99 -s -O3 -D_BSD_SOURCE *.c -o 2048cc.exe`

   On Unix/Linux/MacOSX type:
   `gcc -std=c99 -s -O3 -D_BSD_SOURCE *.c -o 2048cc.out`

   Recent versions of MacOSX do not include the *gcc tool-chain* by
   default, so you may need to install it manually. Please read the
   following thread for instructions: http://stackoverflow.com/questions/9353444/how-to-use-install-gcc-on-mac-os-x-10-8-xcode-4-4

4. See the next section on how to run the executable file.

To compile from within an IDE (with any compiler) follow these steps:

1. Extract the zip file anywhere you want in your hard-drive.
2. Create a new console/terminal project (you may call it: 2048cc).
3. Add to the project all the .c files found inside the *src/* folder.
4. Enable *C99 support* on your compiler.
5. Predefine the directive `_BSD_SOURCE` (only tested with gcc).
6. Build a Release version of the project.
7. See the next section on how to run the executable file.

How to run the game
-------------------

After the compilation is done, move the executable file (either *2048cc.exe*
or *2048cc.out*... see above) out of the *src/* folder.

Actually the executable file should be in the same path with the *replays/*
folder, otherwise you will not be able to *load/save* replays.

Run the executable, enjoy the game and please report any bugs you may find.

Game-play
---------

There are actually 2 modes: normal and replay-mode.

**Normal-mode**

This is the default mode, the one used for playing the game. Use the `arrow-keys`
on your keyboard to make a move on the board. Use any key displayed with a
*right parenthesis* after its name in the *Main Menu*, to issue the corresponding
menu-command. For example, the menu-command `S)kin` is issued by hitting the `S`
key on your keyboard (this particular command cycles through the available color
skins of the game).

**Undo/Redo**

You may undo one or more moves at any given time, but the penalty will be that
**best-score** tracking gets disabled. Re-doing undone moves will **not** re-establish
best-score tracking. The moves-counter on the lower part of the screen displays
the count of the current move. In case one or more Undo has been done, the
counter also displays the count of available moves to be redone.

**Replay-mode**

This mode is entered by issuing the `Rep)lay` command, in the *Main Menu*. Once
inside the replay-mode, a new set of commands is displayed on the lower part
of the screen. They are used for **navigating** inside the current replay, for
**saving/loading** replay files, and for **returning back** to the main menu.

When entering the replay-mode (or when loading a replay-file) the replay is
automatically re-winded to the very 1st move. Then you can either navigate
manually, or let it **play automatically** (in that case you will **not** be able to
stop the auto-play... you'll have to wait until it is finished).

Replays do **not** take into account any Undone moves! That is, the last move in
a replay is the one corresponding to your last *Undo*. However, they do display
any available moves to be Redone (when exiting the replay-mode).

While viewing a replay, you can see on the lower-right part of the screen what
move was applied to the board in the **previous move**, along with what move was
applied to the current move for yielding the **next move**. For example, when you
see **UP|RIGHT**, it means that in the previous move the board was played upwards,
and that for the next move the board was played toward the right side.

Exiting the replay-mode, sets the game to the last recorded move of the replay,
without counting any Undone moves (but, if any, their count will be displayed
and you may Redo them if you want to... best-score tracking will be disabled of
course).

**Load/Save Replay Files**

The replay-mode may also be used for saving **unfinished games**, and load them at
a later time to finish them. That's because when loading a replay-file, it
overwrites completely the current state of the game.

In order to be able to **save and load replay-files**, the executable program MUST
be in the same path with the *replays/* folder. This is hard-coded into the game
and most probably it will stay that way until I or someone else implements
either a proper GUI, or a a more advanced text-user-interface (using ncurses for
example).

With its current primitive text-user-interface, the game doesn't even allow the
user to specify a name when saving a replay-file. Instead, a **pre-defined name**
is generated automatically, using a timestamp from the system clock.

It is of the form **Day_Month_DD_HHMMSS_Year.sav** and it is automatically saved
in the *replays/* folder. Before loading a replay-file, the contents of the
"replays/" folder are listed, and the user has to type-in the name of the file
he wishes to load (but **without** typing the *replays/* folder).

To make your life easier, consider renaming manually any replay-files you have
saved, before attempting to load them from within the game.

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
