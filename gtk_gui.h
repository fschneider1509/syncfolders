#ifndef GTK_GUI_H
#define GTK_GUI_H

/*includes*/
#include <gtk/gtk.h>
#include "readfolder.h"
#include "issues.h"

typedef struct btn_open
{
	GtkWindow *parent;
	GtkTreeStore *store;
	GtkEntry *entry;
	folderst *folder;
	issue_list *issues;
} btn_open_data;

typedef struct sync_data
{
	btn_open_data *a;
	btn_open_data *b;
	GtkProgressBar *bar;
	GtkWindow *parent;
	issue_list *issues;
} sync_folders;

/*defines*/
/*defintion of the window size (startup)*/
#define WIDTH 850
#define HEIGHT 550

/*include "comparefolders.h" after the definition of the struct "sync_folders",
 * because of dependencies*/
#include "comparefolders.h"


/*enums*/
enum /*enum for foldertreeview*/
{
	PIC_COLUMN, /*symbol*/
	NAME_COLUMN, /*folder- or filename*/
	FSIZE_COLUMN, /*filesize*/
	CHDATE_COLUMN, /*changedate*/
	EQUAL_COLUMN, /*equality of the file*/
	N_COLUMNS /*number of cols*/
};

enum /*enum for logview*/
{
	ICO_COLUMN, /*symbol*/
	TYPE_COLUMN, /*message type*/
	PATH_COLUMN, /*file- or foldername*/
	MSG_COLUMN, /*message*/
	NUM_COLUMNS /*number of cols*/
};

/*prototypes*/
extern char *build_msg( char*, char* );
extern void show_msg_dlg( char*, char*, int, GtkWindow* );
extern void add_menubar_to_main_win( GtkWidget* );
extern void set_main_win_attributes( GtkWidget*, gchar* );
extern void append_files_to_treeview( GtkTreeStore*, GtkTreeIter*, GtkTreeIter* , filest*, int, GdkPixbuf* );
extern void add_folder_to_treeview( folderst*, GtkTreeStore*, GtkTreeIter* );
extern GtkTreeStore *add_folder_view( GtkWidget*, GtkWidget** );
extern void button_open_clicked( GtkButton*, btn_open_data* );
extern void button_sync_clicked( GtkButton*, sync_folders* );
extern void button_log_clicked( GtkButton*, issue_list* );
extern void start_gtk_gui( void );

#endif /*GTK_GUI_H*/
