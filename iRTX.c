
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>			// getpid() definition

#include "rtx.h"
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>



// globals
inputbuf  * in_mem_p_key;	// pointer to structure that is the shared memory
outputbuf * in_mem_p_crt;	// pointer to structure that is the shared memory
int in_pid_keyboard;		// pid of keyboard child process
int in_pid_crt;				// pid of crt child process
caddr_t mmap_ptr_keyboard;
caddr_t mmap_ptr_crt;
int bufsize = BUFFERSIZE;
int fid, fid2, status;		//used to create the shared memory

int numOfTicks = 0;
int displayClock = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char * sfilename = "keyboardBuffer";  //the name of the keyboard_memory file
char * cfilename = "crtBuffer";  //the name of the crt_memory file
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//**************************************************************************
// routine to clean up things before terminating main program
// This stuff must be cleaned up or we have child processes and shared
//	memory hanging around after the main process terminates

//this function should be called in terminate
void cleanup()
{

	// terminate child process(es)
	kill(in_pid_keyboard,SIGINT);
	kill(in_pid_crt,SIGINT);

	// remove shared memory segment and do some standard error checks
	status = munmap(mmap_ptr_keyboard, bufsize);
    if (status == -1){
      printf("Bad munmap during cleanup\n");
    } else {
      printf("Unmapped shared memory for keyboard...\n");
    }
    
    status = munmap(mmap_ptr_crt, bufsize);
    if (status == -1){
      printf("Bad munmap during cleanup\n");
    } else {
      printf("Unmapped shared memory for CRT...\n");
    }
    
	// close the temporary mmap file 
    status = close(fid); 
    if (status == -1){
      printf("Bad close of temporary mmap file during cleanup\n");
    } else {
      printf("Closed the file used for shared keyboard...\n");
    }
    
    status = close(fid2); 
    if (status == -1){
      printf("Bad close of temporary mmap file during cleanup\n");
    } else {
      printf("Closed the file used for shared CRT...\n");
    }
    
	// unlink (i.e. delete) the temporary mmap file
    status = unlink(sfilename); 
    if (status == -1){
      printf("Bad unlink during claeanup.\n");
    } else {
      printf("Deleted the file used for shared keyboard...\n");
    }
    
    status = unlink(cfilename); 
    if (status == -1){
      printf("Bad unlink during claeanup.\n");
    } else {
      printf("Deleted the file used for shared CRT...\n");
    }
}


//**************************************************************************
// routine to call before exitting
// This routine gets called when certain signals occur

void die(int signal)
{
	cleanup();
	printf( "Ending main process ...\n" );
	exit(0);
}

// kbd_handler
// Called by signal SIGUSR1 from keyboard reader process



void kbd_handler(int signum)
{
	inputbuf command;	
	
	// copy input buffer
	if (in_mem_p_key->indata[0] != '\0')
	{
	    strcpy(command.indata,in_mem_p_key->indata); 

	    // we should parse the input string and execute the command given,
	    //  but for now we just echo the input
	    //

	    if (!strcmp(command.indata,"t")) {
		die(SIGINT);
	    }

	    if (!strcmp(command.indata,"cd")) {
		//printf("Command Recognized!\n");
		int seconds = 0;
		int mins = 0;
		int hours = 0;

		int numLeft = numOfTicks;

		hours = numLeft/3600;
		numLeft = numLeft%3600;

		mins = numLeft/60;
		numLeft = numLeft%60;

		seconds = numLeft;

		printf("Time Elapsed: %.2d:%.2d:%.2d\n",hours,mins,seconds);
		displayClock = 1;
	    }

	    if (!strcmp(command.indata,"ct")) {
		displayClock = 0;
	    }

	    if (command.indata[0] == 'c' && command.indata[1] == ' ') {
		//for now to set the time, you can only put in one space after
		//typing in c and then specify the hh:mm:ss
		numOfTicks = 0;

		
		
		int hour = atoi(&(command.indata[2]));
		int min = atoi(&(command.indata[5]));
		int sec = atoi(&(command.indata[8]));

		printf("%.2d:%.2d:%.2d",hour,min,sec);

		numOfTicks = hour*3600 + min*60 + sec;

	    }
 
	    //printf("Keyboard input was: %s\n",command.indata);
	    in_mem_p_key->ok_flag = 0;  // tell child that the buffer has been emptied
	    
	    
	}

}

void tick_handler(int signum) {
	
	numOfTicks++;
	if (displayClock == 1) {
		int seconds = 0;
		int mins = 0;
		int hours = 0;

		int numLeft = numOfTicks;

		hours = numLeft/3600;
		numLeft = numLeft%3600;

		mins = numLeft/60;
		numLeft = numLeft%60;

		seconds = numLeft;

		printf("Time Elapsed: %.2d:%.2d:%.2d\n",hours,mins,seconds);
	}
	alarm(1);

}

//**************************************************************************
int main()
{

	
	// catch signals so we can clean up everything before exitting
	// signals defined in /usr/include/signal.h
	// e.g. when we recieved an interrupt signal SIGINT, call die()
	sigset(SIGINT,die);		// catch kill signals 
	sigset(SIGBUS,die);		// catch bus errors
	sigset(SIGHUP,die);		
	sigset(SIGILL,die);		// illegal instruction
	sigset(SIGQUIT,die);
	sigset(SIGABRT,die);
	sigset(SIGTERM,die);
	sigset(SIGSEGV,die);	// catch segmentation faults

	sigset(SIGALRM,tick_handler);

    alarm(1);

	// signal from keyboard reader is SIGUSR1 (user-defined signal)
	// When there is input from the keyboard, call the kbd_handler() routine
	sigset(SIGUSR1,kbd_handler);	
	
	
	/* Create a new mmap file for read/write access with permissions restricted
	to owner rwx access only */
	fid = open(sfilename, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755 );
	if (fid < 0){
		printf("Bad Open of mmap file <%s>\n", sfilename);
		exit(0);
	};

	fid2 = open(cfilename, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755 );
	if (fid2 < 0){
		printf("Bad Open of mmap file <%s>\n", cfilename);
		exit(0);
	};


	// make the file the same size as the buffer 
	status = ftruncate(fid, bufsize ); 
	if (status){
		printf("Failed to ftruncate the file <%s>, status = %d\n", sfilename, status );
		exit(0);
	}
	
	status = ftruncate(fid2, bufsize ); 
	if (status){
		printf("Failed to ftruncate the file <%s>, status = %d\n", cfilename, status );
		exit(0);
	}


	// pass parent's process id and the file id to child
	char childarg1[20], childarg2[20]; // arguments to pass to child process(es)
	int mypid = getpid();			// get current process pid
  
	sprintf(childarg1, "%d", mypid); // convert to string to pass to child
    sprintf(childarg2, "%d", fid);   // convert the file identifier
  

	// now start doing whatever work you are supposed to do
	// in this case, do nothing; only the keyboard handler will do work
	
	printf("\nWelcome to the CCI, the OS is at your disposal: \n");

	// create the keyboard reader process
	// fork() creates a second process identical to the current process,
	// except that the "parent" process has in_pid = new process's ID,
	// while the new (child) process has in_pid = 0.
	// After fork(), we do execl() to start the actual child program.
	// (see the fork and execl man pages for more info)

	in_pid_keyboard = fork();
	if (in_pid_keyboard == 0)	// is this the child process ?
	{
		execl("./keyboard", "keyboard", childarg1, childarg2, (char *)0);
		// should never reach here
		fprintf(stderr,"kernal: can't exec keyboard, errno %d\n",errno);
		cleanup();
		exit(1);
	};
	
	// pass parent's process id and the file id to child CRT
  
	sprintf(childarg1, "%d", mypid); // convert to string to pass to child
    sprintf(childarg2, "%d", fid2);   // convert the file identifier
	
	in_pid_crt = fork();
	if (in_pid_crt == 0)	// is this the child process ?
	{
		execl("./crt", "crt", childarg1, childarg2, (char *)0);
		// should never reach here
		fprintf(stderr,"kernal: can't exec crt, errno %d\n",errno);
		cleanup();
		exit(1);
	};
	
	// the parent process continues executing here

	// sleep for a second to give the child process time to start
	sleep(1);

	// allocate a shared memory region using mmap 
	// the child process also uses this region
	
	mmap_ptr_keyboard = mmap((caddr_t) 0,   /* Memory location, 0 lets O/S choose */
		    bufsize,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
	if (mmap_ptr_keyboard == MAP_FAILED){
		printf("Parent's memory map has failed for keyboard, about to quit!\n");
		die(0);  // do cleanup and terminate
	};
	
	in_mem_p_key = (inputbuf *) mmap_ptr_keyboard;   // pointer to shared memory
	// we can now use 'in_mem_p' as a standard C pointer to access 
	// the created shared memory segment 
	
	in_mem_p_key->ok_flag = 0;
	
	
	// allocate a shared memory region using mmap 
	// the child process also uses this region
	
	mmap_ptr_crt = mmap((caddr_t) 0,   /* Memory location, 0 lets O/S choose */
		    bufsize,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fid2,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
	if (mmap_ptr_crt == MAP_FAILED){
		printf("Parent's memory map has failed for CRT, about to quit!\n");
		die(0);  // do cleanup and terminate
	};
	
	in_mem_p_crt = (outputbuf *) mmap_ptr_crt;   // pointer to shared memory
	// we can now use 'in_mem_p' as a standard C pointer to access 
	// the created shared memory segment 
	

	in_mem_p_crt->ok_flag = 0;
	
	
	while (1);

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
} // main
