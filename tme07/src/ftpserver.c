#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char **argv)
{
    if(argc != 3 || !strcmp(argv[1], "-h")) 
	{
        fprintf(stderr, "usage: %s ARGUMENTS...\n", argv[0]);
        return EXIT_FAILURE;
    }

	int port = atoi(argv[1]);
	//char *dirpath = argv[2];
	int sockd;
	
	sockd = socket(AF_INET, SOCK_STREAM, 0); 

	if(sockd == -1)
	{	
		perror("Socket error");
		exit(EXIT_FAILURE);
	}	
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr));

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr.s_addr   = INADDR_ANY;

	if(bind(sockd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind error");
		exit(EXIT_FAILURE);
	}

	if(listen(sockd, 1) == -1)
	{
		perror("listen error");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in client;
	unsigned int client_s = sizeof(client);
	if(accept(sockd, (struct sockaddr*) &client, &client_s) == -1)
	{
		perror("accept error");
		exit(EXIT_FAILURE);
	}

	char buf[512];

	while(1)
	{
		memset(buf, 0, 512);

		read(sockd, buf, 512);
		printf("received input |%s|\n", buf);

		char *next = strtok(buf, " ");
		if(next == NULL)
		{
			printf("received invalid input\n");
			continue;
		}
		else if(!strcmp(next, "UPLOAD"))
		{
			printf("received upload\n");
		}
		else if(!strcmp(next, "DOWNLOAD"))
		{
			printf("received download\n");
		}
		else if(!strcmp(next, "LIST"))
		{
			printf("received list\n");
		}
		sleep(1);
	}

    return 0;
}
