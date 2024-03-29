/*includes*/
#include "comparefolders.h"

void thread_wrapper_msg( thread_wrap *pdata )
{
	/*show the that the copy process is ready*/
	show_msg_dlg( pdata->msg, pdata->add, pdata->type, pdata->data->parent );
}

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

	for( i = 0; i < pfolder->numfiles; i++ )
	{
		/*check if strings are euqal*/
		comp = strcmp( (*pfile).filename, pfolder->filelist[i].filename );
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
	pfolder->foldername = malloc( sizeof(char) * pathlen + 1 );
	pfolder->rootpath = malloc( sizeof(char) * rootlen + 1 );

	if( pfolder->foldername != NULL && pfolder->rootpath != NULL )
	{
		strcpy( pfolder->foldername, ppath );
		strcpy( pfolder->rootpath, proot );
		return 1;
	}
	else
		return -1;
}

void compare_folders( folderst *pfolder_a, folderst *pfolder_b, GtkProgressBar *pbar, issue_list *pissues )
{
	/*pfoldera is the leading folder*/

	/*variables*/
	unsigned int i = 0;
	unsigned int j = 0;
	int iposfile = 0;
	int iposfolder = 0;
	filest *tmpfile;
	folderst *tmpfolder;
	msg_issue tmp_issue;

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
					start_copy( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]), pbar, pissues );
					break;
				case 2:
					start_copy( &((*pfolder_b).filelist[iposfile]), &((*pfolder_a).filelist[i]), pbar, pissues );
					break;
				case 3:
					/*ask user*/
					{
						switch( ask_user( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]) ) )
						{
						case 1:
							start_copy( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[iposfile]), pbar, pissues );
							break;
						case 2:
							start_copy( &((*pfolder_b).filelist[iposfile]), &((*pfolder_a).filelist[i]), pbar, pissues );
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

			tmpfile = malloc( sizeof( (*pfolder_a).filelist[i] ) * 1 );

			if( tmpfile != NULL )
			{
				memmove( (filest*) tmpfile, (filest*) &((*pfolder_a).filelist[i]), sizeof( (*pfolder_a).filelist[i] ) );

				/*create new filepath*/
				(*tmpfile).filepath = build_path( (*pfolder_b).folderpath, (*pfolder_a).filelist[i].filename );

				/*append file to list*/
				if( append_file_to_list( pfolder_b, tmpfile, pissues ) == 1 )
				{
					/*create shadowfile*/
					if( create_shadow_file( (*tmpfile).filepath ) == 1 )
						/*copy file on disk*/
						start_copy( &((*pfolder_a).filelist[i]), &((*pfolder_b).filelist[(*pfolder_b).numfiles-1]), pbar, pissues );
					else
					{
						/*print_msg( "file could not be copied", (*pfolder_a).filelist[i].filepath, 2 );*/
						issue_set_type( &tmp_issue, "Fehler" );
						issue_set_name( &tmp_issue, (*pfolder_a).filelist[i].filepath );
						issue_set_msg( &tmp_issue, "Datei konnte nicht kopiert werden" );
						append_issue_to_list( &tmp_issue, pissues );
					}
				}
			}
			else
			{
				/*print_msg( "temporary file could not be created", (*pfolder_a).filelist[i].filename, 2 );*/
				issue_set_type( &tmp_issue, "Fehler" );
				issue_set_name( &tmp_issue, (*pfolder_a).filelist[i].filename );
				issue_set_msg( &tmp_issue, "Temporäredatei konnte nicht angelegt werden." );
				append_issue_to_list( &tmp_issue, pissues );
			}
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
			compare_folders( &((*pfolder_a).folderlist[j]), &((*pfolder_b).folderlist[iposfolder]), pbar, pissues );
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

				if( append_sub_folder_to_list( pfolder_b, tmpfolder, pissues ) == 1 )
				{
					copy_folder_on_disk( (*tmpfolder).folderpath );
					/*start recursion to copy the rest of the files*/
					compare_folders( &((*pfolder_a).folderlist[j]), &((*pfolder_b).folderlist[(*pfolder_b).numfolders-1]), pbar, pissues );
				}
				else
				{
					/*print_msg( "folder could not be created", (*pfolder_a).folderlist[j].rootpath, 2 );*/
					issue_set_type( &tmp_issue, "Fehler" );
					issue_set_name( &tmp_issue, (*pfolder_a).folderlist[i].rootpath );
					issue_set_msg( &tmp_issue, "Ordner konnte nicht angelegt werden." );
					append_issue_to_list( &tmp_issue, pissues );
				}
			}
			else
			{
				/*print_msg( "temporary folder could not be created (malloc)", (*pfolder_a).folderlist[j].foldername , 2);*/
				issue_set_type( &tmp_issue, "Fehler" );
				issue_set_name( &tmp_issue, (*pfolder_a).folderlist[i].foldername );
				issue_set_msg( &tmp_issue, "Temporärer Ordner konnte nicht angelegt werden (malloc)" );
				append_issue_to_list( &tmp_issue, pissues );
			}
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

gpointer init_compare( sync_folders *pdata, issue_list *pissues )
{
	/*variables*/
	thread_wrap *thread_data;

	thread_data = g_slice_new( thread_wrap );

	thread_data->data = pdata;

	if( check_root_folders( pdata->a->folder->folderpath, pdata->b->folder->folderpath ) == 1 )
	{
		/*pfoldera is leading*/
		compare_folders( pdata->a->folder, pdata->b->folder, pdata->bar, pissues );
		/*pfolderb is leading*/
		compare_folders( pdata->b->folder, pdata->a->folder, pdata->bar, pissues );

		/*free memory*/
		if( pdata->a->folder->empty == 0 )
			free_sub_folder_list( pdata->a->folder );

		if( pdata->b->folder->empty == 0 )
			free_sub_folder_list( pdata->b->folder );

		strcpy( thread_data->msg, "Synchronisation erledit" );
		strcpy( thread_data->add, "" );
		thread_data->type = 1;

	}
	else
	{
		strcpy( thread_data->msg, "Root-Ordner sind nicht identisch." );
		strcpy( thread_data->add, "Bitte Überprüfen!" );
		thread_data->type = 2;
	}

	/*set action after end of the thread*/
	g_idle_add( thread_wrapper_msg, thread_data );

	g_slice_free( thread_wrap, thread_data );
	return NULL;
}

