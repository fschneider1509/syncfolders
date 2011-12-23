/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include "readfolder.h"
#include "comparefolders.h"
#include "consoleprint.h"
#include <gtk/gtk.h>

/* to do:
  * - write sync dialog
 * - write "README"
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

/*struct for callback function "button_open_clicked"*/
typedef struct btn_open
{
	GtkWindow *parent;
	GtkTreeStore *store;
	GtkEntry *entry;
	folderst *folder;
} btn_open_data;

void add_menubar_to_main_win( GtkWidget *playout )
{
	/*variables*/
	GtkWidget *menubar; /*menu bar*/
	GtkWidget *file_menu; /*file menu*/
	GtkWidget *sync_menu; /*sync menu*/
	GtkWidget *file_top; /*top entry*/
	GtkWidget *sync_top; /*top entry*/
	GtkWidget *sync_entry; /*menu entry for syncing*/
	GtkWidget *exit_entry; /*menu entry for exit application*/

	/*initialize menubar*/
	menubar = gtk_menu_bar_new();
	/*initialize menus*/
	file_menu = gtk_menu_new();
	sync_menu = gtk_menu_new();

	/*set menu labels*/
	/*file menu*/
	file_top = gtk_menu_item_new_with_mnemonic( "_Datei" );
	exit_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_QUIT, NULL );
	/*sync menu*/
	sync_top = gtk_menu_item_new_with_mnemonic( "_Sync" );
	sync_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_COPY, NULL );

	/*add menus*/
	/*file menu*/
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(file_top), file_menu );
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

GdkPixbuf *get_icon( gint type )
{
	/* type: 1 is file, 2 is folder*/

	/*variables*/
	GdkPixbuf *tmp;
	GtkIconTheme *curtheme;

	/*get default gtk theme*/
	curtheme = gtk_icon_theme_get_default();

	if( type == 1 )
		/*load file icon*/
		tmp = gtk_icon_theme_load_icon( curtheme, "document", 16, GTK_ICON_LOOKUP_FORCE_SVG, NULL );
	else
		/*load folder icon*/
		tmp = gtk_icon_theme_load_icon( curtheme, "folder", 16, GTK_ICON_LOOKUP_FORCE_SVG, NULL );

	return tmp;
}

void set_main_win_attributes( GtkWidget *pwindow, gchar *ptitle )
{
	/*variables*/
	GdkPixbuf *icon;
	GError *err = NULL;

	/*set attributes of the main-window*/
	gtk_window_set_title (GTK_WINDOW (pwindow), ptitle );
	gtk_container_set_border_width (GTK_CONTAINER (pwindow), 0);
	gtk_window_set_resizable( GTK_WINDOW(pwindow), TRUE );
	gtk_window_set_default_size( GTK_WINDOW(pwindow), WIDTH, HEIGHT );
	gtk_window_set_position( GTK_WINDOW(pwindow), GTK_WIN_POS_CENTER );

	/*set icon*/
	icon = gdk_pixbuf_new_from_file( "logo.png", &err );
	gtk_window_set_icon( GTK_WINDOW(pwindow), icon );
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
	GtkIconTheme *curtheme;

	if( (*pfolder).numfiles > 0 )
	{
		treeicon = get_icon( 1 );
		gtk_tree_store_append( pstore, &additer, pparent );

		/*append files to treeview*/
		append_files_to_treeview( pstore, &additer, pparent, (*pfolder).filelist, (*pfolder).numfiles, treeicon );
	}

	if( (*pfolder).numfolders > 0 )
		treeicon = get_icon( 2 );

	for( i = 0; i < (*pfolder).numfolders; i++ )
	{
		gtk_tree_store_append( pstore, &additer, pparent );
		gtk_tree_store_set( pstore, &additer, PIC_COLUMN, treeicon,
							NAME_COLUMN, (*pfolder).folderlist[i].foldername,
							FSIZE_COLUMN, 0,
							CHDATE_COLUMN, "-",
							EQUAL_COLUMN, "1", -1 );

		/*append folders (recursively)*/
		add_folder_to_treeview( &((*pfolder).folderlist[i]), pstore, &additer );	
	}
}

GtkTreeStore *add_folder_view( GtkWidget *playout, GtkWidget **ptview )
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

int show_sync_window( GtkWindow *parent )
{
	/*variables*/
	GtkWidget *sync_win;
	GtkWidget *main_layout;
	GtkWidget *src_layout;
	GtkWidget *dest_layout;
	GtkWidget *lblsrc_path;
	GtkWidget *lblsrc_size;
	GtkWidget *lblsrc_changedate;
	GtkWidget *lbldest_path;
	GtkWidget *lbldest_size;
	GtkWidget *lbldest_changedate;
	GtkWidget *entsrc_path;
	GtkWidget *entsrc_size;
	GtkWidget *entsrc_changedate;
	GtkWidget *entdest_path;
	GtkWidget *entdest_size;
	GtkWidget *entdest_changedate;
	GtkWidget *srcframe;
	GtkWidget *destframe;
	GtkWidget *src_layout_p;
	GtkWidget *src_layout_b;
	GtkWidget *src_layout_c;
	GtkWidget *dest_layout_p;
	GtkWidget *dest_layout_b;
	GtkWidget *dest_layout_c;

	/*set window attributes*/
	sync_win = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_position( GTK_WINDOW(sync_win), GTK_WIN_POS_CENTER );
	gtk_window_set_title( GTK_WINDOW(sync_win), "Kopieren..." );
	gtk_window_set_modal( GTK_WINDOW(sync_win), TRUE );
	gtk_window_deiconify( GTK_WINDOW(sync_win) );
	gtk_window_set_default_size( GTK_WINDOW(sync_win), 850, ( HEIGHT / 2 ) );

	/*define layout*/
	main_layout = gtk_vbox_new( TRUE, 3 );
	src_layout = gtk_vbox_new( TRUE, 0 );
	dest_layout = gtk_vbox_new( TRUE, 0 );
	src_layout_p = gtk_hbox_new( FALSE, 0 );
	src_layout_b = gtk_hbox_new( FALSE, 0 );
	src_layout_c = gtk_hbox_new( FALSE, 0 );
	dest_layout_p = gtk_hbox_new( FALSE, 0 );
	dest_layout_b = gtk_hbox_new( FALSE, 0 );
	dest_layout_c = gtk_hbox_new( FALSE, 0 );

	gtk_container_add( GTK_CONTAINER(sync_win), main_layout );

	/*define labels*/
	lblsrc_path = gtk_label_new( "Quelldatei:   " );
	lblsrc_size = gtk_label_new( "Bytes:          " );
	lblsrc_changedate = gtk_label_new( "Changedate:" );
	lbldest_path = gtk_label_new( "Zieldatei:      " );
	lbldest_size = gtk_label_new( "Bytes:          " );
	lbldest_changedate = gtk_label_new( "Changedate:" );
	entsrc_path = gtk_entry_new();
	gtk_entry_set_editable( GTK_ENTRY(entsrc_path), FALSE );
	entsrc_size = gtk_entry_new();
	gtk_entry_set_editable( GTK_ENTRY(entsrc_size), FALSE );
	entsrc_changedate = gtk_entry_new();
	gtk_entry_set_editable( GTK_ENTRY(entsrc_changedate), FALSE );
	entdest_path = gtk_entry_new();
	gtk_entry_set_editable( GTK_ENTRY(entdest_path), FALSE );
	entdest_size = gtk_entry_new();
	gtk_entry_set_editable( GTK_ENTRY(entdest_size), FALSE );
	entdest_changedate = gtk_entry_new();
	gtk_entry_set_editable( GTK_ENTRY(entdest_changedate), FALSE );

	/*define frames*/
	srcframe = gtk_frame_new( "Quelle" );
	destframe = gtk_frame_new( "Ziel" );

	/*set frames on layout*/
	gtk_box_pack_start( GTK_BOX(main_layout), srcframe, TRUE, TRUE, 3 );
	gtk_box_pack_start( GTK_BOX(main_layout), destframe, TRUE, TRUE, 5 );

	/*add layouts to frames*/
	gtk_container_add( GTK_CONTAINER(srcframe), src_layout );
	gtk_container_add( GTK_CONTAINER(destframe), dest_layout );

	/*pack sub-layouts*/
	gtk_box_pack_start( GTK_BOX(src_layout), src_layout_p, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(src_layout), src_layout_b, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(src_layout), src_layout_c, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(dest_layout), dest_layout_p, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(dest_layout), dest_layout_b, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(dest_layout), dest_layout_c, TRUE, TRUE, 0 );

	/*pack labes and entries*/
	/*src*/
	gtk_box_pack_start( GTK_BOX(src_layout_p), lblsrc_path, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(src_layout_p), entsrc_path, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(src_layout_b), lblsrc_size, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(src_layout_b), entsrc_size, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(src_layout_c), lblsrc_changedate, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(src_layout_c), entsrc_changedate, TRUE, TRUE, 0 );
	/*dest*/
	gtk_box_pack_start( GTK_BOX(dest_layout_p), lbldest_path, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(dest_layout_p), entdest_path, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(dest_layout_b), lbldest_size, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(dest_layout_b), entdest_size, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(dest_layout_c), lbldest_changedate, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(dest_layout_c), entdest_changedate, TRUE, TRUE, 0 );




	gtk_widget_show_all( sync_win );

	return 1;
}

/*callback functions*/
void button_open_clicked( GtkButton *pbtn, btn_open_data *data )
{
	/*variables*/
	GtkWidget *fileopen;
	char *folder_path = NULL;
	folderst folder;

	if( data->folder != NULL )
		free_sub_folder_list( data->folder );

	/*create fileopen dialog*/
	fileopen = gtk_file_chooser_dialog_new( "Verzeichnis öffnen", GTK_WINDOW(data->parent)
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
	    if( GTK_IS_TREE_STORE(data->store) )
	    {
	        gtk_tree_store_clear( data->store );
	        add_folder_to_treeview( &folder, data->store, NULL );
	        /*set text to entry*/
	        gtk_entry_set_text( GTK_ENTRY(data->entry), folder_path );
	        /*save address of read folder*/
	        data->folder = &folder;
	     }

	     /*free the string*/
	     g_free (folder_path);
	}
	/*destroy the widget*/
	gtk_widget_destroy (fileopen);
}

void button_sync_clicked( GtkButton *pbtn, gpointer data )
{
	show_sync_window( NULL );
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
	GtkWidget *entry_a;
	GtkWidget *entry_b;
	GtkWidget *frame_a;
	GtkWidget *frame_b;
	btn_open_data *btn_data_a;
	btn_open_data *btn_data_b;
	GtkWidget *btn_sync;
	GtkWidget *btn_close;
	GtkWidget *sync_layout;
	folderst *folder_a;
	folderst *folder_b;

	/*initialize main window*/
	main_win = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	/*initialize layout*/
	menubar_layout = gtk_vbox_new( FALSE, 0 );
	layout_a = gtk_vbox_new( FALSE, 3 );
	layout_b = gtk_vbox_new( FALSE, 3 );
	tree_layout = gtk_hbox_new( TRUE, 4 );
	sync_layout = gtk_hbutton_box_new();

	/*initialize frames*/
	frame_a = gtk_frame_new( "Folder A" );
	frame_b = gtk_frame_new( "Folder B" );


	/*initialize buttons*/
	btn_open_a = gtk_button_new_from_stock( GTK_STOCK_OPEN );
	btn_open_b = gtk_button_new_from_stock( GTK_STOCK_OPEN );
	btn_sync = gtk_button_new_from_stock( GTK_STOCK_COPY );
	btn_close = gtk_button_new_from_stock( GTK_STOCK_QUIT );

	/*initialize entries*/
	entry_a = gtk_entry_new();
	entry_b = gtk_entry_new();
	/*set them not editable*/
	gtk_entry_set_editable( GTK_ENTRY(entry_a), FALSE );
	gtk_entry_set_editable( GTK_ENTRY(entry_b), FALSE );


	/*add layout to main window*/
	gtk_container_add( GTK_CONTAINER(main_win), menubar_layout );


	/*set properties*/
	set_main_win_attributes( main_win, "syncfolders-gtk" );

	/*add menubar*/
	add_menubar_to_main_win( menubar_layout );

	/*pack Layouts and frames*/
	gtk_box_pack_start( GTK_BOX(menubar_layout), tree_layout, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(menubar_layout), sync_layout, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(tree_layout), frame_a, TRUE, TRUE, 3 );
	gtk_box_pack_start( GTK_BOX(tree_layout), frame_b, TRUE, TRUE, 3 );
	gtk_container_add( GTK_CONTAINER(frame_a), layout_a );
	gtk_container_add( GTK_CONTAINER(frame_b), layout_b );

	/*pack buttons*/
	gtk_box_pack_start( GTK_BOX(layout_a), btn_open_a, FALSE, FALSE, 3 );
	gtk_box_pack_start( GTK_BOX(layout_b), btn_open_b, FALSE, FALSE, 3 );

	/*pack entries*/
	gtk_box_pack_start( GTK_BOX(layout_a), entry_a, FALSE, FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(layout_b), entry_b, FALSE, FALSE, 0 );


	/*add treeview and get back the tree store*/
	viewleft = add_folder_view( layout_a, &main_tree_a );
	viewright = add_folder_view( layout_b, &main_tree_b );

	/*add sync button*/
	gtk_hbutton_box_set_layout_default( GTK_BUTTONBOX_START );
	gtk_box_pack_start( GTK_BOX(sync_layout), btn_sync, FALSE, FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(sync_layout), btn_close, FALSE, FALSE, 0 );

	/*set data for callback function*/
	/*alloc memory*/
	btn_data_a = g_slice_new( btn_open_data );
	btn_data_b = g_slice_new( btn_open_data );
	/*button a*/
	btn_data_a->parent = GTK_WINDOW( main_win );
	btn_data_a->store = viewleft;
	btn_data_a->entry = GTK_ENTRY(entry_a);
	btn_data_a->folder = NULL;
	/*button b*/
	btn_data_b->parent = GTK_WINDOW( main_win );
	btn_data_b->store = viewright;
	btn_data_b->entry = GTK_ENTRY(entry_b);
	btn_data_b->folder = NULL;

	/*add signals to buttons*/
	g_signal_connect( btn_open_a, "clicked", G_CALLBACK(button_open_clicked), btn_data_a );
	g_signal_connect( btn_open_b, "clicked", G_CALLBACK(button_open_clicked), btn_data_b );
	g_signal_connect( btn_sync, "clicked", G_CALLBACK(button_sync_clicked), NULL );
	g_signal_connect( btn_close, "clicked", G_CALLBACK(gtk_main_quit), NULL );
	g_signal_connect (main_win, "destroy", G_CALLBACK(gtk_main_quit), NULL );

	gtk_widget_show_all (main_win);
}

int main( int argc, char *argv[] )
{
	gtk_init (&argc, &argv);

	/*start the gtk-gui*/
	start_gtk_gui();


	/*gtk main loop*/
	gtk_main ();

	return 0;
}

