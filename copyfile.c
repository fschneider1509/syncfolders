/*includes*/
#include "copyfile.h"

int set_physical_change_date( time_t *pdate, filest *pfile, issue_list *pissues )
{
	/*variables*/
	struct utimbuf newtime;
	msg_issue tmp_issue;

	newtime.actime = *pdate;
	newtime.modtime = *pdate;

	/*set physical changedate (glibc fuction)*/
	if( utime( (*pfile).filepath, &newtime ) != 0 )
	{
		/*change time could not be changed*/
		/*print_msg( "change time could not be changed", (*pfile).filepath, 2 );*/
		issue_set_type( &tmp_issue, "Fehler" );
		issue_set_name( &tmp_issue, (*pfile).filepath );
		issue_set_msg( &tmp_issue, "Das Änderungsdatum konnte nicht gesetzt werden" );
		append_issue_to_list( &tmp_issue, pissues );
		return -1;
	}
	/*set changedate in struct*/
	(*pfile).changedate = *pdate;
	(*pfile).str_changedate = ctime( pdate );

	return 0;
}

void  set_file_size( unsigned int psize, filest *pfile )
{
	(*pfile).filesize = psize;
}

int create_shadow_file( char *pfilepath )
{
	/*variables*/
	FILE *tmp;

	tmp = fopen( pfilepath, "w+" );

	if( tmp != NULL )
	{
		fclose( tmp );
		return 1;
	}
	else
		return -1;
}

int copy_content( FILE *src, FILE *dest )
{
	/*variables*/
	int tmpc = 0;

	tmpc = fgetc( src );

	/*copy character only, if it is not EOF*/
	while( tmpc != EOF )
	{
		/*put character in ouptputfile*/
		if( fputc( (int) tmpc, dest ) != tmpc )
			return -1;
		tmpc = fgetc( src );
	}
	
	/*close opened files*/
	fclose( src );
	fclose( dest );

	return 1;
}

int copy_file_on_disk( filest *srcfile, filest *destfile, issue_list *pissues )
{
	/*variables*/
	FILE *src;
	FILE *dest;
	msg_issue tmp_issue;

	/*delete destfile*/
	if( remove( (*destfile).filepath ) == 0 )
	{
		/*open srcfile for reading*/
		src = fopen( (*srcfile).filepath, "r" );
		
		if( src != NULL )
		{
			/*create new destfile*/
			dest = fopen( (*destfile).filepath, "w+" );	

			if( dest != NULL )
			{
				if( copy_content( src, dest ) == 1 )
				{
					/*set the changedate to changedate of srcfile*/
					set_physical_change_date( &((*srcfile).changedate), destfile, pissues );
					/*set the filesize to the size of srcfile*/
					set_file_size( (*srcfile).filesize, destfile );
					return 1;
				}
				else
					return -1;
			}
			else
			{
				/*print_msg( "file could not be created", (*srcfile).filepath, 2 );*/
				issue_set_type( &tmp_issue, "Fehler" );
				issue_set_name( &tmp_issue, (*srcfile).filepath );
				issue_set_msg( &tmp_issue, "Datei konnte nicht angelegt werden" );
				append_issue_to_list( &tmp_issue, pissues );
			}
		}
		else
		{
			/*print_msg( "file could not be opened", (*srcfile).filepath, 2 );*/
			issue_set_type( &tmp_issue, "Fehler" );
			issue_set_name( &tmp_issue, (*srcfile).filepath );
			issue_set_msg( &tmp_issue, "Datei konnte zum kopieren nicht geöffnet werden" );
			append_issue_to_list( &tmp_issue, pissues );
		}

	}
	else
	{
		/*print_msg( "file could not be deleted", (*destfile).filepath, 2 );*/
		issue_set_type( &tmp_issue, "Fehler" );
		issue_set_name( &tmp_issue, (*destfile).filepath );
		issue_set_msg( &tmp_issue, "Datei konnte nicht gelöscht werden" );
		append_issue_to_list( &tmp_issue, pissues );
	}

		
	return -1;
}

void start_copy( filest *pfile_a, filest *pfile_b, GtkProgressBar *pbar, issue_list *pissues )
{
	/*set text in progressbar*/
	gtk_progress_bar_set_pulse_step( pbar, 0.2 );
	gtk_progress_bar_pulse( pbar );
	gtk_progress_bar_set_text( pbar, (*pfile_a).filepath );
	gtk_progress_bar_pulse( pbar );

	if( copy_file_on_disk( pfile_a, pfile_b, pissues ) == 1 )
		print_ok();
	else
		print_fail();
}
