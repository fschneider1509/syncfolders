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

	return 0;
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
		print_msg( "filde could not be deleted", (*destfile).filepath, 2 );
		
	return -1;
}
