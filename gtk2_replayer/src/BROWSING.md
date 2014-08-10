2048cc GTK2 Replayer - Source Browsing
=======================================

**Project**: 2048cc GTK2 Replayer  
**Initial author**: [migf1](mailto:mig_f1@hotmail.com)  
**Latest version**: 0.2a (August 7, 2014)  
**License**: Open-source, Free-software, with the limitations specified in the
README.md file at the root directory of the project.

Copyright (c) 2014 [migf1](mailto:mig_f1@hotmail.com)  

Before anything else, please let me say from the very beginning that the
source code of the GTK2 Replayer is still in 1st-draft state. I will be
gradually cleaning it up, adding appropriate comments, and removing outdated
ones.

This document is meant as a starting point to help you browsing through the
source-code. I may be also be useful if you decide to contribute to the project,
or fork it for independent development.

Classes
=======

Except for the files: `main.c`, `misc.c` and `text.h`, you may consider every
other source-module as what is called "**a class**" in OOP languages (well, a
rather simplified form of it).

A "class" consists of a private `.c` **implementation** file, and a public `.h`
**interface** file.

Briefly, the private definition of a "class" (that is, its internal *struct*)
is exposed publicly as an *opaque data-type*, along with a collection of functions
that expect as their 1st argument *a pointer of the opaque data-type* and they
manipulate the *pointed data* (object).

For a concrete example, have a look at the files `gamedata.c` (implementation)
and `gamedata.h` (interface). The `Gamedata` "class" is defined privately in the
`*.c` file, and it is then exposed publicly with a forward declaration as an
opaque data-type, in the `*.h` file.

  NOTE  
  > Sometimes I may refer to the terms "class" and "source module"
interchangeably, in the rest of this document.

Files
=====

The source-code files can be roughly categorized as `gui` and `non-gui` related.

gui files
--------- 
Files having a `gui` prefix in their names are mostly dealing with the GTK+2
graphical-user-interface.

The idea here is that only the `gui.c` file can use public functions defined
in any other `gui_*.c` file.  
On the other hand, the public functions defined in `gui.c` can be used in any
of the `gui_*.c` files.

  > Put otherwise, the `gui_*` source-modules can communicate with each other
**only** via the `gui` source-module.

This makes the code more verbose, but it helps in maintaining it as it grows.

Please keep in mind that most of the `gui_*` source-modules and the `gui`
source-module, do **not** encapsulate generic, reusable GTK+2 abstractions.
Their encapsulated abstractions are mostly very specific to this particular
project.

non-gui files
-------------

Files **not** having a `gui` prefix in their names, usually deal with anything
else but the graphical-user-interface.

**main.c**

The `main()` function separates the core-data from the GUI, by creating a
`Gamedata` and a `Gui` object, and passing the former as an argument to the
constructor of the latter. It then "instructs" the `Gui` object to start,
letting it take care of the rest. When the `Gui` object stops its gui-loop
and returns, the `main()` function cleans up and exits.

**gamedata.c/.h**

Private implementation and public interface of the `Gamedata` "class". The
existence of the `Gamedata` "class" is an attempt to conceptually separate
the GUI from the core-data of the main game.

For this stand-alone GTK+2 Replayer this is a bit of a stretch, because the
actual game-data do NOT pre-exist. They are loaded from replay-files, from
within the GUI.

Nevertheless, it does help to at least separate core-data from the GUI
graphical elements (widgets) and their inner structures. The GUI is not
allowed to directly manipulate the loaded game-data. It can only do so
via the public interface of the `Gamedata` "class".

**misc.c/.h**

This is a small collection of somewhat generic constants, macros and functions,
to be used across the project.

**text.h**

This header file contains all the translatable strings of the project.
Initially it was meant to host translatable strings of both `gui` and `non-gui`
related files, but it turned out that the latter do not output any text at all.
Most probably, the file-name will be changed to something like `gui_text.h` or
`gui_strings.h` in the future.

Coding Style Conventions
========================

I admit that the coding style of the sources is not as consistent as it could be.
However, it is not that bad either.

In general, I'm influenced by Jim Larson's suggested <a href="http://www.jetcafe.org/jim/c-style.html" target="_blank" title="external link">Standards and Style for Coding in ANSI C</a>, but not quite entirely. I'm also fond of quite a few styles suggested in the <a href="https://www.kernel.org/doc/Documentation/CodingStyle" target="_blank" title="external link">Linux kernel coding style</a> and the <a href="https://help.gnome.org/users/programming-guidelines/stable/c-coding-style.html.en" target="_blank" title="external link">Gnome C Coding Style</a>.

Nevertheless, I use them neither blindly, nor as a whole.

Here is a non-exhausted guide for the coding-style used in the sources, which
will hopefully help you browsing through them.

  > If you are willing to contribute to the project, you are **not** strictly
obliged to follow the suggested coding-style. However, **please** try
to respect at least the following suggestions:
  >
  > - [at most 79 characters per line](#line_max_length)
  > - [lines indentation](#lines_indent)
  > - [pointer definitions & declarations](#pointer_definitions)
  > - [bracing](#compound_braces)

General conventions
--------------------

- Anything starting with an **underscore** is meant to be local to the current
file (or, much rarely, to the current scope). It may be a function, a macro, a
constant, a custom data-type, or whatever.

- In general, local functions do not perform **sanity checks** on their arguments,
but publicly available functions **do**.

- **Forward declaring prototypes** of locally defined functions is avoided.
This means that local functions are defined prior to their usage. If this gets
too complicated (it rarely does), then I forward declare the prototypes of
as few functions as possible.

- **Declaring public functions as `extern`** within the source module that
defines them, is also avoided. To accomplish this, I define a pre-processor
directive in the `.c` source file, which I then use in the corresponding `.h`
header file, as following:

  misc.c:
  ```cpp
  /* at the start of the source file: misc.c */
  #define MISC_C
  ...
  /* public function definitions */
  void dbg_print_info( char *fmtxt, ... ) { ... };
  int  int_count_digits( int num ) { ... };
  char *s_fgets( char *s, int n, FILE *fp ) { ... };
  char *s_new_shortfname( const char *s ) { ... };
  ...
  ```

  misc.h:
  ```cpp
  /* then in the corresponding header file: misc.h */
  ...
  #ifndef MISC_C
  extern void dbg_print_info( char *fmtxt, ... );
  extern int  int_count_digits( int num );
  extern char *s_fgets( char *s, int n, FILE *fp );
  extern char *s_new_shortfname( const char *s );
  ...
  #endif
  ...
  ```

Comments
--------

**C89/C90 comments** are preferred everywhere. They are pretty much used as
suggested by <a href="http://www.jetcafe.org/jim/c-style.html#Comments" target="_blank" title="external link">Jim Larson</a>.

```cpp
/* single line comments look like this */

/*
 * Important single line comments look like multi-line comments.
 */

/*
 * Multiline comments look like this.  Put the opening and closing
 * comment sequences on lines by themselves.  Use complete sentences
 * with proper English grammar, capitalization, and punctuation.
 */

/* but you don't need to punctuate or capitalize one-liners */
```

C99/C11 comments are mostly used for temporarily excluding lines from compilation,
before deciding on keeping or permanently deleting them.

For example:

```cpp
...
if ( gui->isPlayPressed ) {
//        gui->isPlayPressed = FALSE;
        gui->isStopPressed = TRUE;
//        while( gtk_events_pending() ) {
//                gtk_main_iteration();
//        }
}
...
```

Naming conventions
------------------

- Anything starting with an **underscore** is meant to be local to the current
file (or, much rarely, to the current scope). It may be a function, a macro, a
constant, a custom data-type, or whatever.

- **Custom data-types** have only their 1st letter capitalized, unless their name
consists of two or more long words. In that case, only the 1st letter of each
word is capitalized.

  Examples:
  ```cpp
  /* local custom data-type */
  struct _Gamedata {
          ...
  };

  /* public custom data-type */
  struct Gamedata {
          ...
  };

  /* typdefed public custom data-type */
  typedef struct _GuiMenuItem GuiMenuItem;
  struct _GuiMenuItem {
          ...
  } GuiMenuItem;
  ...
  ```
<a name="var_names"></a>
- **Variable and `struct` field names** start with lowercase letters, and
most of the time they are written in lowercase only, even if they consist
of more than one words. However, if they get too long or too ambiguous,
then each word after the first one has the 1st letter capitalized.

  Example:
  ```cpp
  struct _Gui
  {
          gint wmax, hmax;  /* width & height of user's full-screen */

          gboolean     quitOnDestroyAppWindow;
          GuiLocale    *locale;
          GtkWidget    *appWindow;
          GuiMenus     *menus;
          GuiBoard     *board;
          GuiStatusbar *statusbar;
          GuiDialogs   *dialogs;
          GuiSlider    *sliderPlayback;
          ...
  };
  ```

- **Function names** are written in lowercase. If they consist of more than one
words, then each word after the first one is separated with an underscore.

  Example:
  ```cpp
  ...
  gamedata_set_from_fname( gui->gamedata, fname );
  ...
  ```

- Names of **pre-processor constants & macros**, and names of **enumerated values**
are written in uppercase. However, names of **constant variables** may follow
either this rule, or the one for the [variable names]( #var_names ).

- Names of **global variables** are prefixed with either `global_` (when they
are meant to be accessed across multiple source modules) or `_global_` (when
they are meant to be accessed only within the source-module that defines them).

  Example:
  ```cpp
  ...
  /* global across multiple files */
  bool global_debugOn;

  /* global to this file only */
  static char  *_global_daylabels[] = {
          "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
  }
  ...
  /* function definitions go after this point */
  ...
  ```

Bracing, Spacing & Indentation
------------------------------

- <a name="compound_braces"></a>**Compound statements** define their blocks with opening and closing braces,
even if they contain a single statement. Compound statements are separated from
the rest of the code with one blank line after them, unless related code makes
sense to be visually grouped with them.

  **Bracing** for compound statements follows <a href="http://en.wikipedia.org/wiki/Indent_style#Variant:_Stroustrup" target="_blank" title="external link">Stroustrup's suggestion</a> (but parentheses
do not, see [compound statement parentheses spacing](#compound_parentheses_spacing) for details).

  However, **bracing for function definitions** follows <a href="http://en.wikipedia.org/wiki/Indent_style#Allman_style" target="_blank" title="external link">Allman's suggestion</a> (for argument and
parameter lists, please see [Spacing](#spacing)).

  The following code snippet shows 2 cases where it makes sense to visually
group `gamedata` and `gui` with their validation `if`-statements:

  ```cpp
  ...

  /* gamedata to be passed to the gui */
  gamedata = make_gamedata();
  if ( NULL == gamedata ) {
          DBG_STDERR_MSG( "*** (fatal error) gamedata not inited!" );
          goto cleanup_and_exit_failure;
  }

  /* spawn the GTK+2 gui */
  gui = new_gui( &argc, &argv, gamedata );
  if ( NULL == gui ) {
          DBG_STDERR_MSG( "(fatal error) gui not inited!" );
          goto cleanup_and_exit_failure;
  }

  ...
  ```

- <a name="spacing"></a>**Spacing** is used generously, being careful not to over-do it.
Contrary to interpreted/scripted languages, compiled languages do not suffer
from runtime overhead due to excessive spacing.

  - *Operators* have a space before and a space after them, unless the
expression is too long (see the [Note](#spacing_note), below).

  - <a name="pointer_definitions"></a>*Pointer definitions & declarations* have the dereferencing symbol prepended
to the pointer name, not appended to the pointer type.
    ```cpp
    int *p; // not int* p;
    ```

  - *Function names* are **not** separated from the opening parenthesis of their
argument & parameter lists.
    ```cpp
    int foo( void )   // preferred
    int foo ( void )  // avoid this, it will bite you if foo is a macro
    ```

  - *Argument & parameter lists* have a space after their opening parenthesis,
and a space before their closing parenthesis, unless they are too long (see the
[Note](#spacing_note), below), or the function call is part of a complex expression.

  - *Arguments & parameters* are separated with a comma and a space, unless
they are too long (see the [Note](#spacing_note), below).

  - *Definition blocks of variables* are separated with 1 blank line from
the code that follows them.

  - *Compound statement starting-keywords* such as `if`, `else if`, `switch`,
`for`, `while`, `do`, etc, are followed by a space (the `switch` statement
may be exempted).

      <a name="compound_parentheses_spacing"></a>Furthermore, if they utilize parenthesized controlling expressions, then there
is a space after the opening parenthesis, and a space before the closing parenthesis
(the `for` statement may be exempted) unless they are too long (see the [Note](#spacing_note), below).

  <a name="spacing_note"></a>NOTE
  > Most of the above rules change if they violate the [at most 79 characters per line](#line_max_length)
rule. Follow the link for examples.

  The following sample snippet demonstrates some of the spacing rules described
above:

  ```cpp
  #include <stdio.h>
  #include <stlib.h>
  #include <string.h>
  
  int main( void )
  {
          char *s1 = NULL;
          char s2[ BUFSIZ ] = {'\0'};
  
          /* make & init s1 */
          if ( NULL == (s1 = calloc(BUFSIZ, 1)) ) {
                  puts( "*** error: calloc() failed, bye... );
                  exit( EXIT_FAILURE );
          }
          strncpy( s1, "spell me out if equal\n", BUFSIZ-1 );
  
          /* read s2 from stdin */
          if ( NULL == fgets(s2, BUFSIZ, stdin) ) {
                  puts( "*** error, bye..." );
                  free( s1 );
                  exit( EXIT_FAILURE );
          }
  
          /* spell it out if equal */
          if ( 0 != strcmp(s2, s1) ) {
                  puts( "not equal" );
          }
          else {
                  size_t len = strlen( s2 ) - 1;  /* exclude '\n' */
                  for (size_t i=0; i < len; i++) {
                          printf( "%c\n", s2[i] );
                  }
          }
  
          /* cleanup & exit */
          free( s1 );
          exit( EXIT_SUCCESS );
  }
  ```

- As already shown with the `calloc()`, `fgets()` and `strcmp()` functions in
the snippet above, **constants are put on the left hand side of equality checks**.

- <a name="lines_indent"></a>**Lines indentation** is done with tabs (not spaces), set to 8 characters per
tab. Any further indentation within a line (e.g. for an inline comment) is done
with spaces.

  Example:
  ```cpp
  int main( void )
  {
          puts( "hello world" );
          return 0;      /* line indented with a tab, but comment with spaces*/
  }
  ```

- <a name="line_max_length"></a>Great effort has been put in keeping all lines **shorter than 80 characters**.
This kinda contradicts with the long function names, with are in turn the result
of using the OOP "classes" approach (the GTK+2 API does not help, either). So,
quite often the code contains snippets like the following:

  Example 1:
  ```cpp
  /* toolNext */
  if ( gui_has_imove_next( gui )
  && !gui_get_isPlayPressed( gui )
  ){
          gtk_widget_set_sensitive(
                  gui->playback.toolNext,
                  TRUE
                  );
          gtk_widget_set_sensitive(
                  gui->playback.toolLast,
                  TRUE
                  );
  }
  ```

  Example 2:
  ```cpp
  gboolean gui_set_slider_playback_value_limits(
          Gui    *gui,
          gdouble minVal,
          gdouble maxVal
          )
  {
          if ( NULL == gui ) {
                  DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
                  return FALSE;
          }

          return gui_slider_set_value_limits(
                  gui->sliderPlayback,
                  minVal,
                  maxVal
                  );
  }
  ```

  Example 3:
  ```cpp
  ...
  GtkWidget **container = &gui->playback.ebContainer;
  
  (*container)
  = GTK_WIDGET( gtk_builder_get_object(builder, "ebPlaybackContainer") );
  ...
  ```

  Example 4:
  ```cpp
  ...
  #define TXT_ERR_INVALID_JUMPTO                           \
  _(                                                       \
          "The requested move was invalid! \n"             \
          "\n"                                             \
          "Perhaps it was out of range,\n"                 \
          "or not a numeric value at all."                 \
  )
  ...
  ```

  The above examples summarize the preferred way of tackling lines longer
than 79 characters. They involve cases such as:

 - *compound statements*
  - *function definitions*
  - *function declarations*
  - *function calls*
  - *conditionals*
  - *assignments*
  - *string literals* (their shown splitting requires a C99 aware compiler).

  Remaining cases not shown in the examples, are tackled in similar ways.

  They may look a bit odd at first, but they really help in adding, removing
and/or commenting out single entities (e.g. conditions, parameters, etc).
