/*includes*/
#include "consoleprint.h"

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

	fprintf( stdout, "file confilict:\n" );
	fprintf( stdout, "\t(1) %s, %d Bytes, changedate: %s\n", (*pfilea).filepath, (*pfilea).filesize, (*pfilea).str_changedate );
	fprintf( stdout, "\t(2) %s, %d Bytes, changedate: %s", (*pfileb).filepath, (*pfileb).filesize, (*pfileb).str_changedate );
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

void print_msg( char *pmsg, char *pparam, int ptype )
{
	switch( ptype )
	{
		case 1:
			fprintf( stdout, "%s:%s\n", pmsg, pparam );
			break;
		case 2:
			fprintf( stderr, "\t%s:%s\n", pmsg, pparam );
			break;
		default:
			fprintf( stdout, "message: %s:%s\n", pmsg, pparam );
			break;
	}
}
