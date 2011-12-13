/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "consoleprint.h"
#include <gtk/gtk.h>

/* to do:
 * - add menu bar
 */

/*functions*/
/*main_window*/

/*enum for tree columns*/
enum
{
	NAME_COLUMN, /*folder- or filename*/
	FSIZE_COLUMN, /*filesize*/
	CHDATE_COLUMN, /*changedate*/
	EQUAL_COLUMN, /*equality of the file*/
	N_COLUMNS /*number of cols*/
};

void append_files_to_view( GtkTreeStore *pstore, GtkTreeIter *piter, filest *plist, int pnumfiles )
{
	/*variables*/
	int i = 0;

	for( i = 0; i < pnumfiles; i++ )
	{
		gtk_tree_store_set( pstore, piter, NAME_COLUMN, plist[i].filename, FSIZE_COLUMN, plist[i].filesize,
				CHDATE_COLUMN, plist[i].str_changedate, EQUAL_COLUMN, 1, -1 );
	}
}

void add_root_to_treeview( folderst *pfolder, GtkTreeStore *pstore )
{
	/*variables*/
	GtkTreeIter additer;

	gtk_tree_store_append( pstore, &additer, NULL );

	/*append files*/
}

void add_treeview( GtkWidget *playout, GtkWidget **ptview, char ppos )
{
	/*variables*/
	GtkTreeStore *filetree;


	filetree = gtk_tree_store_new( N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT );

	*ptview = gtk_tree_view_new_with_model( GTK_TREE_MODEL(filetree) );

	gtk_box_pack_end( GTK_BOX(playout), *ptview, FALSE, FALSE, 0 );
}

void add_menubar( GtkWidget *playout )
{
	/*variables*/
	GtkWidget *menubar; /*menu bar*/
	GtkWidget *file_menu; /*file menu*/
	GtkWidget *sync_menu; /*sync menu*/
	GtkWidget *file_top; /*top entry*/
	GtkWidget *sync_top; /*top entry*/
	GtkWidget *sync_entry; /*menu entry for syncing*/
	GtkWidget *file_separator;
	GtkWidget *open_entry; /*menu entry for file open*/
	GtkWidget *exit_entry; /*menu entry for exit application*/

	/*initialize menubar*/
	menubar = gtk_menu_bar_new();
	/*initialize menus*/
	file_menu = gtk_menu_new();
	sync_menu = gtk_menu_new();

	/*set menu labels*/
	/*file menu*/
	file_top = gtk_menu_item_new_with_mnemonic( "_Datei" );
	open_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_OPEN, NULL );
	exit_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_QUIT, NULL );
	/*sync menu*/
	sync_top = gtk_menu_item_new_with_mnemonic( "_Sync" );
	sync_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_COPY, NULL );

	/*initialize separator*/
	file_separator = gtk_separator_menu_item_new();

	/*add menus*/
	/*file menu*/
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(file_top), file_menu );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), open_entry );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), file_separator );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), exit_entry );
	/*sync menu*/
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(sync_top), sync_menu );
	gtk_menu_shell_append( GTK_MENU_SHELL(sync_menu), sync_entry );

	gtk_menu_shell_append( GTK_MENU_SHELL(menubar), file_top );
	gtk_menu_shell_append( GTK_MENU_SHELL(menubar), sync_top );


	/*add bindings to the menu entries*/
	 g_signal_connect( G_OBJECT(exit_entry), "activate", G_CALLBACK(gtk_main_quit), NULL );


	/*pack menu*/
	gtk_box_pack_start( GTK_BOX(playout), menubar, FALSE, FALSE, 0 );


}

void set_main_win_attribs( GtkWidget *pwindow, gchar *ptitle )
{
	/*set attributes of the main-window*/
	gtk_window_set_title (GTK_WINDOW (pwindow), ptitle );
	gtk_container_set_border_width (GTK_CONTAINER (pwindow), 0);
	gtk_window_set_resizable( GTK_WINDOW(pwindow), TRUE );
	gtk_window_set_default_size( GTK_WINDOW(pwindow), 750, 550 );
	gtk_window_set_position( GTK_WINDOW(pwindow), GTK_WIN_POS_CENTER );
}

void start_gtk_gui( void )
{
	/*variables*/
	GtkWidget *main_win;
	GtkWidget *main_layout;
	GtkWidget *main_tree_a;

	/*initialize main window*/
	main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	/*initialize layout*/
	main_layout = gtk_vbox_new( FALSE, 0 );


	/*add layout to main window*/
	gtk_container_add( GTK_CONTAINER(main_win), main_layout );

	/*set properties*/
	set_main_win_attribs( main_win, "syncfolders-gtk" );

	/*add menubar*/
	add_menubar( main_layout );

	/*add treeview*/
	add_treeview( main_layout, &main_tree_a, 'l' );

	g_signal_connect (main_win, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	gtk_widget_show_all (main_win);
}

int main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	/*start the gtk-gui*/
	start_gtk_gui();


	/*gtk main loop*/
	gtk_main ();

	return 0;
}

