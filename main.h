/**************************************************************
 * SPACE ODDITY                                               *
 **************************************************************
 *    (c) 2009                                                *
 *    Written by Jambonbill                                   *
 *    http://www.sidabitball.com                              *
 **************************************************************/



#ifndef _MAIN_H_
#define _MAIN_H_

#include "allegro.h"
//#include "aldumb.h"
//#include "map.h"
//#include "hisc.h"
//#include "actor.h"
#include "control.h"

// version string
#define		GAME_VERSION_STR	"v0.4 (dev)"

// some game status defines
#define GS_MENU				1
#define GS_GAMEOVER			2
#define GS_QUIT_GAME		3
#define GS_SELECT_LEVEL		4
#define GS_PLAY				5
#define GS_LEVEL_DONE		6
#define GS_GAME_COMPLETE	7
#define GS_INPUTNAME		8
#define GS_SCORES			9
#define GS_EDIT			   10


#define WIDTH         320
#define HEIGHT         256
#define NUM_STUFF         64
//int MAX_BULLET = 16;
#define MAX_BULLET      16
#define MAX_PRUNES      128
#define MAX_PARTICULES     64
#define PI 3.1415926535


int game_status;
int scanlines;
int show_collisions;
int record_video;
long game_count;
long game_scroll;
// functions
//char *get_init_string();



void log2file(char *format, ...);

int do_pause_menu(BITMAP *bg);

//void take_screenshot(BITMAP *bmp);
void take_screenshot();


//void msg_box(char *str);
void game_start();

void draw_background(void);
void draw_particules(void);
void draw_bg_particules(void);
void draw_rasters(void);
void draw_player(void);
void draw_bullets(void);
void draw_stuff(void);

void draw_tile(BITMAP *dest, unsigned char tilenumber, int x, int y);
void draw_bgtile(BITMAP *dest, unsigned char tilenumber, int x, int y);

void draw_tiles_ingame(void);
char tile_colision(int x, int y);

void move_particules(void);
void move_bullets(void);
void move_stuff(void);

int suggested_x;
int suggested_y;
void draw_event(int spritenumber, int x, int y);//used by the editor

//void new_game(int reset_player_data);
//void new_level(char *fname, int level_id, int draw);

void explode(int tx, int ty);//sprite based explosion
void explode2(int tx, int ty);//particule based explosion
void add_bonus(int tx, int ty);

void print(BITMAP *dest, char *txt, int x, int y);
//void print2(char *txt, int x, int y);
//void print3(BITMAP *dest, char *txt, int x, int y);

void draw_frame(void);
void blit_to_screen();//BITMAP *bmp
void update_palette(void);


// PARTICULES //
void add_asteroids(void);
void add_bubbles(int x, int y);

void pop_bonus( int spritenumber );

//void show_scores(int space, Thisc *table);
void burst(int x, int y, int direction);

void poll_music();

//void init_stuff(int id);// stuff is a struct!
int get_free_stuff(void);// stuff is a struct!
int get_free_particule(void);
int get_closest_enemy(int bullet_id);

void play_sound(int sound_number);

void circle_shot(int x, int y);

char get_scroll_speed(void);// SCROLLING SPEED
void set_scroll_speed(char i);//SCROLL SPEED
void scroll_speed_plus(void);
void scroll_speed_minus(void);

void set_background(int bgnum);

char tracknumber;
void load_track(char *fn);
void load_tilemap(char *fn);
void load_tileset(char *fn);
void level_intro(void);
void inform(char *txt);//send a message

void end_level(void);
void poll_GME(void);
void seek_GME(long position);//in ms
void mute_GME(void);// MUTE
void unmute_GME(void);// UNMUTE

long songpos;//songposition in ms

// MATH //
double get_angle(int x1,int y1,int x2, int y2);
double deg2rad(double degree);
int distance(int x1, int y1, int x2, int y2);


//files//
typedef struct file_sys{
	char f_name[64];
	int f_size;
} FILE_SYS;


FILE_SYS bgs[256];//backgrounds
FILE_SYS vgms[256];
FILE_SYS maps[256];//odt files
FILE_SYS tilesets[256];
int count_bgs;// = 0;//nb bgs in dir
int count_vgm;// = 0;//nb vgm in dir
int count_map;// = 0;//nb maps (odt files) in dir
int count_tileset;// = 0;//nb tileset in dir

typedef struct {
   char name[10];
   unsigned int score;
} HISCORE;
HISCORE hiscore_table[10];



typedef struct MAP{
	unsigned char bg_tile;
	unsigned char tile;
	unsigned char event;
} MAP;

MAP mapp[16][2048];
MAP bakk[16][2048];

typedef struct LEVEL
{
    int width;//level width (in tiles)
    int height;//level height (in tiles)
    char level_name[32];//
    char sound_track[64];//music file
    char next_level[32];//next odt file
    char tileset[32];//level tileset
    char message1[32];//Event messages
    char message2[32];//Event messages
    char message3[32];//Event messages
    char message4[32];//Event messages
    char author[32];//Level auhor
    char background_file[32];//bgfile

    unsigned char bg_r, bg_g, bg_b;//level background colors
    unsigned char tile_r, tile_g, tile_b;//level tile colors
    unsigned char bgtile_r, bgtile_g, bgtile_b;//level tile colors
    unsigned char background;//background number
    unsigned char sprite_color;// unused
    char scroll_speed;// start speed
    //double scroll_speed;// start speed

    char modifs[189];//remove some byte from here if needed

    //int width;

} LEVEL;

LEVEL level;

/* player */
typedef struct PLAYER
{
   int x, y;//position
   int energy, invincible;
   int score;//
   char tilt;//player sprite
   char yshift;
   unsigned char life;//player lifes
   unsigned char speed;//player speed
   unsigned char weapon;//current weapon
   unsigned char options;//number of options
   unsigned char homing;//number of homimg missiles
   unsigned char shield;//are you protected ?
   unsigned char slowmotion;//
   char name[10];//player name :)
} PLAYER;

typedef struct POSITION
{
   int x, y;
} POSITION;


/* star field system */
/*
typedef struct TILE
{
   char tilenumber, monster;
} TILE;
*/


/* explosions, effets, etc */
typedef struct PARTICULE
{
   int life;//life time, in frames
   int direction;//angle
   int spritenum, framecount, step;//nb frames
   char blink;
   float x, y, vx, vy, speed;//speed to be used with direction (speed and direction to calculate vx/vy)
} PARTICULE;

PARTICULE particules[MAX_PARTICULES];
PARTICULE bg_particules[MAX_PARTICULES];
int highest_particule_number;

void particule( float x, float y, float speed, int direction, int life, int spritenumber, int framecount, char blink);
void bg_particule( float x, float y, float speed, int direction, int life, int spritenumber, int framecount, char blink);

/* boulders, ennemies, bonus, etc */
typedef struct STUFF
{
    float x, y, xspeed, yspeed;//xspeed is the 'default' speed, used by angular speed calculations
    //float weight;//to use with gravity
    int direction; //angle
    char shotx, shoty;//point du depart du tir
    char w, h;//bounding box
    char fall;//fake gravity
    int spritenum, framecount, step, spritehit;

    char blocked;//tile colision
    char fragile;//die on colision
	char hit;//hit by a bullet ?

    unsigned char delay, event;//trigger 'event' at 'delay' (generators, birth control, etc)

    //int alive;//?
    int score;//points won when killed
    int age;
	int munitions;//nb munitions
	int munition_type;//type d'arme
	int movement_type;	// movement
    int energy;//0 = death
    int bonus;//is it a bonus ? (then dont colide)
} STUFF;

STUFF stuffs[64];

/* munitions player */
typedef struct BULLETS
{
   char running;
   char homing;//seek and destroy
   char bounce;//do not crash into tiles, but bounce :)
   int direction; //angle
   int sprite;//sprite number
   char framecout;
   char step;
   //float x, y, xspeed, yspeed, speed;
   float x, y, speed;
   char w, h;//bbox
} BULLETS;


/* munitions ennemis */
/*
typedef struct PRUNES
{
   char running;
   char homing;//seek and destroy
   int direction; //angle
   int sprite;
   char framecout;
   char step;
   float x, y, xspeed, yspeed;
   char w, h;//bbox
} PRUNES;
*/

char message[64];
unsigned char shift;//message position

char file_name[32];//level file name
//char level_name[32];//level name
//char sound_track[32];//level soundtrack


DATAFILE *gfx;//sprites
DATAFILE *intro;//intro
DATAFILE *sfx;//sound effe
DATAFILE *bg;//backgrounds
//DATAFILE *tiles;//

// a little bounding box quickie
#define check_bb_collision(x1,y1,w1,h1,x2,y2,w2,h2) (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
                                                        ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))


#endif
