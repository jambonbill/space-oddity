

#include <allegro.h>
//#include <stdio.h>
#include <math.h>
#include <string.h>

#include "control.h"
#include "options.h"
#include "main.h"
#include "hisc.h"
#include "editor.h"
#include "dat/gfx.h"

BITMAP *ptibuffer;
BITMAP *buffer;
DATAFILE *intro;//backgrounds
PLAYER playr;

// controls
Tcontrol ctrl;
Toptions options;

// hiscores
Thisc *hisc_table;
Thisc *hisc_table_space;

int menu_status = 0;//sound state
int music_status = 0;//music test
int sound_status = 0;//sound test
int wait = 0;//frames to go


int fn;//framenum
float speed = 1;//??
float elasticity = 0;//??
int amplitude = 80;//

void goto_menu(void){

    level.bg_r = 25;
    level.bg_g = 25;
    level.bg_b = 25;
    playr.y = 0;

    //clear_bitmap( fram );

    update_palette();
    level.background = 0;
    set_scroll_speed(1);

    load_track("menu.vgm");
    wait = 8;
    game_status = GS_MENU;

}



void goto_editor(void){
    load_tilemap( file_name );
    game_status = GS_EDIT;
    wait = 8;
}


void goto_highscore(void){

    level.bg_r = 20;
    level.bg_g = 20;
    level.bg_b = 20;

    update_palette();
    level.background = 0;
    set_scroll_speed(2);

	// loading highscores //
	//PACKFILE *pf;

    //allegro_message("Goto highscore");
	log2file("Loading hiscores\n");
    /*
	pf = pack_fopen("oddity.hi", "rp");
	if (pf) {
		load_hisc_table(hisc_table, pf);
		load_hisc_table(hisc_table_space, pf);
		pack_fclose(pf);
	}
	else {
	    //log2file("  *** failed, resetting");
		reset_hisc_table(hisc_table, "Bill", 25000, 5000);
		sort_hisc_table(hisc_table);
	}
    */

    load_track("hiscores.vgm");
    wait = 8;
    game_status = GS_SCORES;

}

void menu_do(){

    switch(menu_status){

        case 0:// LEVEL 1 - SPACE
            //if(strlen(file_name)==0)strcpy( file_name, "first_level.odt" );
            //game_status = GS_PLAY;
            //allegro_message("game_status=%d", game_status );
            //game_start();
            game_status = GS_SELECT_LEVEL;
            break;

        case 1:// MUSIC TEST
            break;

        case 2:// SOUND TEST
            play_sound( sound_status );
            break;

        case 3:// HI SCORE
            //play_sound( sound_status );
            goto_highscore();
            break;

        case 4:// QUIT
            game_status = GS_QUIT_GAME;
            break;

        case 5://SELECT LEVEL
            break;

    }
}

void menu_up(){
    if(menu_status <= 0)menu_status = 5;
    menu_status--;
    //play_sound( SFX00 );
}

void menu_down(){
    menu_status++;
    if(menu_status >= 5)menu_status = 0;
    //play_sound( SFX00 );
}

void menu_right(){
    //if(menu_status!=4)return;
    switch(menu_status){

        case 0:
        case 1:
            break;

        case 2:
            sound_status++;
            if(sound_status > 16 )sound_status = 16;
            play_sound(sound_status);
            break;


    }
}

void menu_left(){
    //if(menu_status!=4)return;
    switch(menu_status){

        case 0:
        case 1:
            break;

        case 2:
            sound_status--;
            if(sound_status < 0 )sound_status = 0;
            play_sound(sound_status);
            break;


    }
}


void randomize_logo(){

    int rnd = AL_RAND()%15;

    switch(rnd){
        case 0: elasticity=0;   break;
        case 1: elasticity=130; break;
        case 2: elasticity=126.2; break;
        case 3: elasticity=85; break;
        case 4: elasticity=84; break;
        case 5: elasticity=62; break;
        case 6: elasticity=10; break;
        case 7: elasticity=3; break;
        case 8: elasticity=1; break;
        case 9: elasticity=0; break;
        case 10: elasticity=-2; break;
        case 11: elasticity=-3; break;
        case 12: elasticity=-51; break;
        case 13: elasticity=-64.1; break;
        default:
            elasticity = -180 + AL_RAND()%360;
            break;
    }
    amplitude = 4 + AL_RAND()%80;
}



void mainmenu(void){

    int i;
    if(wait == 0){

        if(is_fire(&ctrl))menu_do();
        if(key[KEY_ENTER])menu_do();
        if(key[KEY_PLUS_PAD])elasticity+=0.1;
        if(key[KEY_MINUS_PAD])elasticity-=0.1;

        if(key[KEY_PGUP])amplitude++;
        if(key[KEY_PGDN])amplitude--;
        if(key[KEY_I])game_status = GS_INPUTNAME;

        if(key[KEY_F9])goto_editor();

        if(is_up(&ctrl))menu_up();
        if(is_down(&ctrl))menu_down();
        if(is_left(&ctrl))menu_left();
        if(is_right(&ctrl))menu_right();
        if(is_any(&ctrl)){
            randomize_logo();
            wait = 8;
        }
    }
    if(wait > 0)wait--;//timer for keypress

    int xoffset = 88;
    int yoffset = 0;
    playr.yshift = 0;

    clear_bitmap(ptibuffer);//if you dont do it here, then we cant screenshot it
    clear_bitmap(buffer);//if you dont do it here, then we cant screenshot it


    move_particules();
    draw_background();
    draw_bg_particules();

    print(ptibuffer, message, 0, 1);


    //bg_particule(
    int px= 320;
    //int px= AL_RAND()%320;
    int py= AL_RAND()%240;
    int direction = 80 + AL_RAND()%20;
    //float s = (AL_RAND()%100)/100;
    float s = 0.5;
    //s = 0.5 + (AL_RAND()%100)/100;
    if(AL_RAND()%20==0)s = 2;
    if( fn%20 == 0 )bg_particule( px, py, s, direction, 700, ROCK00 + AL_RAND()%7, 0, 0);

    // TWIST THE LOGO //
    for( i = 0 ; i < 142; i++){
        //void rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
        int newx;
        int newy = i + yoffset;
        fixed fAngle = itofix(i*elasticity + fn);// + wait*8
        newx = fixtoi( fcos(fAngle) * (amplitude + wait*8) );
        newx+= xoffset;//
        masked_blit(intro[0].dat, buffer, 0, i, newx, newy, 144, 1);
    }

    char msg[20];

    if(speed <= 1){


        rectfill(buffer, 0, 110 + menu_status*10, SCREEN_W, 110 + menu_status*10 + 8, 4);//selection

        print(buffer, "PLAY", 100, 110);
        print(buffer, "MUSIC TEST", 100, 120);

        sprintf(msg, "SOUND TEST %d", sound_status);
        print(buffer, msg, 100, 130);

        print(buffer, "HI SCORES", 100, 140);
        print(buffer, "QUIT", 100, 150);

        // DEBUG
        //sprintf(message, "%dHz", get_refresh_rate() );
        //print2(msg, 0, 10);

        //sprintf(msg, "ELASTICITY:%f", elasticity);
        //print(msg, 100, 150);

        // POINTER //
        //print(">", 90, (110 + menu_status * 10) );
        print(buffer, "(c)2009 Brico System", 70, 180);
    }else{
        speed = speed - 0.1;
    }

    // textprintf_ex(buffer, font, 0, 0, 2, -1, "%s", message);

    blit_to_screen(buffer);
    fn++;
}


int level_num;
void select_screen(){//LEVEL SELECTOR
    int i;

    if(wait == 0){

        if(is_up(&ctrl)){//UP
            level_num--;
            if(level_num < 0 )level_num = count_map-1;
            wait = 8;
        }

        if( is_down( &ctrl ) ){//DOWN
            level_num++;
            if(level_num >= count_map)level_num = 0;
            wait = 8;
        }

        if(key[KEY_ESC])goto_menu();

        if( is_fire(&ctrl) || key[KEY_ENTER] ){
            strcpy( file_name, maps[ level_num ].f_name );
            game_start();
        }
    }

    if(wait > 0)wait--;//timer for keypress

    clear_bitmap(ptibuffer);//if you dont do it here, then we cant screenshot it
    clear_bitmap(buffer);//if you dont do it here, then we cant screenshot it

    move_particules();
    draw_background();
    draw_bg_particules();

    rectfill(buffer, 0, 20 + level_num*10, SCREEN_W, 20 + level_num*10 + 8, 4);//selection
    print(buffer, "SELECT LEVEL :", 100, 0 );

    int offset = 0;
    for(i=0; i < 19; i++){
        char chars[32];
        sprintf( chars, "%s", maps[ (i + offset) ].f_name );
        print( buffer, chars, 10, 10*i + 20);
    }

    blit_to_screen( buffer );
    fn++;
}


void gameover(void){
    shift = 0;
    wait = 200;
    game_status = GS_GAMEOVER;
}

void gameover_screen(){


    clear_bitmap(buffer);//

    move_bullets();
    move_stuff();
    move_particules();

    draw_background();
    draw_tiles_ingame();
    draw_particules();
    draw_bullets();
    draw_stuff();

    if(shift < 90)shift+=8;

    print(buffer, "GAME OVER", 100, shift);

    char str[32];
    sprintf(str,"SCORE: %d", playr.score);
    print(buffer, str, 100, shift + 10);

    blit_to_screen(buffer);

    wait--;
    if(wait <= 0){

        // check if player got a high score
        /*
        if (qualify_hisc_table(hisc_table, post)) {
            //enter_hisc_table(hisc_table, post);
            //sort_hisc_table(hisc_table);
            game_status = GS_INPUTNAME;

        }else{
            //goto_menu();
            goto_highscore();
        }
        */
        goto_menu();
        //goto_highscore();
    }

    fn++;
}





void highscore_screen(void){

    if(wait == 0){
        if(is_fire(&ctrl)){
            goto_menu();
            wait = 8;
        }

        if(key[KEY_DEL]){
            //reset_hisc_table(hisc_table, playr.name, 0, 1000);
            //save_hisc_table(hisc_table);
            wait = 8;
        }
    }
    if(wait > 0)wait--;//timer for keypress

    clear_bitmap( ptibuffer );//if you dont do it here, then we cant screenshot it
    clear_bitmap( buffer );//if you dont do it here, then we cant screenshot it
    draw_background();

    print(ptibuffer, "------ HIGH SCORES ------", 40, 10);
    //draw_hisc_table(hisc_table, 10, 0);

    rectfill(buffer, 0, 180, SCREEN_W, 188, 4);//selection
    print(buffer, "Press FIRE", 110, 180 );
    blit_to_screen(buffer);
}








/////////////////////////////////////////////////////
// ENTER YOUR NAME ( WITH A JOYPAD ;)
/////////////////////////////////////////////////////
//add selected letter to player name
char row,col;
const char xoffset = 90;//decalage x
const char yoffset = 20;//decalage x
char txt[1];
char str[32];
const char colnum = 10;
const char padding = 16;
const char hichars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ !-&0123456789";

void add_letter(void){

    strncpy(txt, hichars + (col + row*colnum)  , 1);
    //allegro_message("Add letter, row=%d col=%d, char=%s", row, col, txt);
    //char ASCII = txt & 0xff;
    strcpy( str, playr.name );
    int caret = strlen(str);
    char position = col + row*colnum;

    if( position == 40 ){//DONE
        /*
        Thisc post;
        post.score = playr.score;
        strcpy(post.name, playr.name);

        enter_hisc_table(hisc_table, post);
        sort_hisc_table(hisc_table);

        PACKFILE *pf;

        //log2file("Saving highscores");
        pf = pack_fopen("oddity.hi", "wp");
        if ( pf ) {
            save_hisc_table(hisc_table, pf);
            save_hisc_table(hisc_table_space, pf);
            pack_fclose( pf );
        }
        */
        game_status = GS_SCORES;
        return;
    }

    if( position == 49 ){
        str[caret - 1] = 0;// BACKSPACE
    }else if(caret < 10){
        str[caret] = txt[0];
        str[caret + 1] = 0;
    }
    strcpy( playr.name, str );
}

void inputname_screen(void){

    if(wait == 0){

        if(is_up(&ctrl)){row--;if(row<0)row=4;wait=8;}
        if(is_down(&ctrl)){row++;if(row>4)row=0;wait=8;}
        if(is_left(&ctrl)){col--;if(col<0)col=colnum-1;wait=8;}
        if(is_right(&ctrl)){col++;if(col>=colnum)col=0;wait=8;}
        if(is_fire(&ctrl)){
            //goto_menu();
            add_letter();
            wait = 8;
        }
    }

    if(wait > 0)wait--;//timer for keypress

    buffer  = create_bitmap( 320, 192 ); // virtual screen
    clear_bitmap( ptibuffer );//if you dont do it here, then we cant screenshot it
    clear_bitmap( buffer );//if you dont do it here, then we cant screenshot it
    draw_background();

    int i,j;
    print(ptibuffer, "-------- ENTER YOUR NAME --------", 26, 10);

    //draw_sprite(buffer, gfx[CURSOR].dat, xoffset + col*padding, row*padding);// CURSOR
    rectfill(buffer , xoffset + col*padding - 2 , yoffset + row*padding - 2 , xoffset + col*padding + padding - 6, yoffset + row*padding + padding - 6, 4 );

    int caret = strlen( hichars );

    // draw the alphabet //
    for(i = 0; i < colnum; i++ ){
        for(j = 0; j < 5; j++ ){

            if( (i + j*colnum) > caret )continue;
            strncpy(txt, hichars + (i + j*colnum)  , 1);
            //a[0] = 42 + i;
            print(buffer, txt , xoffset + i*padding, yoffset + j*padding);
        }
    }


    //rectfill(buffer, 0, 110, 320, 118, 4);
    char name[10];
    print(buffer, "----------------------------------", 26, 130);
    sprintf(name, "YOUR NAME: %s", playr.name);
    print(buffer, name, 40, 140);

    sprintf(name, "SCORE: %d", playr.score );print(buffer, name, 40, 100);
    sprintf(name, "DEBUG: %d", (col + row*colnum) );print(buffer, name, 40, 110);



    blit_to_screen(buffer);
}


