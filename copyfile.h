#ifndef COPYFILE_H
#define COPYFILE_H

/*includes*/
#include <utime.h>
#include "readfolder.h"
#include "consoleprint.h"

/*prototypes*/
extern int set_physical_change_date( time_t*, filest* );
extern int copy_content( FILE*, FILE* );
extern int copy_file_on_disk( filest*, filest* );

#endif /*COPYFILE_H*/
