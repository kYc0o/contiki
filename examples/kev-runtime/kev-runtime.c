#include "contiki.h"
#include "dev/serial-line.h"
#include "rtkev.h"
#include <stdio.h>

const ComponentInterface helloWorld;
const ComponentInterface helloWorld_Second;

DECLARE_KEV_TYPES(2, &helloWorld, &helloWorld_Second)

PROCESS(kevRuntime, "KevRuntime");
AUTOSTART_PROCESSES(&kevRuntime);
PROCESS_THREAD(kevRuntime, ev, data)
{
	static uint8_t buf[257];
	static uint32_t received = 0;
	static int32_t processingFile = 0;
	PROCESS_BEGIN();
	
	REGISTER_KEV_TYPES_NOW();

	printf("Kevoree server started !\n");
	while(1) {

		PROCESS_YIELD();
		if (ev == serial_line_event_message) {
			if (!strcmp(data, "ls")) {
				printf("ls, Unimplemented\n");
			}
			else if (strstr(data, "createInstance") == (int)data) {
				printf("Executing createInstance\n");
				char* tmp = strstr(data, " ");
        		tmp++;
				char* tmp2 = strstr(tmp, " ");
				*tmp2 = 0;
				printf("\tParam 0 : %s\n", tmp);
        		tmp2++;
				printf("\tParam 1 : %s\n", tmp2);
				void* ins;
				createInstance(tmp, tmp2, &ins);
				printf("\tThe created instance has adress %p\n", ins);	
			}
			else if (!strcmp(data, "format")) {
				/* format the flash */
				printf("Formatting, Unimplemented\n");
			}
			else if (strstr(data, "cat") == (int)data) {
				printf("cat, Unimplemented\n");
			}
			else if (strstr(data, "loadelf") == (int)data) {
				printf("loadelf, Unimplemented\n");
			}
			else if (strstr(data, "rm") == (int)data) {
				printf("rm, Unimplemented\n");
			}
			else if (strstr(data, "upload") == (int)data) {
				printf("upload, Unimplemented\n");
			}
			else if (!strcmp(data, "endupload")) {
				printf("enduoload, Unimplemented\n");
			}
			else if (processingFile) {
				int n = strlen(data);
				int r = decode(data, n, buf);
				received += r;
			//cfs_write(fdFile, buf, r);
			}
			else  {
			printf("%s (%lu bytes received)\n", (char*)data, received);
			}
		}
	}

	PROCESS_END();
}
