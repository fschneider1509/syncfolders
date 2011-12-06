/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"

/* to do:
 * - free's
 * - check for the same root folder at start
 * - one folder is empty = Speicherzugriffsfehler!
 */

/*functions*/

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
	
	/*prepare folder B*/
	folderst folderb;
	/*fill pfolder and set attributes*/
	reset_folder( &folderb );
	folderb.foldername = get_root_folder( pathb );

	folderb.folderpath = pathb;
	folderb.rootpath = get_root_folder( pathb );
	
	read_folder ( pathb, &folderb );


	/*compare the folders*/
	init_compare( &foldera, &folderb);

	return 0;
}
