#ifndef LANGBAND_H
#define LANGBAND_H

/*
 * DESC: langband.h - langband-related defined/includes/etc.
 * Copyright (c) 2000-2002 - Stig Erik Sand�

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef INTERFACE
#if defined(WIN_MAKEDLL)
#  define INTERFACE __declspec(dllexport)
#elif defined(WIN_USEDLL)
#  define INTERFACE __declspec(dllimport)
#else
#  define INTERFACE
#endif
#endif /* iface */



/*
 * Extract the "SUNOS" flag from the compiler
 */
#if defined(sun)
# ifndef SUNOS
#   define SUNOS
# endif
#endif

/*
 * Extract the "ULTRIX" flag from the compiler
 */
#if defined(ultrix) || defined(Pyramid)
# ifndef ULTRIX
#  define ULTRIX
# endif
#endif

/*
 * Extract the "ATARI" flag from the compiler [cjh]
 */
#if defined(__atarist) || defined(__atarist__)
# ifndef ATARI
#  define ATARI
# endif
#endif

/*
 * Extract the "ACORN" flag from the compiler
 */
#ifdef __riscos
# ifndef ACORN
#  define ACORN
# endif
#endif

/*
 * Extract the "SGI" flag from the compiler
 */
#ifdef sgi
# ifndef SGI
#  define SGI
# endif
#endif

/*
 * Extract the "MSDOS" flag from the compiler
 */
#ifdef __MSDOS__
# ifndef MSDOS
#  define MSDOS
# endif
#endif

/*
 * Extract the "WINDOWS" flag from the compiler
 */
#if defined(_Windows) || defined(__WINDOWS__) || \
    defined(__WIN32__) || defined(WIN32) || \
    defined(__WINNT__) || defined(__NT__)
# ifndef WINDOWS
#  define WINDOWS
# endif
#endif

/*
 * Remove the MSDOS flag when using WINDOWS
 */
#ifdef WINDOWS
# ifdef MSDOS
#  undef MSDOS
# endif
#endif

/*
 * Remove the WINDOWS flag when using MACINTOSH
 */
#ifdef MACINTOSH
# ifdef WINDOWS
#  undef WINDOWS
# endif
#endif



/*
 * OPTION: Define "L64" if a "long" is 64-bits.  See "h-types.h".
 * The only such platform that angband is ported to is currently
 * DEC Alpha AXP running OSF/1 (OpenVMS uses 32-bit longs).
 */
#if defined(__alpha) && defined(__osf__)
# define L64
#endif

#if !defined(NeXT) && !defined(__MWERKS__) && !defined(ACORN)
# include <fcntl.h>
#endif


#include <stdio.h>
#include <ctype.h>

#if defined(NeXT)
# include <libc.h>
#else
# include <stdlib.h>
#endif

#if defined(unix) || defined(DARWIN)
#include <unistd.h>
#endif

#include <time.h>
#include <string.h>
#include <stdarg.h>

/*
 * Basic "types".
 *
 * Note the attempt to make all basic types have 4 letters.
 * This improves readibility and standardizes the code.
 *
 * Likewise, all complex types are at least 4 letters.
 * Thus, almost every 1 to 3 letter word is a legal variable,
 * except for certain reserved words ('for' and 'if' and 'do').
 *
 * Note that the type used in structures for bit flags should be uint.
 * As long as these bit flags are sequential, they will be space smart.
 *
 * Note that on some machines, apparently "signed char" is illegal.
 *
 * A s16b/u16b takes exactly 2 bytes
 * A s32b/u32b takes exactly 4 bytes
 *
 * Also, see <limits.h> for min/max values for sint, uint, long, huge
 * (INT_MIN, INT_MAX, 0, UINT_MAX, LONG_MIN, LONG_MAX, 0, ULONG_MAX).
 * These limits should be verified and coded into "h-constant.h", or
 * perhaps not, since those types have "unknown" length by definition.
 */


/* Signed/Unsigned 16 bit value */
typedef signed short s16b;
typedef unsigned short u16b;

/* Signed/Unsigned 32 bit value */
#ifdef L64	/* 64 bit longs */
typedef signed int s32b;
typedef unsigned int u32b;
#else
typedef signed long s32b;
typedef unsigned long u32b;
#endif


/*** Pointers to Functions of special types (for various purposes) ***/

typedef enum {
    LISPSYS_CMUCL     = 0,
    LISPSYS_ACL       = 1,
    LISPSYS_CLISP     = 2,
    LISPSYS_LISPWORKS = 3,
    LISPSYS_SBCL      = 4,
    LISPSYS_CORMAN    = 5,
    LISPSYS_OPENMCL   = 6,
    LISPSYS_BAD       = 20
} LISP_SYSTEMS;

typedef enum {
    UITYPE_X11        = 0,
    UITYPE_GCU        = 1,
    UITYPE_GTK        = 2,
    UITYPE_WIN        = 3,
    UITYPE_SDL        = 4,
    UITYPE_BAD        = 20
} UITYPES;

typedef enum {
    SOUNDSYSTEM_NONE      = 0,
    SOUNDSYSTEM_SDL_MIXER = 1,
    SOUNDSYSTEM_OPENAL    = 2
} SOUNDSYSTEM;

/*
 * Define some simple constants
 */


/*
 * Hack -- Define NULL
 */
#ifndef NULL
# ifdef __STDC__
#  define NULL ((void*)0)
# else
#  define NULL ((char*)0)
# endif /* __STDC__ */
#endif /* NULL */


/*
 * OPTION: Use "blocking getch() calls" in "main-gcu.c".
 * Hack -- Note that this option will NOT work on many BSD machines
 * Currently used whenever available, if you get a warning about
 * "nodelay()" undefined, then make sure to undefine this.
 */
#if defined(SYS_V) || defined(AMIGA)
# define USE_GETCH
#endif

/* langband-specific hack */

#ifdef linux
#define USE_GETCH
#define USE_CURS_SET
#endif

/*
 * OPTION: Use the "curs_set()" call in "main-gcu.c".
 * Hack -- This option will not work on most BSD machines
 */
#ifdef SYS_V
# define USE_CURS_SET
#endif



/*** Color constants ***/


/*
 * Angband "attributes" (with symbols, and base (R,G,B) codes)
 *
 * The "(R,G,B)" codes are given in "fourths" of the "maximal" value,
 * and should "gamma corrected" on most (non-Macintosh) machines.
 */
#define TERM_DARK		0	/* 'd' */	/* 0,0,0 */
#define TERM_WHITE		1	/* 'w' */	/* 4,4,4 */
#define TERM_SLATE		2	/* 's' */	/* 2,2,2 */
#define TERM_ORANGE		3	/* 'o' */	/* 4,2,0 */
#define TERM_RED		4	/* 'r' */	/* 3,0,0 */
#define TERM_GREEN		5	/* 'g' */	/* 0,2,1 */
#define TERM_BLUE		6	/* 'b' */	/* 0,0,4 */
#define TERM_UMBER		7	/* 'u' */	/* 2,1,0 */
#define TERM_L_DARK		8	/* 'D' */	/* 1,1,1 */
#define TERM_L_WHITE	9	/* 'W' */	/* 3,3,3 */
#define TERM_VIOLET		10	/* 'v' */	/* 4,0,4 */
#define TERM_YELLOW		11	/* 'y' */	/* 4,4,0 */
#define TERM_L_RED		12	/* 'R' */	/* 4,0,0 */
#define TERM_L_GREEN	13	/* 'G' */	/* 0,4,0 */
#define TERM_L_BLUE		14	/* 'B' */	/* 0,4,4 */
#define TERM_L_UMBER	15	/* 'U' */	/* 3,2,1 */


#define NO_FLAGS 0x00
#define LANGBAND_GRAPHICS 0x01
#define LANGBAND_SOUND 0x02
#define LANGBAND_1024 0x04
#define LANGBAND_1280 0x08
#define LANGBAND_FULLSCREEN 0x10

#define LANGBAND_TEXT_END 0x80
#define LANGBAND_GFX_START 0x100

#define IMAGE_ARRAY_SIZE 64

// Gervais values!
#define GFXTILE_WIDTH 32
#define GFXTILE_HEIGHT 32

extern const char *base_source_path;
extern const char *base_config_path;
extern const char *base_data_path;

extern void lb_format(FILE *ofile, int priority, const char *fmt, ...);
#ifdef DEBUG
extern void DBGPUT(const char *fmt, ...);
#else
#define DBGPUT if(1){}else printf
#endif

extern void ERRORMSG(const char *fmt, ...);
#define INFOMSG ERRORMSG
#define DEBUGPUT DBGPUT

INTERFACE int init_graphics();
INTERFACE int init_sound_system(int size);

INTERFACE int paint_image(const char *fname, int x, int y);
INTERFACE int load_gfx_image(const char *fname, int idx, unsigned int transcolour);
INTERFACE int paint_gfx_image(const char *fname, const char *name, int x, int y);
INTERFACE int load_texture(int idx, const char*filename, int twid, int thgt, int alpha);
INTERFACE int init_tile_files();
INTERFACE int fill_area(int image_index, int tile_num, int x1, int y1, int x2, int y2);

INTERFACE int get_sound_status();
INTERFACE int load_sound_effect(const char *fname, int idx);
INTERFACE int play_sound_effect(int sound_idx);
INTERFACE int load_music_file(const char *fname, int idx);
INTERFACE int play_music_file(int sound_idx);

/* remove later */
extern void print_image_list();

INTERFACE int load_scaled_image(const char *filename, int image_index, int width,
				int height, unsigned int transcolour);

INTERFACE int get_image_width(int idx);
INTERFACE int get_image_height(int idx);

extern int use_sound;

INTERFACE int current_ui();
INTERFACE int current_soundsystem();
INTERFACE void print_coloured_token(int wantedTerm, int colour, int token, int row, int col);
INTERFACE void print_coloured_stat(int wantedTerm, int colour, int stat, int row, int col);
INTERFACE void print_coloured_number(int wantedTerm, int colour, long number, int padding, int row, int col);
INTERFACE int init_c_side(const char *ui, const char *sourcePath,
			  const char *confPath, const char *gfxPath,
			  int extra_flags);
INTERFACE char *load_sound(int msg, char *fname);
int play_game_lisp();
void readjust_screen_lisp(int width, int height);
void mouse_clicked(int button, int x, int y);


/** will we access lisp through callbacks? */
extern int lisp_will_use_callback;
extern LISP_SYSTEMS current_lisp_system;
INTERFACE void set_lisp_system(LISP_SYSTEMS type);

//#ifdef WIN32
//INTERFACE int setHINST(long val);
//#else
INTERFACE void set_lisp_callback(char *name, void *ptr);
//#endif


INTERFACE int exp_complex_blit(short win_num, short x, short y, unsigned int img, int flags);
INTERFACE int exp_transparent_blit(short win_num, short x, short y, unsigned int img, short flags);
INTERFACE int exp_full_blit(short win_num, short x, short y, unsigned int img, short flags);
INTERFACE int exp_clear_coords(short win_num, short x, short y, short w, short h);
INTERFACE int exp_flush_coords(short win_num, short x, short y, short w, short h);
INTERFACE int sdl_getEvent(int option);
INTERFACE int listenForEvent(int option);
INTERFACE int cleanup_c_side(void);

#ifdef USE_X11
int cleanup_X11(void);
#endif

#ifdef USE_GCU
INTERFACE int cleanup_GCU(void);
#endif

#ifdef USE_SDL
INTERFACE int cleanup_SDL(void);
#endif


#ifdef WIN32
INTERFACE int main(int argc, char *argv[]);
#endif

typedef struct sound_handle sound_effect;
typedef struct sound_handle music_handle;

struct sound_handle {
    char *filename;
    int buffer_idx;
    void *handle; // probably a Mix_Chunk* for SDL
};

extern sound_effect **sound_effects;
extern music_handle **music_handles;

#ifdef USE_SDL

#include "SDL.h"

//#include "SDL_mixer.h"

#define MAX_IMAGES 64

#define FONT_TYPE_TTF 5
#define FONT_TYPE_HEX 6

struct FontData {
    int width;
    int height;
    
    void *theFont;
    int font_type;
    SDL_Surface *letters[256];
};

struct graf_tiles {

	/*
	 * to find a character:
	 * x = character * w
	 */
	SDL_Surface *face;
	/* 
	 * font metrics.
	 * Obviously, the font system is very minimalist.
	 */
	Uint8 w;
	Uint8 h;

	Uint8 dw; /* width and height of font on destination surface */
	Uint8 dh;

	Uint8 precolorized;
};

struct tile_information {

    SDL_Surface *tiles[MAX_IMAGES];
    char *tile_files[MAX_IMAGES];
    int tile_columns[MAX_IMAGES]; // 
    int num_tiles;

    int tile_width;
    int tile_height;
};


typedef struct FontData FontData; /* must be here to avoid fwd. ref. */
typedef struct graf_tiles graf_tiles; 
typedef struct tile_information TileInformation;

FontData *load_hex_font(const char *filename, int justmetrics);
//int load_HEX_font_sdl(FontData *fd, const char *filename, int justmetrics);
int strtoii(const char *str, Uint32 *w, Uint32 *h);

#ifdef ALLOW_TTF
int display_char(SDL_Surface *surface, SDL_Rect *dest, FontData *fdata, s16b attr, s16b ch);
FontData *load_ttf_font(const char *fname, int size);
#endif /* use_ttf */


//int sdl_textureBackground(int term_num, const char *fname, int alpha);

int JAI_BlitSurfaceAlpha(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect);

extern int sdl_load_gfx_image(const char *fname, int idx, unsigned int transcolour);
extern int sdl_paint_gfx_image(const char *fname, const char *name, int x, int y);
extern int sdl_load_scaled_image(const char *filename, int image_index, int width,
				 int height, unsigned int transcolour);
extern int sdl_switch_terms(int bigterm);
//extern int sdl_term_gfx_use_p(term *t);
extern int sdl_swap_map();

extern int sdl_loadTexture(int idx, const char *filename, int target_width, int target_height, int alpha);
extern int sdl_getEvent(int option);
extern int sdl_getImageWidth(int idx);
extern int sdl_getImageHeight(int idx);
extern int sdl_transparentBlit(short win_num, short x, short y, unsigned int img, short flags);
extern int sdl_fullBlit(short win_num, short x, short y, unsigned int img, short flags);
extern int sdl_clearCoords(short win_num, short x, short y, short w, short h);
extern int sdl_flushCoords(short win_num, short x, short y, short w, short h);

extern SDL_Color color_data_sdl[16];

#endif /* use sdl */

#ifdef USE_GCU

extern int gcu_switch_terms(int bigterm);
extern int gcu_getEvent(int option);
extern int gcu_fullBlit(short win_num, short x, short y, unsigned int img, short flags);
extern int gcu_transparentBlit(short win_num, short x, short y, unsigned int img, short flags);
extern int gcu_clearCoords(short win_num, short x, short y, short w, short h);
extern int gcu_flushCoords(short win_num, short x, short y, short w, short h);

#endif /* use gcu */

#endif /* langband_h */
