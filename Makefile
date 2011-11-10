# Makefile for demo Software
#
# To run the makefile, type "make"
#
# To learn more about makefiles, see the info under uwinfo:
#	 uwinfo / facilities / computers / supported software / unix
# or read the man pages
#


### User Modifiable Macros ###
#
# CC represents the compiler, LINK represents the linker
# CFLAGS represents the compiler command line parameters
# 	-c means compile only, i.e. create an object file
#	-g means add symbol table information for debugging purposes
# LNFLAGS represents the linker command line parameters
#
CC = gcc
CFLAGS = -c -g
LINK = gcc
LNFLAGS = -g


### Program list ###
##  list all of the executables to be created with the makefile


# default target: create all of the programs listed
#
OBJS = crt init iproc kernal keyboard queues iRTX userAPI userProcesses

all: crt init iproc kernal keyboard queues iRTX userAPI userProcesses clean

#   programs
#   these commands link the object files and libraries into executables 

crt:crt.o
	@echo linking crt.o file
	$(LINK) $(LNFLAGS) -o $@ crt.o -lrt
	
init:init.o
	@echo linking init.o file
	$(LINK) $(LNFLAGS) -o $@ init.o -lrt
	
iproc:iproc.o
	@echo linking iproc.o file
	$(LINK) $(LNFLAGS) -o $@ iproc.o -lrt
	

kernal:kernal.o
	@echo linking kernal.o file
	$(LINK) $(LNFLAGS) -o $@ kernal.o -lrt
		
keyboard: keyboard.o
	@echo linking keyboard.o file
	$(LINK) $(LNFLAGS) -o $@ keyboard.o -lrt

queues:queues.o
	@echo linking queues.o file
	$(LINK) $(LNFLAGS) -o $@ queues.o -lrt
	
iRTX:iRTX.o
	@echo linking iRTX.o file
	$(LINK) $(LNFLAGS) -o $@ iRTX.o -lrt

userAPI:userAPI.o
	@echo linking userAPI.o file
	$(LINK) $(LNFLAGS) -o $@ userAPI.o -lrt
	
userProcesses:userProcesses.o
	@echo linking userProcesses.o file
	$(LINK) $(LNFLAGS) -o $@ userProcesses.o -lrt

#compile the source code into object files

crt.o: crt.c kbcrt.h
	@echo building crt.c
	$(CC) $(CFLAGS) crt.c

init.o: init.c
	@echo building init.c
	$(CC) $(CFLAGS) init.c
	
iproc.o: iproc.c
	@echo building iproc.c
	$(CC) $(CFLAGS) iproc.c

kernal.o: kernal.c rtx.h
	@echo building kernal.c
	$(CC) $(CFLAGS) kernal.c

keyboard.o: keyboard.c kbcrt.h
	@echo building keyboard.c
	$(CC) $(CFLAGS) keyboard.c 

queues.o: queues.c
	@echo building queues.c
	$(CC) $(CFLAGS) queues.c
	
iRTX.o: iRTX.c
	@echo building iRTX.c
	$(CC) $(CFLAGS) iRTX.c
	
userAPI.o: userAPI.c
	@echo building userAPI.c
	$(CC) $(CFLAGS) userAPI.c
	
userProcesses.o: userProcesses.c
	@echo building userProcesses.c
	$(CC) $(CFLAGS) userProcesses.c
	
clean:
	@echo Cleaning object files
	rm *.o
	#rm ${OBJS}

