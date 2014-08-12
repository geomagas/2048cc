#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdbool.h>

typedef struct _GameDataMove GamedataMove;
typedef struct _Gamedata Gamedata;

#ifndef GAMEDATA_C
extern Gamedata *new_gamedata( void );
extern Gamedata *gamedata_free( Gamedata *gd );

extern char          *gamedata_get_fname( Gamedata *gd );
extern int           gamedata_get_didundo( Gamedata *gd );
extern long int      gamedata_get_nmoves( Gamedata *gd );
extern int           gamedata_get_dim( Gamedata *gd );
extern int           gamedata_get_sentinel( Gamedata *gd );
extern int           gamedata_get_nrandom( Gamedata *gd );
extern unsigned long gamedata_get_delay( Gamedata *gd );
extern int           gamedata_get_gamewon( Gamedata *gd );

extern bool          gamedata_set_from_fname(
			Gamedata *gd, const char *fname
			);
#endif

#endif
