#include <allegro.h>
#include <allegro.h>
#include <allegro.h>
#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "editor.h"
#include "events.h"
#include "control.h"
#include "main.h"
#include "intro.h"

#include "dat/gfx.h"

#include "gme/gme.h"

#define MAX_BRUSH        256

BITMAP *fram;
BITMAP *tileset;

// controls
Tcontrol ctrl;
track_info_t info;//music info

int offset;//global scrolling offset
char offsety;//global Yscrolling offset

int events_offset;//event selector offset
int toggle_pal = 9;

unsigned char selected_tile;
unsigned char selected_event;//event brush
unsigned char selected_level;//string
unsigned char selected_tune;//
unsigned char selected_tileset;//
unsigned char offset_tune;//
unsigned char selected_brush;//0-MAX_BRUSH
unsigned char settings_index;
unsigned char strings_index;
unsigned char selected_layer;

//int selected_level;//level number
int show_grid;
int pos_x, pos_y, row_y;//current mouse position, in tiles/rows


/////////////////////////////////////////////////////////
// SPECIAL EDITOR FUNCTIONS
/////////////////////////////////////////////////////////
void blit_fram(){//BITMAP *bmp
	acquire_screen();
	vsync();//yup
	blit(fram, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	release_screen();
}


void editor_screenshot(void){

    //check directory
    if(!exists("screenshots")){
        mkdir("screenshots");//ca marche
    }

	PALETTE p;
	BITMAP *b;
	b  = create_bitmap(640, 480); // virtual screen

	char buf[256];//filename
    sprintf(buf, "screenshots/editor.bmp");

	get_palette(p);

    blit(fram, b, 0, 0, 0,0, 640, 480);

	save_bitmap(buf, b, p);
	destroy_bitmap(b);
}


void toggle_layer(){// SWITCH BETWEEN TILE LAYER 0 & 1
    //allegro_message("toggle_layer() %d", selected_layer);
    if(selected_layer == 0)selected_layer = 1; else selected_layer = 0;
    wait=8;
}


void draw_background_in_editor(void)
{
    char i,j;
    BITMAP *bmp = bg[level.background].dat;
    if(bmp == NULL)return;
    int w = bmp->w;
    int h = bmp->h;
    int colnumber = ( SCREEN_W / w ) + 1;
    int rownumber = ( 480 / h ) + 1;
    /*
    for(i = 0; i < colnumber; i++){
        for(j=0; j < 16; j++){
            masked_blit(bmp, fram, 0, j * 16, i * w, j*16, w, 16);
        }
    }
    */
    for(i = 0; i < colnumber; i++){
        for(j = 0; j < rownumber; j++){
            blit( bmp, fram, 0, 0, i * w , j * h, w, h );
        }
    }
}


// STUPID - please remove//
void draw_event_in_editor(int spritenumber, int x, int y)//editor
{
    draw_sprite(fram, gfx[spritenumber].dat, x, y); // Draw the tiles
}



void draw_help(void){
    int position = 0;
    //int x = 530;//xposition
    int x = SCREEN_W - 110;

    switch(toggle_pal){

        case ED_MAIN: position = 0; break;
        case ED_TILES: position = 1; break;
        case ED_EVENTS: position = 2; break;
        case ED_SETTINGS: position = 3; break;
        case ED_TUNES: position = 4; break;

        case ED_TILESETS: position = 6; break;
        case ED_STRINGS: position = 7; break;
        case ED_LEVELS: position = 8; break;
        //case ED_xxx: position = 8; break;
        //case ED_xxx: position = 8; break;

    }

    rectfill(fram, x, 256 + position*10, SCREEN_W, 256 + 8 + position*10, 4);//SELECTION
    print( fram, "F1 :EDIT", x, 256);
    print( fram, "F2 :TILES", x, 256+10);
    print( fram, "F3 :BCKGROUND", x, 256+20);
    print( fram, "F4 :SETTINGS", x, 256+30);
    print( fram, "F5 :MUSIC", x, 256+40);
    print( fram, "F6 :", x, 256+50);
    print( fram, "F7 :TILESETS", x, 256+60);
    print( fram, "F8 :STRINGS", x, 256+70);
    print( fram, "F9 :LOAD", x, 256+80);
    print( fram, "F10:SAVE", x, 256+90);
    print( fram, "F11:SAVE&PLAY", x, 256+100);
    print( fram, "F12:QUIT", x, 256+110);

    //print3("CTRL + CLICK to add tile to clipboard", 320, 140);
    //print3("CTRL + V to paste clipboard", 320, 150);

    char chrs[32];
    int time = (offset*16)/60;
    int min = time/60;
    int sec = time%60;

    print( fram, "TILE:", x, 256+170);

    print( fram, "TPAL:", x, 256+180);
    draw_tileset_palette(fram, x+40, 256+180);//4 color palette

    sprintf(chrs, "OFFS: %04d", offset);
    print( fram, chrs, x, 256+190);
    sprintf(chrs, "TIME: %d:%02d", min, sec);
    print( fram, chrs, x, 256+200);

    time = songpos/1000;
    min = time/60;
    sec = time%60;
    sprintf(chrs, "TUNE: %d:%02d", min, sec);
    print( fram, chrs, x, 256+210);
}



// PUSH THE LEVEL TO THE RIGHT, FROM CURRENT POSITION (ADD SOME SPACE)
void shift_right(){
    historise();//backup
    int i,j;
    for( i = 2047; i > pos_x-2+offset; i-- ){

        for( j = 0; j < 16; j++ ){
            mapp[ j ][ i + 1 ].bg_tile = mapp[ j ][ i ].bg_tile ;
            mapp[ j ][ i + 1 ].tile = mapp[ j ][ i ].tile ;
            mapp[ j ][ i + 1 ].event = mapp[ j ][ i ].event ;
            //mapp[ j ][ i ].tile = 0 ;
        }
    }

    wait = 8;
}


// SHIFT LEVEL TO THE LEFT, DELETE COLUMN BEFORE POSITION
void shift_left(){
    historise();//backup
    int i,j;
    for( i = pos_x+offset; i < 2047; i++ ){

        for( j = 0; j < 16; j++ ){
            mapp[ j ][ i ].bg_tile = mapp[ j ][ i + 1 ].bg_tile ;
            mapp[ j ][ i ].tile = mapp[ j ][ i + 1 ].tile ;
            mapp[ j ][ i ].event = mapp[ j ][ i + 1 ].event ;
        }
    }

    wait = 8;
}



//////////////////////////////////////////////
// SETTINGS
//////////////////////////////////////////////

void settings_up(){
    if(wait > 0)return;
    if(settings_index > 0)settings_index--;
    wait = 8;
}
void settings_down(){
    if(wait > 0)return;
    if(settings_index < 15)settings_index++;
    wait = 8;
}

void settings_right(){
    if(wait>0)return;
    switch(settings_index){
        case 0: if(level.bg_r < 63)level.bg_r++;break;
        case 1: if(level.bg_g < 63)level.bg_g++;break;
        case 2: if(level.bg_b < 63)level.bg_b++;break;
        case 3: if(level.tile_r < 63)level.tile_r++;break;
        case 4: if(level.tile_g < 63)level.tile_g++;break;
        case 5: if(level.tile_b < 63)level.tile_b++;break;
        case 6: if(level.bgtile_r < 63)level.bgtile_r++;break;
        case 7: if(level.bgtile_g < 63)level.bgtile_g++;break;
        case 8: if(level.bgtile_b < 63)level.bgtile_b++;break;

        case 9: if(level.background < 31)level.background++; wait=8;break;//backgrounds
        case 10: toggle_pal = ED_TILESETS;break;//TILESET SELECTOR
        case 11: if(level.scroll_speed < 8){level.scroll_speed++; wait=8;}break;//START SPEED
        //case 13: toggle_pal = ED_STRINGS;break;//
        //case 14: toggle_pal = ED_LEVELS;break;//SELECT NEXT LEVEL
        //case 15: toggle_pal = ED_TUNES;break;//SELECT TUNE
    }
    //reset_palette();
    update_palette();
}
void settings_left(){
    if(wait>0)return;
    switch(settings_index){
        case 0: if(level.bg_r > 0)level.bg_r--;break;
        case 1: if(level.bg_g > 0)level.bg_g--;break;
        case 2: if(level.bg_b > 0)level.bg_b--;break;
        case 3: if(level.tile_r > 0)level.tile_r--;break;
        case 4: if(level.tile_g > 0)level.tile_g--;break;
        case 5: if(level.tile_b > 0)level.tile_b--;break;
        case 6: if(level.bgtile_r > 0)level.bgtile_r--;break;
        case 7: if(level.bgtile_g > 0)level.bgtile_g--;break;
        case 8: if(level.bgtile_b > 0)level.bgtile_b--;break;

        case 9: if(level.background > 0)level.background--; wait=8;break;//backgrounds
        //case 8: if(level.scroll_speed > 1){level.scroll_speed--; wait=8;}break;//START SPEED
    }
    //reset_palette();
    update_palette();
}

void draw_settings(void){

    //print3("LEVEL EDITOR HELP", 0, 0);
    draw_tiles();

    rectfill(fram, 0, settings_index*10 + 256, 320, settings_index*10 + 8 + 256, 4);//selection

    char chars[64];
    //sprintf(chars, "F4 - SETTINGS[%d]", settings_index);
    //print3(chars,320, 0);
    print(fram, "+/- to change values", 320, 256);

    sprintf(chars, "BACK.R:  %02d", level.bg_r);  print(fram, chars, 10, 256);
    sprintf(chars, "BACK.G:  %02d", level.bg_g);  print(fram, chars, 10, 256+10);
    sprintf(chars, "BACK.B:  %02d", level.bg_b);  print(fram, chars, 10, 256+20);

    sprintf(chars, "TILE.R:%02d", level.tile_r);  print(fram, chars, 10, 256+30);
    sprintf(chars, "TILE.G:%02d", level.tile_g);  print(fram, chars, 10, 256+40);
    sprintf(chars, "TILE.B:%02d", level.tile_b);  print(fram, chars, 10, 256+50);

    sprintf(chars, "BGTI.R:%02d", level.bgtile_r);  print(fram, chars, 10, 256+60);
    sprintf(chars, "BGTI.G:%02d", level.bgtile_g);  print(fram, chars, 10, 256+70);
    sprintf(chars, "BGTI.B:%02d", level.bgtile_b);  print(fram, chars, 10, 256+80);

    draw_tileset_palette(fram, 100, 290);

    sprintf(chars, "BACKGROUND:%02d", level.background);  print(fram, chars, 10, 256+90);
    sprintf(chars, "TILESET: %s", level.tileset);  print(fram, chars, 10, 256+100);
    sprintf(chars, "START SPEED:%d", level.scroll_speed);  print(fram, chars, 10, 256+110);

    sprintf(chars, "FILE NAME: %s", file_name);  print(fram, chars, 10, 256+120);
    sprintf(chars, "LEVL NAME: %s", level.level_name);  print(fram, chars, 10, 256+130);
    sprintf(chars, "WARP TO: %s", level.next_level);  print(fram, chars, 10, 256+140);

}

/////////////////////////////////////////////////////////////////////////
// STRINGS
/////////////////////////////////////////////////////////////////////////

void strings_up(){
    if( wait  > 0 )return;
    if(strings_index > 0)strings_index--;
    wait = 8;
}

void strings_down(){
    if(wait > 0)return;
    if(strings_index < 5)strings_index++;
    wait = 8;
}



// http://www.allegro.cc/forums/thread/523281

void strings_input(){

    char str[32];

    if( keypressed() ){

        int newkey = readkey();
        char ASCII = newkey & 0xff;

        switch(strings_index){
            case 0: sprintf( str, level.level_name ); break;
            case 1: sprintf( str, level.author ); break;
            case 2: sprintf( str, level.message1 ); break;
            case 3: sprintf( str, level.message2 ); break;
            case 4: sprintf( str, level.message3 ); break;
            case 5: sprintf( str, level.message4 ); break;
        }

        int caret = strlen(str);
        if( ASCII == 8 )str[caret - 1] = 0;// BACKSPACE
        if( ASCII >= 32 && caret < 31){
            str[caret] = ASCII;
            str[caret + 1] = 0;
        }

        switch(strings_index){
            case 0: sprintf( level.level_name, str ); break;
            case 1: sprintf( level.author, str ); break;
            case 2: sprintf( level.message1, str ); break;
            case 3: sprintf( level.message2, str ); break;
            case 4: sprintf( level.message3, str ); break;
            case 5: sprintf( level.message4, str ); break;
        }

     }
}

void draw_strings(void){

    char chars[32];
    //sprintf(chars, "F8 - STRINGS[%d]", strings_index);
    //print3(chars, 320, 0);

    rectfill(fram, 0, 10 + strings_index*20 + 256, 320, 10 + strings_index*20 + 8 + 256, 4);//selection


    print( fram, " LEVEL NAME:", 0, 256);
    sprintf(chars,"%s",level.level_name);
    print(fram, chars, 0, 256+10);

    print( fram, " AUTHOR:", 0, 256+20);
    sprintf( chars,"%s",level.author );
    print( fram, chars, 0, 256+30);


    print( fram, " MSG1:", 0, 256+40);
    sprintf(chars,"%s",level.message1);
    print( fram, chars, 0, 256+50);

    print( fram, " MSG2:", 0, 256+60);
    sprintf(chars,"%s",level.message2);
    print(fram, chars, 0, 256+70);

    print( fram, " MSG3:", 0, 256+80);
    sprintf(chars,"%s",level.message3);
    print( fram, chars, 0, 256+90);

    print(fram, " MSG4:", 0, 256+100);
    sprintf(chars,"%s",level.message4);
    print(fram, chars, 0, 256+110);
}

void draw_tiles(void)// MAIN DRAW FUNCTION
{
    int i,j;
    int colnumber = SCREEN_W/16;
    // TILES //
    for(i=0; i < colnumber; i++){
        for(j=0; j < 16; j++){

            //draw_bgtile(fram, mapp[j][i + offset].bg_tile, i*16, j*16);//background tiles
            //draw_tile(fram, mapp[j][i + offset].tile, i*16, j*16);//first tile plane

            if(selected_layer == 1){//fisrt plane
                draw_tile(fram, mapp[j][i + offset].tile, i*16, j*16);//first tile plane
                draw_bgtile(fram, mapp[j][i + offset].bg_tile, i*16, j*16);//background tiles
            }else{
                draw_bgtile(fram, mapp[j][i + offset].bg_tile, i*16, j*16);//background tiles
                draw_tile(fram, mapp[j][i + offset].tile, i*16, j*16);//first tile plane
            }

        }
    }

    // EVENTS //
    for(i=0; i < colnumber; i++){
        for(j=0; j < 16; j++){

            unsigned char event = mapp[ j ][ i + offset ].event;
            char chars[20];
            sprintf( chars, "%02d", event);
            if( event > 0 ){
                int sprite = get_event_sprite(event);
                if( sprite < 0 ){
                    print(fram, chars, i*16, j*16 + 256);
                }else{
                    draw_event_in_editor(sprite, i*16, j*16 );
                }
            }
        }
    }
}


void draw_big_brush(void){//draw the big brush

    int i,j;

    print( fram, "CTRL+B", 330, 40);
    print( fram, ">PASTE", 330, 50);
    print( fram, " BRUSH", 330, 60);



    char chars[32];
    sprintf(chars, "BRUSH[%03d/256]", selected_brush + 1 );
    print( fram, chars, 384, 258);
    print( fram, "PG-UP->NXT", 384, 270);
    print( fram, "PG-DN->PRV", 384, 280);
    print( fram, "D->EYEDROP", 384, 290);

    //rectfill(fram,320+64,0,320+256+64,256,4);//playground

    for( i=0; i < 16; i++ ){//Y
        for( j=0; j < 16; j++){//X
            //draw_tile(fram, 33, 320 + 64 + i*16, j*16);
            draw_sprite(fram, gfx[ICO_EMPTY].dat, 320 + 64 + j*16, i*16);
            draw_tile(fram, brush[selected_brush][j+i*16].tile, 320 + 64 + j*16, i*16);//draw the tile
        }
    }
}


////////////////////////////////////////////////////////
// DRAW CURRENT BRUSH (F1)
////////////////////////////////////////////////////////
void draw_brush( int x , int y ){// draw the big brush

    int i,j;
    int high_x = 0;
    int high_y = 0;
    char chars[32];

    for( i=0; i < 16; i++ ){// Y
        for( j=0; j < 16; j++ ){// X
            int tile_id = brush[selected_brush][j+i*16].tile;

            if(selected_layer==1){
                draw_bgtile( fram , tile_id , x + j*16 , y + i*16 );// draw the tile //
            }else{
                draw_tile( fram , tile_id , x + j*16 , y + i*16 );// draw the tile //
            }
            // detect highest position //
            if( tile_id > 0 ){
                if( i > high_y )high_y = i;
                if( j > high_x )high_x = j;
            }
        }
    }

    if(high_x == 0 && high_y == 0){// BRUSH NOT FOUND //
        sprintf(chars, "NO BRUSH[%03d/256]", selected_brush + 1 );
        print(fram, chars, x, y + 2);
        return;
    }

    high_x++;
    high_y++;
    rect( fram, x , y , x + 16*high_x , y + 16*high_y , 4 );// show brush outline //

    sprintf(chars, "BRUSH[%03d/256]", selected_brush + 1 );
    print(fram, chars, x, y + 16*high_y + 2 );
    //print(fram, "PASTE W CTRL+B", x, y + 16*high_y + 12 );
}


////////////////////////////////////////////////////////
// SHOW TILESET
////////////////////////////////////////////////////////
void draw_tiles_palette( void )
{
    char chars[32];
    //unsigned char tnum = (pos_y)*20 + pos_x;//palette tile number


    if(tileset == NULL){
        rectfill(fram,0, 256, 320, 256 + 8, 4);
        sprintf(chars, "PRESS F7 TO LOAD TILESET");
        print(fram, chars, 0, 256);
    }else{

        sprintf(chars, "%s",level.tileset);//TILESET FILE NAME
        print(fram, chars, 2, 212 );
        //print(fram, "Right click to select a tile", 0, 256+10);

        sprintf(chars, "TILE:%03d", selected_tile);
        print(fram, chars, 0, 256+40);

        // CURRENT BRUSH

        if(tileset !=NULL){

           //draw_tile_in_editor(selected_tile, 80, 256 + 30);
           draw_tile(fram, selected_tile, 80, 256 + 30);
           //draw_sprite_streched(fram, tiles[selected_tile].dat, 80, 256 + 30, 32, 32 ); // Draw the tile
           //stretch_sprite(fram, tiles[selected_tile].dat, 80, 256 + 30, 32, 32 ); // Draw the tile
        }
    }

    int i,j;
    int usage = 0;
    for( i = 0; i < 20; i++ ){
        for( j = 0; j <= 13; j++ ){
            int tile_id = i + j * 20;
            if(tile_id > 255)tile_id = 0;
            if(is_empty_tile( tile_id )){
                //rectfill( fram, i*16, j*16, i*16 + 16, j*16 +16, 4 );//unused tile
                draw_sprite(fram, gfx[ICO_EMPTY].dat, i*16, j*16);
            }else{
                //draw_tile_in_editor(tile_id, i*16, j*16);
                draw_tile(fram, tile_id, i*16, j*16);
                usage++;
            }
        }
    }

    sprintf(chars, "USAGE: %d/255", usage);
    print(fram, chars, 2, 224);

}

void draw_color_palette(BITMAP *dest, int x, int y){
    int i,j;
    for(i=0; i < 16; i++){
        for(j=0; j < 16; j++){
            rectfill( dest , x+i*2, y+j*2, x+i*2+2, y+j*2+2, i+j*16 );
            //void putpixel(BITMAP *bmp, int x, int y, int color);
            //putpixel(dest, x + i, y + j, i+j*i);
        }
    }
}

void draw_tileset_palette(BITMAP *dest, int x, int y){

    // TILE COLOR PALETTE !!!
    //rectfill( dest, x, y, x + 37, y + 10, 15);//contour blanc
    int i;
    for(i=0; i < 12; i++){
        rectfill( dest, x + 8*i + i, y, x + 8 + 8*i + i , y + 8, 241 + i );
    }
    rectfill( dest, x + 8*i + i, y, x + 8 + 8*i + i , y + 8, 0 );//transparent color
}



void select_tunes_up(void){
    wait = 8;
    if( selected_tune > 0 )selected_tune--;
}

void select_tunes_down(void){
    wait = 8;
    selected_tune++;
}

void select_level_up(void){
    wait = 8;
    if(selected_level == 0)selected_level = count_map;
    selected_level--;

}

void select_level_down(void){
    wait = 8;
    selected_level++;
    if(selected_level >= count_map)selected_level = 0;
}


void show_tunes(){// F5

    int i;
    int offset = 0;
    if(selected_tune >= 18){
        offset = selected_tune - 18;
        rectfill(fram, 0, 18*10 + 256, 320, 18*10 + 8 + 256, 4);//selection
    }else{
        rectfill(fram, 0, selected_tune*10 + 256, 320, selected_tune*10 + 8 + 256, 4);//selection
    }


    char chars[64];
    //sprintf(chars, "F5 - TUNES[%02d]", selected_tune);
    //print3(chars, 420, 0);//TITLE

    if( level.sound_track ){
        sprintf(chars, "%s", level.sound_track );    print(fram, chars, 320, 256+130);
        sprintf(chars, "Music : %s", info.song );   print(fram, chars, 320 , 256+140 );
        sprintf(chars, "Artist: %s", info.author ); print(fram, chars, 320 , 256+150 );
        sprintf(chars, "(c) %s", info.copyright );  print(fram, chars, 320 , 256+160 );
        sprintf(chars, "LENGTH:%ld:%02ld", (long) info.length / 1000 / 60, (long) info.length / 1000 % 60 );
        print(fram, chars, 320, 256+170);
        sprintf(chars, "PLAYED:%ld:%02ld", (long) songpos / 1000 / 60, (long) songpos / 1000 % 60 );
        print(fram, chars, 320, 256+180);
    }else{
        print(fram, "[PRESS RETURN]", 320, 256);
    }


    for(i = 0; i < 22; i++){
        sprintf(chars, "%s", vgms[(i + offset)].f_name );
        print(fram, chars, 10, 10*i+256);
    }


}


void show_levels(){// F6

    draw_tiles();

    int i;
    int offset = 0;
    if(selected_level >= 18){
        offset = selected_level - 18;
        rectfill(fram, 0, 18*10 + 256, 320, 18*10 + 8 + 256, 4);//selection
    }else{
        rectfill(fram, 0, selected_level*10 + 256, 320, selected_level*10 + 256 + 8, 4);//selection
    }

    print(fram, "<- SELECT A LEVEL", 320, 256);

    if(strlen(file_name) > 0){

        print(fram, "** CURRENT LEVEL **", 320, 280);
        print(fram, file_name, 320, 290);
        print(fram, level.level_name, 320, 300);

        print(fram, "** TILESET: **", 320, 370);
        print(fram, level.tileset, 320, 380);
        if(strlen(level.tileset) == 0)print(fram, "NO TILESET", 320, 380);
        print(fram, "** MUSIC: **", 320, 400);
        print(fram, level.sound_track, 320, 410);
        if(strlen(level.sound_track) == 0)print(fram, "NO MUSIC", 320, 410);
    }

    if(count_map == 0){
        print(fram, "First write a .odt file in ./levels",0,256);
        print(fram, "(you have to create a level.odt file)",0,256+10);
    }


    for(i=0; i < 19; i++){
        char chars[32];
        sprintf(chars, "%s", maps[(i + offset)].f_name );
        print(fram, chars, 10, 10*i+256);
    }
    //print3(">",0,10 + 10*row_y);

}


void show_tilesets(){// F7 //

    draw_tiles();
    int i,j;
    int offset = 0;
    char chars[32];

    print(fram, "<- SELECT A TILESET", 320, 10+256);


    print(fram, "CURRENT:", 320, 30+256);
    sprintf(chars, "%s", level.tileset );
    print(fram, chars, 320, 40+256);


    rectfill(fram, 0, selected_tileset*10 + 256, 320, selected_tileset*10 + 8 + 256, 4);//selection

    // SHOW FILES //
    for(i = 0; i < 19; i++){
        //sprintf(chars, "%s", maps[(i + offset)].f_name );
        sprintf(chars, "%s", tilesets[(i + offset)].f_name );
        print(fram, chars, 10, 10*i+256);
    }

    // SHOW TILES //
    rectfill(fram, 0, 0, 320, 240, 16);//draw a black background

    for( i = 0; i < 20; i++ ){
        for( j = 0; j < 13; j++ ){
            int tile_id = i + j * 20;
            if(tile_id > 255)tile_id = 0;
            draw_tile(fram, tile_id, i*16, j*16);
        }
    }
}


void select_tile(void){// RIGHTCLICK SELECT A TILE FROM TILESET (F2) //

    if(pos_x>=20){
        //DELETE FROM BIGBRUSH//
        if(pos_x>=24)brush[selected_brush][(pos_x-24) + pos_y*16].tile = 0;
        return;
    }

    unsigned char selection = pos_x + pos_y*20;
    if( is_empty_tile( selection ) )return;//do not draw with empty tiles ! ;)
    selected_tile = selection;

}


void select_event(){
    selected_event = row_y + events_offset;
    toggle_pal = 0;//quit event palette
}

void set_tile(void){
    historise();//backup
    if( pos_y >= 16)return;
    if(selected_tile<1)return;
    //tilesheet[pos_y][pos_x + offset] = selected_tile;
    if(selected_layer == 1){// background
        mapp[pos_y][pos_x + offset].bg_tile = selected_tile;
    }else{
        mapp[pos_y][pos_x + offset].tile = selected_tile;
    }
}

void set_event(void){
    historise();//backup
    if( pos_y >= 16)return;
    //if(selected_event<1)return;
    //events[pos_y][pos_x + offset] = selected_event;
    mapp[pos_y][pos_x + offset].event = selected_event;
}

void clear_tile(void){//right click on the map -> clear a tile

    historise();//backup
    if( pos_y >= 16)return;
    if( key[KEY_LCONTROL] ){
        // REMOVE FROM CLIPBOARD //
        int i;
        for( i = 0; i < 64; i++ ){
            if( clipb[i].x == (pos_x + offset) && clipb[i].y == pos_y){
                clipb[i].tile = 0;//clear
                clipb[i].x = 0;//clear
                clipb[i].y = 0;//clear
            }
        }

    }else{// CLEAR
        if( selected_layer == 1 ){//BACKGROUND
            mapp[pos_y][pos_x + offset].bg_tile = 0;
        }else{
            mapp[pos_y][pos_x + offset].tile = 0;
        }
        //mapp[pos_y][pos_x + offset].tile = 0;
    }
}


void clear_event(void){//right click on the map -> clear a event

    historise();//backup
    if( pos_y >= 16)return;
    mapp[pos_y][pos_x + offset].event = 0;
}


void show_selection( void ){//draw a square around the tile to have a better 'view'
    // or 'SHOW MOUSE POSITION'

    int x = pos_x*16;
    int y = pos_y*16;

    int color = game_count%16;

    if(key[KEY_B])return;//NO NEED TO SHOW IT IN BRUSH MODE

    if(key[KEY_E]){//EVENTS
        color = 5;
        char chars[20];
        sprintf(chars, "%s", event_table[selected_event]);
        print(fram, chars, x, y - 8 + 256);
        draw_event_in_editor( get_event_sprite(selected_event), x, y );
        draw_events_helper();
    }else{
        if(selected_layer==1) draw_bgtile(fram, selected_tile, x, y);// DRAW SELECTED BGTILE
        else draw_tile(fram, selected_tile, x, y);// DRAW SELECTED TILE
    }

    hline(fram, x, y, x + 16, color);
    hline(fram, x, y + 16, x + 16, color);
    vline(fram, x, y, y + 16, color);
    vline(fram, x + 16, y, y + 16, color);
    //putpixel(fram, mouse_x, mouse_y, 15);
    rectfill(fram, mouse_x, mouse_y/1.875, mouse_x+2, (mouse_y/1.875)+2, 15);//mouse 'pointer'
}

void draw_events_helper(){//draw events in a convenient way :)

    rectfill(fram, 0, 256, SCREEN_W, 256 + 32, 16);//BLACK BACKGROUND
    int i = 0;
    int spritenumber = get_event_sprite( selected_event );

    BITMAP *bmp = gfx[spritenumber].dat;
    int w = bmp->w;
    int h = bmp->h;

    rectfill(fram, 40*8, 270-1, 40*8+w, 270+h, 5);// SELECTION

    for( i = 0; i < 16; i++ ){

        spritenumber = get_event_sprite( i + selected_event - 8 );
        draw_sprite(fram, gfx[spritenumber].dat, i*40 , 270 ); // Draw the tiles
        //char chars[20];
        //sprintf(chars, "%d",  i + selected_event + 8 );
        //print(fram, chars, i*40, 260);
    }

    //draw_square(40*8, 270);
}


void draw_tiles_helper(){

    //rectfill(fram, 0, 256, 640, 480, 16);//BLACK BACKGROUND
    int i;

    //selection
    rectfill(fram, 16*17-1, 270-1, 16*17+16, 270+16, 5);// SELECTION

    for( i = 0; i < 38; i++ ){
        //void draw_tile(BITMAP *dest, unsigned char tilenumber, int x, int y);
        if(selected_layer==1) draw_bgtile( fram, i+selected_tile-16, i*17, 270 );
        else draw_tile( fram, i+selected_tile-16, i*17, 270 );
    }

    //char chars[32];
    //sprintf(chars, "Tileset: %s",  level.tileset );
    //print(fram, chars, 100, 300);
    //print(fram, "USE MOUSEWHEEL TO SCROLL", 100, 310);
}


void draw_square( int pos_x, int pos_y ){
    int x = pos_x*16;
    int y = pos_y*16;
    unsigned char color = 16+game_count%16;
    hline(fram, x, y, x + 16, color);
    hline(fram, x, y + 16, x + 16, color);
    vline(fram, x, y, y + 16, color);
    vline(fram, x + 16, y, y + 16, color);
}


void show_selected_tile(unsigned char tile){
    if( tile == 0 )return;
    int pos_y = tile/20;
    int pos_x = tile%20;
    draw_square(pos_x, pos_y);
}




void pipette(void)// PRESS D TO PICK A TILE //
{
    //int tile = tilesheet[pos_y][pos_x+offset];
    if( toggle_pal == ED_TILES ){//special
        //TODO
        //allegro_message("pipette\npos_x=%d; pos_y=%d", pos_x, pos_y );

        if(pos_x<=19){// TILESET //
            int tile = pos_y*20 + pos_x;
            if(tile > 0)selected_tile = tile;
        }

        if(pos_x>=24){// BRUSH //
            int posx = pos_x-24;
            int offset = pos_y*16+posx;

            int tile = brush[selected_brush][offset].tile;

            //int tile = 33;
            if(tile > 0)selected_tile = tile;
        }

    }else{// MAIN //
        int tile = mapp[pos_y][pos_x + offset].tile;
        if(tile > 0)selected_tile = tile;
    }
}

void suppr(){
    historise();//backup
    mapp[pos_y][pos_x + offset].tile = 0;
}



void default_colors(){
    level.bg_r = 16;
    level.bg_g = 16;
    level.bg_b = 16;
    level.tile_r=60;
    level.tile_g=60;
    level.tile_b=55;
    update_palette();
}

////////////////////////////////////////////////////////
// http://www.allegro.cc/forums/thread/162229
////////////////////////////////////////////////////////

// load our map, its load_tilemap, in main.c


// save our map
void save_map()
{
    char fn[256];// = "levels/";
    sprintf(fn, "levels/%s", file_name);
    //strcat(fn, file_name);

    if(strlen(file_name) == 0){
      allegro_message("Error: strlen(file_name) == 0\nfn=%s", fn);
      return;
    }

    if(!exists(fn)){
        //allegro_message("Error saving map\nfile '%s' not found", fn);
        //strcpy(fn, "levels/new.odt");
        //return;
    }

    log2file("Saving ./%s", fn);

    PACKFILE *pfile;
    pfile = pack_fopen(fn, "wp");

    int i,j;

    // HEADER //
    pack_fputs( "ODTM" , pfile );
    pack_fwrite(&level, sizeof(LEVEL), pfile);//

    // save level //
    for (i=0; i < 2048; i++)
        for (j=0; j < 16; j++)
            pack_fwrite(&mapp[j][i], sizeof(MAP), pfile);



    pack_fclose( pfile );
    //sprintf(message, "File %s saved", file_name);
    //sprintf( message, "Saved" );
    //game_status = GS_MENU;//back to menu

}


/////////////////////////////////
// SAVE BRUSHES
/////////////////////////////////
void save_brush(void){

    int i,j;


    // CHECK EMPTYNESS //
    int empty = 0;
    for( i = 0; i < MAX_BRUSH; i++ )
        for( j = 0; j < 256; j++)
            if( brush[i][j].tile > 0 )empty++;

    if(empty == 0)return;// DO NOT SAVE EMPTY BRUSH SET //


    char fn[256];// = "levels/";
    sprintf(fn, "dat/%s.brush", level.tileset);

    if( strlen( level.tileset ) == 0 ){
      allegro_message("Error: strlen(file_name) == 0\nfn=%s", fn);
      return;
    }

    log2file("Saving ./%s", fn);
    PACKFILE *pfile;
    pfile = pack_fopen(fn, "wp");

    // HEADER //
    pack_fputs("BRSH", pfile );//4 LETTERS//

    for ( i = 0; i < MAX_BRUSH; i++ )
        for ( j = 0; j < 256; j++ )
            pack_fwrite(&brush[i][j], sizeof(CLIPBOARD), pfile);

    pack_fclose( pfile );
    //allegro_message("brush %s saved", fn);
    toggle_pal = ED_MAIN;//to prove that something hapened
}

/////////////////////////////////////////////////////////
// LOAD BRUSH
/////////////////////////////////////////////////////////

void load_brush(char *file){

    char fn[256];
    sprintf(fn, "dat/%s.brush", file);

    clear_brushes();

    if( !exists( fn ) ){
        log2file("Error: ./%s not found", fn);
        return;
    }

	PACKFILE *pfile;
	pfile = pack_fopen( fn, "rp");

	if( !pfile ){
	    log2file("!pfile", fn);
	    return;
	}

    int i,j;
    char header[5];
    // HEADER //
    pack_fgets( header, 5, pfile);//4 CHARS HEADER (BRSH)

    // load brushes //
    for (i=0; i < MAX_BRUSH; i++)
        for (j=0; j<256; j++)
            pack_fread(&brush[i][j], sizeof(CLIPBOARD), pfile);

    //allegro_message("brush %s loaded", fn);
}


void clear_brushes(void){
    int i,j;
    for ( i = 0; i < MAX_BRUSH; i++ )
    {
        for (j = 0; j < 256; j++)
        {
            brush[i][j].tile = 0;
        }
    }
    selected_brush = 0;
}

/////////////////////////////////////////////////////////
// MOUSE SCROLL
/////////////////////////////////////////////////////////

int lastscroll=0;
int scrollwheel(){
    int mickey = 0;
    if(lastscroll == mouse_z)return 0;
    mickey = lastscroll - mouse_z;//difference
    lastscroll = mouse_z;
    return mickey;
}







void empty_clipboard(){
    int i;
    for(i=0; i<64; i++){
        clipb[i].x = 0;
        clipb[i].y = 0;
        clipb[i].tile = 0;
    }
}

int get_free_clipboard(){
    int i;
    for( i = 0; i < 64; i++ )if(clipb[i].tile==0)return i;
    return -1;
}

int paste_x = 0;//paste position
int paste_y = 0;//paste position

void show_clipboard(){//draw 'clipboard'

    int i;
    int past_x = 9999;
    int past_y = 9999;
    int x,y;

    for(i = 0; i < 64; i++){
        if(clipb[i].tile>0){
            //draw rectangle
            draw_square(clipb[i].x - offset, clipb[i].y);
            if(clipb[i].x < past_x)past_x = clipb[i].x;
            if(clipb[i].y < past_y)past_y = clipb[i].y;
        }
    }

    paste_x = past_x;
    paste_y = past_y;

    // SHOW PASTE BLOC POSITION //
    x = (past_x-offset)*16;
    y = past_y*16;
    hline(fram, x, y, x + 16, 4);
    hline(fram, x, y + 16, x + 16, 4);
    vline(fram, x, y, y + 16, 4);
    vline(fram, x + 16, y, y + 16, 4);

}


///////////////////////////////////////////////
// CLIPBOARD
///////////////////////////////////////////////
void add2clipboard(){//copy selection
    int free,i;
    free = get_free_clipboard();
    if(free < 0)return;

    //check if not already in clipboard
    for( i = 0; i < 64; i++ ){
        if( clipb[i].x == (pos_x + offset) && clipb[i].y == pos_y)return;
    }
    //add it//
    clipb[free].x = pos_x + offset;
    clipb[free].y = pos_y;
    //clipb[free].tile = tilesheet[pos_y][pos_x+offset];
    clipb[free].tile = mapp[pos_y][pos_x+offset].tile;
}
///////////////////////////////////////////////
// BIGBRUSH
///////////////////////////////////////////////
void add2bigbrush(){//copy selection
    if(pos_x < 24)return;

    int x = (pos_x-24);
    int y = (pos_y);
    //allegro_message("add2bigbrush() %dx%d", (pos_x-24), pos_y);//ok
    brush[selected_brush][x+y*16].tile = selected_tile;//error

}

void shift_brush_up(void){//
    wait=8;
    int i;
    unsigned char row[16];
    for(i=0; i < 16; i++)row[i] = brush[selected_brush][i].tile;//SAVE FIRST ROW

    //SHIFT//
    for(i=0; i < 256; i++){
        int replace = i+16;
        brush[selected_brush][i].tile = brush[selected_brush][replace].tile;
    }

    for(i=0; i < 16; i++)brush[selected_brush][16*15+i].tile = row[i];//REPLACE LAST ROW
}

void shift_brush_down(void){
    wait = 8;
    int i;
    unsigned char row[16];
    for(i=0; i < 16; i++)row[i] = brush[selected_brush][16*15+i].tile;//SAVE LAST ROW

    for(i=256; i > 0; i--){
        int replace = i-16;
        brush[selected_brush][i].tile = brush[selected_brush][replace].tile;
    }

    for(i=0; i < 16; i++)brush[selected_brush][i].tile = row[i];//REPLACE FIRST ROW
}

void shift_brush_left(void){
    wait=8;
    int i;
    unsigned char row[16];
    for(i=0; i < 16; i++)row[i] = brush[selected_brush][16*i].tile;//SAVE FIRST COL
    for(i=0; i < 255 ; i++){//SHIFT TO LEFT
        int replace = i+1;
        brush[selected_brush][i].tile = brush[selected_brush][replace].tile;
    }
    for(i=0; i < 16; i++)brush[selected_brush][16*i+15].tile = row[i];//REPLACE LAST COL
}

void shift_brush_right(void){
    wait=8;
    int i;
    unsigned char row[16];
    for( i=0; i < 16; i++ )row[i] = brush[selected_brush][16*i+15].tile;//SAVE LAST COL

    for( i=256; i > 0 ; i-- ){// SHIFT TO RIGHT //
        int replace = i-1;
        brush[selected_brush][i].tile = brush[selected_brush][replace].tile;
    }
    for( i=0; i < 16; i++ )brush[selected_brush][16*i].tile = row[i];//REPLACE FIRST COL

}





void paste(void){// PASTE Selection //
    int i;
    historise();//backup
    for( i = 0; i < 64; i++ ){//browse clipboard
        if(clipb[i].tile<=0)continue;
        int x = clipb[i].x;
        int y = clipb[i].y;
        int px = pos_x + offset + x - paste_x;
        int py = pos_y + y - paste_y;
        if(py>15)continue;//out of bounds
        //tilesheet[py][px] = clipb[i].tile;
        if( selected_layer == 1 ){//BACKGROUND
            mapp[py][px].bg_tile = brush[selected_brush][i].tile;
        }else{
            mapp[py][px].tile = brush[selected_brush][i].tile;
        }
    }
}


void paste_brush(void){//paste BIG BUSH
    int i;
    for( i = 0; i < 256; i++ ){//browse clipboard
        if( brush[selected_brush][i].tile <= 0 )continue;
        //allegro_message("paste_brush[%d]", i);
        int x = i%16;
        int y = i/16;
        int px = pos_x + offset + x;
        int py = pos_y + y;
        if(py>15)continue;//out of bounds
        if( selected_layer == 1 ){//BACKGROUND
            mapp[py][px].bg_tile = brush[selected_brush][i].tile;
        }else{
            mapp[py][px].tile = brush[selected_brush][i].tile;
        }
    }
}

void erase_brush(void){//erase with BIG BUSH
    int i;
    for( i = 0; i < 256; i++ ){//browse clipboard
        if( brush[selected_brush][i].tile <= 0 )continue;
        //allegro_message("paste_brush[%d]", i);
        int x = i%16;
        int y = i/16;
        int px = pos_x + offset + x;
        int py = pos_y + y;
        if(py>15)continue;//out of bounds
        if( selected_layer == 1 ){//BACKGROUND
            mapp[py][px].bg_tile = 0;
        }else{
            mapp[py][px].tile = 0;
        }
    }
}



//////////////////////////////////////////
// UNDO REDO HISTORY
//////////////////////////////////////////

void historise(){// COPY THE WHOLE LEVEL TO THE BACKUP MEMORY
    //allegro_message("historise()");
    //wait=4;
    int i,j;
    for(i=0; i<2048; i++){
        for(j=0; j<16; j++){
            bakk[j][i] = mapp[j][i];
        }
    }
}


void undo(){
    //allegro_message("undo()");
    int i,j;
    for(i=0; i<2048; i++){
        for(j=0; j<16; j++){
            mapp[j][i] = bakk[j][i];
        }
    }
    wait = 8;
}

void goto_end(){
    // GET THE LAST TILE TO FIND THE CORRECT OFFSET //
    int i,j;
    for(i=2008; i>40; i--){
        for(j=0; j<16; j++){
            if(mapp[j][i].tile > 0){
                offset = i-20;
                return;
            }
        }
    }

}


//////////////////////////////////////////
// INPUTS
//////////////////////////////////////////
void editor_input(){

    if( wait > 0 )return;//

    if(key[KEY_D])pipette();//SELECT TILE (PIPETTE)
    if(key[KEY_DEL])suppr();//REMOVE TILE

    if(key[KEY_F1])toggle_pal = ED_MAIN;//
    if(key[KEY_F2])toggle_pal = ED_TILES;//SHOW TILES PALETTE
    if(key[KEY_F3])toggle_pal = ED_EVENTS;//SHOW EVENTS
    if(key[KEY_F4])toggle_pal = ED_SETTINGS;//COLORS

    if(key[KEY_F5])toggle_pal = ED_TUNES;//TUNES SELECTOR

    if(key[KEY_F7])toggle_pal = ED_TILESETS;//TILESETS SELECTOR
    if(key[KEY_F8])toggle_pal = ED_STRINGS;//
    //if(key[KEY_F9])seek_GME(offset);
    if(key[KEY_F9])toggle_pal = ED_LEVELS;//LEVEL SELECTOR

    if(key[KEY_F10]){
        save_map();//SAVE
        save_brush();
    }

    if(key[KEY_F11]){//SAVE & PLAY LEVEL
        clear_keybuf();
        save_map();
        save_brush();
        game_start();
    }

    if(key[KEY_ESC] || key[KEY_F12]){
        goto_menu();//
        save_brush();
    }


    if(key[KEY_TAB]){
        if( toggle_pal == ED_MAIN ){
            toggle_pal = ED_TILES;
        }else{
            toggle_pal = ED_MAIN;
        }
        wait = 8;
    }




    switch( toggle_pal ){

        case ED_MAIN://MAIN SCREEN

            if( key[KEY_E] ){//EVENTS

                if( mouse_b == 1 )set_event();
                if( mouse_b == 2 )clear_event();//right click ->efface
                selected_event += scrollwheel();

            }else if(key[KEY_R]){// TILES HELPER
                draw_tiles_helper();
                selected_tile += scrollwheel();
                if(mouse_b == 1 )set_tile();//
                if(mouse_b == 2 )clear_tile();//right click ->efface

            }else if(key[KEY_B]){// BRUSHES
                selected_layer = 0;
                if( mouse_b == 1 )paste_brush();
                if( mouse_b == 2 )erase_brush();
                selected_brush+=scrollwheel();
                draw_brush( pos_x*16, pos_y*16);

            }else if(key[KEY_N]){//BG BRUSHES
                selected_layer = 1;
                if( mouse_b == 1 )paste_brush();
                if( mouse_b == 2 )erase_brush();
                selected_brush+=scrollwheel();
                draw_brush( pos_x*16, pos_y*16);

            }else{//TILES

                offset+=scrollwheel();

                if(mouse_b == 2 )clear_tile();//right click ->efface
                if(mouse_b == 1 ){
                    if(key[KEY_LCONTROL]){
                        add2clipboard();
                    }else{
                        set_tile();// DRAW
                    }
                }

                if(mouse_b == 4)toggle_pal = 1;// SHOW TILE PALETTE //
                if(key[KEY_LCONTROL] && key[KEY_V])paste();

                //if(key[KEY_LCONTROL] && key[KEY_B])paste_brush();

                if(key[KEY_RIGHT])offset++;//MOVE RIGHT
                if(key[KEY_LEFT])offset--;//MOVE LFT
                if(key[KEY_HOME])offset= 0;//BEG
                if(key[KEY_END])goto_end();//END

                if(key[KEY_CAPSLOCK]){toggle_layer();}

                if(key[KEY_LCONTROL] && key[KEY_Z])undo();
                if(key[KEY_LCONTROL] && key[KEY_LSHIFT])empty_clipboard();

                if(key[KEY_PGUP]){ selected_brush--; wait=8; }
                if(key[KEY_PGDN]){ selected_brush++; wait=8; }

                if(key[KEY_PLUS_PAD]){selected_tile++;wait=8;}
                if(key[KEY_MINUS_PAD]){selected_tile--;wait=8;}

                if(key[KEY_LSHIFT] && key[KEY_LEFT])shift_left();
                if(key[KEY_LSHIFT] && key[KEY_RIGHT])shift_right();
                //if(key[KEY_R])make_random_level();
            }
            break;



        case ED_TILES:// TILE PALETTE

            selected_brush += scrollwheel();

            if(mouse_b == 1)add2bigbrush();//CLIC
            if(mouse_b == 2)select_tile();//CLIC
            if(key[KEY_PLUS_PAD]){selected_tile++; wait=8;}
            if(key[KEY_MINUS_PAD]){selected_tile--; wait=8;}
            if(key[KEY_LCONTROL] && key[KEY_X])export_tilemap();
            if(key[KEY_LCONTROL] && key[KEY_S])save_brush();
            if(key[KEY_LCONTROL] && key[KEY_O])load_brush(level.tileset);
            if(key[KEY_PGUP]){ selected_brush--; wait=8; }
            if(key[KEY_PGDN]){ selected_brush++; wait=8; }

            if(key[KEY_LCONTROL] && key[KEY_UP])shift_brush_up();
            if(key[KEY_LCONTROL] && key[KEY_DOWN])shift_brush_down();
            if(key[KEY_LCONTROL] && key[KEY_LEFT])shift_brush_left();
            if(key[KEY_LCONTROL] && key[KEY_RIGHT])shift_brush_right();

            break;

        case ED_EVENTS://EVENTS

            if(key[KEY_HOME])events_offset = 0;
            if(key[KEY_END])events_offset = 240;

            if(key[KEY_PGUP])events_offset-= 16;
            if(key[KEY_PGDN])events_offset+= 16;

            if(key[KEY_UP])events_offset--;
            if(key[KEY_DOWN])events_offset++;
            events_offset += scrollwheel();
            if(key[KEY_SPACE] || key[KEY_ENTER]){
                select_event();//CLIC
                toggle_pal=0;
            }
            break;



        case ED_TUNES:// F5
            if(key[KEY_UP])select_tunes_up();
            if(key[KEY_DOWN])select_tunes_down();
            if(key[KEY_ENTER]){
                wait=8;
                sprintf(level.sound_track, vgms[selected_tune].f_name);
                load_track(level.sound_track);
            }
            offset+=scrollwheel();
            //if(key[KEY_M])mute_GME();
            //if(key[KEY_N])unmute_GME();

            break;



        case ED_LEVELS://ODT FILES
            if(key[KEY_UP])select_level_up();
            if(key[KEY_DOWN])select_level_down();
            if(key[KEY_ENTER]){
                wait = 8;
                sprintf(file_name, maps[selected_level].f_name);
                load_tilemap(file_name);
                clear_brushes();
                load_brush(level.tileset);
                //toggle_pal = ED_SETTINGS;
            }

            if(key[KEY_END]){//set next level
                sprintf(level.next_level, maps[selected_level].f_name);
                toggle_pal = ED_SETTINGS;
            }

            break;

        case ED_TILESETS:
            if(key[KEY_UP]){if(selected_tileset>0)selected_tileset--;wait=8;}
            if(key[KEY_DOWN]){selected_tileset++;wait=8;}

            if(key[KEY_ENTER]){
                sprintf(level.tileset, tilesets[selected_tileset].f_name);
                load_tileset(level.tileset);
                load_brush(level.tileset);
                wait = 8;
            }
            break;


        case ED_SETTINGS:
            if(key[KEY_UP])settings_up();
            if(key[KEY_DOWN])settings_down();
            if(key[KEY_PLUS_PAD])settings_right();
            if(key[KEY_MINUS_PAD])settings_left();
            break;

        case ED_STRINGS:
            if(key[KEY_UP])strings_up();
            if(key[KEY_DOWN])strings_down();
            strings_input();
            break;
    }

    // Bien vu gregos ;)
    if(mouse_x >= SCREEN_W-2)offset++;
    if(mouse_x <= 1 )offset--;

    if(events_offset < 0)events_offset = 0;
    if(events_offset > 255-16)events_offset = 255-16;
    if(offset < 0)offset = 0;
    if(offset > 2008)offset=2008;
}

void draw_main(){// F1 - MAIN EDIT SCREEN

    draw_tiles();
    show_clipboard();

    char chars[20];

    int event_id = mapp[pos_y][pos_x+offset].event;//hover event

    sprintf(chars, "NAME: %s", level.level_name);   print(fram, chars, 0, 256);
    sprintf(chars, "FILE: %s", file_name);          print(fram, chars, 0, 256+10);
    sprintf(chars, "TUNE: %s", level.sound_track);  print(fram, chars, 0, 256+20);
    sprintf(chars, "TSET: %s", level.tileset);      print(fram, chars, 0, 256+30);
    sprintf(chars, "WARP: %s", level.next_level);   print(fram, chars, 0, 256+40);
    sprintf(chars, "BRUSH:%03d", selected_tile);    print(fram, chars, 0, 256+60);

    print(fram, "+/- FOR NXT/PRV", 100, 256+60);

    if( tileset != NULL ){
        draw_tile(fram, selected_tile, 80, 256 + 60 );//CURRENT TILE
    }

    draw_brush(270, 256);//DA BIG BRUSH ()

    sprintf(chars, "EVENT:%03d:%03d", selected_event, event_id);
    print(fram, chars, 0, 256 + 90);

    print(fram, "B -> SOLID BRUSH", 0, 256 + 100);
    print(fram, "N -> BG BRUSH", 0, 256 + 110);
    print(fram, "E -> EVENTS", 0, 256 + 120);
    print(fram, "R -> TILES", 0, 256 + 130);
    print(fram, "D -> EYEDROP", 0, 256 + 140);
    print(fram, "TAB -> BRUSH EDITOR", 0, 256 + 150);

    if(selected_layer == 1)print(fram, "BACKGROUND MODE", 200, 256 + 110);//BACKGROUND
}


void editor(void){// main tile editor frame

    draw_background_in_editor();
    //if(show_grid)draw_grid();


    int colnumber = SCREEN_W/16;
    pos_x = mouse_x / (SCREEN_W/colnumber);
    //pos_x = mouse_x / (SCREEN_W/40);//SCREEN MUST BE 640px
    pos_y = (mouse_y) / (SCREEN_H/16);
    row_y = (mouse_y) / (SCREEN_H/10);

    if(pos_y>15)pos_y=15;

    // REVERSED PRIORITIES //
    if( strlen(level.tileset) == 0)toggle_pal = ED_TILESETS;//SELECT A TILESET
    if( strlen(file_name) == 0)toggle_pal = ED_LEVELS;//SELECT A LEVEL


    draw_help();// DRAW SHORTCUTS ETC

    switch(toggle_pal){

        default:
        case ED_MAIN:// main screen (EDITOR) - F1
            draw_main();
            break;



        case ED_TILES:// TILE palette
            draw_tiles_palette();
            show_selected_tile( selected_tile );
            draw_big_brush();
            break;

        case ED_EVENTS://events
            draw_tiles();
            break;

        case ED_TUNES://
            draw_tiles();
            show_tunes();
            break;

        case ED_LEVELS://level file selector
            draw_tiles();
            show_levels();
            break;

        case ED_TILESETS://
            draw_tiles();
            show_tilesets();
            break;


        case ED_SETTINGS://
            draw_tiles();
            draw_settings();
            break;

        case ED_STRINGS:
            draw_tiles();
            draw_strings();
            break;

        case ED_QUIT://
            clear_bitmap(fram);
            game_status = GS_MENU;//back to menu
            toggle_pal = ED_MAIN;
            break;

    }

    if( key[KEY_PRTSCR] && wait== 0 ){
        editor_screenshot();
        wait=8;
    }

    show_selection();//SHOW MOUSE POSITION

    editor_input();//read mouse and keyboard

    blit_fram();
    clear_bitmap(fram);
    if( wait > 0 )wait--;

    game_count++;
    if(game_count>=60)game_count=0;
}




//////////////////////////////////////////////////////////////////
// SAVE A TILESET (PLUS PALETTE)
//////////////////////////////////////////////////////////////////
void export_tilemap(void){


    if(tileset == NULL){
        allegro_message("tileset == NULL");
        return;
    }

    if(strlen(level.tileset) == 0){
        allegro_message("level.tileset == NULL");
        return;
    }

	PALETTE p;
	BITMAP *b;

	b = create_bitmap_ex(8, 320, 240);//So we can put the 255 tiles plus some crap
	clear_bitmap(b);

	char buf[256];//filename
	sprintf(buf, level.tileset);

	get_palette(p);

    // TRANSPARENT COLOR (FUSHIA)//
    p[0].r = 63;//GREY
    p[0].g = 0;//BG BASE G COLOR
    p[0].b = 63;//BG BASE B COLOR


    // FILL PALETTE WITH FUSHIA TO MAKE IT 'UNUSABLE' WITH PHOTOSHOP //

    int i;
    for(i=1; i <= 240; i++){
        p[i].r = 63;
        p[i].g = 0;
        p[i].b = 63;
    }

    for(i=245; i <= 255; i++){
        p[i].r = 63;
        p[i].g = 0;
        p[i].b = 63;
    }

    blit(tileset, b, 0, 0, 0, 0, 320, 240 );
    rectfill(b, 0,208,320,240, 244);//make a black footer

    draw_tileset_palette(b, 50, 218);//4 color palette + transparent color
    draw_color_palette(b, 288, 208);

	save_bitmap( buf, b, p);
	destroy_bitmap(b);
    //allegro_message("Tileset saved");
    // JUMP TO ANOTHER PAGE //
    toggle_pal = ED_MAIN;
}


int is_empty_tile( int tilenum ){

    int row = tilenum/20;
    int col = tilenum%20;
    int c;
    int i,j;
    int total = 0;

    //int first_color = getpixel( tileset , (col*16) , (row*16) );//retourne l'index de la couleur :)
    //char same = 1;

    //log2file("Tile[%d]", tilenum);
    for(i = 0; i < 16; i++){
        for(j = 0; j < 16; j++){
            c = getpixel( tileset , (col*16) + i , (row*16) + j );//retourne l'index de la couleur :)
            //if( c != first_color)return 0;//variety detected ;)
            total+=c;
        }
    }

    if(total==0)return 1;//tiles toute transparente
    //return 1;// TILE IS EMPTY
    return 0;// TILE IS EMPTY
}

