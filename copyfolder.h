#ifndef COPYFOLDER_H
#define COPYFOLDER_H

/*includes*/
#include <sys/types.h>
#include <sys/stat.h>
#include "readfolder.h"

/*given rights of mkdir()*/
#define MODE 0777

/*prototypes*/
extern int copy_folder_on_disk( char* );
extern void reset_lists( folderst* );

#endif /*COPYFOLDER_H*/
