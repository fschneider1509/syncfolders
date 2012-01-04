#ifndef ISSUES_H
#define ISSUES_H

/*includes*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*structs*/
typedef struct issue
{
	char *type;
	char *msg;
	char *name;
} msg_issue;

typedef struct list
{
	msg_issue *issues;
	size_t num_issues;
} issue_list;

/*protoypes*/
extern void issue_set_type( msg_issue*, char* );
extern void issue_set_name( msg_issue*, char* );
extern int append_issue_to_list( msg_issue*, issue_list* );
extern void reset_issue_list( issue_list* );

#endif /*ISSUES_H*/
