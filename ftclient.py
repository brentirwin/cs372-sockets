"""
Filename: ftclient.py
Author: Brent Irwin
Date: 26 November 2017
Description: ftclient app for CS 372

I started with my chatserve python app and modified from there,
since that was the only other thing I've ever written in Python.

Just like in chatserve, the official Python Socket Programming HOWTO
was extremely useful to me.

Citations will be numbered and listed like a bibliography at the bottom.

"""

import sys
import socket
import time

# Connects to the server via either the connection or data port
def initiateContact(port):
	# The following lines are from Socket Programming HOWTO [1]
	# StackOverflow link [2] taught me how to check socket connection
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect(('localhost', port))
	except:
		error("Error connecting to port %d" % (port), 1)
	print "Connected to port %d" % (port)
	return s

# Send the request to the connection server
def makeRequest(s, request):
	# send() and close() from [1]
	try:
		s.send(request)
	except:
		error("Error making request", 1)

# Get the file from -g from the data server
def receiveFile(server_socket, data_socket):
	# Get file contents
	file_data = getResponse(server_socket, data_socket)
	
	# I want to do something like strtok() in C to append "copy" to the
	# filename, but before the extension, using '.' as a delimiter. [3]
	filename_split = filename.split('.', 1)
	new_filename = filename_split[0] + '_copy.' + filename_split[1]

	# Make new file and put the contents in it
	# I used CS 344 MyPython program as a reference for basic file handling
	f = open(new_filename, "w+")
	f.write(file_data)
	f.close

	# Print success
	print new_filename + ' successfully created!'

# Handle errors
def error(msg, value):
	print "%s" % (msg)
	sys.exit(value)

# Get data from host
def getResponse(server_socket, data_socket):
	# Check the server_socket for size/acknowledgment
#	size = int(server_socket.recv(8))
#	print "received %s" % (size)
	# Get the list and print it
#	if size == 0:
#		error("Server acknowledged request but did not process response", 2)
	response = data_socket.recv(50000)
	return response

############
# Main
############

# Check input arguments
if ((sys.argv[3] == '-l' and len(sys.argv) != 4) or
	(sys.argv[3] == '-g' and len(sys.argv) != 5) or
	(sys.argv[3] != '-l' and sys.argv[3] != '-g')):
	error("USAGE: python ftclient.py <server_port> <data_port> <command> <filename (-g only)>", 4)

# Interpret arguments
server_port = int(sys.argv[1])
data_port = int(sys.argv[2])
command = sys.argv[3]
filename = ""
if command == '-g':
	filename = sys.argv[4]

# Set up the server_port socket
server_socket = initiateContact(server_port)

# Send the command and the return socket in the client
if command == '-g':
	request = '-g' + ';' + sys.argv[2] + ';' + filename
	makeRequest(server_socket, request)
if command == '-l':
	request = '-l' + ';' + sys.argv[2]
	makeRequest(server_socket, request)

# Give the server time to set up the new connection
time.sleep(.1)

# Set up the data_port socket, just like above
data_socket = initiateContact(data_port)

# -l command (list?)
if command == '-l':
	# print file list
	file_list = getResponse(server_socket, data_socket)
	print file_list

# -g command (get?)
else:
	receiveFile(server_socket, data_socket)

# Close 'em all up
server_socket.close()
data_socket.close()

"""
Works Cited

[1] Official Python Socket Programming HOWTO
https://docs.python.org/2/howto/sockets/html

[2] Stack Overflow - Testing socket connection in Python
https://stackoverflow.com/questions/177389/testing-socket-connection-in-python

[3] Mkyong - Python - How to split a String
https://www.mkyong.com/python/python-how-to-split-a-string/

"""
