all: server sender receiver
sender: sender.c DieWithError.c
	gcc sender.c DieWithError.c -o sender
receiver: receiver.c DieWithError.c
	gcc receiver.c DieWithError.c -o receiver
server: server.c DieWithError.c
	gcc server.c DieWithError.c HandleTCPClient.c -o server
