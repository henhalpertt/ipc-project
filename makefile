# Server makefile 
#gcc -ansi -pedantic srv_main.c  protocol.c server_net.c srv_mng.c user.c usr_mng.c -L. -l:libdyn.so -o srvmain.out

TARGET= srvmain1.out
CC= gcc
CFLAGS= -ansi -pedantic
OBJS = srv_main.o srv_mng.o user.o usr_mng.o server_net.o protocol.o group.o grp_mng.o 

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -L. -l:libdyn.so
    
clnt_main.o: clnt_main.c
	$(CC) $(CFLAGS) -c clnt_main.c
	
srv_mng.o: srv_mng.c srv_mng.h
	$(CC) $(CFLAGS) -c srv_mng.c

user.o: user.c user.h
	$(CC) $(CFLAGS) -c user.c
	
usr_mng.o: usr_mng.c usr_mng.h
	$(CC) $(CFLAGS) -c usr_mng.c

protocol.o: server_net.c server_net.h
	$(CC) $(CFLAGS) -c server_net.c
	
group.o: group.c group.h
	$(CC) $(CFLAGS) -c group.c
	
grp_mng.o: grp_mng.c grp_mng.h
	$(CC) $(CFLAGS) -c grp_mng.c
	
clean:
	rm -f $(TARGET) $(OBJS)

