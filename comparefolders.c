/*includes*/
#include "comparefolders.h"

int find_folder_in_list( folderst *pdestfolder, folderst *psearchfolder )
{
	/*variables*/
	unsigned int i = 0;
	int comp_a = 0;
	int comp_b = 0;



	for( i = 0; i < (*psearchfolder).numfolders; i++ )
	{
		/*compare:
		 * - foldername
		 * - folderlayer
		 * - rootpath
		 */

		/*check if the strings are equal*/
		comp_a = strcmp( (*pdestfolder).foldername, (*psearchfolder).folderlist[i].foldername );
		comp_b = strcmp( (*pdestfolder).rootpath, (*psearchfolder).folderlist[i].rootpath );


		if( comp_a == 0 && comp_b == 0	)
		{
			return i;
		}
	}

	return -1;
}

int find_file_in_list( filest *pfile, folderst *pfolder )
{
	/*variables*/
	unsigned int i = 0;
	int comp = 0;

	for( i = 0; i < (*pfolder).numfiles; i++ )
	{
		/*check if strings are euqal*/
		comp = strcmp( (*pfile).filename, (*pfolder).filelist[i].filename );
		if( comp == 0 )
		{
			return i;
		}
	}

	return -1;
}

int compare_files( filest *pfile_a, filest *pfile_b )
{
	/* return codes:
	 * 0: files are equal
	 * 1: copy fileA to fileB
	 * 2: copy fileB to fileA
	 * 3: ask user
	 */
	if( (*pfile_a).filesize == (*pfile_b).filesize &&
	    (*pfile_a).changedate == (*pfile_b).changedate )
			return 0;
	else if( (*pfile_a).filesize > (*pfile_b).filesize &&
	         (*pfile_a).changedate > (*pfile_b).changedate )
			return 1;
	else if( (*pfile_a).filesize < (*pfile_b).filesize &&
	         (*pfile_a).changedate < (*pfile_b).changedate )
			return 2;
	else
		return 3;
}

int copy_pathes( folderst *pfolder, char *ppath, char *proot )
{
	/*variables*/
	size_t pathlen;
	size_t rootlen;

	/*get length of string*/
	pathlen = strlen( ppath );
	rootlen = strlen( proot );

	/*allocate space for memory*/
	(*pfolder).foldername = malloc( sizeof(char) * pathlen + 1 );
	(*pfolder).rootpath = malloc( sizeof(char) * rootlen + 1 );

	if( (*pfolder).foldername != NULL && (*pfolder).rootpath != NULL )
	{
		strcpy( (*pfolder).foldername, ppath );
		strcpy( (*pfolder).rootpath, proot );
		return 1;
	}
	else
		return -1;
}

void compare_folders( folderst *pfolder_a, folderst *pfolder_b )
{
	/*pfoldera is the leading folder*/

	/*variables*/
	unsigned int i = 0;
	unsigned int j = 0;
	int iposfile = 0;
	int iposfolder = 0;
	filest *tmpfile;
	folderst *tmpfolder;

	/*first the files*/
	for( i = 0; i < (*pfolder_a).numfiles; i++ )
	{
		iposfile = find_file_in_list( &((*pfolder_a).filelist[i]), pfolder_b );
		if( iposfile > -1 )
		{
			switch( compare_files( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]) ) )
			{
				case 0:
					/*do nothing*/
					break;
				case 1:
					start_copy( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]) );
					break;
				case 2:
					start_copy( &((*pfolder_b).filelist[iposfile]), &((*pfolder_a).filelist[i]) );
					break;
				case 3:
					/*ask user*/
					{
						switch( ask_user( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]) ) )
						{
						case 1:
							start_copy( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]) );
							break;
						case 2:
							start_copy( &((*pfolder_b).filelist[iposfile]), &((*pfolder_a).filelist[i]) );
							break;
						default:
							/*do nothing*/
							break;
						}
					}
					break;
			}
		}
		else
		{
			/*file was not found;
			 *copy file from foldera to folderb;
			 * -> create new file and append it to the file list
			 *    of the current folder
			 */

			/*make a copy of the current file form foldera*/
			//tmpfile = (*pfolder_a).filelist[i];
			tmpfile = malloc( sizeof( (*pfolder_a).filelist[i] ) * 1 );

			if( tmpfile != NULL )
			{
				memmove( (filest*) tmpfile, (filest*) &((*pfolder_a).filelist[i]), sizeof( (*pfolder_a).filelist[i] ) );

				/*create new filepath*/
				(*tmpfile).filepath = build_path( (*pfolder_b).folderpath, (*pfolder_a).filelist[i].filename );

				/*append file to list*/
				if( append_file_to_list( pfolder_b, tmpfile ) == 1 )
				{
					/*create shadowfile*/
					if( create_shadow_file( (*tmpfile).filepath ) == 1 )
						/*copy file on disk*/
						start_copy( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[(*pfolder_b).numfiles-1]) );
					else
						print_msg( "file could not be copied", (*pfolder_a).filelist[i].filepath, 2 );
				}
			}
			else
				print_msg( "temporary file could not be created", (*pfolder_a).filelist[i].filename, 2 );
		}
	}

	/*copy folders*/
	for( j = 0; j < (*pfolder_a).numfolders; j++ )
	{
		/*search for current folder in list of folderb*/
		iposfolder = find_folder_in_list( &((*pfolder_a).folderlist[j]), pfolder_b );

		if( iposfolder != -1 )
		{
			/*start recursion*/
			compare_folders( &((*pfolder_a).folderlist[j]), &((*pfolder_b).folderlist[iposfolder]) );
		}
		else
		{
			/*copy folder*/
			tmpfolder = malloc( sizeof((*pfolder_a).folderlist[j]) * 1 );

			if( tmpfolder != NULL )
			{
				reset_lists( tmpfolder );
				(*tmpfolder).folderpath = build_path( (*pfolder_b).folderpath, (*pfolder_a).folderlist[j].foldername );
				/*set folder and rootpath*/
				copy_pathes( tmpfolder, (*pfolder_a).folderlist[j].foldername, (*pfolder_a).folderlist[j].rootpath );
				/*set layer*/
				(*tmpfolder).folderlayer = (*pfolder_a).folderlist[j].folderlayer;

				if( append_sub_folder_to_list( pfolder_b, tmpfolder ) == 1 )
				{
					copy_folder_on_disk( (*tmpfolder).folderpath );
					/*start recursion to copy the rest of the files*/
					compare_folders( &((*pfolder_a).folderlist[j]), &((*pfolder_b).folderlist[(*pfolder_b).numfolders-1]) );
				}
				else
					print_msg( "folder could not be created", (*pfolder_a).folderlist[j].rootpath, 2 );
			}
			else
				print_msg( "temporary folder could not be created (malloc)", (*pfolder_a).folderlist[j].foldername , 2);
		}
	}
}

int check_root_folders( char *ppath_a, char *ppath_b )
{
	if( strcmp( get_root_folder( ppath_a ), get_root_folder( ppath_b ) ) == 0 )
		return 1;
	else
		return -1;
}

void init_compare( folderst *pfolder_a, folderst *pfolder_b )
{
	if( check_root_folders( (*pfolder_a).folderpath, (*pfolder_b).folderpath ) == 1 )
	{
		/*pfoldera is leading*/
		compare_folders( pfolder_a, pfolder_b );
		/*pfolderb is leading*/
		compare_folders( pfolder_b, pfolder_a );

		/*free memory*/
		if( (*pfolder_a).empty == 0 )
			free_sub_folder_list( pfolder_a );

		if( (*pfolder_b).empty == 0 )
			free_sub_folder_list( pfolder_b );
	}
	else
		print_msg( "folders do not have the same root path", "check them", 2 );
}

