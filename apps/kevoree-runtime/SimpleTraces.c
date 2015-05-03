#include "SimpleTraces.h"

#include "cfs/cfs.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 120

/** Read a new trace from the file
	@param fd is the file descriptor
	@param trace pointer to a location that will held the trace information, the caller is responsible for it
	@return 0 if it could read a trace, a negative value otherwise
  */
int nextTrace(int fd, struct SimpleTrace* trace)
{
	static int read = 0;
	static char* buff = NULL;
	int n;
	char* newline;
	// check if the trace address is valid
	if (!trace) return -1;
	// check if we must allocate some memory
	if (!buff) buff = (char*)malloc(BUFF_SIZE);

	n = cfs_read(fd, &buff[read], BUFF_SIZE - read);

	// couldn't read
	if (!n && !read) goto stop_reading_traces;

	// adjunt the buffer length
	n += read;

	// find new line
	newline = strstr(buff, "\n");
	if (!newline) goto stop_reading_traces;

	// mark where the next line begins
	*newline = 0;
	newline++;
	buff[n] = 0;
	read = n - (newline - buff);
	
	// parse current line
	if (buff[0] != TRACE_NEW_INSTANCE)
		sscanf(buff, "%c %s %s", &trace->type, trace->nodeName, trace->param0.deployUnit);
	else
		sscanf(buff, "%c %s %s %s", &trace->type, trace->nodeName, trace->param0.deployUnit, trace->param1.instanceName);

	strcpy(buff, newline);

	return 0;

stop_reading_traces:
	free(buff);
	buff = NULL;
	return -1;
}
