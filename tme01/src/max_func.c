#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int max_func(const int * const tab, const int s)
{
	assert(s > 0 && "cant max an empty int array");	
	
	int i, max;	
	max = tab[0];	
	for(i=1;i<s;i++)
	{
		max = (max < tab[i]) ? tab[i] : max ;
	}

	return max;
}
