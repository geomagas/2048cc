2048cc GTK2 Replayer - Sources
==============================

**Initial author**: [migf1](mailto:mig_f1@hotmail.com)  
**Latest version**: 0.2a (August 7, 2014)  
**License**: Open-source, Free-software, with the limitations specified in the
README.md file at the root directory of the project.

Copyright (c) 2014 [migf1](mailto:mig_f1@hotmail.com)  

Before anything else, please let me say from the very beginning that the
source code of the GTK2 Replayer is still in 1st-draft state. I will be
gradually cleaning it up, adding appropriate comments, and removing outdated
ones.

This file is provided for helping you compile the sources yourselves on Windows
and Linux (and possibly giving you hints for other platforms too, like MacOSX
or Unix).

Prerequisites
=============

The **2048cc GTK2 Replayer** is written in ISO C99 using the GTK+2.24.1 API.
The sources are meant to be cross-platform across at least Windows and Linux.

All you need is a properly installed `GTK+2.24.10 developing environment` and
a `ISO C99 aware compiler` utilizing it (I use & recommend either `gcc` or
`mingw-gcc`).

The replayer is being developed on 32-bit Windows XP SP3, and tested on 64-bit
Windows 7 Home Edition, and on 32-bit Ubuntu 12.10.

<a name="gtk2_devel"></a>
Installing the GTK+2 Development Environment
--------------------------------------------

In order to be able to compile any GTK+2 application (thus, this one too),
the first thing you need is a properly installed GTK+2 development environment
on your system.

**Linux**

Most Linux distros come with the GTK+2 runtime pre-installed, so you can run
GTK+ applications. But some distros do *not* include by default the GTK+2
development environment, needed for writing GTK+ applications. Unfortunately,
the commands for manually installing it differ from distro to distro. If your
distro lacks the GTK+2 developing environment, you may need to do some
googling.

On Debian (e.g. Ubuntu), you can open a terminal and type: `sudo apt-get install libgtk2.0-dev`  
On Fedora (e.g. Redhat), you can open a terminal and type: `su -c 'yum install gtk2-devel'`

  >To compile the sources of this project you need at least GTK+2.24.10
  >(GTK+2.24.8 may also work).

**Windows**

On Windows, the installation is more cumbersome. Here are some simple steps on
how to do it, that work fine for me:

1. Download the *GTK+2.24.10 all-in-one-bundle for 32-bit Windows* 
[zip file]( http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip ) (the link is taken directly from the official [GTK+ website]( http://www.gtk.org/download/win32.php )).

2. Extract the contents of the zip-file into any folder you like, but **make sure
that its full path does not contain any spaces**. A good candidate folder
would be `C:\gtk2`. The rest of my instructions will assume this folder.

3. Next you need to create the following environment variables: 

        GTK_HOME = C:\GTK2  
        PKG_CONFIG_PATH = C:\GTK2\LIB\pkgconfig  

  >HINT
  >
  >If you don't know how to create/modify environment variables, it's quite easy:
  - **Windows XP/Vista/7**: right-click on `My Computer` and then select:  
  `Properties`->`[Advanced]`->`Environment variables...`
  > 
  - **Windows 8/8.1**: Please read [this link]( http://www.7tutorials.com/simple-questions-what-are-environment-variables ).  
  >  
  >  
  >A window shows up, containing `User variables` and `System variables`.
  >The former affect the current user, while the latter affect all users
  >on your machine.
  >  
  >  
  >NOTE  
  >My advice is to work your additions/modifications in the `User variables`,
  >thus reducing the risk of screwing up all users if something goes wrong.

4. Add the folder with the GTK+2 binaries `C:\GTK2\bin` to the `PATH` environment
variable. Here is [a link]( https://www.java.com/en/download/help/path.xml )
explaining how to create/modify the `PATH` environment variable on all versions
of Windows.

  >NOTE  
  >  
  >The article of the above mentioned link, shows how to tweak the `System
  >variables`. For the reasons I explained in step 3, I much prefer to work
  >with `User variables` instead.

For the steps 3 and 4, if any of the mentioned environment variables already
exists on you system, then **DO NOT** delete their previous contents (especially
if you have decided to go against my advice and you are tweaking the `System
variables`).

Instead, add your changes before or after the existing contents of each environment
variable, using the semi-colon (;) as the delimiter character. To make sure that
your changes will take priority over the previous contents, put them before the
old contents).

Finally, in case you have a command-prompt window open, close it and re-open it
so the changes take effect.

**Mac OSX**

Unfortunately I don't have access to this platform, thus I don't have first-hand
experience on how to install the GTK+2 development environment. I do know though
that in the past it was a real pain. However, according to the
[Gnome GTK-OSX wiki]( https://wiki.gnome.org/Projects/GTK%2B/OSX/Building ),
things may be better these days. The article in the above mentioned link seems
like putting some serious effort into explaining how to build a GTK+ development
environment on OSX. So, if you are interested or just curious, reading it should
be a good starting point.

<a name="gcc_toolchain"></a>
Installing a GCC/MinGW-GCC Tool Chain
-------------------------------------

The `GCC tool-chain` or any of its `MinGW variants` are recommended for
compiling the sources of the **2048cc GTK2 Replayer**. This does not mean that
you cannot use any other C99 aware tool-chain. It only means that this guide
focuses mainly on the above mentioned tool-chains.

**Linux**

Most (if not all) Linux distros come with the `GCC tool-chain` pre-installed.
Unless it is a very old version that does not support the ISO C99 standard
(that is, older than version 4.5), that's all you need. Otherwise please
consider upgrading it.

Your distro may provide the `MinGW/MinGW-64 GCC` tool-chain instead of the
`GCC` one. This is just fine, as long as it supports ISO C99.

**Windows**

On Windows, there are currently 3 popular implementations of the GCC tool-chain:

1. [MinGW]( http://sourceforge.net/projects/mingw/?source=navbar )
2. [MinGW-w64]( http://sourceforge.net/projects/mingw-w64/?source=navbar )
3. [TDM-GCC MinGW]( http://sourceforge.net/projects/tdm-gcc/ ).

The latter two can built 32-bit and 64-bit executables, while the former can
build 32-bit executables.

Another option is the [Cygwin project]( https://cygwin.com/install.html ),
which implements a Posix layer on top of Windows, but I think it is a bit too
much to go that route, just for compiling the GTK2 Replayer (unless of course
you already have Cygwin installed in your system).

It's beyond the scope of this documentation to provide a step-by-step
installation guide for the above tool-chains.

However, I can express my opinion that the installation of the original `MinGW`
tool-chain (the 1st one listed above, and the one I use) is the most hassle free
one, if you are not familiar with that kind of stuff. The `TDM-GCC MinGW`
tool-chain comes as a close second.

The `MinGW-w64` tool-chain is not really difficult to install, once you figure
out [which distribution]( http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/ ) you should download. For building 32-bit applications, chances are you will
want either the *...posix-sjlj...* or the *...win32-sjlj...* variant, depending
on which threads-model you are interested in. Please keep also in mind that this
tool-chain is the most actively developed, and it is the default in Cygwin for
quite some time now.

**Since the [GTK+2 64-bit Windows binaries]( http://www.gtk.org/download/win64.php )
are still marked as _"Experimental"_ I advice against using them. Consequently,
I also advice against building 64-bit GTK+ applications.**

  NOTE
  >No matter which one of the above mentioned tool-chains you choose to install
  >(btw, they all provide convenient installers) an important thing is that
  >they all come with **`MSYS`**, a bash-like shell that is way more advanced
  >than the Windows' native command-prompt.
  >
  >Among lots of other things, it can interpret backticks, which as I explain
  >in the following section *["Compiling GTK+2 Applications"]( #gtk2_compile )*
  >it can really help us simplifying the compilation of any GTK+2 application.

<a name="gtk2_compile"></a>
Compiling GTK+2 Applications
============================

GTK+ is a bit of a beast, with many dependences on other libraries, stand-alone
or not (glib, cairo, pango, atk, etc). As a result, the compiler must be aware
of quite a few include & library directories in order to successfully produce
a GTK+ executable.

Provided you have already installed the GTK+2 development environment on your
system, as described in the section *["Installing the GTK+2 Development
Environment"]( #gtk2_devel )*, you can open a terminal or a command-prompt window and type the
following 2 lines:

    pkg-config --cflags gtk+-2.0  
    pkg-config --libs gtk+-2.0

The 1st line will display all the directories to be searched for including the
needed header files. If you are on Windows, the output should be quite similar
to this:

  >-mms-bitfields -IC:/gtk2/include/gtk-2.0 -IC:/gtk2/lib/gtk-2.0/include -IC:/gtk2/include/atk-1.0 -IC:/gtk2/include/cairo -IC:/gtk2/include/gdk-pixbuf-2.0 -IC:/gtk2/include/pango-1.0 -IC:/gtk2/include/glib-2.0 -IC:/gtk2/lib/glib-2.0/include -IC:/gtk2/include -IC:/gtk2/include/freetype2 -IC:/gtk2/include/libpng14

Similarly, the 2nd line will display all the need libraries:

  >-LC:/gtk2/lib -lgtk-win32-2.0 -lgdk-win32-2.0 -latk-1.0 -lgio-2.0 -lpangowin32-1.0 -lgdi32 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl

Those `C:/gtk2/`'s will be different for you if you have installed GTK+2 into a
folder other than C:\gtk2

To make a long story short, the `pkg-config` utility is available on Linux,
but on Windows it gets installed when installing the GTK+2 development
environment. It can be called directly from the compilation command-line
of any C tool-chain that understands the `-I`, `-L` and `-l` command-line
options. The most popular such C tool-chains are: `GCC` (including its several
`mingw` variants) and `clang`.

Providing a shell capable of interpreting backticks is available, compiling a
GTK+2 *hello.c* source file using `gcc` and the `pkg-config` utility, is greatly
simplified into the following line:

    gcc hello.c `pkg-config --cflags --libs gtk+-2.0`

As described in the section *["Installing a GCC/MinGW-GCC Tool Chain"]( #gcc_toolchain )*,
on Windows you need a replacement of the native command-prompt, capable of
interpreting backticks (such as **`MSYS`**).

For more information, please refer to the [official GTK+2 documentation]( https://developer.gnome.org/gtk2/2.24/gtk-compiling.html ).


Compiling the 2048cc GTK2 Replayer sources
==========================================

Assuming that the [GTK+2 development environment]( #gtk2_devel ) and a
[GGC/MinGW tool-chain]( #gcc_toolchain ) are both  properly installed on your
system (as described in the previous sections), you may compile the sources by
following the instructions presented below.

Please keep in mind that the `gui` directory must be in the same path with the
sources. Otherwise the compilation will fail. As a side note, the `gui` directory
must also be in the same path with the produced executable (actually, that's
the reason why I have included it twice in the distribution zip-file).

On Linux
--------

Open a terminal, navigate to the main directory of the GTK2 Replayer, and type:

    cd src
    gcc -std=c99 -O3 *.c `pkg-config --cflags --libs gtk+-2.0`

The executable file `a.out` will be created. You may run it by typing: `./a.out`

On Windows
--------

**Using MSYS**

Open an `MSYS` window, navigate to the main directory of the GTK2 Replayer,
and type:

    cd src
    gcc -std=c99 -O3 *.c -o gtk2_player.exe `pkg-config --cflags --libs gtk+-2.0`

The executable file `gtk2_player.exe` will be created. You may run it by typing:
`./gtk2_player.exe`  (or by double-clicking on its icon).

**Using the native command prompt**

You may also compile the sources from the Window's native command-prompt, by
adding manually to the command-line of `gcc` the output of the `pkg-config`
utility, as described in the section *["Compiling GTK+2 Applications"]( #gtk2_compile )* .

However, a better approach is to store permanently the output of the `pkg-config`
utility to one or two environment variables, and use those instead.

For example, assuming you have installed GTK+2 in the directory: `C:\gtk2`,
you can define just once an environment variable like the following (or split
it in 2 envrironment variables, say: GTK\_CFLAGS and GTK2\_LIBS):

  >GTK2\_COMPILE = -mms-bitfields -IC:/gtk2/include/gtk-2.0 -IC:/gtk2/lib/gtk-2.0/include -IC:/gtk2/include/atk-1.0 -IC:/gtk2/include/cairo -IC:/gtk2/include/gdk-pixbuf-2.0 -IC:/gtk2/include/pango-1.0 -IC:/gtk2/include/glib-2.0 -IC:/gtk2/lib/glib-2.0/include -IC:/gtk2/include -IC:/gtk2/include/freetype2 -IC:/gtk2/include/libpng14 -LC:/gtk2/lib -lgtk-win32-2.0 -lgdk-win32-2.0 -latk-1.0 -lgio-2.0 -lpangowin32-1.0 -lgdi32 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl

You may now compile the sources of the **2048cc GTK2 Replayer** directly from
the Windows' native command-prompt. Navigate to the main directory of the
replayer, and type:

    cd src
    gcc -std=c99 -O3 *.c -o gtk2_player.exe %GTK2_COMPILE%

The executable file `gtk2_player.exe` will be created. You may run it by typing:
`gtk2_player.exe` (or by double-clicking on its icon).

  NOTE
  >If you don't know how to define environment variables, please see the *Windows*
  >paragraph, in the section *["Installing the GTK+2 Development Environment"]( #gtk2_devel )*.
