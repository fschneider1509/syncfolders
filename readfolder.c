/*includes*/
#include "readfolder.h"

/*functions*/
void reset_folder( folderst *pfolder )
{
	pfolder->foldername = NULL;
	pfolder->folderpath = NULL;
	pfolder->rootpath = NULL;
	pfolder->numfolders = 0;
	pfolder->numfiles = 0;
	pfolder->folderlayer = 0;
	pfolder->folderlist = NULL;
	pfolder->filelist = NULL;
	pfolder->empty = 1;
}

void reset_file( filest *pfile )
{
	(*pfile).filename = NULL;
	(*pfile).filepath = NULL;
	(*pfile).rootpath = NULL;
	(*pfile).filesize = 0;
	(*pfile).changedate = 0;
}

char *remove_trailing_newline( char *psrc )
{
	/*variables*/
	char *new;
	char *tmp;
	size_t istringlen;

	/*get length of psrc*/
	istringlen = strlen( psrc );

	/*do not add +1 because the '\n' "slot" is free :)*/
	new = malloc( sizeof(char) * istringlen );

	/*save first address of the string*/
	tmp = new;

	if( new != NULL )
	{
		do
		{
			*new++ = *psrc++;
		}while( *psrc != '\n' );
		*new++ = '\0';
		return tmp;
	}
	else
		return NULL;
}

char *build_path(char *ppath, char *pfile)
{
	/*variables*/
	size_t ipathlen;
	size_t ifilelen;
	char *newpath;

	/*get length of the parameters*/
	ipathlen = strlen( ppath );
	ifilelen = strlen( pfile );

	/*allocate memory for the new path-string*/
	newpath = (char*) malloc( sizeof( char ) * (ipathlen + ifilelen + 1) + 1);

	if( newpath != NULL )
	{
		strcpy( newpath, ppath );
		strcat( newpath, "/");/*add slash to path*/
		strcat( newpath, pfile );

		return newpath;		
	}
	else
	{
		/*if memory could not be allocated return NULL*/
		return NULL;
	}
}

int is_hidden_type( char *pname )
{
	/*the subfolders "." and ".." are hidden types*/
	if( strcmp( pname, ".") == 0 || strcmp( pname, ".." ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int check_type( struct stat *pattributes, char *pname )
{
	/*return:
	 *		1: is dir
	 *		2: is file
	 *		-1: is hidden folder type ("." and "..")
	 */
	if( S_ISDIR( pattributes->st_mode ) && !is_hidden_type( pname ) )
	{
		return 1;
	}
	else if( is_hidden_type( pname ) )
	{
		return -1;
	}
	else
	{
	   return 2;
	}
}

unsigned int get_file_size( struct stat *pattributes )
{
	return (*pattributes).st_size;
}

size_t get_change_date( struct stat *pattributes )
{
	return (*pattributes).st_mtime;
}

char *get_root_folder( char *ppath )
{
	/*variables*/
	char *tmp;
	int pathlen;
	char *root;

	pathlen = strlen( ppath );

	tmp = malloc( sizeof(char) * pathlen + 1 );

	if( tmp != NULL )
	{
		/*copy path to temporary string*/
		strcpy( tmp, ppath );

		/*get last occurance of / int the string*/
		root = strrchr( tmp, '/' );

		return root+1;
	}
	else
		return NULL;
}

int append_sub_folder_to_list( folderst *pfolder, folderst *psubfolder, issue_list *issues )
{
	/*allocate memory for a new folder entry in the list*/
	pfolder->folderlist = realloc( pfolder->folderlist, sizeof( folderst ) * (pfolder->numfolders + 1) );

	if( pfolder->folderlist != NULL )
	{
		pfolder->folderlist[pfolder->numfolders] =  *psubfolder;
		pfolder->numfolders++;
		return 1;
	}
	else
	{
		/*print_msg( "appending to list failed", pfolder->foldername, 2 );*/
		show_msg_dlg( "Ordner konnte nicht an Liste angehängt werden:", pfolder->foldername, 2, NULL );
		return -1;
	}	
}

int append_file_to_list( folderst *pfolder, filest *pfile, issue_list *issues )
{
	/*allocate memory for the entry in the list*/
	pfolder->filelist = realloc( pfolder->filelist, sizeof( filest ) * (pfolder->numfiles + 1) );

	if( pfolder->filelist != NULL )
	{
		pfolder->filelist[pfolder->numfiles] = *pfile;
		pfolder->numfiles++;
		return 1;
	}
	else
	{
		/*print_msg( "appending to list failed", pfolder->foldername, 2 );*/
		show_msg_dlg( "Datei konnte nicht an Liste angehängt werden:", pfolder->foldername, 2, NULL );
		return -1;
	}	
}

void free_file_list( folderst *pfolder )
{
	free( pfolder->filelist );
}

void free_sub_folder_list( folderst *pfolder )
{
	/*variables*/
	unsigned int i;

	for( i = 0; i < pfolder->numfolders; i++ )
	{
		free_sub_folder_list( (&pfolder->folderlist[i]) );		
	}

	if( pfolder->folderlist != NULL && pfolder->numfolders > 0 )
		/*free memory of the current folder list*/
		free( pfolder->folderlist );

	/*free memory of current file list*/
	if( pfolder->filelist != NULL && pfolder->numfiles > 0 )
	{
		free_file_list( pfolder );
	}

	if( strcmp( pfolder->foldername, pfolder->rootpath ) != 0 )
	{
		/*free folder- and rootpath*/
		if( pfolder->folderpath != NULL )
			free( pfolder->folderpath );

		if( pfolder->rootpath != NULL )
			free( pfolder->rootpath );	
	}
}

/*void print_file_struct( folderst *pfolder )
{
	variables
	unsigned int i;

	for( i = 0; i < pfolder->numfiles; i++ )
	{
		fprintf( stdout, "\t\t\t%s\t%d Bytes\n", pfolder->filelist[i].filename, pfolder->filelist[i].filesize );
	}
}*/

/*void printf_folder_struct( folderst *pfolder )
{
	variables
	unsigned int i;

	for( i = 0; i < pfolder->numfolders; i++ )
	{
		print folder path
		fprintf( stdout, "\n\t%s\n", pfolder->folderlist[i].folderpath );
		fprintf( stdout, "\t\tfiles:\n" );
		print file list of the current folder
		print_file_struct( &(pfolder->folderlist[i]) );
		do it recursively for all subfolders
		printf_folder_struct( &(pfolder->folderlist[i]) );
	}
}*/

int check_is_empty( folderst *pfolder )
{
	if( pfolder->folderlist == NULL && pfolder->filelist == NULL &&
			pfolder->numfiles == 0 && pfolder->numfolders == 0 )
		return 1;
	else
		return 0;
}

void set_root_folder_attributes( folderst *pfolder, char *ppath, issue_list *pissues )
{
	/*variables*/
	size_t pathlen;

	pathlen = strlen( ppath );

	pfolder->folderpath = malloc( sizeof( char ) * pathlen + 1 );

	if( pfolder->folderpath != NULL )
		/*set folderpath*/
		strcpy(	pfolder->folderpath, ppath );
	else
		/*print_msg( "could not allocate memory for path", ppath, 2 );*/
		show_msg_dlg( "Speicher für Pfad konnte nicht allokiert werden", ppath, 2, NULL );

	/*set foldername*/
	pfolder->foldername = get_root_folder( ppath );
	/*set root folder*/
	pfolder->rootpath = get_root_folder( ppath );
}

void set_folder_attributes( folderst *subfolder, folderst *rootfolder, char *pname, char *ppath, issue_list *pissues )
{
	/*variables*/
	size_t namelen = 0;

	namelen = strlen( pname );

	(*subfolder).foldername = malloc( sizeof( char ) * namelen + 1 );

	if( (*subfolder).foldername != NULL )
	{
		/*set foldername*/
		strcpy( (*subfolder).foldername, pname );
	}
	else
		/*print_msg( "memory for foldername could not be allocated", ppath, 2 );*/
		show_msg_dlg( "Speicher für Ordnername konnte nicht allokiert werden", ppath, 2, NULL );

	/*set folderlayer*/
	(*subfolder).folderlayer = (*rootfolder).folderlayer + 1;
	/*set folderpath*/
	(*subfolder).folderpath = ppath;
	/*set rootpath*/
	(*subfolder).rootpath = build_path( (*rootfolder).rootpath , pname );
}

void set_file_attributes( filest *pfile, char *pname, struct stat *pfattributes, char *ppath, folderst *rootfolder, issue_list *pissues )
{
	/*variables*/
	size_t namelen = 0;

	namelen = strlen( pname );

	(*pfile).filename = malloc( sizeof(char) * namelen + 1 );

	if( (*pfile).filename != NULL )
	{
		/*set filename*/
		strcpy( (*pfile).filename, pname );
	}
	else
		/*print_msg( "memory for filename could not be allocated", ppath, 2 );*/
		show_msg_dlg( "Speicher für Dateipfad konnte nicht allokiert werden:", ppath, 2, NULL );

	/*get filesize*/
	(*pfile).filesize = get_file_size( pfattributes );
	/*get changedate*/
	(*pfile).changedate = get_change_date( pfattributes );
	(*pfile).str_changedate = remove_trailing_newline( ctime( &((*pfile).changedate ) ) );
	/*set filepath*/
	(*pfile).filepath = ppath;
	/*set rootpath*/
	(*pfile).rootpath = build_path( (*rootfolder).rootpath, ppath );
}

int read_folder( char *ppath, folderst *pfolder, issue_list *pissues )
{
	/*variables*/
	DIR *curdir;
	struct dirent *dirst;
	struct stat fileattributes;
	char *curfl;
	
	/*open dir*/
	curdir = opendir( ppath );

	/*build up a tree for the current folder*/
	if( curdir != NULL )
	{
		/*get folder content*/
		while( (dirst = readdir( curdir )) != NULL )
		{
			/*build new path*/
			curfl = build_path ( ppath, (*dirst).d_name );

			/*get attributes of file or folder*/
			if( stat( curfl, &fileattributes ) == 0 )
			{
				switch( check_type( &fileattributes, (*dirst).d_name ) )
				{
					case 1:
					{
						/*create temp folder struct*/
						folderst subfl;
						reset_folder( &subfl );

						/*set folders attributes*/
						set_folder_attributes( &subfl, pfolder, (*dirst).d_name, curfl, pissues );

						/*read content of the subfolder (recursive)*/
						read_folder ( curfl, &subfl, pissues );

						/*add folder to folder list*/
						append_sub_folder_to_list ( pfolder, &subfl, pissues );
						break;
					}
					case 2:
					{
						/*create temp file struct*/
						filest tmpfl;
						reset_file( &tmpfl );

						/*set file attributes*/
						set_file_attributes( &tmpfl, (*dirst).d_name, &fileattributes, curfl, pfolder, pissues );

						/*add file to file list*/
						append_file_to_list( pfolder, &tmpfl, pissues );
						break;
					}
					case -1:
						/*do nothing, because it's a hidden type folder*/
						break;
					default:
						/*print_msg ( "unknown file", curfl, 2 );*/
						show_msg_dlg( "Unbekannter Dateityp:", curfl, 2, NULL );
						break;
				}
			}
			else
			{
				/*print_msg( "attributes could not be read", curfl, 2 );*/
				show_msg_dlg( "Attribute konnten nicht gelesen werden:", curfl, 1, NULL );
			}

			(*pfolder).empty = check_is_empty( pfolder );
		}
	}
	else
	{
		/*print_msg ( "error opening folder", ppath, 2 );*/
		show_msg_dlg( "Ordner konnte nicht geöffnet werden:", ppath, 2, NULL );
		return -1;
	}
	closedir( curdir );
	return 1;
}

char *remove_trailing_slash( char *ppath )
{
	/*variables*/
	char *tmp;
	char *new;
	unsigned int i = 0;
	size_t pathlen;

	/*get path length*/
	pathlen = strlen( ppath );

	if( ppath[pathlen-1] == '/' )
	{
		/*allocate memory for the new string;
		 * don't need to add + 1, because one slot is free*/
		new = malloc( sizeof(char) * pathlen );

		/*save new pointer*/
		tmp = new;

		if( new != NULL )
		{
			do
			{
				*new++ = *ppath++;
				i++;
			}while( i < (pathlen - 1) );
			*new++ = '\0';
			return tmp;
		}
		else
			return NULL;
	}
	else
		return ppath;
}
