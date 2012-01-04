/*includes*/
#include "gtk_gui.h"

char *build_msg( char *pmsg, char *padd )
{
	/*variables*/
	char *new;
	int lenmsg;
	int lenadd;

	/*get length of strings*/
	lenmsg = strlen( pmsg );
	lenadd = strlen( padd );

	new = malloc( sizeof( char ) * (lenmsg + lenadd + 2) + 1 );

	if( new != NULL )
	{
		strcpy( new, pmsg );
		strcat( new, " " );
		strcat( new, padd );
	}

	return new;
}

void show_msg_dlg( char* pmsg, char *padd, int ptype, GtkWindow *parent )
{
	/*variables*/
	GtkWidget *msgdialog;
	char *msg;

	msg = build_msg( pmsg, padd );

	switch( ptype )
	{
	case 1:
		/*info*/
		msgdialog = gtk_message_dialog_new( parent, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg );
		break;
	case 2:
		/*error*/
		msgdialog = gtk_message_dialog_new( parent, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, msg );
		break;
	default:
		msgdialog = gtk_message_dialog_new( parent, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_CLOSE, msg );
		break;
	}

	/*start dialog*/
	gtk_dialog_run( GTK_DIALOG(msgdialog) );

	/*destroy dialog*/
	gtk_widget_destroy( msgdialog );

	/*free memory of string*/
	free( msg );
}

void add_menubar_to_main_win( GtkWidget *playout )
{
	/*variables*/
	GtkWidget *menubar; /*menu bar*/
	GtkWidget *file_menu; /*file menu*/
	GtkWidget *file_top; /*top entry*/

	GtkWidget *exit_entry; /*menu entry for exit application*/

	/*initialize menubar*/
	menubar = gtk_menu_bar_new();
	/*initialize menus*/
	file_menu = gtk_menu_new();

	/*set menu labels*/
	/*file menu*/
	file_top = gtk_menu_item_new_with_mnemonic( "_Datei" );
	exit_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_QUIT, NULL );

	/*add menus*/
	/*file menu*/
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(file_top), file_menu );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), exit_entry );

	gtk_menu_shell_append( GTK_MENU_SHELL(menubar), file_top );

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

/*****************************************************************************************************************************************/
/*callback functions*/
void button_open_clicked( GtkButton *pbtn, btn_open_data *data )
{
	/*variables*/
	GtkWidget *fileopen;
	char *folder_path = NULL;

	/*create fileopen dialog*/
	fileopen = gtk_file_chooser_dialog_new( "Verzeichnis öffnen", GTK_WINDOW(data->parent)
			, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );

	if( gtk_dialog_run (GTK_DIALOG (fileopen)) == GTK_RESPONSE_ACCEPT )
	{
	    /*save return path*/
	    folder_path = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (fileopen) );

	    /*read folder a*/
	    reset_folder( data->folder );
	    set_root_folder_attributes( data->folder, folder_path, data->issues );
	    read_folder( folder_path , data->folder, data->issues );

	    /*add folder to treeview; clear before adding*/
	    if( GTK_IS_TREE_STORE(data->store) )
	    {
	        gtk_tree_store_clear( data->store );
	        add_folder_to_treeview( data->folder, data->store, NULL );
	        /*set text to entry*/
	        gtk_entry_set_text( GTK_ENTRY(data->entry), folder_path );
	    }

	     /*free the string*/
	     g_free (folder_path);
	}
	/*destroy the widget*/
	gtk_widget_destroy (fileopen);

	print_issues( data->issues );
}

void button_sync_clicked( GtkButton *pbtn, sync_folders *param )
{

	/*start thread for syncing*/
	if( g_thread_create( init_compare, param, FALSE, NULL ) == NULL )
		/*error*/
		show_msg_dlg( "Synchronisationsvorgang konnte nicht gestartet werden.", "", 1, param->parent );
}

void button_log_clicked( GtkButton *pbtn, gpointer data )
{

}
/*****************************************************************************************************************************************/

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
	sync_folders *sync_fl;
	GtkWidget *progressbar;
	GtkWidget *progressframe;
	GtkWidget *progress_layout;
	issue_list *all_issues;

	/*init threads*/
	g_thread_init( NULL );

	/*initialize main window*/
	main_win = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	/*initialize layout*/
	menubar_layout = gtk_vbox_new( FALSE, 0 );
	layout_a = gtk_vbox_new( FALSE, 3 );
	layout_b = gtk_vbox_new( FALSE, 3 );
	progress_layout = gtk_vbox_new( FALSE, 0 );
	tree_layout = gtk_hbox_new( TRUE, 4 );
	sync_layout = gtk_hbutton_box_new();

	/*initialize frames*/
	frame_a = gtk_frame_new( "Folder A" );
	frame_b = gtk_frame_new( "Folder B" );
	progressframe = gtk_frame_new( "Synchronisation" );

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
	
	/*init progressbar*/
	progressbar = gtk_progress_bar_new();
	gtk_progress_bar_pulse( GTK_PROGRESS_BAR(progressbar) );

	/*add layout to main window*/
	gtk_container_add( GTK_CONTAINER(main_win), menubar_layout );


	/*set properties*/
	set_main_win_attributes( main_win, "syncfolders-gtk" );

	/*add menubar*/
	add_menubar_to_main_win( menubar_layout );

	/*pack Layouts and frames*/
	gtk_box_pack_start( GTK_BOX(menubar_layout), tree_layout, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(menubar_layout), progress_layout, TRUE, TRUE, 0);
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

	/*add progressbar*/
	gtk_box_pack_start( GTK_BOX(progress_layout), progressframe, TRUE, TRUE, 3 );
	/*set properties of the progressbar*/
	//gtk_progress_bar_set_ellipsize( GTK_PROGRESS_BAR(progressbar), PANGO_ELLIPSIZE_MIDDLE );
	gtk_progress_bar_set_bar_style( GTK_PROGRESS_BAR(progressbar), GTK_PROGRESS_DISCRETE );

	/*add sync button*/
	gtk_hbutton_box_set_layout_default( GTK_BUTTONBOX_START );
	gtk_box_pack_start( GTK_BOX(sync_layout), btn_sync, FALSE, FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(sync_layout), btn_close, FALSE, FALSE, 0 );

	/*add progressbar*/
	gtk_container_add( GTK_CONTAINER(progressframe), progressbar );

	/*set data for callback function*/
	/*alloc memory*/
	btn_data_a = g_slice_new( btn_open_data );
	btn_data_b = g_slice_new( btn_open_data );
	btn_data_a->folder = g_slice_new( folderst );
	btn_data_b->folder = g_slice_new( folderst );
	sync_fl = g_slice_new( sync_folders );
	all_issues = g_slice_new( issue_list );

	reset_issue_list( all_issues );

	/*save button data*/
	sync_fl->a = btn_data_a;
	sync_fl->b = btn_data_b;
	sync_fl->bar = GTK_PROGRESS_BAR(progressbar);
	sync_fl->parent = GTK_WINDOW( main_win );
	sync_fl->issues = all_issues;

	/*button a*/
	btn_data_a->parent = GTK_WINDOW( main_win );
	btn_data_a->store = viewleft;
	btn_data_a->entry = GTK_ENTRY(entry_a);
	btn_data_a->issues = all_issues;
	/*button b*/
	btn_data_b->parent = GTK_WINDOW( main_win );
	btn_data_b->store = viewright;
	btn_data_b->entry = GTK_ENTRY(entry_b);
	btn_data_a->issues = all_issues;

	/*add signals to buttons*/
	g_signal_connect( btn_open_a, "clicked", G_CALLBACK(button_open_clicked), btn_data_a );
	g_signal_connect( btn_open_b, "clicked", G_CALLBACK(button_open_clicked), btn_data_b );
	g_signal_connect( btn_sync, "clicked", G_CALLBACK(button_sync_clicked), sync_fl );
	g_signal_connect( btn_close, "clicked", G_CALLBACK(gtk_main_quit), NULL );
	g_signal_connect (main_win, "destroy", G_CALLBACK(gtk_main_quit), NULL );

	gtk_widget_show_all (main_win);
}
