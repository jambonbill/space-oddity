
#ifndef _HISC_H_
#define _HISC_H_

#include "allegro.h"

// highscore list length
#define MAX_SCORES      16

// a hiscore post
typedef struct {
   char name[10];
   unsigned int score;
} Thisc;


// functions
void destroy_hisc_table(Thisc *);
int qualify_hisc_table(Thisc *table, Thisc post);
void sort_hisc_table(Thisc *table);
void enter_hisc_table(Thisc *table, Thisc post);
void reset_hisc_table(Thisc *table, char *name, int hi, int lo);
int load_hisc_table(Thisc *table, PACKFILE *fp);
void save_hisc_table(Thisc *table, PACKFILE *fp);

//void draw_hisc_post(Thisc *table, BITMAP *bmp, FONT *fnt, int x, int y, int color, int show_level);
//void draw_hisc_table(Thisc *table, BITMAP *bmp, FONT *fnt, int x, int y, int color, int show_level);
void draw_hisc_post(Thisc *table, int x, int y);
void draw_hisc_table(Thisc *table, int x, int y);

#endif
