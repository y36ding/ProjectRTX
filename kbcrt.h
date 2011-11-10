#ifndef _KB_CRT_HEADER_
#define _KB_CRT_HEADER_

#define BUFFERSIZE 100;

// structures for shared memory
#define MAXCHAR 80	

typedef struct {
	int ok_flag;
	char indata[MAXCHAR];
	int length;
} inputbuf;

typedef struct {
	int ok_flag;
	char outdata[MAXCHAR];
	int length;
} outputbuf;

#endif
