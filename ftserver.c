/***************************************************************
 * Filename: ftserver.c
 * Author: Brent Irwin
 * Date: 26 November 2017
 * Description: ftserver for CS 372 project 2
 * 		Adapted from the OTP_D program for CS_344 as a starting point
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#define SIZE 50000

// Global variables for easy access by error() and sendMessage()
int serverSocketFD, dataSocketFD;

/***************************************************************
Error function used for reporting issues
***************************************************************/
void error(const char *msg) {
	perror(msg);
	close(serverSocketFD);
	close(dataSocketFD);
	exit(1);
}

/***************************************************************
* openSocket(portNumber) is taken straight from CS 344, and it still works
***************************************************************/
int openSocket(int portNumber) {

	int listenSocketFD;
	struct sockaddr_in serverAddress;

	// Set up the address struct for this process (the server)
	// Clear out the address struct
		memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	// Create a network-capable socket
		serverAddress.sin_family = AF_INET;
	// Store the port number
		serverAddress.sin_port = htons(portNumber);
	// Any address is allowed for connection to this process
		serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	printf("Server open on %d\n", portNumber);

	return listenSocketFD;
}

/***************************************************************
* getRequest() clears the buffer and fills it with the newest request.
***************************************************************/
void getRequest(char* buffer, int socketFD) {
	int charsRead;
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer, SIZE-1, 0);
	if (charsRead < 0) error("ERROR reading from socket");
}

/***************************************************************
* pwdToBuffer(buffer) sends the current directory contents to the buffer.
* I used "Simulation of ls command" as a reference.
* https://codethecode.wordpress.com/2013/04/25/simulation-of-ls-command/
***************************************************************/
void pwdToBuffer(char* buffer) {
	memset(buffer, '\0', sizeof(buffer));

	DIR* p;
	struct dirent *d;
	p = opendir(".");
	while (d = readdir(p)) {
		// Do not use hidden files (that start with '.')
		if (d->d_name[0] != '.') {
			strcat(buffer, "\n");
			strcat(buffer, d->d_name);
		}
	}
}

/***************************************************************
* sendResponse() sends a size via the serverSocket and the message
* via the dataSocket
***************************************************************/
void sendResponse(char* buffer) {
	int charsRead;
	
	// Send length over server socket
	char* bufferLength;
	// integer to string from
	// https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
/*	printf(bufferLength, "%d", strlen(buffer));
	charsRead = send(serverSocketFD, bufferLength, strlen(bufferLength), 0);
	if (charsRead < 0) error("ERROR writing to client socket");
*/	
	// Send message over data socket
	charsRead = send(dataSocketFD, buffer, strlen(buffer), 0);
	if (charsRead < 0) error("ERROR writing to data socket");
}

/***************************************************************
* main()
***************************************************************/
int main(int argc, char *argv[])
{
	// Check arguments
	if (argc != 2) {
		error("Usage: ftserver <port_number>");
	}
	
	// Initialize variables
	int portNumber = atoi(argv[1]);
	int dataPort, socketFD, pid, charsRead;
	char buffer[SIZE];
	char command[8];

	// Open up the server socket
	socketFD = openSocket(portNumber);

	while(1) {
		// Accept
		serverSocketFD = accept(socketFD, NULL, NULL);
		if (serverSocketFD < 0) error("ERROR on accept");

		// Time to fork
		pid = fork();
		switch (pid) {

			// Error
			case -1: ;
				error("ERROR creating fork");
				exit(0);
				break;
			
			// Success
			case 0: ;
				
				// Receive command and port number
				getRequest(buffer, serverSocketFD);
				
				// For the life of me I can never remember how to use strtok()
				// This is my reference every time.
				// https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
				const char s[2] = ";";
				char *token;
				
				// Get command
				token = strtok(buffer, s);
				strcpy(command, token);

				// Get port number
				token = strtok(NULL, s);
				dataPort = atoi(token);

				// Open data socket
				socketFD = openSocket(dataPort);
				dataSocketFD = accept(socketFD, NULL, NULL);
				if (dataSocketFD < 0) error("ERROR on accept");	

				// Handle -l command (List?)
				if (!strcmp(command, "-l")) {
					pwdToBuffer(buffer);
					sendResponse(buffer);
				}
				
				// Handle -g command (Get?)
				else if (!strcmp(command, "-g")){
					// Get filename
					token = strtok(NULL, s);

					// Open said file and copy its contents to buffer
					FILE* fileToRead = fopen(token, "r");
					fgets(buffer, SIZE, fileToRead);
					fclose(fileToRead);

					// Send contents as response
					sendResponse(buffer);
				}
				
				// Bad command (which shouldn't happen)
				else {
					sendResponse("Error: received wrong command\n");
					error("Invalid command\n");
				}

				// Close data connection
				close(dataSocketFD);
				break;
		
			// This is supposed to be here...(?) 
			default: ;
		}
	}

	// We're all done here!
	close(serverSocketFD);
	return 0;
}
