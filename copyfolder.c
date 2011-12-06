/*includes*/
#include "copyfolder.h"

int copy_folder_on_disk( char *pfolderpath )
{
	if( mkdir( pfolderpath,MODE ) != -1 )
		return 0;
	else
		return -1;
}

void reset_lists( folderst *pfolder )
{
	(*pfolder).filelist = NULL;
	(*pfolder).folderlist = NULL;
	(*pfolder).numfiles = 0;
	(*pfolder).numfolders = 0;
	(*pfolder).folderpath = NULL;
	(*pfolder).empty = 1;
}
