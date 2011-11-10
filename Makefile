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
OBJS = crt iRTX keyboard

all: crt iRTX keyboard clean

#   programs
#   these commands link the object files and libraries into executables 

iRTX:iRTX.o userAPI.o MsgEnvQueue.o kernal.o
	@echo linking iRTX.o file
	$(LINK) $(LNFLAGS) iRTX.o userAPI.o MsgEnvQueue.o kernal.o -o iRTX

crt:crt.o
	@echo linking crt.o file
	$(LINK) $(LNFLAGS) crt.o -o crt
	
keyboard:keyboard.o
	@echo linking keyboard.o file
	$(LINK) $(LNFLAGS) keyboard.o -o keyboard


#compile the source code into object files

crt.o: crt.c 
	@echo building crt.c
	$(CC) $(CFLAGS) crt.c
	
MsgEnvQueue.o:	MsgEnvQueue.c 
	@echo building MsgEnvQueue
	$(CC) $(CFLAGS) MsgEnvQueue.c
	    
kernal.o: kernal.c 
	@echo building kernal
	$(CC) $(CFLAGS) kernal.c

keyboard.o: keyboard.c
	@echo building keyboard.c
	$(CC) $(CFLAGS) keyboard.c 

iRTX.o: iRTX.c rtx.h
	@echo building iRTX.c
	$(CC) $(CFLAGS) iRTX.c
	
userAPI.o: userAPI.c 
	@echo building userAPI.c
	$(CC) $(CFLAGS) userAPI.c
	
processP.o: processP.c
	@echo building processP.c
	$(CC) $(CFLAGS) processP.c
		
clean:
	@echo Cleaning object files
	rm *.o
	#rm ${OBJS}
