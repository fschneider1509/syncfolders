#ifndef COPYFILE_H
#define COPYFILE_H

/*includes*/
#include <utime.h>
#include <time.h>
#include "readfolder.h"

/*prototypes*/
extern char *remove_trailing_newline( char* );
extern int ask_user( filest*, filest* );
extern void print_copy_activity( filest*, filest* );
extern void print_ok();
extern void print_false();
extern int set_physical_change_date( time_t*, filest* );
extern int copy_content( FILE*, FILE* );
extern int copy_file_on_disk( filest*, filest* );

#endif /*COPYFILE_H*/
