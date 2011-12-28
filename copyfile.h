#ifndef COPYFILE_H
#define COPYFILE_H

/*includes*/
#include <utime.h>
#include <gtk/gtk.h>
#include "readfolder.h"
#include "consoleprint.h"

/*prototypes*/
extern int set_physical_change_date( time_t*, filest* );
extern int copy_content( FILE*, FILE* );
extern int copy_file_on_disk( filest*, filest* );
extern int create_shadow_file( char* );
extern void start_copy( filest*, filest*, GtkProgressBar* );

#endif /*COPYFILE_H*/
