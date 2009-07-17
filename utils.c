#include "bnxc.h"

int stringcmp(const void *a, const void *b)
{
	const char **ia = (const char **)a;
	const char **ib = (const char **)b;
	return strcmp(*ia, *ib);
}

/* remove a leading '-' char from a string */
char* rm_dash(char *str)
{
	char *new;
	unsigned int i;

	if(str[0] != '-')
		return str;

	new = malloc(sizeof(char)*(strlen(str)));

	for(i=1; i<=strlen(str); i++) /* don't copy the first char, but *do* copy the \0 */
		new[i-1] = str[i];

	return new;
}

/* remove the newline char from the end of a string */
char* strip_nl(char *str)
{
	str[strlen(str)-1] = '\0';

	return str;
}
