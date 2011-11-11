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

			current_process = (pcb*)pid_to_pcb(P_PROCESS_ID);

			env = receive_message();
		}

		fflush(stdout);
		ps("processP got message from CRT\n");
		fflush(stdout);

	}
	release_msg_env(env);
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

	if (init_globals() != SUCCESS) {
		printf("Globals failed to initilize!");
		cleanup();
	}

	if (init_all_lists() != SUCCESS) {
		printf("Failed to initialize the process and message envelope list. Exiting the OS.\n");
		cleanup();
	}

    if (init_mmaps() != SUCCESS) {
    	printf("Failed to init mmaps.\n");
    	cleanup();
    }

    processP();

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
} // main
