#ifndef READFOLDER_H
#define READFOLDER_H

/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

/*structs*/
struct filestruct{
	char *filename;
	char *filepath;
	char *rootpath;
	unsigned int filesize;
	time_t changedate;
	char *str_changedate;
};
typedef struct filestruct filest;

struct folderstruct{
	char *foldername;
	char *folderpath;
	char *rootpath;
	struct folderstruct *folderlist;
	struct filestruct *filelist;
	unsigned int numfolders;
	unsigned int numfiles;
	unsigned int folderlayer;
	unsigned int empty;
};
typedef struct folderstruct folderst;

/*include it later because it needs the structs*/
#include "consoleprint.h"

/*function prototypes*/
extern void reset_folder( folderst* );
extern void reset_file( filest* );
extern char *remove_trailing_newline( char* );
extern char *build_path( char*, char* );
extern int is_hidden( char* );
extern int check_type( struct stat*, char* );
extern unsigned int get_file_size( struct stat* );
extern size_t get_change_date( struct stat* );
extern int append_sub_folder_to_list( folderst*, folderst* );
extern int append_file_to_list( folderst*, filest* );
/*extern void print_file_struct( folderst* );
extern void printf_folder_struct( folderst* );*/
extern char *get_root_folder( char* );
extern void set_folder_attributes( folderst*, folderst* , char* , char* );
extern void set_file_attributes( filest*, char*, struct stat*, char*, folderst* );
extern int read_folder( char*, folderst* );
extern void free_file_list( folderst* );
extern void free_sub_folder_list( folderst* );
extern char *remove_trailing_slash( char* );

#endif /*READFOLDER_H*/
