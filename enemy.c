
#include <allegro.h>
//#include <stdio.h>
#include <math.h>
//#include <string.h>

#include "main.h"
#include "dat/gfx.h"
#include "sfx.h"

PLAYER playr;

///////////////////////////////////////////////////////////////
// ENEMY GENERATION
///////////////////////////////////////////////////////////////

int please_stuff( int sprite ){ // try to get a free stuff, and 'stuff it with the desired sprite

    int i = get_free_stuff();
    if(i < 0)return i;

    stuffs[i].spritenum = sprite;
    BITMAP *bmp;
    bmp = gfx[sprite].dat;
    stuffs[i].w = bmp->w;
    stuffs[i].h = bmp->h;

    //set some default values :p
    stuffs[i].x = 320;
    stuffs[i].y = suggested_y;
    return i;//ok
}




void new_invader(void){
    int i = please_stuff( INVADER02 );
    if( i <0 )return;

    //stuffs[i].x = 320;
    //stuffs[i].movement_type = 6;//random directions :p
    //stuffs[i].xspeed = 2;//
    stuffs[i].framecount = 1;
    stuffs[i].fall = 1;
    //stuffs[i].y = suggested_y;
    //if(stuffs[i].y < 32)stuffs[i].direction = 270;//vers le bas
    //if(stuffs[i].y >=160)stuffs[i].direction = 90;//vers le haut
    stuffs[i].energy = 1;//life

}

void new_bee(void){

    int i = please_stuff( BEE00 );
    if( i <0 )return;

    stuffs[i].x = 320;
    stuffs[i].movement_type = 6;//random directions :p
    stuffs[i].xspeed = 2;//
    stuffs[i].framecount = 1;
    stuffs[i].x = suggested_x;
    if(stuffs[i].y < 32)stuffs[i].direction = 270;//vers le bas
    if(stuffs[i].y >=160)stuffs[i].direction = 90;//vers le haut
    //stuffs[i].fragile=1;
    stuffs[i].energy = 1;//life
    stuffs[i].score = 100;//

}

void new_squartron( void ){

    int i = please_stuff( SQUARTRON );
    if( i <0 )return;

    //stuffs[i].x = 320;
    stuffs[i].movement_type = 11;//SQUARTRON
    //stuffs[i].xspeed = 1;//
    stuffs[i].direction = (AL_RAND()%4)*90;//random
    //stuffs[i].direction = 8;//random

    stuffs[i].energy = 2;//life
    stuffs[i].score = 500;//
    stuffs[i].munitions = 2;

}

// RANDOM ASTEROID //
void new_boulder(void){

    int i = get_free_stuff();
    if(i < 0)return;

    int boulder_size = AL_RAND() % 2;//BOULER02 est trop petit

    stuffs[i].x = 320 + (AL_RAND() % 64);
    //stuffs[i].y = -20 + (AL_RAND() % 240);
    stuffs[i].y = suggested_y;
    //stuffs[i].yspeed = -1 + (AL_RAND() % 3);
    //stuffs[i].xspeed = (AL_RAND() % 3);
    //stuffs[i].speed = 0.5;//!!!!
    stuffs[i].spritenum = BOULDER00 + boulder_size;

    BITMAP *bmp;
    bmp = gfx[stuffs[i].spritenum].dat;
    stuffs[i].w = bmp->w;
    stuffs[i].h = bmp->h;

    stuffs[i].energy = 2;//life
    stuffs[i].score = 50;//
}

void new_medium_boulder(int x, int y){

    int i = please_stuff( BOULDER01 );
    if(i < 0)return;

    stuffs[i].x = x;
    stuffs[i].y = y;
    //stuffs[i].yspeed = -1 + (AL_RAND() % 3);
    //stuffs[i].xspeed =  (AL_RAND() % 3);
    stuffs[i].energy = 2;//life
    stuffs[i].score = 50;//

}

// SMALL CRAB //
// http://www.sidabitball.com/oddity/doku.php?id=crabe
void new_crab(void){

    int i = please_stuff( CRAB00 );
    if(i < 0)return;

    stuffs[i].framecount = 3;
    stuffs[i].score = 1000;//nm points when killed
    stuffs[i].movement_type = 12;//CRAB
    stuffs[i].fall = 1;
    stuffs[i].munitions = 10;
    stuffs[i].shoty = 6;
    stuffs[i].energy = 4;//life
}


void new_cockpitbee(void){

    int i = please_stuff( COCKPITBEE00 );
    if(i < 0)return;

    stuffs[i].framecount = 2;
    stuffs[i].movement_type = 3;//SIN
    stuffs[i].x = 320;
    stuffs[i].y = suggested_y;
    stuffs[i].xspeed = 1;
    stuffs[i].energy = 1;//life
    stuffs[i].fragile=1;
    stuffs[i].score = 20;//nm points when killed

}


void new_doubleshot(void){
    int i = please_stuff( DOUBLESHOT00 );

    if(i < 0)return;
    stuffs[i].framecount = 5;
    stuffs[i].x = 320;
    stuffs[i].y = suggested_y;
    stuffs[i].xspeed = 1;
    stuffs[i].energy = 2;//life
    stuffs[i].fragile=1;
}


void new_generator(){

    int i = please_stuff( NEST00 );
    if(i < 0)return;

    stuffs[i].spritehit = NEST01;
    stuffs[i].x = 320;
    stuffs[i].y = suggested_y;
    stuffs[i].movement_type = 21;
    stuffs[i].delay = 32;
    stuffs[i].event = 14;//new bee

    stuffs[i].energy = 16;//life
    stuffs[i].score = 1000;
}


void new_rocket(void){//ROCKET

    int i = please_stuff( ROCKET00 );
    if(i < 0)return;

    stuffs[i].x = suggested_x;
    stuffs[i].y = suggested_y;
    stuffs[i].movement_type = 9;
    stuffs[i].energy = 1;//life
}

void new_pic(void){// TODO : CHANGE THE SPRITE

    int i = please_stuff( BOULDER01 );
    if(i < 0)return;
    stuffs[i].x = suggested_x;
    stuffs[i].y = suggested_y;
    stuffs[i].movement_type = 10;

    stuffs[i].energy = 1;//life
}

void new_roto(void){//ROTOGEAR (PETIT MACHIN)

    int i = please_stuff( ROTO00 );
    if(i < 0)return;

    stuffs[i].framecount = 3;
    stuffs[i].movement_type = 3;//SIN
    stuffs[i].xspeed = 0.5;
    stuffs[i].munitions = 1;
    stuffs[i].shoty = 8;
    stuffs[i].score = 20;//nm points when killed
    stuffs[i].energy = 2;//life
}



void new_jellyfish(void){// PETITE MEDUSE //
    int i = please_stuff( JELLY00 );
    if(i < 0)return;

    stuffs[i].framecount = 3;
    stuffs[i].movement_type=8;//seek
    stuffs[i].yspeed = 1;
    stuffs[i].xspeed = 1;
    stuffs[i].energy = 1;
    stuffs[i].score = 50;
}


void new_meduse(void){// GROSSE MEDUSE //
    int i = please_stuff(MEDUSE00);
    if(i < 0)return;

    stuffs[i].spritehit = MEDUSE99;
    stuffs[i].framecount = 5;

    stuffs[i].shotx = 16;
    stuffs[i].shoty = 32;

    stuffs[i].y = suggested_y;
    stuffs[i].x = 320;
    stuffs[i].yspeed = 0.6;
    stuffs[i].energy = 4;
    stuffs[i].score = 200;
}


void new_canon_up(void){// CANON
    int i = please_stuff(CANON00);
    if(i < 0)return;

    stuffs[i].munitions = 100;
    stuffs[i].shotx = 8;
    stuffs[i].shoty = 8;

    stuffs[i].energy = 1;//life
    stuffs[i].score = 500;//nm points when killed
}


void new_canon_down(void){// CANON
    int i = please_stuff( CANON01 );
    if(i < 0)return;

    stuffs[i].munitions = 100;
    stuffs[i].shotx = 8;
    stuffs[i].shoty = 8;
    stuffs[i].energy = 1;//life
    stuffs[i].score = 500;//nm points when killed
}

void new_crevette(void){// CREVETTE //
    int i = please_stuff( CREVETTE00 );
    if(i < 0)return;

    stuffs[i].framecount = 5;
    stuffs[i].movement_type = 20;//come and go back
    stuffs[i].energy = 8;//life
    stuffs[i].score = 800;//nm points when killed
}


void new_fishship(void){// FISHSHIP //
    int i = please_stuff( FISHSHIP00 );
    if(i < 0)return;
    stuffs[i].framecount = 2;
    stuffs[i].movement_type = 3;//SIN

    stuffs[i].xspeed = 1;
    stuffs[i].energy = 1;//life
    stuffs[i].score = 100;//nm points when killed

}



void new_fly(void){// GROSSE MOUCHE //
    int i = please_stuff( MOUCHE00 );
    if(i < 0)return;

    stuffs[i].spritehit = MOUCHE99;

    stuffs[i].shotx = 16;
    stuffs[i].shoty = 28;
    stuffs[i].framecount = 3;
    stuffs[i].energy = 5;
    stuffs[i].munitions = 5;

    stuffs[i].yspeed = 0.2;
    stuffs[i].score = 500;
}


void new_friend(void){// GROSSE MOUCHE //
    int i = please_stuff( FRIENDLY00 );
    if(i < 0)return;

    stuffs[i].shotx = 16;
    stuffs[i].shoty = 28;
    stuffs[i].framecount = 3;
    stuffs[i].energy = 1;
    stuffs[i].xspeed = 1;
    stuffs[i].fragile = 1;
    stuffs[i].score = 10;//nm points when killed
}

void new_larve(){//LARVE
    int i = please_stuff(LARVE00);
    if(i < 0)return;

    stuffs[i].spritehit = LARVE99;
    stuffs[i].shotx = 16;
    stuffs[i].shoty = 30;
    stuffs[i].framecount = 3;
    stuffs[i].energy = 6;
    stuffs[i].munitions = 6;
    stuffs[i].yspeed = -0.1;
    stuffs[i].score = 500;
}

void new_machoire(){//MACHOIRE
    int i = please_stuff( MACHOIRE00 );
    if(i < 0)return;
    stuffs[i].framecount = 3;
    stuffs[i].energy = 6;
    stuffs[i].munitions = 6;
    stuffs[i].shotx = 8;
    stuffs[i].energy = 8;
}


void new_mine(){//MINE
    int i = please_stuff( MINE );
    if(i < 0)return;

    stuffs[i].energy = 4;
    stuffs[i].energy = 2;
    stuffs[i].score = 500;//nm points when killed
    stuffs[i].movement_type = 4;

}





void new_spike(void){// SPIKE //

    int i = please_stuff( SPIKE00 );
    if(i < 0)return;
    stuffs[i].framecount = 3;
    stuffs[i].xspeed = 1;
    stuffs[i].energy = 2;//life
    stuffs[i].score = 200;//nm points when killed
}


//BOF
void new_tiefighter(void)
{
    int i = please_stuff( TIE00 );
    if(i < 0)return;

    stuffs[i].shoty = 10;
    stuffs[i].framecount = 7;
    stuffs[i].energy = 4;
    stuffs[i].munitions = 4;
    stuffs[i].score = 300;

}




// NEW //
void new_rond(void)
{
    int i = please_stuff( ROND );
    if(i < 0)return;

    stuffs[i].energy = 1;
    stuffs[i].xspeed = 1;
    stuffs[i].score = 100;
    stuffs[i].movement_type = 19;

}

// NEW //
void new_merdouille(void)
{
    int i = please_stuff( ROND00 );
    if(i < 0)return;

    stuffs[i].framecount = 1;
    stuffs[i].energy = 1;
    stuffs[i].xspeed = 1;
    stuffs[i].score = 100;
    stuffs[i].movement_type = 18;

}











////////////////////////////////////////////////////////
// ENEMY DEATH //
// SOME SPECIAL ACTIONS ON ENEMY DEATH !
////////////////////////////////////////////////////////

void enemy_death(int id)
{
    int x = stuffs[id].x;
    int y = stuffs[id].y;
    int i;
    int spritenum = stuffs[id].spritenum;

    // MAKE SURE ITS DEAD //
    stuffs[id].energy = 0;

    // ADD SCORE //
    playr.score += stuffs[id].score;

    explode2(x, y);

    // CREATE A DEAD ENEMY PARTICULE //
    i = get_free_particule();
    if(i>=0){
        particules[i].x = x;
        particules[i].y = y;
        particules[i].vy = 0;//move up
        particules[i].vx = 0;
        particules[i].spritenum = stuffs[id].spritenum + stuffs[id].step;// enemy sprite
        particules[i].blink = 1;
        particules[i].life = 20;//make it real for n frames !
    }

    //SPECIAL DEATH
    switch (spritenum)
    {

        case MEDUSE00://BIG MEDUSE
            play_sound(SFX00);
            circle_shot(x, y);
            break;

        case BOULDER00:// BIG
            play_sound(SFX01);
            // SPLIT INTO TWO SMALL
             new_medium_boulder(x, y);
             new_medium_boulder(x, y);
            break;

        case BOULDER01:// MEDIUM
            play_sound(SFX01);
            break;

        case BOULDER02:// SMALL
            play_sound(SFX01);
            break;

        case FRIENDLY00:
            add_bonus(x, y);//add some reconfort
            break;

        case MINE:
            circle_shot(x, y);
            break;

        case LARVE00:
            circle_shot(x, y);
            break;

        case SPIKE00:
            circle_shot(x, y);
            break;

        case MOUCHE00:
            suggested_x = x;
            suggested_y = y;
            for( i = 0; i < 8; i++){
                new_bee();
            }
            break;

        default:
            //score += stuffs[id].score;//add some points to hiscore
            play_sound( 3 );
            /*
            if((AL_RAND() % 2) == 0){//cadeau
                add_bonus(x, y);//add some reconfort
            }
            */
            break;
    }
}



