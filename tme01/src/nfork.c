#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
 
int nfork (int nbfils)
{
	int i,f,p;
	f=0;
	for(i=0;i<nbfils;i++)
	{
		p = fork();
		if (p == 0)
			return 0;
		else if (p > 0)
			f++;
		else
			return (f>0) ? f : -1;
	}
	return (f>0) ? f : -1;
}
 
int main (void) {
	int p;
	int i=1; int N = 3;
	do {
		p = nfork (i) ;
		if (p != 0 )
			printf ("%d \n",p);
	} while ((p ==0) && (++i<=N));
	printf ("FIN \n");     
	return EXIT_SUCCESS;
} 
