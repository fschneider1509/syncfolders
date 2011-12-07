/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "consoleprint.h"

/* to do:
 * - check and remove trailing slashes in filepath's
 * - start writing the gui  
 */

/*functions*/

/*main*/
int main(int argc, char *argv[])
{
	/*variables*/
	char *path_a;
	char *path_b;

	/*check number of arguments*/
	if( argc == 3 )
	{
		path_a = remove_trailing_slash( argv[1] );
		path_b = remove_trailing_slash( argv[2] );

		/*prepare folder A*/
		folderst folder_a;
		/*reset folder and set attributes*/
		reset_folder( &folder_a );
		set_root_folder_attributes( &folder_a, path_a );

		/*read folder a*/
		read_folder ( path_a, &folder_a );

		/*prepare folder B*/
		folderst folder_b;
		/*reset folder and set attributes*/
		reset_folder( &folder_b );
		set_root_folder_attributes( &folder_b, path_b );

		/*read folder b*/
		read_folder ( path_b, &folder_b );

		/*compare the folders*/
		init_compare( &folder_a, &folder_b);

		return 0;
	}
	else
		print_usage();
}
