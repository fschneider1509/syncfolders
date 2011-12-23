/*includes*/
#include "copyfile.h"

int set_physical_change_date( time_t *pdate, filest *pfile )
{
	struct utimbuf newtime;

	newtime.actime = *pdate;
	newtime.modtime = *pdate;

	/*set physical changedate (glibc fuction)*/
	if( utime( (*pfile).filepath, &newtime ) != 0 )
	{
		/*change time could not be changed*/
		print_msg( "change time could not be changed", (*pfile).filepath, 2 );
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

int copy_file_on_disk( filest *srcfile, filest *destfile )
{
	/*variables*/
	FILE *src;
	FILE *dest;

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
					set_physical_change_date( &((*srcfile).changedate), destfile );
					/*set the filesize to the size of srcfile*/
					set_file_size( (*srcfile).filesize, destfile );
					return 1;
				}
				else
					return -1;
			}
			else
				print_msg( "file could not be created", (*srcfile).filepath, 2 );
		}
		else
			print_msg( "file could not be opened", (*srcfile).filepath, 2 );
			
	}
	else
		print_msg( "file could not be deleted", (*destfile).filepath, 2 );
		
	return -1;
}

void start_copy( filest *pfile_a, filest *pfile_b, btn_sync_data *pwidgets )
{
	/*update sync window*/
	update_sync_window( pfile_a, pfile_b, pwidgets );

	if( copy_file_on_disk( pfile_a, pfile_b ) == 1 )
		print_ok();
	else
		print_fail();
}
