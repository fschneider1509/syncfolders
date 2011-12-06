/*includes*/
#include "comparefolders.h"

int find_folder_in_list( folderst *pdestfolder, folderst *psearchfolder )
{
	/*variables*/
	unsigned int i = 0;

	for( i = 0; i < (*psearchfolder).numfolders; i++ )
	{
		/*compare:
		 * - foldername
		 * - folderlayer
		 * - rootpath
		 */
		if( strcmp( (*pdestfolder).foldername, (*psearchfolder).folderlist[i].foldername ) == 0 &&
			(*pdestfolder).folderlayer == (*psearchfolder).folderlist[i].folderlayer &&
			strcmp( (*pdestfolder).rootpath, (*psearchfolder).folderlist[i].rootpath ) == 0	)
			return i;
		else
			return -1;
	}

	return -1;
}

int find_file_in_list( filest *pfile, folderst *pfolder )
{
	/*variables*/
	unsigned int i = 0;

	for( i = 0; i < (*pfolder).numfiles; i++ )
	{
		if( strcmp( (*pfile).filename, (*pfolder).filelist[i].filename ) == 0 )
			return i;
	}

	return -1;
}

int compare_files( filest *pfile_one, filest *pfile_two )
{
	/* return codes:
	 * 0: files are equal
	 * 1: copy fileA to fileB
	 * 2: copy fileB to fileA
	 * 3: ask user
	 */
	if( (*pfile_one).filesize == (*pfile_two).filesize &&
	    (*pfile_one).changedate == (*pfile_two).changedate )
			return 0;
	else if( (*pfile_one).filesize > (*pfile_two).filesize &&
	         (*pfile_one).changedate > (*pfile_two).changedate )
			return 1;
	else if( (*pfile_one).filesize < (*pfile_two).filesize &&
	         (*pfile_one).changedate < (*pfile_two).changedate )
			return 2;
	else
		return 3;
}

void compare_folders( folderst *pfoldera, folderst *pfolderb )
{
	/*pfoldera is the leading folder*/

	/*variables*/
	unsigned int i = 0;
	unsigned int j = 0;
	int iposfile = 0;
	int iposfolder = 0;
	filest tmpfile;

	/*first the files*/
	for( i = 0; i < (*pfoldera).numfiles; i++ )
	{
		iposfile = find_file_in_list( &((*pfoldera).filelist[i]), pfolderb );
		if( iposfile > -1 )
		{
			switch( compare_files( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[iposfile]) ) )
			{
				case 0:
					/*do nothing*/
					break;
				case 1:
					start_copy( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[iposfile]) );
					break;
				case 2:
					start_copy( &((*pfolderb).filelist[iposfile]), &((*pfoldera).filelist[i]) );
					break;
				case 3:
					/*ask user*/
					{
						switch( ask_user( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[iposfile]) ) )
						{
						case 1:
							start_copy( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[iposfile]) );
							break;
						case 2:
							start_copy( &((*pfolderb).filelist[iposfile]), &((*pfoldera).filelist[i]) );
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
			tmpfile = (*pfoldera).filelist[i];

			/*create new filepath*/
			tmpfile.filepath = build_path( (*pfolderb).folderpath, (*pfoldera).filelist[i].filename );

			/*append file to list*/
			if( append_file_to_list( pfolderb, &tmpfile ) == 1 )
			{
				/*create shadowfile*/
				if( create_shadow_file( tmpfile.filepath ) == 1 )
					/*copy file on disk*/
					start_copy( &((*pfoldera).filelist[i]), &tmpfile );
				else
					print_msg( "file could not be copied", (*pfoldera).filelist[i].filepath, 2 );
			}
		}
	}

	/*copy folders*/
	for( j = 0; j < (*pfoldera).numfolders; j++ )
	{
		/*search for current folder in list of folderb*/
		iposfolder = find_folder_in_list( &((*pfoldera).folderlist[j]), pfolderb );

		if( iposfolder != -1 )
		{
			/*start recursion*/
			compare_folders( &((*pfoldera).folderlist[j]), &((*pfolderb).folderlist[iposfolder]) );
		}
		else
		{
			/*copy folder*/
			folderst tmpfolder = (*pfoldera).folderlist[j];
			reset_lists( &tmpfolder );
			tmpfolder.folderpath = build_path( (*pfolderb).folderpath, (*pfoldera).folderlist[j].foldername );

			if( append_sub_folder_to_list( pfolderb, &tmpfolder ) == 1 )
			{
				copy_folder_on_disk( tmpfolder.folderpath );
				/*start recursion to copy the rest of the files*/
				compare_folders( &((*pfoldera).folderlist[j]), &((*pfolderb).folderlist[(*pfolderb).numfolders-1]) );
			}
			else
				print_msg( "folder could not be created", (*pfoldera).folderlist[j].rootpath, 2 );
		}

	}
}

void init_compare( folderst *pfoldera, folderst *pfolderb )
{
	/*pfoldera is leading*/
	compare_folders( pfoldera, pfolderb );
	/*pfolderb is leading*/
	compare_folders( pfolderb, pfoldera );

	/*free memory*/
	if( (*pfoldera).empty == 0 )
		free_sub_folder_list( pfoldera );
	if( (*pfolderb).empty == 0 )
		free_sub_folder_list( pfolderb );
}

