#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#include "rtx.h"
#include "kbcrt.h"
#include "userAPI.h"
#include "rtx_init.h"

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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char * sfilename = "keyboardBuffer";  //the name of the keyboard_memory file
char * cfilename = "crtBuffer";  //the name of the crt_memory file
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//**************************************************************************
// routine to clean up things before terminating main program
// This stuff must be cleaned up or we have child processes and shared
//	memory hanging around after the main process terminates

//this function should be called in terminate

void processP()
{

    ps("ProcessP Started");
    const tWait = 500000;
	MsgEnv* env;
	env = request_msg_env();

	env->sender_pid = current_process->pid;
    ps("Envelopes Allocated");


	while(1) {

        ps("Asking for Characters");
		get_console_chars (env);
		ps("process changed to ProcessP");

		env = receive_message();
		while(env==NULL) {
			usleep(tWait);
			env = (MsgEnv*)k_receive_message();
		}

		ps("processP got message from keyboard");

		send_console_chars(env);
		env = receive_message();

		while(env==NULL) {
			usleep(tWait);

			current_process = pid_to_pcb(P_PROCESS_ID);

			env = receive_message();
		}

		fflush(stdout);
		ps("processP got message from CRT\n");
		fflush(stdout);

	}
	release_msg_env(env);
}

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
      printf("Bad unlink during clean up.\n");
    } else {
      printf("Deleted the file used for shared CRT...\n");
    }

    // It is possible allocation failed in initialization
    // We try to ensure we don't try to free memory that was never allocated by always checking whether the pointer
    // is NULL or not
	int i;
	printf("Freeing All Queues\n");
	MsgEnvQ_destroy(free_env_queue);
	printf("Freeing PCBs\n");
	for (i = 0; i < PROCESS_COUNT; ++i)
	{
		// deallocate memory until we reach location where allocation may have failed
		if (pcb_list[i] == NULL)
			break;
		else
		{
			MsgEnvQ_destroy(pcb_list[i]->rcv_msg_queue);
			free(pcb_list[i]);
		}
	}


	printf("Freeing Messages and Their Data\n");
	for (i = 0; i < MSG_ENV_COUNT; ++i)
	{
		// deallocate memory until we reach location where allocation may have failed
		if (msg_list[i] == NULL)
			break;
		else
		{
			if (msg_list[i]->data == NULL)
			{
				free(msg_list[i]);
				break;
			}
			else
				free(msg_list[i]);
		}
	}
}

void crt_i_proc(int signum)
{
	int error = k_pseudo_process_switch(CRT_I_PROCESS_ID);
	if (error != SUCCESS)
		ps("CRT I PROC ERROR!");

	ps("Inside CRT I proc");


	if (signum == SIGUSR2) {
		if (DEBUG==1) {
			fflush(stdout);
			printf("Current PCB msgQ size is %i for process 1\n", MsgEnvQ_size(current_process->rcv_msg_queue) );
			ps("Got SIGUSR2");
		}
			MsgEnv* envTemp = NULL;
			envTemp = MsgEnvQ_dequeue(displayQ);
			envTemp->msg_type = DISPLAY_ACK;
			k_send_message(P_PROCESS_ID,envTemp);
			ps("Display ACK sent by crt");
			k_return_from_switch();
			return;

	}

	MsgEnv* env = (MsgEnv*)k_receive_message();

	outputbuf command;

	if (env==NULL) {
		env = (MsgEnv*)k_receive_message();
	}

	if (DEBUG==1) {
		fflush(stdout);
		printf("Message received by crt i proc\n");
		fflush(stdout);
		printf("Current PCB msgQ size is %i for process 1\n", MsgEnvQ_size(current_process->rcv_msg_queue) );
		printf("The message data section holds \"%s\" \n",env->data);
		fflush(stdout);
	}

	//in_mem_p_crt->outdata[0] = env->data;
	strcpy(in_mem_p_crt->outdata,env->data);

	if (DEBUG==1) {
		printf("The message data section holds \"%s\" \n",in_mem_p_crt->outdata);
	}
	MsgEnvQ_enqueue(displayQ,env);
	//displayQueue = env;
	in_mem_p_crt->ok_flag = 1;

	k_return_from_switch();
	return;
}

void kbd_i_proc(int signum)
{
	int error = k_pseudo_process_switch(KB_I_PROCESS_ID);
	if (error != SUCCESS)
		ps("KBD I PROC ERROR!");

	ps("Inside keyboard I proc");
	MsgEnv* env = (MsgEnv*)k_receive_message();

	if (env != NULL) {

		fflush(stdout);
		ps("Envelope recognized by kbd_i_proc");
		fflush(stdout);


		while (in_mem_p_key->ok_flag==0);

		//if (in_mem_p_key->indata[0] != '\0') return;
		//strcpy(env->data,in_mem_p_key->indata);

		//env->data = "some data\0";
		memcpy(env->data,in_mem_p_key->indata,in_mem_p_key->length + 1);

		//k_send_message(env->sender_pid,env);
		k_send_message(2,env);
		if (DEBUG==1) {
			fflush(stdout);
			printf("Keyboard sent message\n");
			fflush(stdout);
		}
		in_mem_p_key->ok_flag = 0;
		k_return_from_switch();
		return;
	}
	k_return_from_switch();
	return;
}

void die(int signal)
{
	cleanup();
	printf( "Ending main process ...\n" );
	exit(0);
}

//**************************************************************************
int main()
{
	free_env_queue = MsgEnvQ_create();
	displayQ = MsgEnvQ_create();

	if (init_all_lists() != SUCCESS)
	{
		printf("Failed to initialize the process and message envelope list. Exiting the OS.\n");
		cleanup();
	}

	current_process = pid_to_pcb(P_PROCESS_ID);
	current_process->state = NEVER_BLK_RCV;
	prev_process = current_process;

	//signals
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

	// signal from keyboard reader is SIGUSR1 (user-defined signal)
	// When there is input from the keyboard, call the kbd_i_proc() routine
	sigset(SIGUSR1,kbd_i_proc);
    sigset(SIGUSR2,crt_i_proc);

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

    processP();

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
} // main
