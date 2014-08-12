#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdbool.h>

typedef struct _GamedataMove GamedataMove;
typedef struct _Gamedata Gamedata;

#ifndef GAMEDATA_C
extern Gamedata      *make_gamedata( void );
extern Gamedata      *gamedata_free( Gamedata *gd );

extern char          *gamedata_get_fname( Gamedata *gd );
extern int           gamedata_get_didundo( Gamedata *gd );
extern long int      gamedata_get_nmoves( Gamedata *gd );
extern int           gamedata_get_dim( Gamedata *gd );
extern int           gamedata_get_sentinel( Gamedata *gd );
extern int           gamedata_get_nrandom( Gamedata *gd );
extern unsigned long gamedata_get_delay( Gamedata *gd );
extern int           gamedata_get_gamewon( Gamedata *gd );
extern long int      gamedata_get_tile_of_move(
                           Gamedata *gd,
			   int      itile,
			   long int imove
			   );
extern long int      gamedata_get_score_of_move(
                           Gamedata *gd,
                           long int imove
                     );
extern long int      gamedata_get_bestscore_of_move(
                           Gamedata *gd,
                           long int imove
                     );
extern int           gamedata_get_prevmv_of_move(
                           Gamedata *gd,
                           long int imove
                           );
extern int           gamedata_get_nextmv_of_move(
                           Gamedata *gd,
                           long int imove
                           );


extern void          dbg_gamedata_print_tiles_of_move(
                           Gamedata *gd,
                           long int imove
                           );
extern void          dbg_gamedata_print_tiles( Gamedata *gd );

extern bool          gamedata_set_from_fname(
			   Gamedata *gd, const char *fname
			   );
#endif

#endif
