/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"

/* to do:
 * - free's
 * - one folder is empty = Speicherzugriffsfehler!
 */

/*functions*/

/*main*/
int main(int argc, char *argv[])
{
	/*variables*/
	char *path_a = argv[1];
	char *path_b = argv[2];

	/*prepare folder A*/
	folderst folder_a;
	/*reset folder and set attributes*/
	reset_folder( &folder_a );
	set_root_folder_attributes( &folder_a, path_a );

	read_folder ( path_a, &folder_a );
	
	/*prepare folder B*/
	folderst folder_b;
	/*reset folder and set attributes*/
	reset_folder( &folder_b );
	set_root_folder_attributes( &folder_b, path_b );
	
	read_folder ( path_b, &folder_b );


	/*compare the folders*/
	init_compare( &folder_a, &folder_b);

	return 0;
}
