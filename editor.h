#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

// some editor status defines
#define ED_MAIN				0
#define ED_TILES			1
#define ED_EVENTS  			2
#define ED_TUNES	        3
#define ED_LEVELS        	4
#define ED_TILESETS			5
#define ED_xxxxx		    6
#define ED_STRINGS	        7
#define ED_SETTINGS   		8
#define ED_HELP 			9
#define ED_QUIT			   10

#define ED_BIGBOULDER		0
#define ED_SMALLBOULDER		1
#define ED_Cockpitbee       2
#define ED_Doubleshot       3
#define ED_FishShip         4
#define ED_Crab             5
#define ED_Jellyfish        6
#define ED_Meduse           7
#define ED_Twin             8
#define ED_Fly              9
#define ED_Machoire         10
#define ED_Tie              11
#define ED_Larve            12
#define ED_Spike            13
#define ED_Asteroids        14


void editor(void);//editor main frame
void editor_screenshot(void);

//void load_map(int mapnumber);//load current map
void save_map(void);//save current map
void save_brush(void);//save brushes for this level
void load_brush(char *file);
void clear_brushes(void);//BIG BRUSH CLEAR ALL

//void draw_grid(void);
void draw_help(void);
void draw_settings(void);

void pipette(void);// PRESS D TO USE THE TILE EYEDROPER
void select_tile(void);//
void draw_tiles(void);
void draw_tiles_palette(void);
void draw_tiles_helper(void);
void draw_events_helper(void);
void draw_big_brush(void);//THE BIG 16x16 tiles brush
void draw_brush( int x, int y );//THE small version for F1

void shift_brush_up(void);
void shift_brush_down(void);
void shift_brush_right(void);
void shift_brush_left(void);

void draw_square(int x, int y);
void draw_color_palette(BITMAP *dest, int x, int y);
void draw_tileset_palette(BITMAP *dest, int x, int y);
void export_tilemap(void);//export tilemap plus palette as a bitmap
int is_empty_tile(int tilenum);

void draw_background_in_editor(void);
void toggle_layer(void);
void historise(void);
void undo(void);

//void make_random_level(void);

int wait;//

typedef struct CLIPBOARD
{
    int x;
    unsigned char y;
    unsigned char tile;
} CLIPBOARD;

CLIPBOARD clipb[64];
CLIPBOARD brush[256][256];//256 16x16 tiles brush :D

#endif // EDITOR_H_INCLUDED
