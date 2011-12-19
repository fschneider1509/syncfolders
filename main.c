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

void add_menubar_to_main_win( GtkWidget *playout )
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

void set_main_win_attributes( GtkWidget *pwindow, gchar *ptitle )
{
	/*set attributes of the main-window*/
	gtk_window_set_title (GTK_WINDOW (pwindow), ptitle );
	gtk_container_set_border_width (GTK_CONTAINER (pwindow), 0);
	gtk_window_set_resizable( GTK_WINDOW(pwindow), TRUE );
	gtk_window_set_default_size( GTK_WINDOW(pwindow), 750, 550 );
	gtk_window_set_position( GTK_WINDOW(pwindow), GTK_WIN_POS_CENTER );
}


/*enum for tree columns*/
enum
{
	PIC_COLUMN, /*symbol*/
	NAME_COLUMN, /*folder- or filename*/
	FSIZE_COLUMN, /*filesize*/
	CHDATE_COLUMN, /*changedate*/
	EQUAL_COLUMN, /*equality of the file*/
	N_COLUMNS /*number of cols*/
};

void append_files_to_treeview( GtkTreeStore *pstore, GtkTreeIter *piter, GtkTreeIter *pparent,filest *plist, int pnumfiles, GdkPixbuf *picon )
{
	/*variables*/
	int i = 0;

	for( i = 0; i < pnumfiles; i++ )
	{
		/*set current entry of the treeview*/
		gtk_tree_store_set( pstore, piter,
				PIC_COLUMN, picon,
				NAME_COLUMN, plist[i].filename,
				FSIZE_COLUMN, plist[i].filesize,
				CHDATE_COLUMN, plist[i].str_changedate,
				EQUAL_COLUMN, "1", -1 );

		/*append remaining files to the list*/
		if( i < pnumfiles - 1 )
			gtk_tree_store_append( pstore, piter, pparent );
	}
}

void add_folder_to_treeview( folderst *pfolder, GtkTreeStore *pstore, GtkTreeIter *pparent )
{
	/*variables*/
	GtkTreeIter additer;
	unsigned int i = 0;
	GdkPixbuf *treeicon;
	GError *picerr; /*error for loading pixbuf*/
	GtkIconTheme *curtheme;

	/*get default gtk theme*/
	curtheme = gtk_icon_theme_get_default();

	/*load file icon*/
	treeicon = gtk_icon_theme_load_icon( curtheme, "document", 16, GTK_ICON_LOOKUP_FORCE_SVG, NULL );

	gtk_tree_store_append( pstore, &additer, pparent );

	append_files_to_treeview( pstore, &additer, pparent,(*pfolder).filelist, (*pfolder).numfiles, treeicon );

	treeicon = gtk_icon_theme_load_icon( curtheme, "folder", 16, GTK_ICON_LOOKUP_FORCE_SVG, NULL );

	for( i = 0; i < (*pfolder).numfolders; i++ )
	{

		gtk_tree_store_set( pstore, &additer,
							PIC_COLUMN, treeicon,
							NAME_COLUMN, (*pfolder).folderlist[i].foldername,
							FSIZE_COLUMN, 0,
							CHDATE_COLUMN, "-",
							EQUAL_COLUMN, "1", -1 );

		/*append files to folder*/
		add_folder_to_treeview( &((*pfolder).folderlist[i]), pstore, &additer );

		if( i < (*pfolder).numfolders -1 )
			gtk_tree_store_append( pstore, &additer, pparent );
	}
}

void add_treeview( GtkWidget *playout, GtkWidget **ptview, char ppos )
{
	/*variables*/
	GtkTreeStore *filetree;
	GtkCellRenderer *treerenderer;
	GtkTreeViewColumn *piccolumn;
	GtkTreeViewColumn *namecolumn;
	GtkTreeViewColumn *sizecolumn;
	GtkTreeViewColumn *chdatecolumn;
	GtkTreeViewColumn *equalcolumn;
	folderst folder_a;

	reset_folder( &folder_a );
	set_root_folder_attributes( &folder_a, "/home/fabi/temp/folderA" );
	read_folder( "/home/fabi/temp/folderA", &folder_a );

	filetree = gtk_tree_store_new( N_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING );

	add_folder_to_treeview( &folder_a, filetree, NULL );

	*ptview = gtk_tree_view_new_with_model( GTK_TREE_MODEL(filetree) );

	/*define columns*/
	treerenderer = gtk_cell_renderer_pixbuf_new();
	piccolumn = gtk_tree_view_column_new_with_attributes( NULL, treerenderer, "pixbuf", PIC_COLUMN, NULL );
	treerenderer = gtk_cell_renderer_text_new();
	namecolumn = gtk_tree_view_column_new_with_attributes( "Name", treerenderer, "text", NAME_COLUMN, NULL );
	sizecolumn = gtk_tree_view_column_new_with_attributes( "Size", treerenderer, "text", FSIZE_COLUMN, NULL );
	chdatecolumn = gtk_tree_view_column_new_with_attributes( "Changedate", treerenderer, "text", CHDATE_COLUMN, NULL );
	equalcolumn = gtk_tree_view_column_new_with_attributes( "Equal?", treerenderer, "text", EQUAL_COLUMN, NULL );

	/*append columns*/
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), piccolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), namecolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), sizecolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), chdatecolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), equalcolumn );

	gtk_box_pack_start( GTK_BOX(playout), *ptview, FALSE, FALSE, 0 );
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
	set_main_win_attributes( main_win, "syncfolders-gtk" );

	/*add menubar*/
	add_menubar_to_main_win( main_layout );

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

