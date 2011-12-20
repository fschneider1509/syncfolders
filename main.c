/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "consoleprint.h"
#include <gtk/gtk.h>

/* to do:
 * - remove "Open" form menu "File"
 * - add logo to gui
 * - add labels for the folders
 * - Add "sync" button
 * - pack gui stuff into own module
 */

/*functions*/
/*main_window*/

/*defintion of the window size (startup)*/
#define WIDTH 850
#define HEIGHT 550

/*enum for treeview columns*/
enum
{
	PIC_COLUMN, /*symbol*/
	NAME_COLUMN, /*folder- or filename*/
	FSIZE_COLUMN, /*filesize*/
	CHDATE_COLUMN, /*changedate*/
	EQUAL_COLUMN, /*equality of the file*/
	N_COLUMNS /*number of cols*/
};

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
	gtk_window_set_default_size( GTK_WINDOW(pwindow), WIDTH, HEIGHT );
	gtk_window_set_position( GTK_WINDOW(pwindow), GTK_WIN_POS_CENTER );
}

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

	gtk_tree_store_append( pstore, &additer, pparent );

	/*load file icon*/
	treeicon = gtk_icon_theme_load_icon( curtheme, "document", 16, GTK_ICON_LOOKUP_FORCE_SVG, NULL );
	/*append files to treeview*/
	append_files_to_treeview( pstore, &additer, pparent,(*pfolder).filelist, (*pfolder).numfiles, treeicon );

	/*load folder icon*/
	treeicon = gtk_icon_theme_load_icon( curtheme, "folder", 16, GTK_ICON_LOOKUP_FORCE_SVG, NULL );

	for( i = 0; i < (*pfolder).numfolders; i++ )
	{

		gtk_tree_store_set( pstore, &additer,
							PIC_COLUMN, treeicon,
							NAME_COLUMN, (*pfolder).folderlist[i].foldername,
							FSIZE_COLUMN, 0,
							CHDATE_COLUMN, "-",
							EQUAL_COLUMN, "1", -1 );

		/*append folders (recursively)*/
		add_folder_to_treeview( &((*pfolder).folderlist[i]), pstore, &additer );

		if( i < (*pfolder).numfolders -1 )
			gtk_tree_store_append( pstore, &additer, pparent );

	}
}

GtkTreeStore *add_treeview( GtkWidget *playout, GtkWidget **ptview )
{
	/*variables*/
	GtkTreeStore *filetree;
	GtkCellRenderer *treerenderer;
	GtkTreeViewColumn *piccolumn;
	GtkTreeViewColumn *namecolumn;
	GtkTreeViewColumn *sizecolumn;
	GtkTreeViewColumn *chdatecolumn;
	GtkTreeViewColumn *equalcolumn;
	GtkWidget *scrollarea;


	/*add scrollarea for scrolling in the treewidget*/
	scrollarea = gtk_scrolled_window_new( NULL, NULL );
	gtk_widget_set_size_request( GTK_WIDGET(scrollarea), 440, 350 );

	filetree = gtk_tree_store_new( N_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING );

	*ptview = gtk_tree_view_new_with_model( GTK_TREE_MODEL(filetree) );

	/*define columns*/
	treerenderer = gtk_cell_renderer_pixbuf_new();
	namecolumn = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title( namecolumn, "Name" );
	gtk_tree_view_column_pack_start( namecolumn, treerenderer, FALSE );
	gtk_tree_view_column_set_attributes( namecolumn, treerenderer, "pixbuf", PIC_COLUMN, NULL );
	treerenderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start( namecolumn, treerenderer, TRUE );
	gtk_tree_view_column_set_attributes( namecolumn, treerenderer, "text", NAME_COLUMN, NULL );

	sizecolumn = gtk_tree_view_column_new_with_attributes( "Bytes", treerenderer, "text", FSIZE_COLUMN, NULL );
	chdatecolumn = gtk_tree_view_column_new_with_attributes( "Changedate", treerenderer, "text", CHDATE_COLUMN, NULL );
	equalcolumn = gtk_tree_view_column_new_with_attributes( "Equal?", treerenderer, "text", EQUAL_COLUMN, NULL );

	/*append columns*/
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), namecolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), sizecolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), chdatecolumn );
	gtk_tree_view_append_column( GTK_TREE_VIEW(*ptview), equalcolumn );


	gtk_container_add( GTK_CONTAINER(scrollarea), *ptview );
	gtk_box_pack_start( GTK_BOX(playout), scrollarea, TRUE, TRUE, 4 );

	return filetree;
}

/*callback functions*/
void button_open_clicked( GtkButton *pbtn, gpointer data)
{
	/*variables*/
	GtkWidget *fileopen;
	char *folder_path;
	folderst folder;
	GtkTreeStore *tmp = (GtkTreeStore*) data;

	/*create fileopen dialog*/
	fileopen = gtk_file_chooser_dialog_new( "Verzeichnis öffnen", NULL
			, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );

	if( gtk_dialog_run (GTK_DIALOG (fileopen)) == GTK_RESPONSE_ACCEPT )
	{
	    /*save return path*/
		folder_path = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (fileopen) );

	    /*read folder a*/
	    reset_folder( &folder );
	    set_root_folder_attributes( &folder, folder_path );
	    read_folder( folder_path , &folder );

	    /*add folder to treeview; clear before adding*/
	    if( GTK_IS_TREE_STORE(tmp) )
	    {
	    	gtk_tree_store_clear( tmp );
	    	add_folder_to_treeview( &folder, tmp, NULL );
	    }
	    g_print( "%s\n", folder_path );

	    /*free the string*/
	    g_free (folder_path);
	}
	/*destroy the widget*/
	gtk_widget_destroy (fileopen);
}

void start_gtk_gui( void )
{
	/*variables*/
	GtkWidget *main_win;
	GtkWidget *menubar_layout;
	GtkWidget *tree_layout;
	GtkWidget *layout_a;
	GtkWidget *layout_b;
	GtkWidget *main_tree_a;
	GtkWidget *main_tree_b;
	GtkTreeStore *viewright;
	GtkTreeStore *viewleft;
	GtkWidget *btn_open_a;
	GtkWidget *btn_open_b;

	/*initialize main window*/
	main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	/*initialize layout*/
	menubar_layout = gtk_vbox_new( FALSE, 0 );
	layout_a = gtk_vbox_new( FALSE, 0 );
	layout_b = gtk_vbox_new( FALSE, 0 );
	tree_layout = gtk_hbox_new( TRUE, 4 );

	/*initialize buttons*/
	btn_open_a = gtk_button_new_from_stock( GTK_STOCK_OPEN );
	btn_open_b = gtk_button_new_from_stock( GTK_STOCK_OPEN );


	/*add layout to main window*/
	gtk_container_add( GTK_CONTAINER(main_win), menubar_layout );


	/*set properties*/
	set_main_win_attributes( main_win, "syncfolders-gtk" );

	/*add menubar*/
	add_menubar_to_main_win( menubar_layout );

	/*pack Layouts*/
	gtk_box_pack_start( GTK_BOX(menubar_layout), tree_layout, TRUE, TRUE, 5 );
	gtk_box_pack_start( GTK_BOX(tree_layout), layout_a, TRUE, TRUE, 5 );
	gtk_box_pack_start( GTK_BOX(tree_layout), layout_b, TRUE, TRUE, 5 );

	/*pack buttons*/
	gtk_box_pack_start( GTK_BOX(layout_a), btn_open_a, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(layout_b), btn_open_b, FALSE, FALSE, 3 );


	/*add treeview and get back the tree store*/
	viewleft = add_treeview( layout_a, &main_tree_a );
	viewright = add_treeview( layout_b, &main_tree_b );

	/*add signals to buttons*/
	g_signal_connect( btn_open_a, "clicked", G_CALLBACK(button_open_clicked), viewleft );
	g_signal_connect( btn_open_b, "clicked", G_CALLBACK(button_open_clicked), viewright );
	g_signal_connect (main_win, "destroy", G_CALLBACK(gtk_main_quit), NULL );

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

