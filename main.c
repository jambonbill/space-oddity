
#include <allegro.h> // You must include the Allegro Header file
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "dat/gfx.h"
#include "sfx.h"
#include "control.h"
#include "options.h"
#include "hisc.h"
#include "enemy.h"
#include "intro.h"
#include "editor.h"
#include "events.h"
#include "main.h"
#include "gme/gme.h"

/* "(c)2009 BRICO SYSTEM" */


/* Timer stuff */
volatile long speed_counter = 0; // A long integer which will store the value of the speed counter.

/* Music stuff */
char TRACK_LOADED = 0;
short* buf;//chip buffer

// Audio stream stuff
const int sample_rate = 44100;
const int frame_count = 2048;

AUDIOSTREAM *stream;
/* Open music file in new emulator */
Music_Emu* emu;
track_info_t info;//music info

/* pointer to data file */
BITMAP *ptibuffer;//game/editor infos
BITMAP *buffer;//gamebuffer
BITMAP *fram; //640x480 buffer
BITMAP *tileset;//320*240 bitmap (2)
BITMAP *bgtileset;//320*240 bitmap (2)
PALETTE org_pal;//

//Thisc *hisc_table;
//Thisc *hisc_table_space;

// GLOBALES //

int got_sound = 0;
int got_joystick = 0;
char music_vol = 100;
char sfx_vol = 100;
unsigned char delay;//alert frames (how many frames is the alert displayed)

/*
typedef struct vector_s {
    double x, y;
} vector_t;

vector_t vector_div(vector_t a, vector_t b)
{
    vector_t vect = { a.x / b.x, a.y / b.y };
    return vect;
}

vector_t vector_add(vector_t a, vector_t b)
{
    vector_t vect = { a.x + b.x, a.y + b.y };
    return vect;
}

vector_t vector_sub(vector_t a, vector_t b)
{
    vector_t vect = { a.x - b.x, a.y - b.y };
    return vect;
}
*/


int last_column = 0;//tile events column


char get_scroll_speed(void){
    return level.scroll_speed;
}

void set_scroll_speed(char i){
    if(i < 0)i = 0;
    if(i > 8)i = 8;
    level.scroll_speed = i;
}

//EVENT//
void scroll_speed_plus(void){
    level.scroll_speed++;
    if(level.scroll_speed>8)level.scroll_speed = 8;
}

//EVENT//
void scroll_speed_minus(void){
    level.scroll_speed--;
    if(level.scroll_speed < 1)level.scroll_speed = 1;
}



//char init_string[] = "[YhJPJKUSY`0-'\"7 ";//alex data pack
//char message[]="                                ";

/*
void increment_speed_counter() // A function to increment the speed counter
{
  speed_counter++; // This will just increment the speed counter by one.
}
END_OF_FUNCTION(increment_speed_counter); // Make sure you tell it that it's the end of the function
*/






POSITION playerpos[64];//last player positions (for options) DO NOT CHANGE
//POSITION bulletpos[16][8];// LAST BULLET POSITIONS //

//STUFF stuffs[NUM_STUFF];
BULLETS ammo[64];//max 64 bullets (player bullets)
BULLETS pruno[MAX_PRUNES];//max 64 prunes (ennemy bullets)

PLAYER playr;

// controls
Tcontrol ctrl;
Toptions options;
/*
char bgspeed[32][16] = {// PARALAX, 1 is base scroll speed, higher values are scroll speed dividers
    { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},//BG 00 - SPACE DEBRIS
    { 1, 2, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 2, 1},//BG 01 - TUNNEL
    { 1, 2, 4, 6, 8, 8, 8, 8, 6, 4, 2, 1, 1, 1, 1, 1},//BG 02 - TUNNEL
    { 1, 2, 4, 6, 8, 8, 8, 8, 6, 4, 2, 1, 1, 1, 1, 1},//BG 03 - GROTTE
    { 1, 2, 4, 6, 8, 8, 8, 8, 6, 4, 2, 1, 1, 1, 1, 1},//BG 04 - FUTURO
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//BG 05 - SPACE DEBRIS
    { 8, 6, 4, 2, 1, 1, 1, 1, 2, 4, 6, 8, 1, 1, 1, 1},//BG 06 -
    { 32, 32, 32, 32, 32, 32, 32, 16, 8, 4, 2, 1, 1, 1, 1, 1},//BG 07 - MONTAGNES
    { 1, 2, 2, 4, 4, 4, 4, 2, 2, 1, 1, 1, 1, 1, 1, 1 },//BG 08 - VERRUES
    { 8, 8, 8, 8, 8, 8, 6, 6, 4, 4, 2, 1, 1, 1, 1, 1},//BG 09 - CITY
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//BG 10
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//BG 11
    { 1, 2, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//BG 12 WEIRD
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//BG 13
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},//BG 14
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //BG 15
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //BG 16
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //BG 17
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} //BG 18
    //BG ...
};
*/

///////////////////////////////////////////////////////////////////////
// TILES
///////////////////////////////////////////////////////////////////////
void load_tileset(char *file)
{
    //LOAD TILESET//

    char fn[256] = "dat/";
    strcat(fn, file);

    log2file("Loading ./%s", fn);

    /*
    if(exists( fn )){
        //ok
    }else{
        allegro_message("Tileset error, load default instead");
        sprintf(fn, "dat/tileset.bmp");
    }
    */
    /*
	if (tiles == NULL) {
    	log2file("load tileset failed");
		allegro_message("Failed to load %s tileset", fn);
		return;
	}
    */

	// NEW //
	if(exists( fn )){
        PALETTE p;
        get_palette(p);
        tileset = load_bitmap(fn, p );

        //copy tileset to bgtileset
        int i,j;
        for( i = 0; i < 320; i++){
            for(j=0; j < 240; j++){
                int color = getpixel( tileset, i, j );
                if(color>0)color+=4;//decale
                putpixel( bgtileset, i, j, color );
            }
        }

	}else{
	    allegro_message("Tileset %s not found", fn);
	}

}


void free_level(void){

    sprintf(level.level_name, "New level");
    sprintf(level.sound_track, "");
    sprintf(level.next_level, "");
    sprintf(level.tileset, "");
    sprintf(level.author, "");
    sprintf(level.message1, "");
    sprintf(level.message2, "");
    sprintf(level.message3, "");
    sprintf(level.message4, "");

    //unsigned char bg_r, bg_g, bg_b;//level background colors
    level.tile_r=60;//level tile colors
    level.tile_g=60;//level tile colors
    level.tile_b=60;//level tile colors
    //unsigned char background;//background number
    //unsigned char sprite_color;// :)
    //unsigned char scroll_speed;// :)
    int i,j;
    for (i=0; i < 2048; i++)
    {
        for (j=0; j<16; j++)
        {
            mapp[j][i].tile = 0;
            mapp[j][i].event = 0;
        }
    }
    log2file("Level freed");
}

void load_tilemap(char *file)//(OR 'LEVEL.odt')
{
    //file_name = "";
    char header[5];
    char fn[256] = "levels/";
    strcat(fn, file);

    log2file("Loading level ./%s", fn);

    if(!exists(fn)){
        log2file("Error: ./%s not found", fn);
        return;
    }

	PACKFILE *pfile;
	pfile = pack_fopen(fn, "rp");
	if(!pfile){
	    log2file("!pfile", fn);
	    // HERE WE HAVE TO CLEAR A NUMBER OF THINGS //
	    free_level();
	    return;
	}

    int i,j;

    // HEADER //
    pack_fgets(header, 5, pfile);
    //allegro_message("Header: %s", header);

    //load level settings
    pack_fread(&level, sizeof(LEVEL), pfile);
    if(level.scroll_speed == 0)level.scroll_speed=1;

    //load level tilemap&events
    for (i=0; i < 2048; i++)
    {
        //for (j=0; j<12; j++)
        for (j=0; j<16; j++)
        {
            pack_fread(&mapp[j][i], sizeof( MAP ), pfile);
        }
    }


    load_tileset( level.tileset );

    update_palette();

    pack_fclose( pfile );

    game_scroll= 0;
    game_count = 0;
    last_column= 0;
    //scroll_speed = level.start_speed;
    //set_background(level.background);

    load_track(level.sound_track);
}




void read_tile_events(void){
    int j;

    if ( (game_scroll + 320 + 1)/16 > last_column){

        last_column = (game_scroll + 320 + 1)/16;

        // LIMIT TO 2048 COLUMN //
        if(last_column >= 2048){
            set_scroll_speed(0);
            return;
        }

        for (j=0; j<16; j++)
        {
            //unsigned char e = event[j][last_column];
            unsigned char e = mapp[j][last_column].event;
            if(e==255)continue;
            if(e>0){
                suggested_y = j*16;
                suggested_x = 320;
                //sprintf(message, "Event %d", e);
                do_event(e);
            }
        }
    }
}



/////////////////////////////////////////////////////////////////////
// MUSIC
/////////////////////////////////////////////////////////////////////


void load_track(char *file){


    char fn[256] = "vgm/";
    strcat(fn, file);

    log2file("Loading ./%s", fn);

     if( !exists( fn ) ){
         TRACK_LOADED = 0;
         log2file("Error: VGM File %s not found", fn);
         //allegro_message("Music File %s not found", fn);
         return;
     }


    if( gme_open_file( fn, &emu, sample_rate ) ){
        //error
        TRACK_LOADED = 0;
        return;
    }

    gme_track_info( emu, &info, 0 );//load track info

	/* Add some stereo enhancement */
	gme_set_stereo_depth( emu, 0.50 );

	/* Adjust equalizer for crisp, bassy sound */
	{
		gme_equalizer_t eq;
		eq.treble = 0.0;
		eq.bass   = 20;
		gme_set_equalizer( emu, &eq );
	}


    /* Start track 0 */
	if( gme_start_track( emu, 0 ) ){
	    TRACK_LOADED = 0;
	    return;
	}

    TRACK_LOADED = 1;
}

void seek_GME(long offset){
    if( TRACK_LOADED > 0 ){
        int time = (offset*16)/60;//in sec
        //allegro_message("seek time %d", time );
        gme_seek( emu, time*1000 );
    }
}

void poll_GME(void){

    if( TRACK_LOADED > 0 ){

        buf = (short*)get_audio_stream_buffer( stream );

        songpos = gme_tell( emu );

        if(buf){
            // Give frame_count*2 since a stereo frame contains two samples
            gme_play( emu, frame_count * 2, buf );

            // XOR every sample with 0x8000 to switch the signedness
            int i;
            for( i = 0; i < frame_count * 2; ++i)buf[i] = buf[i] ^ 0x8000;

            free_audio_stream_buffer( stream );
        }
    }else{
        /*
        buf = (short*)get_audio_stream_buffer( stream );
        int i;
        for( i = 0; i < frame_count * 2; ++i)buf[i] = 0;
        free_audio_stream_buffer( stream );
        //stop_audio_stream( stream );
        */
    }
}


void mute_GME(void){
    gme_mute_voices(emu, 0xFF);
}

void unmute_GME(void){
    gme_mute_voices(emu, 0);
}


///////////////////////////////////////////////////////////////////////
// PALETTE
///////////////////////////////////////////////////////////////////////

void update_palette(){

    PALETTE pal;
    // TILE BRIGHT COLOR // FIRST PLANE
    pal[241].r = level.tile_r;
    pal[241].g = level.tile_g;
    pal[241].b = level.tile_b;
    int i;
    for( i = 0; i < 3; i++ ){//MAKE A 4BIT PALETTE FOR TILES :)
        char new_r = (level.tile_r/3)*(2-i); if(new_r < 0 || new_r > 63)new_r = 0;
        char new_g = (level.tile_g/3)*(2-i); if(new_g < 0 || new_g > 63)new_g = 0;
        char new_b = (level.tile_b/3)*(2-i); if(new_b < 0 || new_b > 63)new_b = 0;
        pal[ 242 + i ].r = new_r;
        pal[ 242 + i ].g = new_g;
        pal[ 242 + i ].b = new_b;
    }

    // TILE BG COLOR // SECOND PLANE
    pal[245].r = level.bgtile_r;
    pal[245].g = level.bgtile_g;
    pal[245].b = level.bgtile_b;
    for( i = 0; i < 3; i++ ){//MAKE A 4BIT PALETTE FOR BACKGROUNDS :)
        char new_r = level.bgtile_r/3*(2-i); if(new_r < 0 || new_r > 63)new_r = 0;
        char new_g = level.bgtile_g/3*(2-i); if(new_g < 0 || new_g > 63)new_g = 0;
        char new_b = level.bgtile_b/3*(2-i); if(new_b < 0 || new_b > 63)new_b = 0;
        pal[ 246 + i ].r = new_r;
        pal[ 246 + i ].g = new_g;
        pal[ 246 + i ].b = new_b;
    }

    // BACKGROUND BRIGHT COLOR // BG
    pal[249].r = level.bg_r;//BG BASE R COLOR
    pal[249].g = level.bg_g;//BG BASE G COLOR
    pal[249].b = level.bg_b;//BG BASE B COLOR
    for( i = 0; i < 3; i++ ){//MAKE A 4BIT PALETTE FOR BACKGROUNDS :)
        char new_r = level.bg_r/3*(2-i); if(new_r < 0 || new_r > 63)new_r = 0;
        char new_g = level.bg_g/3*(2-i); if(new_g < 0 || new_g > 63)new_g = 0;
        char new_b = level.bg_b/3*(2-i); if(new_b < 0 || new_b > 63)new_b = 0;
        pal[ 250 + i ].r = new_r;
        pal[ 250 + i ].g = new_g;
        pal[ 250 + i ].b = new_b;
    }

    set_palette_range(pal, 241, 254, 0);

}

///////////////////////////////////////////////////////////////////////
// MATH STUFF
///////////////////////////////////////////////////////////////////////

double get_angle(int x1, int y1, int x2, int y2)
{
    double opp;
    double adj;
    double ang1;

    // ARBITRARY //
    // UP : 0 Deg
    // RIGHT : 90 Deg
    // DOWN : 180 Deg
    // LEFT : 270 Deg

    //calculate vector differences
    opp = y1-y2;
    adj = x1-x2;

    if(x1==x2 && y1==y2) return(-1);

    //trig function to calculate angle
    if(adj==0) // to catch vertical co-ord to prevent division by 0
    {
        if(opp>=0){
            return(0);
        }else{
            return(180);
        }
    }
    else
    {
        ang1 = (atan(opp/adj))*180/PI;
        //the angle calculated will range from +90 degrees to -90 degrees
        //so the angle needs to be adjusted if point x1 is less or greater then x2

        if(x1 >= x2){
            //ang1 = 90-ang1;
            ang1 = 180+ang1;
        }else{
            //ang1 = 180+ang1;
        }

    }
    return(ang1);
}

double deg2rad(double degree)
{
    //degree= (radian/PI)*180
    //radian= (degree/180)*PI
    return (degree/180)*PI;
}


int distance(int x1, int y1, int x2, int y2)
{
    return (int)hypot(x1 - x2, y1 - y2);
}


///////////////////////////////////////////////////////////////////////
// VOIDS ALEX THE GATOR :P  (rhoooo)
///////////////////////////////////////////////////////////////////////

// loggs the text to the text file
void log2file(char *format, ...) {

	va_list ptr; // get an arg pointer //
 	FILE *fp;

	fp = fopen("log.txt", "at");
	if (fp) {
		// initialize ptr to point to the first argument after the format string
		va_start(ptr, format);

		// Write to logfile.
		vfprintf(fp, format, ptr); // Write passed text.
		fprintf(fp, "\n"); // New line..

		va_end(ptr);
		fclose(fp);
	}

}

//yeah

void blit_to_screen(){//BITMAP *bmp
    int i;

	acquire_screen();

	vsync();//yup

	for(i=0;i<playr.slowmotion;i++)vsync();//SLOW MOTION

    clear_bitmap( fram );

	if(SCREEN_W == 640){//THE GOOD WAY

        //clear_to_color(fram, 5);//debug

	    stretch_blit(ptibuffer, fram, 0, 0, ptibuffer->w, ptibuffer->h, 0, 0, 640, 48 );//STRETCHED

        //stretch_blit(buffer, fram, 0, 0, buffer->w, buffer->h, 0, 40, 640, (192*2) );//STRETCHED (WORKS)
        //blit( buffer, fram, 0, 0, 0, 40, 320, 256);
        //masked_stretch_blit(BITMAP *source, BITMAP *dest, int source_x, source_y, source_w, source_h, int dest_x, dest_y, dest_w, dest_h);

        masked_stretch_blit(buffer, fram, 0, playr.yshift, 320, 220, 0, 40, 640, 220*2 );//UPSCALE

        if(scanlines==1)for(i=0; i<240; i++)hline(fram, 0, i*2, SCREEN_W, 0);//interlace
        blit(fram, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

	}else{//EDITOR MODE

	    blit(fram, screen,0,0,0,0,SCREEN_W, SCREEN_H );//slow ?
	    blit(ptibuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	    blit(buffer, screen, 0, 0, 0, 24, SCREEN_W, SCREEN_H);
	}
	release_screen();
}



// saves a screenshot
//void take_screenshot(BITMAP *bmp){
void take_screenshot(void){

    //check directory
    if(!exists("screenshots")){
        mkdir("screenshots");//ca marche
    }

	//static int number = 0;

	PALETTE p;
	BITMAP *b;
	b  = create_bitmap(320, 240); // virtual screen

	char buf[256];//filename
	//int ok = 0;

	// check if the file name already exists
	/*
	do {
		sprintf(buf, "screenshots/oddity_%06d.bmp", number ++);
		if (!exists(buf)) ok = 1;
		if (number > 999) return;
	} while(!ok);
    */

    sprintf(buf, "screenshots/oddity_%06d.bmp", (int)game_count );

	get_palette( p );

	//b = create_sub_bitmap(bmp, 0, 0, bmp->w, bmp->h);
    blit(ptibuffer, b, 0, 0, 0, 0, 320, 10);
    blit(buffer, b, 0, playr.yshift, 0, 10, 320, 220);

	save_bitmap(buf, b, p);
	destroy_bitmap(b);

}


///////////////////////////////////////////////////////////////////////
// INITS
///////////////////////////////////////////////////////////////////////
void init_stuff(void){
    int i;
    for(i=0; i<NUM_STUFF;i++){
        //stuffs[i].alive = 0;
        stuffs[i].bonus = 0;
        stuffs[i].energy = 0;
        stuffs[i].framecount = 0;
        stuffs[i].step = 0;
    }
}

void init_particules(void){
    int i;
    for(i=0; i<MAX_PARTICULES;i++){
        particules[i].life = 0;
        particules[i].spritenum=0;
    }
}

void init_bullets(void)
{
    int i;
    for (i = 0; i<MAX_BULLET; i++) {
        //ammo[i].xspeed = 0;//bullet speed
        //ammo[i].yspeed = 0;
        ammo[i].speed = 0;
        pruno[i].direction = 0;
        ammo[i].running  = 0;
    }

    for (i = 0; i< MAX_PRUNES; i++) {
        pruno[i].speed = 0;//bullet speed
        pruno[i].direction = 0;
        pruno[i].running  = 0;
    }
}


///////////////////////////////////////////////////////////////////////
// VOIDS ETC
///////////////////////////////////////////////////////////////////////

void print(BITMAP *dest, char *txt, int x, int y){//print with a nice blue 'dropshadow'
    textout_ex(dest, font, txt, x+1, y+1, makecol(0, 0, 255), -1);//BLUE
    textout_ex(dest, font, txt, x, y, 255, -1);//WHITE
}

char alerte[64];//foireux non ?
void inform(char *txt){
    shift = 0;
    sprintf(alerte, txt);
    delay = 120;
}

void draw_alert(void){

    char txt[64];

    if( delay == 0 )return;

    sprintf(txt, alerte);

    if( delay > 120 - 12 )shift += 8;
    if( delay < 15 )shift += 8;
    int len = strlen( alerte );
    int x_origin = 160 - ( len*8 )/2;

    print(buffer, txt, x_origin, shift + playr.yshift);

    delay--;
}


void play_sound(int sound_number){
    //int play_sample(const SAMPLE *spl, int vol, int pan, int freq, int loop);
    play_sample(sfx[sound_number].dat, 128, 128, 1000, FALSE);
}

int get_free_bullet(void){//player ammo
    int i;
    for(i = 0; i < MAX_BULLET; i++){
        if(ammo[i].running == 0){
            ammo[i].step = 0;
            ammo[i].framecout = 0;
            ammo[i].sprite = 0;
            ammo[i].homing = 0;
            //ammo[i].xspeed = 0;//old
            //ammo[i].yspeed = 0;//old
            ammo[i].direction = 0;//good
            ammo[i].speed = 0;//the good one
            ammo[i].bounce = 0;
            ammo[i].sprite = 0;
            //clear_bullet_positions(i);
            return i;
        }
    }
    return -1;
}

int get_free_pruno( void ){//ennemy ammo
    int i;
    for(i = 0; i < MAX_PRUNES; i++){
        if(pruno[i].running == 0)return i;
    }
    return -1;
}


char count_homing_missile(void){
    char num = 0;
    int i;
    for(i = 0; i < MAX_BULLET; i++)if(ammo[i].homing > 0 && ammo[i].running > 0)num++;
    return num;
}

void shot_homing_missile( void ){

    if(playr.homing<1)return;

    int i = get_free_bullet();

    if(count_homing_missile() >= playr.homing )return;


    //POSITION
    ammo[i].x = playr.x + 8;
    ammo[i].y = playr.y + 4;

    ammo[i].running = 1;//
    ammo[i].homing = 1;//
    ammo[i].direction = 45;//degrees
    ammo[i].speed = 2;
    ammo[i].sprite = BULLET15;//BLUE BALL
    // allegro_message("shot_homing_missile");
}


int next_shot = 0;

void shot_bullet(void){//player shot

    if(next_shot > 0)return;
    next_shot = 8;

    int i;

    float speed = 4;//8 as defaut
    //float xspeed = 8;//8 as defaut
    //float yspeed = 0;

    //// player ////
    i = get_free_bullet();

    if( i >= 0 ){

        switch( playr.weapon ){// global

            case BULLET00://SPRINKLER
                play_sound(SHOT00);
                break;

            case BULLET01:
                play_sound(SHOT01);
                break;

            case BULLET02://laser
                speed = 16;//
                play_sound(SHOT02);
                break;

            case BULLET03://fire
            case BULLET04:
                play_sound(SHOT00);
                ammo[i].framecout = 1;
                break;

            case BULLET05://boules
                play_sound(SHOT02);
                ammo[i].bounce = 1;
                break;

            default:
                //play_sound(0);
                break;
        }

        ammo[i].running = 1;
        ammo[i].x = playr.x + 8;
        ammo[i].y = playr.y + 4;
        ammo[i].speed = speed;
        //ammo[i].xspeed = xspeed;
        //ammo[i].xspeed = xspeed;
        ammo[i].direction = 0;//tout droit !!
        ammo[i].sprite = playr.weapon;//(AL_RAND() % 8)/8;//bof

        BITMAP *bmp;
        bmp = gfx[ammo[i].sprite].dat;
        ammo[i].w = bmp->w;
        ammo[i].h = bmp->h;

    }

    // options //

    int o;
    float angle = 0;
    for(o=1; o <= playr.options; o++){
        i = get_free_bullet();
        if(i >= 0){

            int optx = playerpos[o*4].x;//option xpos
            int opty = playerpos[o*4].y + 4;//option ypos
            ammo[i].x = optx;
            ammo[i].y = opty;


            if(playr.weapon == BULLET05){// GROSSES PATATES
                angle = get_angle(optx, opty, playr.x, playr.y);
                //angle += AL_RAND() % 20;//JITTER
                //xspeed = sin( deg2rad(angle) ) * 4;
                //yspeed = cos( deg2rad(angle) ) * 4;
            }

            //ammo[i].xspeed = xspeed;//
            //ammo[i].yspeed = yspeed;//
            ammo[i].speed = speed;
            ammo[i].direction = angle;
            ammo[i].sprite = playr.weapon;//
            ammo[i].running = 1;
        }
    }

    shot_homing_missile();

}


int get_closest_enemy(int bullet_id){

    int x = ammo[bullet_id].x;
    int y = ammo[bullet_id].y;
    int i;
    int shortest = 9999;
    int id = -1;
    // BROWSE ENEMIES //
    for( i = 0; i < NUM_STUFF; i++ ){
        if(stuffs[i].energy == 0)continue;
        if(stuffs[i].x < 0)continue;
        if(stuffs[i].y < 0)continue;
        if( stuffs[i].bonus == TRUE )continue;

        int dist = distance(x, y, stuffs[i].x, stuffs[i].y);
        if( dist < shortest ){
            shortest = dist;
            id = i;
        }
    }
    return id;
}


void update_homing_missile(int i){

    // SEEK CLOSEST ENEMY //
    int nmy_id = get_closest_enemy( i );

    if(nmy_id == -1){// no ennemies
        // do not recalculate direction

        ammo[i].direction += 20;// :) marrant : fait tourner la prune sur elle meme en attendant une victime

        //ammo[i].xspeed = ( sin( deg2rad( ammo[i].direction ) ) * 4 );
        //ammo[i].yspeed = ( cos( deg2rad( ammo[i].direction ) ) * 4 );

    }else{

        int nmy_x = stuffs[nmy_id].x;//enemy position
        int nmy_y = stuffs[nmy_id].y;//enemy position
        //draw_sprite(buffer, gfx[CROSS].dat, nmy_x - 8, nmy_y);//GREEN CROSS (DEBUG)
        //int angle = get_angle( nmy_x, nmy_y, ammo[i].x, ammo[i].y );
        int angle = get_angle( ammo[i].x, ammo[i].y, nmy_x, nmy_y );
        ammo[i].direction = angle;//straight copy

        //ammo[i].xspeed = ( sin( deg2rad( ammo[i].direction ) ) * 4 );
        //ammo[i].yspeed = ( cos( deg2rad( ammo[i].direction ) ) * 4 );

        //draw_sprite(buffer, gfx[CROSS].dat, ammo[i].x + ammo[i].xspeed*8, ammo[i].y + ammo[i].yspeed*8);//CROSS DEBUG
    }


    // SET THE RIGHT HOMING MISSILE SPRITE //
    /*
    int a = ammo[i].direction %360;

    if(a < 23){// Down
        ammo[i].sprite = HOMING00;
    }else if(a < 45 + 23){// Down right
        ammo[i].sprite = HOMING01;
    }else if(a < 90 + 23){// Right
        ammo[i].sprite = HOMING02;
    }else if(a < 135 + 23){// Uppper right
        ammo[i].sprite = HOMING03;
    }else if(a < 180 + 23){// UP
        ammo[i].sprite = HOMING04;
    }else if(a < 225 + 23){// Up left
        ammo[i].sprite = HOMING05;
    }else if(a < 270 + 23){// Left
        ammo[i].sprite = HOMING06;
    }else if(a < 315 + 23){// Down left
        ammo[i].sprite = HOMING07;
    }else{// Down
        ammo[i].sprite = HOMING00;
    }
    */
    burst(ammo[i].x, ammo[i].y, ammo[i].direction );
}



void move_bullets(void)//UPDATE BULLET POSITIONS
{
    int i;
    //player bullets
    for ( i = 0; i < MAX_BULLET; i++){

        if(ammo[i].running == 0)continue;//DEAD BULLET
        if(ammo[i].homing  == 0){// NORMAL BULLET
            // well... theres nothing to do :)
        }else{// HOMING MISSILE
            update_homing_missile( i );
        }

        float x;
        float y;
        char w = ammo[i].w;
        char h = ammo[i].h;
        //ammo[i].xspeed = ( cos( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
        //ammo[i].yspeed = ( sin( deg2rad( ammo[i].direction ) ) * ammo[i].speed );

        // NEW POSITION //
        x = ammo[i].x + ( cos( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
        y = ammo[i].y + ( sin( deg2rad( ammo[i].direction ) ) * ammo[i].speed );

        //bounce
        //http://www.allegro.cc/forums/thread/176841/176841#target
        if(ammo[i].bounce > 0){//bounce bullet//

            if( x > ammo[i].x){//move right
                //Anyway, I think that this would be much simplier:
                //ball.ang = 2*normal - ball.ang + 180;
                if( tile_colision(x + w, y) || tile_colision(x + w, y + h ) ){
                    //ammo[i].direction = 2*45 - ammo[i].direction + 180;
                    ammo[i].direction= 270;
                    // NEW POSITION //
                    x = ammo[i].x + ( cos( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
                    y = ammo[i].y + ( sin( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
                }
            }

            if( x < ammo[i].x){//move left
            }

            if( y > ammo[i].y){//move down
                if( tile_colision(x, y + h) || tile_colision(x + w, y + h ) ){
                    ammo[i].direction= 270;
                    // NEW POSITION //
                    x = ammo[i].x + ( cos( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
                    y = ammo[i].y + ( sin( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
                }
            }

            if( y < ammo[i].y){//move up
                if( tile_colision(x, y) || tile_colision(x + w, y) ){
                    ammo[i].direction= 90;
                    // NEW POSITION //
                    x = ammo[i].x + ( cos( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
                    y = ammo[i].y + ( sin( deg2rad( ammo[i].direction ) ) * ammo[i].speed );
                }
            }
        }

        ammo[i].x = x;
        ammo[i].y = y;

        // FREE LOST BULLETS //
        if(ammo[i].x < -16 || ammo[i].x > 320)ammo[i].running  = 0;//free this ammo
        if(ammo[i].y < -16 || ammo[i].y > 260)ammo[i].running  = 0;//free this ammo
    }

    // Enemy bullets //
    for (i = 0; i < MAX_PRUNES; i++) {
        if(pruno[i].running  == 0)continue;

        //pruno[i].x+= pruno[i].xspeed;
        //pruno[i].y+= pruno[i].yspeed;
        pruno[i].x+= ( cos( deg2rad( pruno[i].direction ) ) * pruno[i].speed );
        pruno[i].y+= ( sin( deg2rad( pruno[i].direction ) ) * pruno[i].speed );

        if(pruno[i].x < -16 || pruno[i].x > 320)pruno[i].running  = 0;//free this ammo
        if(pruno[i].y < -16 || pruno[i].y > 260)pruno[i].running  = 0;//free this ammo
    }
}





void draw_bullets(void)
{
    int i;
    // PLAYER BULLETS //
    for (i=0; i < MAX_BULLET; i++) {
        if(ammo[i].running  == 0)continue;
        if(game_count%4 == 0)ammo[i].step++;
        if(ammo[i].step > ammo[i].framecout)ammo[i].step = 0;
        draw_sprite(buffer, gfx[ammo[i].sprite + ammo[i].step].dat, ammo[i].x, ammo[i].y);
    }

    // ENEMY BULLETS //
    for (i=0; i < MAX_PRUNES; i++) {
        if(pruno[i].running  == 0)continue;
        if(game_count%4 == 0)pruno[i].step++;
        if(pruno[i].step > pruno[i].framecout)pruno[i].step = 0;
        draw_sprite(buffer, gfx[pruno[i].sprite + pruno[i].step].dat, pruno[i].x, pruno[i].y);
    }

}


void circle_shot(int x, int y)//some enemies launch those special 'circle' shots
{
    int i;

    for(i = 0; i < 18; i++){

        int free = get_free_pruno();//warning, may return -1
        if( free < 0 )return;

        int angle =  (i * 20);
        //float spdx = ( cos( deg2rad(angle) ) * 1 );
        //float spdy = ( sin( deg2rad(angle) ) * 1 );

        pruno[free].x = x;
        pruno[free].y = y;
        pruno[free].speed = 1;
        pruno[free].direction = angle;
        //pruno[free].xspeed = spdx;
        //pruno[free].yspeed = spdy;
        pruno[free].sprite = BULLET13;
        pruno[free].framecout = 1;
        pruno[free].running = 1;

    }
}


void move_stuff(void)//enemies, bonuses
{
    int i;
    for (i=0; i< NUM_STUFF; i++) {

        if(stuffs[i].energy < 1 )continue;//its dead
        stuffs[i].age++;//its going older :p

        float x = stuffs[i].x;//CURRENT POSITION
        float y = stuffs[i].y;//CURRENT POSITION
        float w = stuffs[i].w;//
        float h = stuffs[i].h;//

        switch( stuffs[i].movement_type )
        {

            default:// linear
                x -= stuffs[i].xspeed + get_scroll_speed();
                y -= stuffs[i].yspeed;
                break;

            case 2:   // RND //
                x -= stuffs[i].xspeed;
                y -= -2 + (AL_RAND() % 5);

                //stuffs[i].x -= -2 + (AL_RAND() % 5);
                break;

            case 3:// SWING
                stuffs[i].yspeed = sin( deg2rad(game_count + stuffs[i].x*2) );//swing
                x -= stuffs[i].xspeed + get_scroll_speed();
                y -= stuffs[i].yspeed;
                break;

            case 4:// lil SWING
                stuffs[i].yspeed = sin( deg2rad(game_count + stuffs[i].x*4)*8 );//gentle swing
                x -= stuffs[i].xspeed + get_scroll_speed();
                y -= stuffs[i].yspeed;
                break;

            case 5:// total randomness (jerky)
                stuffs[i].xspeed = -1 + (AL_RAND() % 5);
                stuffs[i].yspeed = -3 + (AL_RAND() % 7);
                x -= stuffs[i].xspeed;
                y -= stuffs[i].yspeed;
                break;

            case 6:// random directions (smooth curves, constant speed, good for bees)
                //stuffs[i].direction += -20 + AL_RAND()%40;
                if(stuffs[i].blocked > 0 )stuffs[i].direction += AL_RAND()%180;
                stuffs[i].direction += -10 + AL_RAND()%20;
                stuffs[i].xspeed = ( cos( deg2rad( stuffs[i].direction ) ) * 2 );
                stuffs[i].yspeed = ( sin( deg2rad( stuffs[i].direction ) ) * 2 );
                x -= stuffs[i].xspeed + get_scroll_speed();
                y -= stuffs[i].yspeed;
                //add bounce  ?
                break;

            case 7:// random directions (smooth curves, constant speed, no translation )
                stuffs[i].direction += -10 + AL_RAND()%20;
                stuffs[i].xspeed = ( cos( deg2rad( stuffs[i].direction ) ) * 2 );
                stuffs[i].yspeed = ( sin( deg2rad( stuffs[i].direction ) ) * 2 );
                x -= stuffs[i].xspeed;
                y -= stuffs[i].yspeed;
                break;


            case 8://seek player (jelly fishes)
                // find player
                stuffs[i].direction = get_angle(playr.x, playr.y, stuffs[i].x, stuffs[i].y);
                float spdx = ( sin( deg2rad(stuffs[i].direction) ) * 1.2 );
                float spdy = ( cos( deg2rad(stuffs[i].direction) ) * 1.2 );

                stuffs[i].xspeed = spdx;
                stuffs[i].yspeed = spdy;
                y += stuffs[i].yspeed;
                x += stuffs[i].xspeed;
                break;

            case 9://ROCKET (WAIT FOR PLAYER TO TRIG Y SPEED)

                if(playr.x >= stuffs[i].x)stuffs[i].yspeed = 4;
                y -= stuffs[i].yspeed;
                x -= stuffs[i].xspeed + get_scroll_speed();
                break;

            case 10://PIC (WAIT FOR PLAYER TO TRIG Y SPEED)

                if(playr.x >= stuffs[i].x)stuffs[i].yspeed = -4;
                y -= stuffs[i].yspeed;
                x -= stuffs[i].xspeed + get_scroll_speed();
                break;

            case 11://SQUARTRON - MOVE ONLY STRAIGHT

                if(stuffs[i].blocked > 0 || stuffs[i].hit > 0)stuffs[i].direction = (AL_RAND() % 4)*90;

                //if(stuffs[i].blocked > 0 )stuffs[i].direction+= 90;//work

                stuffs[i].xspeed = ( cos( deg2rad( stuffs[i].direction ) ) * 2 );
                stuffs[i].yspeed = ( sin( deg2rad( stuffs[i].direction ) ) * 2 );
                x -= stuffs[i].xspeed + get_scroll_speed();
                y -= stuffs[i].yspeed;
                break;


            case 12: //crabs

                break;

            case 18:// ZIGZAG :) //

                /*
                if(stuffs[i].age < 80)stuffs[i].direction = 90;//straight
                else if (stuffs[i].age == 80){
                    if(stuffs[i].y<128)stuffs[i].direction = 250;//diagonale vers la bas
                    else stuffs[i].direction = 290;//diagonale vers la haut
                }
                else if(stuffs[i].age > 160)stuffs[i].direction = 90;//straight
                */
                if(stuffs[i].blocked>0)stuffs[i].direction+= 90;//

                stuffs[i].xspeed = sin( deg2rad( stuffs[i].direction ) ) * 4;
                stuffs[i].yspeed = cos( deg2rad( stuffs[i].direction ) ) * 4;
                y -= stuffs[i].yspeed;
                x -= stuffs[i].xspeed;
                break;

            case 19:// MERDOUILLES ATTACK (DEMI TOUR COURBE)//
                //arrive par le haut, fait un demi tour courbe et repart (facon coryoon)
                if( stuffs[i].age < 60 )stuffs[i].direction = 90;
                else if (stuffs[i].direction < 270) stuffs[i].direction+=3;

                if(stuffs[i].blocked>0)stuffs[i].direction+=3;

                stuffs[i].xspeed = sin( deg2rad( stuffs[i].direction ) ) * 4;
                stuffs[i].yspeed = cos( deg2rad( stuffs[i].direction ) ) * 4;
                y -= stuffs[i].yspeed;
                x -= stuffs[i].xspeed;
                break;


            case 20://COME BY THE RIGHT, THEM TRY TO ESCAPE
                if(stuffs[i].age < 30) stuffs[i].xspeed += 0.1;
                else if (stuffs[i].age < 180) stuffs[i].xspeed = get_scroll_speed();
                else stuffs[i].xspeed= -1;//retourne chez ta mere

                y -= stuffs[i].yspeed;
                x -= stuffs[i].xspeed;// + get_scroll_speed();

                break;

            case 21://COME BY THE RIGHT, THEN STOP AT DEFINED POSITION (BEE NEST)
                if(stuffs[i].x > 200 ) stuffs[i].xspeed += 0.1;
                else stuffs[i].xspeed = 0;
                y -= stuffs[i].yspeed;
                x -= stuffs[i].xspeed;// + get_scroll_speed();
                //if(stuffs[i].energy < 8)stuffs[i].yspeed = sin( deg2rad( game_count ) );//test
                break;

        }

        if( stuffs[i].fall > 0 )y++;

        stuffs[i].blocked = 0;//NO COLISION

        // DETECT COLISION WITH TILE //
        if( x < stuffs[i].x){//MOVE LEFT
            if( tile_colision(x,y) || tile_colision(x,y+h) ){
                stuffs[i].blocked = 1;//COLISION
                particule(x-2, y-2 + h/2, 0, 0 , 8, CROSS, 0, 0);
            }
        }

        if( x > stuffs[i].x){//MOVE RIGHT
            if( tile_colision(x+w,y) || tile_colision(x+w,y+h) ){
                stuffs[i].x-= get_scroll_speed();
                stuffs[i].blocked = 1;//COLISION
                particule(x+w-2, y-2 + h/2, 0, 0 , 8, CROSS, 0, 0);
            }
        }

        if( y < stuffs[i].y){// MOVE UP
            if( tile_colision(x,y) || tile_colision(x+w,y) ){
                stuffs[i].blocked = 1;//COLISION
                particule(x-2+w/2, y-2, 0, 0 , 8, CROSS, 0, 0);
            }
        }

        if( y > stuffs[i].y){// MOVE DOWN
            if( tile_colision(x,y+h) || tile_colision(x+w,y+h) ){//TOUCEH LE SOL
                stuffs[i].blocked = 1;//COLISION
                particule(x-2+w, y-2+h, 0, 0 , 8, CROSS, 0, 0);
            }
        }

        if(stuffs[i].blocked == 0){
            stuffs[i].x = x;
            stuffs[i].y = y;
        }else{
            stuffs[i].x-=get_scroll_speed();

        }
/*
        if( stuffs[i].fall > 0 ){// so its not flying :) //
            //we must check if there is no "ground" around

            //int w = stuffs[i].w/2;
            if( tile_colision( x+w/2, y + h) > 0 ){
                //ca touche, faut pas descendre plus bas
                stuffs[i].x-= get_scroll_speed();
            }else{
                //ca degringole
                stuffs[i].y++;
                stuffs[i].x-= get_scroll_speed();
                continue;
            }
        }
*/

        // fragile stuffs die on colision
        if(stuffs[i].fragile>0 && stuffs[i].blocked > 0)enemy_death(i);

        //SET NEW POSITION
        //stuffs[i].x = x;
        //stuffs[i].y = y;


        // DELAY AN ACTION //
        if(stuffs[i].delay > 0){
            suggested_x = stuffs[i].x;
            suggested_y = stuffs[i].y;
            if(game_count%stuffs[i].delay == 0)do_event(stuffs[i].event);
        }



        /*
        if(stuffs[i].bonus == TRUE){//its a bonus, it must swing
            //stuffs[i].yspeed = sin( deg2rad(stuffs[i].x*4) );//swing
            stuffs[i].yspeed = sin( deg2rad(game_count*4 + stuffs[i].x) );//swing
        }
        */

       //if(stuffs[i].y<=0)stuffs[i].yspeed = -stuffs[i].yspeed;//bounce
        if(stuffs[i].hit){// hit by a bullet //
            //stuffs[i].hit = 0;//on fait ca dans draw_stuff()
            stuffs[i].x += 2;
        }




        // STUFF OUT OF BOUNDARIES //
        if(stuffs[i].y < -64 || stuffs[i].x < -640 || stuffs[i].y > 256 + 64 || stuffs[i].x > 640)stuffs[i].energy = 0;//dead


        // ENEMY SHOT A BULLET //
       if(stuffs[i].bonus == 0 && stuffs[i].munitions > 0){

           if(AL_RAND() %64 == 0){

               int free = get_free_pruno();//warning, may return -1
               if(free<0)return;
               stuffs[i].munitions--;

                // find (center of) player
               //int angle = get_angle(playr.x + 8, playr.y + 8, stuffs[i].x + stuffs[i].shotx, stuffs[i].y + stuffs[i].shoty );
               int angle = get_angle(stuffs[i].x + stuffs[i].shotx, stuffs[i].y + stuffs[i].shoty, playr.x + 8, playr.y + 8 );

               pruno[free].speed = 2;
               pruno[free].direction = angle;

               //float spdx = ( sin( deg2rad(angle) ) * 2 );
               //float spdy = ( cos( deg2rad(angle) ) * 2 );

               pruno[free].x = stuffs[i].x + stuffs[i].shotx;
               pruno[free].y = stuffs[i].y + stuffs[i].shoty;


               //pruno[free].xspeed = spdx;
               //pruno[free].yspeed = spdy;

               pruno[free].sprite = BULLET13;
               pruno[free].framecout = 1;
               pruno[free].running = 1;
           }
       }

    }
}







void draw_stuff(void)//draw enemies and powerups etc
{
    int i;
    for (i=0; i< NUM_STUFF; i++){

        if(stuffs[i].energy < 1 )continue;//its dead

        /*
        if(stuffs[i].x && stuffs[i].y && stuffs[i].w && stuffs[i].h){
            rectfill(buffer, stuffs[i].x, stuffs[i].y, stuffs[i].x + stuffs[i].w, stuffs[i].y + stuffs[i].h, 4);//DEBUG RECTANGLE
        }
        */
        int offset = 0;

        if(stuffs[i].hit > 0 && stuffs[i].spritehit > 0){
            draw_sprite(buffer, gfx[stuffs[i].spritehit].dat, stuffs[i].x, stuffs[i].y - offset);//colision sprite
        }else{
            draw_sprite(buffer, gfx[(stuffs[i].spritenum + stuffs[i].step)].dat, stuffs[i].x, stuffs[i].y - offset);
        }


        if(stuffs[i].hit > 0)stuffs[i].hit--;//BING

        if(stuffs[i].framecount>0){//animated sprite
            if(game_count%4 == 0)stuffs[i].step++;
            if(stuffs[i].step > stuffs[i].framecount) stuffs[i].step = 0;//loop animation
        }

        // debug //
        /*
        if(stuffs[i].bonus == 0){
            // get angle from ennemy to player//
            int angle = get_angle(playr.x+8, playr.y+8, stuffs[i].x, stuffs[i].y);
            int newx = stuffs[i].x + stuffs[i].shotx + ( sin( deg2rad(angle) ) * 1 );
            int newy = stuffs[i].y + stuffs[i].shoty + ( cos( deg2rad(angle) ) * 1 );
            draw_sprite(buffer, gfx[CROSS].dat, newx, newy);//CROSS
        }
        */
    }
}



void init_stuffnum(int id){

    stuffs[id].age = 0;
    stuffs[id].bonus = FALSE;
    stuffs[id].score = 0;

    stuffs[id].step = 0;//current frame
    stuffs[id].spritenum = 0;
    stuffs[id].spritehit = 0;
    stuffs[id].framecount = 0;//no anim
    stuffs[id].fall = 0;//dont fall

    stuffs[id].delay = 0;
    stuffs[id].event = 0;

    stuffs[id].hit = 0;
    stuffs[id].blocked = 0;
    stuffs[id].fragile = 0;

    stuffs[id].direction = 0;
    stuffs[id].xspeed = 0;
    stuffs[id].yspeed = 0;
    stuffs[id].shotx = 0;
    stuffs[id].shoty = 0;

    stuffs[id].energy = 0;//dead
    stuffs[id].munitions = 0;
    stuffs[id].munition_type = 0;
    stuffs[id].movement_type = 0;

}

int get_free_stuff(void)//get free enemy/bonus slot
{
    int i;
    for(i=0; i < NUM_STUFF; i++){
        if(stuffs[i].energy < 1 ){
            init_stuffnum(i);//ready to use
            return i;// use this one
        }
    }
    return -1;
}


void add_bonus(int tx, int ty)
{

    int bonus_number = (AL_RAND() % 10);//bonus sprite
    int i = get_free_stuff();
    if(i < 0)return;

    switch( bonus_number ){

        case 0:// SPEEDUP //
            if( playr.speed >= 4 ){add_bonus(tx, ty); return;}//we already have the maximum speed
            stuffs[i].spritenum = SPEED00;
            stuffs[i].framecount = 7;
            break;

        case 1:// OPTION //
            if( playr.options >= 3 ){add_bonus(tx, ty); return;}//we already have the maximum options
            stuffs[i].spritenum = OPTION00;//OPTION !!
            //stuffs[i].framecount = 3;
            break;

        case 2://weapon 1
            if( playr.weapon == BULLET00 ){add_bonus(tx, ty); return;}//we already have this weapon
            stuffs[i].spritenum = POW_00;//basic
            break;

        case 3://weapon 2
            if( playr.weapon == BULLET02 ){add_bonus(tx, ty); return;}//we already have this weapon
            stuffs[i].spritenum = POW_LAZER;//LASER
            break;

        case 4://weapon
            if( playr.weapon == BULLET03 ){add_bonus(tx, ty); return;}//we already have this weapon
            stuffs[i].spritenum = POW_02;//FIRE
            //stuffs[i].framecount = 1;
            break;

        case 5://weapon
            if( playr.weapon == BULLET05 ){add_bonus(tx, ty); return;}//we already have this weapon
            stuffs[i].spritenum = POW_BOUNCE;//BOULE
            break;

        case 6://HOMING MISSILE
            if( playr.homing >= 4 ){add_bonus(tx, ty); return;}//we already have this weapon
            stuffs[i].spritenum = POW_HOMING;//HOMING BULLET
            //stuffs[i].framecount = 1;
            break;

        case 7:// SHIELD
            if( playr.shield >0 ){add_bonus(tx, ty); return;}//ONE SHIELD IS ENOUGH
            stuffs[i].spritenum = POW_05;//SHIELD
            //stuffs[i].framecount = 1;
            break;

        case 8:// extra life
            if( playr.life >= 2 ){add_bonus(tx, ty); return;}//dont make it too easy
            stuffs[i].spritenum = POW_LIFE;//VIE
            break;

        case 9://CASH
            stuffs[i].spritenum = POW_CASH;//
            break;
    }

    stuffs[i].bonus = TRUE;
    stuffs[i].fall = 1;
    stuffs[i].energy = 1;
    stuffs[i].x = tx;
    stuffs[i].y = ty - 8;
    stuffs[i].yspeed = 0;
    stuffs[i].xspeed = 0;

    BITMAP *bmp;
    bmp = gfx[ stuffs[i].spritenum ].dat;
    stuffs[i].w = bmp->w;
    stuffs[i].h = bmp->h;

}





void change_player_speed(void)//change player speed mode
{
    playr.speed++;
    if(playr.speed > 8)playr.speed = 1;
}

void init_player(void)
{
    playr.speed = 2;
    playr.options = 0;
    playr.shield = 0;
    playr.homing = 0;
    playr.weapon = BULLET00;
    playr.invincible = 100;
    playr.x = 0;
    playr.y = 100;
    //playr.energy = 3;
    playr.life = 2;



}

void game_start(){

    //    LEVEL = level;

    clear_keybuf();

    char fn[256] = "levels/";
    strcat(fn, file_name);
    if(!exists(fn)){
        sprintf(message, "Map file '%s' not found", fn);
        return;
    }

    //allegro_message("Map '%s'", file_name);

    srand(time(NULL));

    game_status = GS_PLAY;

    init_player();
    init_stuff();
    init_particules();
    init_bullets();
    playr.score = 0;


    load_tilemap(file_name);

    game_count = 0;
    game_scroll = 0;
    last_column = 0;
    shift=0;//moche
}


void player_explode(){

    int i;

    // EXPLOSION
    i = get_free_particule();
    if(i<0)return;
    particules[i].x = playr.x;
    particules[i].y = playr.y;
    particules[i].spritenum = EXPLODE00;//default sprite
    particules[i].framecount = 4;//just one sprite
    particules[i].life = 16;//make it real for n frames !

    for(i=0;i<16;i++){
        int free = get_free_particule();
        if(free < 0)return;
        particules[free].x = playr.x;
        particules[free].y = playr.y;

        particules[free].spritenum = PARTICULE00 + AL_RAND() %2;//un peu gros

        int angle = AL_RAND() % 360;
        float speed = (AL_RAND()%5 / 2);

        particules[free].vx = ( cos( deg2rad(angle) ) * speed );
        particules[free].vy = ( sin( deg2rad(angle) ) * speed );

        particules[free].framecount = 0;//just one sprite
        particules[free].life = 10 + (AL_RAND() % 10);//make it real for n frames !
    }
    play_sound(SFX03);
}

void player_die(void)// kill the player, etc
{

    playr.options = 0;
    playr.homing = 0;
    playr.slowmotion = 0;

    if(playr.life > 0){
        playr.life--;//et ouif
    }else{
        gameover();
    }

    player_explode();

    playr.x = 0;
    //playr.y = 100;
    playr.speed = 2;
    playr.invincible = 200;//200 frames of invincibility

    int i;
    for( i=0; i < 32; i++ ){//CLEAR OPTIONS POSITIONS
        playerpos[i].x = playr.x;
        playerpos[i].y = playr.y;
    }

    srand(time(NULL));//SHAKE THOSE DICES BABY

    //playr.energy = 3;//UNUSED
}

void player_hit(void){

    if(playr.shield > 0){
        playr.shield = 0;
        playr.invincible = 50;//50 frames of invincibility
        return;
    }

    if(playr.options <= 0){
        player_die();
        return;
    }

    playr.options--;
    playr.invincible = 50;//50 frames of invincibility
    play_sound(SFX13);
    pop_bonus(BONUS_OUCH);
}


//int player_sprite = 2;
void draw_player(void)
{
    int i;
    //if(playr.invincible > 0 && playr.invincible %2)return;

    for(i=0; i < playr.options; i++){//options
        draw_sprite(buffer, gfx[OPTION00 + ( (game_count+i)%16)/4].dat, playerpos[8 + i*4].x, playerpos[8 + i*4].y); // Draw options
    }

    int spritenum = SHIP03;

    if(playr.tilt > 4)spritenum = SHIP04;
    if(playr.tilt > 8)spritenum = SHIP05;
    if(playr.tilt > 12)spritenum = SHIP06;

    if(playr.tilt < -4)spritenum = SHIP02;
    if(playr.tilt < -8)spritenum = SHIP01;
    if(playr.tilt < -12)spritenum = SHIP00;

    if(playr.invincible > 0)playr.invincible--;
    if(playr.invincible%2)return;

    draw_sprite(buffer, gfx[spritenum].dat, playr.x, playr.y); // Draw the player

    if(playr.shield > 0){// PLAYER SHIELD :)
        if(game_count%8 < 4 ){
            draw_sprite(buffer, gfx[SHIELD00].dat, playr.x-2, playr.y-2); // Draw the Shield
        }else{
            draw_sprite(buffer, gfx[SHIELD01].dat, playr.x-2, playr.y-2); // Draw the Shield
        }
    }

    // balance //
    if(playr.tilt > 0)playr.tilt--;
    if(playr.tilt < 0)playr.tilt++;
}


void draw_rasters(void)
{
    int i;
    for( i = 0; i < 256; i++ ){
        hline(buffer, 0, i, SCREEN_W, 16 + AL_RAND() % 6);
    }
}


void draw_tile(BITMAP *dest, unsigned char tilenumber, int x, int y){

    if (!tileset)return;
    if( tilenumber == 0 )return;
    int row = tilenumber/20;
    int col = tilenumber%20;
    //masked_blit(tileset , buffer, col*16 , row*16 , x , y , 16 , 16);
    masked_blit(tileset , dest, col*16 , row*16 , x , y , 16 , 16);
}

void draw_bgtile(BITMAP *dest, unsigned char tilenumber, int x, int y){

    if (!tileset)return;
    if( tilenumber == 0 )return;
    int row = tilenumber/20;
    int col = tilenumber%20;
    //masked_blit(tileset , buffer, col*16 , row*16 , x , y , 16 , 16);
    masked_blit(bgtileset , dest, col*16 , row*16 , x , y , 16 , 16);
}


void draw_event(int spritenumber, int x, int y)//editor
{
    draw_sprite(buffer, gfx[spritenumber].dat, x, y); // Draw the tiles
}



/*
void set_background(int bgnum)
{
    if(bgnum < 0)return;
    if(bgnum > 16)return;
//    BGNUM = bgnum;
    sprintf(message, "BG:%d", bgnum);//
}
*/

/*
void set_random_background(){
    set_background(AL_RAND() % 10);
    init_particules();
    init_stuff();
    init_bullets();
}
*/

void draw_background(void)
{
    //int i,j;
    int i,j;
    BITMAP *bmp = bg[level.background].dat;
    if( bmp == NULL )return;
    int w = bmp->w;
    int h = bmp->h;

    int colnumber = ( 320 / w ) + 2;
    int rownumber = ( 256 / h ) + 1;

    for(i = 0; i < colnumber; i++){
        for(j = 0; j < rownumber; j++){
            blit( bmp, buffer, 0, 0, i * w - ( (int)(game_scroll / 4) % w) , j*h, w, h );
        }
    }


    /*
    for(i = 0; i < colnumber; i++){
        for(j=0; j < 16; j++){
            //int w = 256;

            //masked_blit(bmp, buffer, 0, j * 16, i * w - ( (int)(game_scroll / bgspeed[level.background][j]) % w), j*16, w, 16);
            masked_blit(bmp, buffer, 0, j * 16, i * w - ( (int)(game_scroll / 4) % w), j*16, w, 16);
        }
    }
    */

    //destroy_bitmap(bmp);
}


void draw_tiles_ingame(void)
{
    if(tileset == NULL)return;
    int i,j;
    for(i = 0; i < 21; i++){//ca sert a rien de dessiner au dela de l'ecran, tile_colision() ne se sert pas du buffer ;)
        for(j = 0; j < 16; j++){
            //draw_sprite(buffer, tiles[0].dat, i*16, j*16);//Draw the tiles
            //char tile_id = map[j][i + (game_scroll)/16];
            char bgtile_id = mapp[j][i + (game_scroll)/16].bg_tile;
            char tile_id = mapp[j][i + (game_scroll)/16].tile;
            draw_bgtile(buffer, bgtile_id, i*16 - (game_scroll)%16, j*16);
            draw_tile(buffer, tile_id, i*16 - (game_scroll)%16, j*16);
        }
    }
}


void draw_particules(void)
{
    int i;
    for (i = 0; i < MAX_PARTICULES; i++){

        if(particules[i].life < 1 )continue;//its a dead particule
        if( particules[i].spritenum == 0 )return;//its an error

        if( particules[i].blink > 0){
            if(game_count%2)return;//
        }

        if(game_count%4 == 0)particules[i].step++;
        if(particules[i].step > particules[i].framecount)particules[i].step = 0;
        draw_sprite(buffer, gfx[particules[i].spritenum + particules[i].step].dat, particules[i].x, particules[i].y);
    }
}

void draw_bg_particules(void)
{
    int i;
    for (i = 0; i < MAX_PARTICULES; i++){

        if( bg_particules[i].life < 1 )continue;//its a dead particule
        if( bg_particules[i].spritenum == 0 )return;//its an error

        if( bg_particules[i].blink > 0){
            if(game_count%2)return;//
        }

        if(game_count%4 == 0)bg_particules[i].step++;
        if(bg_particules[i].step > bg_particules[i].framecount)bg_particules[i].step = 0;
        draw_sprite(buffer, gfx[bg_particules[i].spritenum + bg_particules[i].step].dat, bg_particules[i].x, bg_particules[i].y);
    }
}

int get_free_particule(void){
    int i;
    for (i=0; i< MAX_PARTICULES; i++){
        if(particules[i].life == 0){
            particules[i].x = 0;
            particules[i].y = 0;
            particules[i].vx = 0;
            particules[i].vy = 0;
            particules[i].framecount = 0;
            particules[i].spritenum = 0;
            particules[i].step = 0;
            particules[i].blink = 0;
            if( i > highest_particule_number )highest_particule_number = i;
            return i;
        }
    }
    return -1;
}

int get_free_bg_particule(void){
    int i;
    for (i=0; i< MAX_PARTICULES; i++){
        if(bg_particules[i].life == 0){
            bg_particules[i].x = 0;
            bg_particules[i].y = 0;
            bg_particules[i].vx = 0;
            bg_particules[i].vy = 0;
            bg_particules[i].framecount = 0;
            bg_particules[i].spritenum = 0;
            bg_particules[i].step = 0;
            bg_particules[i].blink = 0;
            //if( i > highest_particule_number )highest_particule_number = i;
            return i;
        }
    }
    return -1;
}

void colide(int tx, int ty)//create particules from colision at position tx, ty
{
    int i;

    // EXPLOSION
    i = get_free_particule();
    if(i<0)return;
    /*
    particules[i].x = tx - 8;
    particules[i].y = ty - 8;
    particules[i].spritenum = EXPLODE00;//default sprite
    particules[i].framecount = 4;//just one sprite
    */

    //DEBUG PARTICULE
    if(show_collisions>0){
        particules[i].x = tx-3;//CENTER THE 5x5 CROSS
        particules[i].y = ty-3;//CENTER THE 5x5 CROSS
        particules[i].spritenum = CROSS;
        particules[i].life = 64;//make it real for n frames !
        particules[i].vx = get_scroll_speed();
    }

    int PARTICULE;
    switch(playr.weapon){//des particules adaptees en fonctions de l'arme, c'est la classe

        case BULLET02://LASER
            PARTICULE = STAR02;
            break;

        case BULLET03://FIRE
        case BULLET04://FIRE
            PARTICULE = STAR08;
            break;

        case BULLET05://BOULE
            PARTICULE = STAR08;
            break;

        default:
            PARTICULE = STAR06;
            break;
    }

    // PETITE PARTICULE //
    i = get_free_particule();
    if(i<0)return;
    particules[i].x = tx;
    particules[i].y = ty;
    particules[i].vx = 2 + (AL_RAND() % 4);
    particules[i].vy = -2 + (AL_RAND() % 4);
    particules[i].spritenum = PARTICULE;//default sprite
    particules[i].life = 10;//make it real for n frames !

    //une deuxieme pendant qu'on y est
    i = get_free_particule();
    if(i<0)return;
    particules[i].x = tx;
    particules[i].y = ty;
    particules[i].vx = 2 + (AL_RAND() % 4);
    particules[i].vy = -2 + (AL_RAND() % 4);
    particules[i].spritenum = PARTICULE;//default sprite
    particules[i].life = 10;//make it real for n frames !
}


void explode2(int tx, int ty)//create explosion at position tx,ty
{
    int i;
    //int free = get_free_particule;

    for (i=0; i< (5 + AL_RAND()%12); i++){
        int free = get_free_particule();
        if(free<0)return;
        particules[free].x = tx;
        particules[free].y = ty;
        particules[free].spritenum = PARTICULE00 + AL_RAND() %2;//un peu gros
        int angle = AL_RAND() % 360;
        particules[free].vx = ( cos( deg2rad(angle) ) * 1 - AL_RAND() %4 );
        particules[free].vy = ( sin( deg2rad(angle) ) * 2 );
        particules[free].life = (5 + AL_RAND()%22);
        //particule( tx, ty,
    }
}


void burst(int x, int y, int direction){//player reactor particules

    if(game_count%2)return;

    int jitter = -20 + AL_RAND()%40;
    direction += jitter;
    int speed = 2;
    int life = 5 + AL_RAND()%10;//
    particule( x,  y, speed, direction, life , STAR00, 2, 0);
}

void move_particules()//
{
    int i;
    for ( i=0; i < MAX_PARTICULES; i++ ){
        if(particules[i].life < 1 )continue;//its a dead particule
        particules[i].life--;
        //if(particules[i].spritenum > 0)particules[i].spritenum--;//explosions
        particules[i].x-= particules[i].vx;
        particules[i].y-= particules[i].vy;
    }

    // BG PARTICULES //
    for ( i=0; i < MAX_PARTICULES; i++ ){
        if(bg_particules[i].life < 1 )continue;//its a dead particule
        bg_particules[i].life--;
        //if(particules[i].spritenum > 0)particules[i].spritenum--;//explosions
        bg_particules[i].x-= bg_particules[i].vx + get_scroll_speed();
        bg_particules[i].y-= bg_particules[i].vy;
    }

}

/*
void draw_timer(){//time and stuff
    //textout_ex(buffer, font, "oddity v0.0-beta1", 0, 0, makecol(255, 255, 255), -1);
    textprintf_ex(buffer, font, 0, 0, 5, -1, "%i", 60);
}
*/

void draw_game_info()// todo: change this name
{
    // print2(message, 0, 10);
    char chr[32];
    sprintf(chr, "P1 %07d - HI %07d", playr.score, 0); //
    print(ptibuffer, chr, 60, 1);

    int i;
    for( i=0; i <= playr.life; i++)draw_sprite( ptibuffer, gfx[LIFE].dat, 310 - i*10, 2 );//vies

}

void draw_frame(void){

    /////////////////////////////////////////////////////////////////////////////
    // DRAW THE SHIT
    /////////////////////////////////////////////////////////////////////////////

    draw_background();
    draw_bg_particules();
    draw_tiles_ingame();
    draw_particules();
    draw_stuff();//ennemies, powerups, etc
    draw_bullets();//player bullets
    draw_game_info();//score, lifes
    //draw_timer();//fps shit
    draw_player();//
    draw_alert();//messages

    //level_intro();//intro text shit MUSIC BY ...

    //sprintf(message, "%06d", game_count);//GAME COUNTER
}

int info_pos;
void level_intro(void){

    if(game_count > 180)return;

    // SHOW MUSIC INFO :)
    char chars[64];
    if(game_count < 30)info_pos = 230 - game_count;
    if(game_count > 150 )info_pos++;

    //info_pos+=playr.yshift;
    rectfill(buffer, 0, info_pos - 2 + playr.yshift, 320, info_pos + playr.yshift + 22, 16 );
    sprintf(chars, "Music : %s", info.song ); print(buffer, chars, 4 , info_pos + playr.yshift );
    sprintf(chars, "Artist: %s", info.author ); print(buffer, chars, 4 , info_pos + 10 + playr.yshift );

    if(game_count == 30)inform(level.level_name);
}

void pop_bonus( int spritenumber ){
    particule(playr.x, playr.y, 1, 0, 32, spritenumber, 0, 1);
}

void particule( float x, float y, float speed, int direction, int life, int spritenumber, int framecount, char blink)//CREATE A NEW PARTICULE
{
    int i = get_free_particule();
    if( i < 0 )return;
    particules[i].x = x;
    particules[i].y = y;
    particules[i].vx = sin( deg2rad(direction) ) * speed;
    particules[i].vy = cos( deg2rad(direction) ) * speed;
    particules[i].spritenum = spritenumber;//sprite
    particules[i].framecount = framecount;
    particules[i].blink = blink;//
    particules[i].life = life;//make it real for n frames !
}

void bg_particule( float x, float y, float speed, int direction, int life, int spritenumber, int framecount, char blink)//CREATE A NEW PARTICULE
{
    int i = get_free_bg_particule();
    if( i < 0 )return;
    bg_particules[i].x = x;
    bg_particules[i].y = y;
    bg_particules[i].vx = sin( deg2rad(direction) ) * speed;
    bg_particules[i].vy = cos( deg2rad(direction) ) * speed;
    bg_particules[i].spritenum = spritenumber;//sprite
    bg_particules[i].framecount = framecount;
    bg_particules[i].blink = blink;//
    bg_particules[i].life = life;//make it real for n frames !
}

void get_bonus(int e){//get bonus

    stuffs[e].energy = 0;//pick it up
    int type  = stuffs[e].spritenum;



    switch( type ){

        case BONUS00:// Energy
            //sprintf( message, "BONUS"); //
            playr.score+=1000;
            play_sound( 9 );
            break;

        case OPTION00://I ??
            //sprintf(message, "OPTION++"); //
            play_sound( 10 );
            if(playr.options < 3)playr.options++;
            pop_bonus( BONUS_OPTION );
            break;

        case SPEED00:// Speedup
            if( playr.speed < 4 )playr.speed++;
            //sprintf( message, "SPEED:%d", playr.speed); //
            play_sound( 8 );
            pop_bonus( BONUS_SPEED );
            break;

        case BONUS03:// P
            //MAX_BULLET++;//
            //ammo[MAX_BULLET].xspeed = 8;//bullet speed
            //ammo[MAX_BULLET].yspeed = -4 + (AL_RAND() %8);
            play_sound( 11 );
            break;

        case BULLET00://BASIC
            //sprintf( message, "BULLET00"); //
            playr.weapon = BULLET00;
            break;

        case POW_LAZER://LASER
            //sprintf( message, "BULLET02");//LASER
            playr.weapon = BULLET02;
            pop_bonus( BONUS_LAZER );
            break;

        case BULLET03://FIRE
            //sprintf( message, "BULLET04"); //FIRE
            playr.weapon = BULLET04;
            pop_bonus( BONUS_FIRE );
            break;

        case POW_BOUNCE://BOULE
            //sprintf( message, "BULLET05"); //
            playr.weapon = BULLET05;
            pop_bonus( BONUS_BOUNCE );
            break;

        case POW_HOMING:
            playr.homing++;
            pop_bonus( BONUS_MISSILE );
            break;

        case SHIELD00://
            playr.shield = 1;
            //inform("Shield");
            pop_bonus( BONUS_SHIELD );
            break;

        case POW_CASH:
            playr.score+=1000;
            pop_bonus( BONUS_CASH );
            break;

        case POW_LIFE://LIFE
            if(playr.life < 9)playr.life++;
            pop_bonus( BONUS_LIFE );
            break;

        default:
            //sprintf( message, "POWERUP?%d", type); //
            play_sound( 12 );
            pop_bonus( BONUS_QUESTION );
            break;
    }


}


char tile_colision(int x, int y){//or "is the point x,y subject to tile colision"

    if(x < -64 || x > 640 || y < 0 || y > 256)return 0;//out of bounds

    int til_x = (x + game_scroll%16)/16;
    int til_y = y/16;
    //char tile_id = map[til_y][til_x + ((game_scroll)/16) ];
    unsigned char tile_id = mapp[til_y][til_x + ((game_scroll)/16) ].tile;

    //if(tile_id < 20)return 0;//it's a plant, or a 'non colidable' tile

    if(tile_id<=0){
        return 0;//DO NOT COLIDE
    }else{
        // PIXEL VERIFICATION : DO NOT COLIDE WITH TRANSPARENT PIXELS //
        int row = tile_id/20;
        int col = tile_id%20;
        // ACHTUNG //
        int c = getpixel( tileset , col*16 + (x - game_scroll%16)%16 , row*16 + y%16  );// retourne l'index de la couleur :) //
        if( c == 0 )return 0;

        //COLISION !
        return 1;
    }
}


void check_player_with_enemies(){//and tiles

    int e;

    // CONTACT COLISION //
    for(e = 0; e < NUM_STUFF; e++) {

        if(stuffs[e].energy < 1)continue;//its dead
        if (check_bb_collision( playr.x + 2, playr.y + 2, 14, 14, stuffs[e].x, stuffs[e].y, stuffs[e].w, stuffs[e].h)) {
             if(stuffs[e].bonus == TRUE){//yeah
                 get_bonus(e);
             }else{//ouch
                if( playr.invincible > 0 )return;
                player_hit();
                //player_die();
             }
         }
    }

    if(playr.invincible > 0)return;

    // CHECK PLAYER WITH TILE COLISION // (in 3 points -> x,y / x,y+16 / x+16, y+8)
    if( tile_colision(playr.x, playr.y + 2) ){player_hit();return;}//LE HAUT, ET JE SUIS SYMPA
    if( tile_colision(playr.x, playr.y + 12) ){player_hit();return;}//LE BAS, AVEC UNE PETITE TOLERANCE
    if( tile_colision(playr.x + 14, playr.y + 8) ){player_hit();return;}//LE NEZ

    //putpixel(buffer,playr.x, playr.y + 2, 4);//point rouge
    //putpixel(buffer,playr.x, playr.y + 12, 4);//point rouge

    //check player with enemy bullets
    for(e = 0; e < MAX_PRUNES; e ++) {

        if(pruno[e].running < 1)continue;//its dead
        if (check_bb_collision(playr.x + 2, playr.y + 2, 14, 14, pruno[e].x, pruno[e].y, 4, 4)) {
            player_hit();
            pruno[e].running = 0;//stop the pruno
        }

        // CHECK PRUNES (ENEMY SHOTS) WITH TILES //
        if(tile_colision(pruno[e].x, pruno[e].y))pruno[e].running = 0;//stop enemy bullet
    }
}



void check_bullet_with_enemies(int bullet_id){

    int e;

    // GET BULLET SIZE //
    int sprite = ammo[bullet_id].sprite;

    BITMAP *bmp;
    bmp = gfx[sprite].dat;
    if(!bmp)return;

    int bullet_width = bmp->w;
    int bullet_height= bmp->h;

    for(e = 0; e < NUM_STUFF; e ++){

        if(ammo[bullet_id].running == 0)continue;

        if(stuffs[e].energy < 1)continue;
        if(stuffs[e].bonus == TRUE)continue;//on va pas degommer un bonus quand meme !
        //stuffs[e].spritenum;

        if (check_bb_collision(ammo[bullet_id].x, ammo[bullet_id].y, bullet_width, bullet_height, stuffs[e].x, stuffs[e].y, stuffs[e].w, stuffs[e].h)){

            ammo[bullet_id].running = 0;//stop bullet
            colide( ammo[bullet_id].x + bullet_width, ammo[bullet_id].y);//show colision

            if(show_collisions>0)rectfill(buffer, ammo[bullet_id].x, ammo[bullet_id].y, stuffs[e].x + stuffs[e].w, stuffs[e].y + stuffs[e].h, 4);

            stuffs[e].energy--;
            stuffs[e].hit = 2;//

            if(stuffs[e].energy == 0)
            {
                enemy_death(e);// you did it !
            }else{//its not dead yet
                //play_sound(1);
            }
        }
    }


    if(ammo[bullet_id].running==0)return;


    // CHECK BULLET WITH TILE COLISION //
    int colision = -1;
    //int i;

    if( tile_colision( ammo[bullet_id].x, ammo[bullet_id].y ) )colision = 1;//TOP LEFT
    if( tile_colision( ammo[bullet_id].x + bullet_width , ammo[bullet_id].y ) )colision = 2;//TOP RIGHT
    if( tile_colision( ammo[bullet_id].x + bullet_width , ammo[bullet_id].y + bullet_height ) )colision = 3;//BOTTOM RIGHT
    if( tile_colision( ammo[bullet_id].x, ammo[bullet_id].y + bullet_height) )colision = 4;//BOTTOM LEFT
    if(colision < 0)return;

    // IF COLISION IS DETECTED :
    ammo[bullet_id].running = 0;//stop bullet

    switch(colision){
        case 1:
            colide(ammo[bullet_id].x, ammo[bullet_id].y);//add some sparks
            break;

        case 2:
            colide(ammo[bullet_id].x + bullet_width, ammo[bullet_id].y);//add some sparks
            break;

        case 3:
            colide(ammo[bullet_id].x + bullet_width, ammo[bullet_id].y + bullet_height);//add some sparks
            break;

        case 4:
            colide(ammo[bullet_id].x , ammo[bullet_id].y + bullet_height);//add some sparks
            break;
    }

    // DESTROY THE TILE //
    int til_x = (ammo[bullet_id].x + bullet_width + game_scroll%16)/16;
    int til_y = ammo[bullet_id].y/16;
    unsigned char event_id = mapp[til_y][til_x + ((game_scroll)/16) ].event;
    if(event_id == 255){//break tile
        mapp[til_y][til_x + ((game_scroll)/16) ].tile = 0;
        explode2(ammo[bullet_id].x, ammo[bullet_id].y);
        playr.score+=10;
        play_sound(5);
    }

}


void check_bullets_with_enemies(){
    int e;
    for(e = 0; e < MAX_BULLET; e ++)check_bullet_with_enemies(e);
}


int load_data(void){

    //int ntile= (320/16);
    //allegro_message("(320/16)=%i", ntile);

	// load data
	log2file("Loading game data.");

	//packfile_password(init_string);
	log2file("Loading intro");
	intro = load_datafile("dat/intro.dat");
	//packfile_password(NULL);
	if (intro  == NULL) {
    	log2file("load intro data failed");
		allegro_message("Failed to load intro data.");
		return 1;
	}



	log2file("Loading gfx");
	gfx = load_datafile("dat/gfx.dat");
	//packfile_password(NULL);
	if (gfx == NULL) {
    	log2file("load data  *** failed");
		allegro_message("Failed to load gfx data.");
		return 1;
	}


   //load sound effects
    log2file("Loading sfx");
   sfx = load_datafile("dat/sfx.dat");
   if (sfx == NULL) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      log2file("Error loading sound fx!\n");
      allegro_message("Error loading sound fx!\n");
      return 1;
   }

    log2file("Loading backgrounds");
   bg = load_datafile("dat/bg.dat");
   if (bg == NULL) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        log2file("Error loading backgrounds");
        allegro_message("Error loading backgrounds");
        return 1;
   }


    log2file("Ready.\n");
    return 0;
}

void unload_data(void){// QUIT -  - CLOSE

    log2file("\nClosing Space oddity");
    log2file("--------------------");

    log2file("Highest particule number: %d", highest_particule_number);

    log2file("Stop audio stream");
    if(stream != NULL)stop_audio_stream(stream);

    free(stream);//audio

	/* Cleanup */
	log2file("Destroying emulator");
	gme_delete( emu );//destroy emulator


    free(stuffs);
    free(ammo);
    free(pruno);
    free(mapp);
    free(particules);


    if(intro != NULL) unload_datafile(intro);
    if(gfx != NULL) unload_datafile(gfx);
    if(sfx != NULL) unload_datafile(sfx);
    if(bg != NULL) unload_datafile(bg);
    //if(tiles!=NULL) unload_datafile(tiles);//old

    destroy_bitmap(ptibuffer); //Release the bitmap data
    destroy_bitmap(buffer); //Release the bitmap data
    destroy_bitmap(fram); //Release the bitmap data
    destroy_bitmap(tileset); //Release the bitmap data
    destroy_bitmap(bgtileset); //Release the bitmap data

}

/////////////////////
// PLAYER MOVEMENT //
/////////////////////

void move_up(){
    if(playr.y <= 0)return;
    playr.y -= playr.speed; // Moving up
    if(playr.tilt > -14)playr.tilt-=2;
}

void move_down(){
    if(playr.y >= 256-16)return;
    playr.y += playr.speed; // Moving down
    if(playr.tilt < 14)playr.tilt+=2;
}

void move_left(){
    if(playr.x <= 0)return;
    playr.x -= playr.speed;
}

void move_right(){
    if(playr.x >= 304)return;
    playr.x += playr.speed;
    burst(playr.x, playr.y + 5, 90);//show combustion
}

// save last positions for options, motion blur, etc
void update_player_pos(void){

    if(playr.x == playerpos[0].x && playr.y == playerpos[0].y)return;//pas bouge, pas d'update
    int i;
    for( i = 32; i > 0;  i--)playerpos[i] = playerpos[i-1];//last positions, usefull for 'gradius' options :)
    playerpos[0].x = playr.x;
    playerpos[0].y = playr.y;

    //Calcul du Yshift
    playr.yshift = -8 + (float)( 64*((float)(playr.y)/(200) ) );

    // WARNING : CHANGING THIS MAY CRASH : SEE BLIT TO SCREEN
    if( playr.yshift >= 36 )playr.yshift = 36;
    if( playr.yshift < 0 )playr.yshift = 0;
}


void player_input(){
    poll_control(&ctrl);
    //if(key[KEY_F1])draw_rasters();
    if(key[KEY_PRTSCR])take_screenshot();
}


/////////////////////////////////////////////////////
// INIT GAME
/////////////////////////////////////////////////////
int add_bgs(const char *filename, int attrib, void *param)//fill file list
{
    sprintf( bgs[count_bgs].f_name, get_filename( filename ) );
    count_bgs++;
    return 0;
}

int add_vgm(const char *filename, int attrib, void *param)//fill file list
{
    sprintf( vgms[count_vgm].f_name, get_filename( filename ) );
    //vgms[filecount].f_size = 0;
    count_vgm++;
    return 0;
}

int add_map(const char *filename, int attrib, void *param)//fill file list
{
    sprintf(maps[count_map].f_name, get_filename( filename ) );
    //maps[filecount].f_size = 0;
    count_map++;
    return 0;
}

int add_tileset(const char *filename, int attrib, void *param)//fill file list
{
    sprintf(tilesets[count_tileset].f_name, get_filename( filename ) );
    //maps[filecount].f_size = 0;
    //allegro_message("%s", filename);
    count_tileset++;
    return 0;
}


void allegroVersion(){
	// Display version info
	log2file("Allegro version: %s", allegro_id );

	//year, month, year;
	int year = ALLEGRO_DATE / 10000;
	int month = (ALLEGRO_DATE / 100) % 100;
	int day = ALLEGRO_DATE % 100;

	log2file("Revision Date: %d,%d,%d", month, day, year);

}//allegroVersion



void operatingSystem(){

	// Display operating system //
	char os[32];
	switch (os_type){
        case OSTYPE_UNKNOWN: strcpy(os, "Unknown/DOS"); break;
        case OSTYPE_WIN3: strcpy(os, "Windows 3.1"); break;
        case OSTYPE_WIN95: strcpy(os, "Windows 95"); break;
        case OSTYPE_WIN98: strcpy(os, "Windows 98"); break;
        case OSTYPE_WINME: strcpy(os, "Windows ME"); break;
        case OSTYPE_WINNT: strcpy(os, "Windows NT"); break;
        case OSTYPE_WIN2000: strcpy(os, "Windows 2000"); break;
        case OSTYPE_WINXP: strcpy(os, "Windows XP"); break;
        case OSTYPE_OS2: strcpy(os, "OS/2"); break;
        case OSTYPE_WARP: strcpy(os, "OS/2 Warp 3"); break;
        case OSTYPE_DOSEMU: strcpy(os, "Linux DOSEMU"); break;
        case OSTYPE_OPENDOS: strcpy(os, "Caldera OpenDOS"); break;
        case OSTYPE_LINUX: strcpy(os, "Linux"); break;
        case OSTYPE_SUNOS: strcpy(os, "SunOS/Solaris"); break;
        case OSTYPE_FREEBSD: strcpy(os, "FreeBSD"); break;
        case OSTYPE_NETBSD: strcpy(os, "NetBSD"); break;
        case OSTYPE_IRIX: strcpy(os, "IRIX"); break;
        case OSTYPE_DARWIN: strcpy(os, "Darwin"); break;
        case OSTYPE_QNX: strcpy(os, "QNX"); break;
        case OSTYPE_UNIX: strcpy(os, "Unix"); break;
        case OSTYPE_BEOS: strcpy(os, "BeOS"); break;
        case OSTYPE_MACOS: strcpy(os, "MacOS"); break;
        case OSTYPE_MACOSX: strcpy(os, "MacOS X"); break;

	}//switch case OS Type

	// display version tacked onto end of O/S name
	log2file("OS: %s %i %i", os, os_version, os_revision );
}//operatingSystem



void init_game(void)//Game engine initialisation
{
    FILE *fp;
   	// start logfile
	fp = fopen("log.txt", "wt");
	if (fp) {
		fprintf(fp, "Space Oddity %s - log\n-----------------------------------\n", GAME_VERSION_STR);
		fclose(fp);
	}

    allegroVersion();
    operatingSystem();

    //log2file("cpu_family: %d", cpu_family);
    //log2file("cpu_model: %d", cpu_model);
    //log2file("cpu_capabilities: %d", cpu_capabilities);

    //int a = 0;//4
    //char a = 0;//1
    //double a = 0;//8
    //allegro_message( "a=%d" , sizeof( a ) );


    if(!exists("./levels")){
        mkdir("levels");//ca marche
        //log2file("./levels folder created");
    }

    if(!exists("vgm")){
        mkdir("vgm");//ca marche
        //log2file("./vgm folder created");
    }


    //for_each_file_ex("dat/bg_*.bmp", FA_NONE, 0, add_vgm, 0);//browse backgrounds

    for_each_file_ex("vgm/*.ay", FA_NONE, 0, add_vgm, 0);//browse vgm directory
    for_each_file_ex("vgm/*.nsf", FA_NONE, 0, add_vgm, 0);//browse vgm directory
    for_each_file_ex("vgm/*.sap", FA_NONE, 0, add_vgm, 0);//browse vgm directory
    for_each_file_ex("vgm/*.vgm", FA_NONE, 0, add_vgm, 0);//browse vgm directory

    for_each_file_ex("levels/*.odt", FA_NONE, 0, add_map, 0);//browse maps directory
    for_each_file_ex("dat/tileset_*.bmp", FA_NONE, 0, add_tileset, 0);//browse maps directory

    install_keyboard(); // Initialize keyboard routines
    install_mouse();//

    set_config_file("oddity.ini");
    scanlines = get_config_int("settings", "scanlines", 1);//scanlines par default
    show_collisions = get_config_int("settings", "collisions", 0);//dont show collisions par default
    record_video = get_config_int("settings", "record_video", 0);//dont record video as default

    enable_hardware_cursor();
    install_timer(); // Initialize the timer routines

    set_control(&ctrl, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_LCONTROL, KEY_SPACE);
    ctrl.use_joy = 1;


   /* the first thing is to initialise the joystick driver */
   if (install_joystick( JOY_TYPE_AUTODETECT ) != 0) {
      //allegro_message("Error initialising joystick\n%s\n", allegro_error);
      log2file("Error initialising joystick\n");
   }

   /* make sure that we really do have a joystick */
   if (!num_joysticks) {
      log2file("\nJoystick not found :(\n");
   }else{
       log2file("Joystick found :)\n");
       got_joystick = 1;
   }

    install_sound( DIGI_AUTODETECT , MIDI_NONE , NULL );

    /*
    int mute = get_config_int("settings", "sound", 0);
    if(mute == 1){
        set_volume(0 , 0);//MUTE SOUNDS
    }else{
        set_volume(100, 0);//
    }
    */

    set_volume(100, 0);//

    /*\
    LOCK_VARIABLE(speed_counter); // Used to set the timer which regulates the game's
    LOCK_FUNCTION(increment_speed_counter); // speed.
    install_int_ex(increment_speed_counter, BPS_TO_TIMER( 60 ) ); // Set our BPS
    */

    set_color_depth( 8 ); // 8BIT OR DIE

    request_refresh_rate( 60 );

    int windowed = get_config_int("settings", "windowed", 0);
    if(windowed == 1){
        //set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640 , 480 , 0 , 0 ); // Change our graphics mode
        set_gfx_mode( GFX_AUTODETECT_WINDOWED, 1280 , 480 , 0 , 0 ); // Change our graphics mode
    }else{
        set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0); // Change our graphics mode
    }

    log2file("Refresh rate: %dHz\n", get_refresh_rate() );

    if (get_refresh_rate() != 60){
        allegro_message("Couldn't set refresh rate to 60Hz!");
        log2file("Couldn't set refresh rate to 60Hz!");
        return;
    }



    // INIT BITMAPS //
    ptibuffer  = create_bitmap(320, 24); // bande du haut
    buffer  = create_bitmap(320, 256); // virtual screen
    fram  = create_bitmap(SCREEN_W, SCREEN_H); // virtual screen
    tileset = create_bitmap(320, 240);//20x13 tiles (last 4 tiles are used to show the palette)
    bgtileset = create_bitmap(320, 240);//20x13 tiles (last 4 tiles are used to show the palette)

    clear_bitmap( ptibuffer );//nettoie les impuretees et purifie le cuir chevelu
    clear_bitmap( buffer );
    clear_bitmap( fram );
    clear_bitmap( tileset );
    clear_bitmap( bgtileset );
}


void end_level(void){

    //allegro_message("End level\nNext level: %s", level.next_level);
    char fn[256] = "levels/";
    strcat(fn, level.next_level);

    if(exists(fn)){
        load_tilemap(level.next_level);
    }else{
        //goto_menu();
        wait = 100;
        game_status = GS_GAMEOVER;
        gme_set_fade(emu, 0);
    }
}





///////////////////////////////////////////////////////////////
// PARTICULE GENERATORS
///////////////////////////////////////////////////////////////

void add_bubbles(int x, int y){//bubbles particules, for the fun

    int sprite;

    switch(AL_RAND()%2){

        case 0://PETITE BULLE
            sprite = BUBBLE00;
            break;

        case 1:
            sprite = BUBBLE02;
            break;

        default:
            return;
            break;
    }

    if(x == 0)x = AL_RAND() % 320;
    if(y == 0)y = 256;

    bg_particule( x, y, 0.5, 0, 200, sprite, 1, 0);
}

/*
void add_stars(){//star particules, for the fun

    int free = get_free_particule();
    if(free < 0)return;

    particules[free].spritenum = STAR00 + AL_RAND() % 5;
    particules[free].vy = 0;//YSPEED
    particules[free].vx = 1 + AL_RAND() % 3;//XSPEED

    particules[free].x = 320;
    particules[free].y = AL_RAND() % 200;

    particules[free].framecount = 0;
    particules[free].life = 320;//alleluia
}
*/

void add_asteroids(){
    //BG PARTICULES
    int direction = 80 + AL_RAND()%20;

    float s = 0.5;//speed
    if(AL_RAND()%20==0)s = 2;//hi speed asteroid

    if( game_count%20 == 0 )bg_particule( 320, -20 + AL_RAND()%260, s, direction, 700, ROCK00 + AL_RAND()%6, 0, 0);
}

void add_buildings(){

    int direction = 90;
    float s = 0.5;//speed
    int sprite  = BUILDING00 + AL_RAND()%2;

    if( game_count%40 == 0 )bg_particule( 320, AL_RAND()%240, s, direction, 700, sprite, 0, 0);
}


/////////////////////////////////////////////////////
// GAME LOOP
/////////////////////////////////////////////////////
int next;// =  6000 + (AL_RAND() % 6000); // next wave
int max_boulder = 4;

void  game_loop(void)
{

    if(is_up(&ctrl))move_up();
    if(is_down(&ctrl))move_down();
    if(is_left(&ctrl))move_left();
    if(is_right(&ctrl))move_right();
    if(is_fire(&ctrl))shot_bullet();
    //if(is_jump(&ctrl))new_boulder();
    if(key[KEY_MINUS_PAD]){
        set_scroll_speed(0);
        game_scroll--;
    }
    if(key[KEY_HOME])game_scroll=0;
    if(key[KEY_ESC])goto_menu();
    //if(key[KEY_END])goto_menu();

    if(key[KEY_PAUSE]){
        goto_menu();
        //game_status = GS_EDIT;
    }

    if(key[KEY_F1])playr.weapon = BULLET00;//sprinkler
    if(key[KEY_F2])playr.weapon = BULLET02;//LASER
    if(key[KEY_F3])playr.weapon = BULLET03;//FIRE
    if(key[KEY_F4])playr.weapon = BULLET05;//boules
    if(key[KEY_F5])playr.slowmotion++;
    if(key[KEY_F6])playr.slowmotion=0;
    //if(key[KEY_F6])TRACK_LOADED = 0;//fuck the music

    if(key[KEY_0_PAD])level.scroll_speed = 0;
    if(key[KEY_1_PAD])level.scroll_speed = 1;
    if(key[KEY_2_PAD])level.scroll_speed = 2;
    if(key[KEY_3_PAD])level.scroll_speed = 3;
    if(key[KEY_4_PAD])level.scroll_speed = 4;

    /////////////////////////////////////////////////////////////////////////////
    // DRAW THE SHIT
    /////////////////////////////////////////////////////////////////////////////

    clear_bitmap( ptibuffer );//if you dont do it here, then we cant screenshot it
    clear_bitmap( buffer );//if you dont do it here, then we cant screenshot it

    update_player_pos();//save new position for the 'gradius' options

    add_asteroids();
    //add_buildings();

    move_stuff();
    move_bullets();
    move_particules();

    read_tile_events();

    //colisions//
    check_bullets_with_enemies();
    check_player_with_enemies();

    draw_frame();//yo

    if(record_video > 0 && game_count%2)take_screenshot();

    blit_to_screen();
    if(next_shot>0)next_shot--;//player shot rate

    //sprintf(message,"Test %d/%d", game_count, game_scroll);
    //sprintf(message,"Test %d", game_count);

}



/////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

   if (allegro_init() != 0) return 1;// Initialize Allegro

    init_game();

    stream = play_audio_stream( frame_count, 16, TRUE, sample_rate, 255, 128 );

    int i;
    for(i=0;i<30;i++){
        draw_rasters();
        //print("loading",0,0);
        blit_to_screen(buffer);
    }

    // FAIRE UN TRUC MIEUX ICI //
    if( load_data() != 0){
        log2file("Error loading data\n");
        return 1;//load datafiles
    }

    init_bullets();
    init_player();


    //show_mouse(screen);//test

    //game_status = GS_MENU;
    goto_menu();

    //while(!key[KEY_ESC])//If the user hits escape, quit the program
    while( game_status != GS_QUIT_GAME )//
    {

        player_input();
        poll_GME();//music

        switch( game_status ){

            case GS_MENU:
                mainmenu();
                break;

            case GS_SELECT_LEVEL:
                select_screen();
                break;

            case GS_PLAY:
                game_loop();
                break;

            case GS_GAMEOVER:
                gameover_screen();
                break;

            case GS_INPUTNAME:
                inputname_screen();
                break;

            case GS_SCORES:
                highscore_screen();
                break;

            case GS_EDIT:
                //game_count = 0;
                //game_scroll = 0;
                editor();
                break;
        }
        game_count ++;//global game counter
        game_scroll+= level.scroll_speed;//global game counter
    }

    unload_data();//
    log2file("\nDone.\n");

    return 0; // Exit with no errors
}


END_OF_MAIN() // This must be called right after the closing bracket of your MAIN function. It is Allegro specific.


