#ifndef CONSOLEPRINT_H
#define CONSOLEPRINT_H

/*includes*/
#include <stdio.h>

/*prototypes*/
extern int ask_user( filest*, filest* );
extern void print_copy_activity( filest*, filest* );
extern void print_ok();
extern void print_false();
extern void print_msg( char*, char*, int );

#endif /*CONSOLEPRINT_H*/
