/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "copyfile.h"

/*functions*/

/*comparefolders*/
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

void start_copy( filest *pfilea, filest *pfileb )
{
	print_copy_activity( pfilea, pfileb );
	if( copy_file_on_disk( pfilea, pfileb ) == 1 )
		print_ok();
	else
		print_fail();
}

int create_shadow_file( char *pfilepath )
{
	/*variables*/
	FILE *tmp;

	tmp = fopen( pfilepath, "w+" );

	if( tmp != NULL )
	{
		fclose( tmp );
		return 1;
	}
	else
		return -1;
}

void compare_folders( folderst *pfoldera, folderst *pfolderb )
{
	/*pfoldera is the leading folder*/
	
	/*variables*/
	unsigned int i = 0;
	int ipos = 0;
	filest tmpfile;
	
	/*first the files*/
	for( i = 0; i < (*pfoldera).numfiles; i++ )
	{
		ipos = find_file_in_list( &((*pfoldera).filelist[i]), pfolderb );
		if( ipos > -1 )
		{
			switch( compare_files( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[ipos]) ) )
			{
				case 0:
					/*do nothing*/
					break;
				case 1:
					start_copy( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[ipos]) );
					break;
				case 2:
					start_copy( &((*pfolderb).filelist[ipos]), &((*pfoldera).filelist[i]) );
					break;
				case 3:
					/*ask user*/
					{
						switch( ask_user( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[ipos]) ) )
						{
						case 1:
							start_copy( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[ipos]) );
							break;
						case 2:
							start_copy( &((*pfolderb).filelist[ipos]), &((*pfoldera).filelist[i]) );
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
}

/*main*/
int main(int argc, char *argv[])
{
	/*variables*/
	//char *patha = argv[1];
	//char *pathb = argv[2];
	char *patha = "/home/fabi/temp/folderA";
	char *pathb = "/home/fabi/temp/subsetB/folderA";

	/*prepare folder A*/
	folderst foldera;
	/*fill pfolder and set attributes*/
	reset_folder( &foldera );
	foldera.foldername = get_root_folder( patha );

	foldera.folderpath = patha;
	foldera.rootpath = get_root_folder( patha );
	
	read_folder ( patha, &foldera );
	/*fprintf( stdout, "content folder %s\n", foldera.folderpath );
	print_file_struct( &foldera );
	printf("folders:\n");
	printf_folder_struct( &foldera );

	fprintf( stdout, "\n\n\n" );*/
	
	/*prepare folder B*/
	folderst folderb;
	/*fill pfolder and set attributes*/
	reset_folder( &folderb );
	folderb.foldername = get_root_folder( pathb );

	folderb.folderpath = pathb;
	folderb.rootpath = get_root_folder( pathb );
	
	read_folder ( pathb, &folderb );
	/*fprintf( stdout ,"content folder %s\n", folderb.folderpath );
	print_file_struct( &folderb );
	printf("folders:\n");
	printf_folder_struct( &folderb );*/

	/*compare the folders*/
	compare_folders( &foldera, &folderb );

	/*free memory*/
	free_sub_folder_list( &foldera );
	free_sub_folder_list( &folderb );
	
	return 0;
}
