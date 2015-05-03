#ifndef __SIMPLE_TRACES__
#define __SIMPLE_TRACES__

#define TRACE_INST_DEPLOY_UNIT 'i'
#define TRACE_REM_DEPLOY_UNIT 'r'

#define TRACE_NEW_INSTANCE 'n'
#define TRACE_DEL_INSTANCE 'd'

#define TRACE_START_INSTANCE 's'
#define TRACE_STOP_INSTANCE 't'

#define MAX_NODE_NAME 30

struct SimpleTrace {
	struct SimpleTrace* next;
	char type;
	char nodeName[MAX_NODE_NAME];
	union {
		char deployUnit[30]; // this is valid for TRACE_INST_DEPLOY_UNIT and TRACE_REM_DEPLOY_UNIT
		char kevType[30]; // this is valid for TRACE_NEW_INSTANCE
		char instanceName[30]; // this is valid for TRACE_DEL_INSTANCE, TRACE_START_INSTANCE and TRACE_STOP_INSTANCE
	} param0;
	union {
		char instanceName[30]; // this is valid for TRACE_NEW_INSTANCE
	} param1;
};

/** Read a new trace from the file
	@param fd is the file descriptor
	@param trace pointer to a location that will held the trace information, the caller is responsible for it
	@return 0 if it could read a trace, a negative value otherwise
  */
int nextTrace(int fd, struct SimpleTrace* trace);

#endif
