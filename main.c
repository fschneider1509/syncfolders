/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "consoleprint.h"

/* to do:
 * - free() after malloc() with memcpy() in comparefolders.c
 * - check success of malloc() with if(...)
 * - search for malloc() without free()
 * - check and remove trailing slashes in filepath's
 * - replace more '=' by memcpy()
 * - start writing the gui  
 */

/*functions*/

/*main*/
int main(int argc, char *argv[])
{
	
	if( argc == 3 )
	{
		/*prepare folder A*/
		folderst folder_a;
		/*reset folder and set attributes*/
		reset_folder( &folder_a );
		set_root_folder_attributes( &folder_a, argv[1] );

		read_folder ( argv[1], &folder_a );

		/*prepare folder B*/
		folderst folder_b;
		/*reset folder and set attributes*/
		reset_folder( &folder_b );
		set_root_folder_attributes( &folder_b, argv[2] );

		read_folder ( argv[2], &folder_b );

	
		/*compare the folders*/
		init_compare( &folder_a, &folder_b);

		return 0;
	}
	else
		print_usage();

}
