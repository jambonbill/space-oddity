
#include <allegro.h> // You must include the Allegro Header file
#include <string.h>
#include "main.h"
#include "enemy.h"
#include "dat/gfx.h"

/*
const char event_table[256][16] =
{
"Big boulder", //
"Small boulder",//
"Cockpitbee",
"Doubleshot",//
"FishShip",//
"Crab",//
"Jellyfish",//
"Meduse"//
"Twin",//
"Fly",//
"Machoire",//
"Tie",//
"Larve",
"Spike",
"MuchAsteroids",
"Asteroids",//
"", //
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"Speed Up",
"Speed Dn",
"",
"",
"",
"",
"",
"",
"",
"Fade out",
"Fade in",
"",
"Earthquake",
"Warning",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"Option",// BONUSES //
"Speedup",//
"",
"",
"",
"",
"",
"",
"",
"?",
"?",
"?",
"-",
"-",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"GAME OVER",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
};
*/


int get_event_sprite(unsigned char event){//obvious no ?
    switch(event){

        // PETITS ENEMIES
        case 0: return 0;break;//NULL
        case 1: return BOULDER01;break;// SMALL BOULDER
        case 2: return FISHSHIP00;break;//
        case 3: return MEDUSE00;break;// MEDUSE
        case 4: return JELLY00;break;//
        case 5: return CRAB00;break;//
        case 6: return ROTO00;break;//
        case 7: return DOUBLESHOT00;break;//
        case 8: return COCKPITBEE00;break;//
        case 9: return LARVE00;break;//

        //MOYENS
        case 10:return MEDUSE00;break;// MEDUSE
        case 11:return FRIENDLY00;break;// BONUS MAKER
        case 12:return CANON00;break;//CANON
        case 13:return CANON01;break;//CANON
        case 14:return BEE00;break;//
        case 15:return ROCKET00;break;//

        case 16:return SQUARTRON;break;//NEW
        case 17:return ROND;break;//ROND
        case 18:return ROND00;break;//MERDOUILLE

        // GROS
        case 19:return MINE;break;
        case 20:return CREVETTE00;break;
        case 21:return MACHOIRE00;break;
        case 22:return MOUCHE00;break;
        case 23:return INVADER01;break;

        // SPECIAUX //
        case 30:return NEST00;break;

        // BONUSES
        case 100: return OPTION00;break;
        case 101: return BONUS00;break;

        // EVENTS //
        case 128: return ICO_SPEEDUP;break;
        case 129: return ICO_SPEEDDOWN;break;

        // COLORS //
        case 240: return ICO_BGR_PLUS;break;//bg red++
        case 241: return ICO_BGG_PLUS;break;//bg green++
        case 242: return ICO_BGB_PLUS;break;//bg blue++
        case 243: return ICO_BGR_MINUS;break;//bg red--
        case 244: return ICO_BGG_MINUS;break;//bg green--
        case 245: return ICO_BGB_MINUS;break;//bg blue--
        case 246: return ICO_CASH;break;// THIS TILE GIVE SOME CASH


        case 250: return ICO_MSG1;break;
        case 251: return ICO_MSG2;break;
        case 252: return ICO_MSG3;break;
        case 253: return ICO_MSG4;break;

        case 254: return ICO_EXIT;break;
        case 255: return ICO_DESTROY;break;
        default: return 0;
    }
}

void do_event(unsigned char e){


    switch(e){

        // PETITS ENEMIES
        case 0:new_boulder();break;// BIG BOULDER
        case 1:new_boulder();break;// SMALL BOULDER
        case 2:new_fishship();break;//
        case 3:new_meduse();break;// MEDUSE
        case 4:new_jellyfish();break;//
        case 5:new_crab();break;//
        case 6:new_roto();break;//
        case 7:new_doubleshot();break;//
        case 8:new_cockpitbee();break;//
        case 9:new_larve();break;//

        //MOYENS
        case 10:new_meduse();break;// MEDUSE
        case 11:new_friend();break;// FRIENDY
        case 12:new_canon_up();break;// CANON EN HAUT
        case 13:new_canon_down();break;// CANON EN BAS
        case 14:new_bee();break;//
        case 15:new_rocket();break;//
        case 16:new_squartron();break;//
        case 17:new_rond();break;//
        case 18:new_merdouille();break;//

        // GROS
        case 19:new_mine();break;
        case 20:new_crevette();break;
        case 21:new_machoire();break;
        case 22:new_fly();break;
        case 23:new_invader();break;

        //SPECIAUX
        case 30:new_generator();break;

        // BONUSES
        //case 100: return OPTION00;break;
        //case 101: return BONUS00;break;

        // EVENTS //
        case 128: scroll_speed_plus();break;
        case 129: scroll_speed_minus();break;

        case 240: if(level.bg_r < 63)level.bg_r++; update_palette(); break;//bg red++
        case 241: if(level.bg_g < 63)level.bg_g++; update_palette(); break;//bg green++
        case 242: if(level.bg_b < 63)level.bg_b++; update_palette(); break;//bg blue++
        case 243: if(level.bg_r > 0) level.bg_r--; update_palette(); break;//bg red--
        case 244: if(level.bg_g > 0) level.bg_g--; update_palette(); break;//bg green--
        case 245: if(level.bg_b > 0) level.bg_b--; update_palette(); break;//bg blue--

        case 250: inform(level.message1);break;// MESSAGE1
        case 251: inform(level.message2);break;// MESSAGE2
        case 252: inform(level.message3);break;// MESSAGE3
        case 253: inform(level.message4);break;// MESSAGE4

        case 254: end_level();break;

        default:
            //do nothing
            break;

    }

}
