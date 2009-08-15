#include "bnxc.h"

int stringcmp(const void *a, const void *b)
{
	const char **ia = (const char **)a;
	const char **ib = (const char **)b;
	return strcmp(*ia, *ib);
}

/* remove the newline char from the end of a string */
char* strip_nl(char *str)
{
	str[strlen(str)-1] = '\0';

	return str;
}

void destroy_menu_params(menu_parameters params)
{
	free(params->list);
	free(params);
}
