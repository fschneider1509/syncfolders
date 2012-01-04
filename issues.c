/*includes*/
#include "issue.h"

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

int append_issue_to_list( msg_issue *pissue, issue_list *plist )
{
	/*allocate memory for the next entry in the list*/
	plist->issues = realloc( plist->issues, sizeof(msg_issue) * plist->num_issues + 1 );

	if( plist->issues != NULL )
	{
		/*append issue to list*/
		plist->issues[plist->num_issues] = *pissue;
		plist->num_issues + 1;
		return 0;
	}
	else
		return -1;
}

void reset_issue_list( issue_list *plist )
{
	plist->issues = NULL;
	plist->num_issues = 0;
}
