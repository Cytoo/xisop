#define _XOPEN_SOURCE 700
#include <assert.h>

int max_func(const int * const tab, const int s) {
	int i, max;	

	assert(s > 0 && "can't max an empty int array");
	max = tab[0];	

	for(i = 1; i < s; ++i)
		max = (max < tab[i]) ? tab[i] : max ;

	return max;
}
