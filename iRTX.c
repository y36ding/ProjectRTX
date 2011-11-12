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
#include "kernal.h"


void processP()
{

	const tWait = 500000;
	MsgEnv* env;
	env = request_msg_env();


	while(1) {
		// Request keyboard input
		get_console_chars (env);

		// Check if keyboard i proc sent a confirmation message
		env = receive_message();
		while(env==NULL) {
			usleep(tWait);
			env = k_receive_message();
			if (env != NULL && env->msg_type == CONSOLE_INPUT)
			{
#if DEBUG
				printf("Keyboard Input Acknowledged");
#endif
			}
		}

		// Send the input to CRT
		send_console_chars(env);

		// Check if CRT displayed
		env = (MsgEnv *)receive_message();
		while(env==NULL) {
			usleep(tWait);
			env = (MsgEnv *)receive_message();
			if (env != NULL && env->msg_type == DISPLAY_ACK)
			{
#if DEBUG
				printf("CRT Display Acknowledged");
#endif
			}
		}
	}
	release_msg_env(env);
}


void die(int signal)
{
	cleanup();
	fflush(stdout);
	printf( "Ending main process ...\n" );
	fflush(stdout);
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
