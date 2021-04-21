// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";

    int nobody_id = 65534;

    printf("execve=0x%p\n", execve);

	//printf("SERVER_CHILD: %s\n", getenv("SERVER_CHILD"));
    if(getenv("SERVER_CHILD") == NULL){	
		// Creating socket file descriptor
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    	{
            perror("socket failed");
            exit(EXIT_FAILURE);
    	}

    	// Attaching socket to port 80
    	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    	{
        	perror("setsockopt");
        	exit(EXIT_FAILURE);
    	}
    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = INADDR_ANY;
    	address.sin_port = htons( PORT );

		// Forcefully attaching socket to the port 80
		if (bind(server_fd, (struct sockaddr *)&address,
									 sizeof(address))<0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(server_fd, 3) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
						   (socklen_t*)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		pid_t child = fork();
		if (child == 0) { 		// child code
			//printf("new_socket value: %d\n", new_socket);
			if (setuid(nobody_id) == -1) {
					perror("setuid(65534)\n");
			}
			char c = new_socket + '0';
			char *p = &c;

			setenv("SERVER_CHILD", "True", 1);
			setenv("SOCKET_VAL", p, 1);
			
			execvp("./server", (char * const *)argv);
		}
		else if (child > 0) { 	// Parent code
			child = wait(NULL);
			send(new_socket , hello , strlen(hello) , 0 );
			printf("Hello message sent\n");
		}

		return 0;
	}
	else {//if (getenv("SERVER_CHILD") == "True"){
		new_socket = atoi(getenv("SOCKET_VAL"));
		//printf("reading from socket: %d\n", new_socket);
		valread = read( new_socket , buffer, 1024);
		printf("%s\n",buffer );

		return 0;
	}
}
