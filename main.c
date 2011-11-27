/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "copyfile.h"

#include <time.h>

/*functions*/

/*comparefolders*/
int find_file_in_list( filest *pfile, folderst *pfolder )
{
	/*variables*/
	int i = 0;

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

void print_copy_activity( filest *pfilea, filest *pfileb )
{
	/*print copy information to the console (or stdout)*/
	fprintf( stdout, "\tCopy %s (%d Bytes, last change: %s)\n", (*pfilea).filepath, (*pfilea).filesize, ctime( &((*pfilea).changedate) ) );
	fprintf( stdout, "\tto\n" );
	fprintf( stdout, "\t%s (%d Bytes, last change: %s)\n", (*pfileb).filepath, (*pfileb).filesize, ctime( &((*pfileb).changedate) ) );
	fprintf( stdout, "\tCopy state: ");
}

void print_ok( void )
{
	fprintf( stdout, "...Ok\n" );
}

void print_fail( void )
{
	fprintf( stdout, "...Fail\n" );
}

int ask_user( char *pfilea, char *pfileb )
{
	/* return codes:
	 * 1: copy file a to file b
	 * 2: copy file b to file a
	 * 3: do nothing
	 */

	/*variables*/
	int answer = 0;

	return answer;
}

void compare_folders( folderst *pfoldera, folderst *pfolderb )
{
	/*pfoldera is the leading folder*/
	
	/*variables*/
	int i = 0;
	int ipos = 0;
	
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
					print_copy_activity( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[ipos]) );
					if( copy_file_on_disk( &((*pfoldera).filelist[i]), &((*pfolderb).filelist[ipos]) ) == 1 )
						print_ok();
					else
						print_fail();
					break;
				case 2:
					print_copy_activity( &((*pfolderb).filelist[ipos]), &((*pfoldera).filelist[i]) );
					if( copy_file_on_disk( &((*pfolderb).filelist[ipos]), &((*pfoldera).filelist[i]) ) == 1 )
						print_ok();
					else
						print_fail();
					break;
				case 3:
					/*ask user*/
					break;
			}
		}
		else
		{
			/*copy file from foldera to folderb*/
		}
	}
}

/*main*/
int main(int argc, char *argv[])
{
	/*variables*/
	//char *patha = argv[1];
	//char *pathb = argv[2];
	char *patha = "/Users/fabi/temp/folderA";
	char *pathb = "/Users/fabi/temp/subsetB/folderA";

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
