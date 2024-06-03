#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED


void do_event(unsigned char e);
int get_event_sprite(unsigned char event);
const char event_table[256][16];

#endif // EVENTS_H_INCLUDED
