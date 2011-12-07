#ifndef COMPAREFOLDERS_H
#define COMPAREFOLDERS_H

/*includes*/
#include <string.h>
#include "readfolder.h"
#include "consoleprint.h"
#include "copyfile.h"
#include "copyfolder.h"

/*prototypes*/
extern int find_folder_in_list( folderst *, folderst* );
extern int find_file_in_list( filest*, folderst* );
extern int compare_files( filest*, filest* );
extern void compare_folders( folderst*, folderst* );
extern int check_root_folders( char*, char* );
extern void init_compare( folderst*, folderst* );

#endif /*COMPAREFOLDERS_H*/
