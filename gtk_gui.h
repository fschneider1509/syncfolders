#ifndef GTK_GUI_H
#define GTK_GUI_H

/*includes*/
#include <gtk/gtk.h>
#include "readfolder.h"

#include <unistd.h>

/*defines*/
/*defintion of the window size (startup)*/
#define WIDTH 850
#define HEIGHT 550

/*enums*/

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

/*structs*/

/*struct for callback function "button_open_clicked"*/
typedef struct btn_open
{
	GtkWindow *parent;
	GtkTreeStore *store;
	GtkEntry *entry;
	folderst *folder;
} btn_open_data;

typedef struct btn_sync
{
	GtkEntry *src_path;
	GtkEntry *src_size;
	GtkEntry *src_changedate;
	GtkEntry *dest_path;
	GtkEntry *dest_size;
	GtkEntry *dest_changedate;
} btn_sync_data;

typedef struct sync_data
{
	btn_open_data *a;
	btn_open_data *b;
} sync_folders;

/*prototypes*/
extern void add_menubar_to_main_win( GtkWidget* );
extern void set_main_win_attributes( GtkWidget*, gchar* );
extern void append_files_to_treeview( GtkTreeStore*, GtkTreeIter*, GtkTreeIter* , filest*, int, GdkPixbuf* );
extern void add_folder_to_treeview( folderst*, GtkTreeStore*, GtkTreeIter* );
extern GtkTreeStore *add_folder_view( GtkWidget*, GtkWidget** );
extern int show_sync_window( btn_sync_data* );
extern void button_open_clicked( GtkButton*, btn_open_data* );
extern void button_sync_clicked( GtkButton*, sync_folders* );
extern void start_gtk_gui( void );

#endif /*GTK_GUI_H*/
