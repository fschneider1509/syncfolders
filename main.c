/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "consoleprint.h"
#include "gtk_gui.h"

/* to do:
  * - write sync dialog
 * - write "README"
 * - pack gui stuff into own module
 */

/*functions*/

int main( int argc, char *argv[] )
{
	gtk_init (&argc, &argv);

	/*start the gtk-gui*/
	start_gtk_gui();


	/*gtk main loop*/
	gtk_main ();

	return 0;
}

