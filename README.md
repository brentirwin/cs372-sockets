# CS 372 - Introudction to Computer Networks - Project 2
ftclient.py and ftserver.c

## Objectives
1. Implement 2-connection client-server network application
2. Practice using the sockets API
3. Refresh programming skills

## Instructions
1. Open two instances of Flip.
2. In one instance, type "make" and hit enter.
3. To start the server, type:
	```
	./ftserver <port_number>
	```
4. It will wait for a connection from ftclient.
5. To end the server, use ^C.
6. In the other instance, type:
	```
	python ftclient.py <port_number> <data_port> -l
	```
	to receive a directory listing at the server's pwd.
7. Or type
   ```
   python ftclient.py <port_number> <data_port> -g <filename>
   ```
   to receive a specific file from the server's directory.
