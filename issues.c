/*includes*/
#include "issues.h"

void issue_set_type( msg_issue *pissue, char *ptype )
{
	/*variables*/
	size_t typelen = 0;

	typelen = strlen( ptype );

	pissue->type = malloc( sizeof( char ) * typelen + 1 );

	if( pissue->type != NULL )
	{
		strcpy( pissue->type, ptype );
	}
	else
		pissue->type = NULL;
}

void issue_set_name( msg_issue *pissue, char *pname )
{
	/*attention name == path*/

	/*variables*/
	size_t namelen = 0;

	namelen = strlen( pname );

	pissue->name = malloc( sizeof( char ) * namelen + 1 );

	if( pissue->name != NULL )
	{
		strcpy( pissue->name, pname );
	}
	else
		pissue->name = NULL;
}

void issue_set_msg( msg_issue *pissue, char *pmsg )
{
	/*variables*/
	size_t msglen = 0;

	msglen = strlen( pmsg );

	pissue->msg = malloc( sizeof( char ) * msglen + 1 );

	if( pissue->msg != NULL )
	{
		strcpy( pissue->msg, pmsg );
	}
	else
		pissue->msg = NULL;
}

int append_issue_to_list( msg_issue *pissue, issue_list *pissues )
{
	/*allocate memory for the next entry in the list*/
	pissues->issues = realloc( pissues->issues, sizeof( msg_issue ) * (pissues->num_issues + 1) );

	if( pissues->issues != NULL )
	{
		/*append issue to list*/
		pissues->issues[pissues->num_issues] = *pissue;
		pissues->num_issues++;
		return 0;
	}
	else
		return -1;
}

void reset_issue_list( issue_list *pissues )
{
	pissues->issues = NULL;
	pissues->num_issues = 0;
}

void print_issues( issue_list *pissues )
{
	/*for testing only*/

	/*variables*/
	int i = 0;

	for( i = 0; i < pissues->num_issues; i++ )
	{
		/*print: Issue-Type Filename/-path Message \n*/
		fprintf( stdout, "%s %s %s \n", pissues->issues[i].type, pissues->issues[i].name, pissues->issues[i].msg );
	}
}
