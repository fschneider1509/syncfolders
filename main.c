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

void add_menubar( GtkWidget *pwindow )
{
	/*variables*/
	GtkWidget *menu_box; /*container box, holds elements*/
	GtkWidget *menubar; /*menu bar*/
	GtkWidget *file_menu; /*file menu*/
	GtkWidget *file_top; /*top entry*/
	GtkWidget *open_entry; /*menu entry for file open*/
	GtkWidget *exit_entry; /*menu entry for exit application*/

	/*initialize menu containerbox*/
	menu_box = gtk_vbox_new( FALSE, 0 );
	/*add box*/
	gtk_container_add( GTK_CONTAINER(pwindow), menu_box );

	/*initialize menubar*/
	menubar = gtk_menu_bar_new();
	/*initialize menus*/
	file_menu = gtk_menu_new();

	/*set menu labels*/
	file_top = gtk_menu_item_new_with_label( "File" );
	//open_entry = gtk_menu_item_new_with_label( "Open" );
	open_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_OPEN, NULL );
	exit_entry = gtk_image_menu_item_new_from_stock( GTK_STOCK_QUIT, NULL );

	/*add menus*/
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(file_top), file_menu );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), open_entry );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), exit_entry );
	gtk_menu_shell_append( GTK_MENU_SHELL(menubar), file_top );

	/*pack menu*/
	gtk_box_pack_start( GTK_BOX(menu_box), menubar, FALSE, FALSE, 0 );


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

int main (int argc, char *argv[])
{
	/*variables*/
	GtkWidget *main_win;



	gtk_init (&argc, &argv);

	/*initialize main window*/
	main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	/*set properties*/
	set_main_win_attribs( main_win, "syncfolders-gtk" );

	/*add menubar*/
	add_menubar( main_win );

	g_signal_connect (main_win, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	gtk_widget_show_all (main_win);

	gtk_main ();

	return 0;
}

