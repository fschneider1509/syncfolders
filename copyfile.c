/*includes*/
#include "copyfile.h"


/*Funktion funktioniert nicht*/
char *remove_trailing_newline( char *pstring )
{
	/*variables*/
	char *new;
	size_t istringlen;
	int icnt = 0;

	/*get length of pstring*/
	istringlen = strlen( pstring );

	new = malloc( sizeof(char) * istringlen-1 + 1 );

	if( new != NULL )
	{
		/*copy string without '\n'*/
		while( pstring[icnt] != '\n' )
		{
			new[icnt] = pstring[icnt];
			icnt++;
		}
		icnt++;
		new[icnt] = '\0';

		return new;
	}
	else
		return NULL;
}

int ask_user( filest *pfilea, filest *pfileb )
{
	/* return codes:
	 * 1: copy file a to file b
	 * 2: copy file b to file a
	 * 3: do nothing
	 */

	/*variables*/
	int answer = 0;
	int check = 0;

	fprintf( stdout, "files:\n" );
	fprintf( stdout, "\t(1) %s, %d Bytes, changedate: %s", (*pfilea).filepath, (*pfilea).filesize, ctime( &((*pfilea).changedate )) );
	fprintf( stdout, "\t(2) %s, %d Bytes, changedate: %s", (*pfileb).filepath, (*pfileb).filesize, ctime( &((*pfileb).changedate )) );
	fprintf( stdout, "\n\n" );

	do{
		fprintf( stdout, "1: override file (2) with file (1), 2: override file (1) with file (2), 3: do nothing\n" );
		fprintf( stdout, "Answer: ");
		scanf( "%d", &answer );

		/*check answer*/
		if( answer == 1 || answer == 2 || answer == 3 )
			check = 1;

	}while( check != 1 );

	return answer;
}

void print_copy_activity( filest *pfilea, filest *pfileb )
{
	/*print copy information to the console (or stdout)*/
	fprintf( stdout, "\tCopy file %s\n", (*pfilea).filepath );
	fprintf( stdout, "\tsize %d Bytes\n", (*pfilea).filesize );
	fprintf( stdout, "\tchangedate %s\n", remove_trailing_newline( ctime( &((*pfilea).changedate) ) ) );
	fprintf( stdout, "\t-to-\n" );
	fprintf( stdout, "\tfile %s\n", (*pfileb).filepath );
	fprintf( stdout, "\tsize %d Bytes\n", (*pfileb).filesize );
	fprintf( stdout, "\tchangedate %s\n", remove_trailing_newline( ctime( &((*pfileb).changedate) ) ) );
	fprintf( stdout, "\tCopy state: ");
}

void print_ok( void )
{
	fprintf( stdout, "...Ok\n\n" );
}

void print_fail( void )
{
	fprintf( stdout, "...Fail\n\n" );
}

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
