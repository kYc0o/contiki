/*
 * kev-runtime.c
 * This file is part of Kevoree-Contiki
 *
 * Copyright (C) 2015 - Inti Gonzalez-Herrera
 *
 * Kevoree-Contiki is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Kevoree-Contiki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kevoree-Contiki. If not, see <http://www.gnu.org/licenses/>.
 */


#include "contiki.h"
#include "dev/serial-line.h"
#include "cfs/cfs.h"
#include "loader/elfloader.h"
#include "jsonparse.h"

#include "rtkev.h"
#include "shell_group.h"
#include "deluge_group.h"
#include "udpComponent.h"
#include "ShellBasedDeployUnitRetriever.h"

#include "ContainerRoot.h"
#include "JSONModelLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* list of retrieving strategies */
#define NAIVE_UDP_BASED_RETRIEVER 0
#define SMART_UDP_BASED_RETRIEVER 1
#define SHELL_BASED_RETRIEVER 2

/* strategy to use */
#define DEPLOY_UNIT_RETRIEVER_STRATEGY SHELL_BASED_RETRIEVER

/* built-in kevoree types */
extern const GroupInterface ShellGroupInterface;
extern struct process shellGroupP;

/* declaring built-in instances */
DECLARE_KEV_TYPES(3, &ShellGroupInterface, &DelugeRimeGroupInterface, &UDPClientInterface)
struct Built_In_Instance {
	const char* type_name;
	const char* instance_name;
	const bool enabled;
};
static const struct Built_In_Instance built_in_instances [] ={
	{
		.type_name = "ShellGroupType",
		.instance_name = "shellGroup0",
		.enabled = true
	},
	{
		.type_name = DELUGE_GROUP_TYPENAME,
		.instance_name = "delugeGroup0",
		.enabled = true
	},
	{
		.type_name = UDP_CLIENT_COMPONENT_TYPE_NAME,
		.instance_name = "udpClient0",
		.enabled = false
	}
};

PROCESS(kevRuntime, "KevRuntime");
AUTOSTART_PROCESSES(&kevRuntime);
PROCESS_THREAD(kevRuntime, ev, data)
{
	static uint8_t buf[257];
	static uint8_t processingFile = 0;
	static uint32_t received = 0;
	static struct cfs_dirent dirent;
	static struct cfs_dir dir;
	static uint32_t fdFile;
	static char *filename;
	static int i;
	
	void* instTmp;
	
	int result;

	PROCESS_BEGIN();

	/* definitively we want to dynamically load modules */
	elfloader_init();
	
	/* initialize Kevoree Runtime */
	result = 1;
#if DEPLOY_UNIT_RETRIEVER_STRATEGY == NAIVE_UDP_BASED_RETRIEVER
	
#elif DEPLOY_UNIT_RETRIEVER_STRATEGY == SMART_UDP_BASED_RETRIEVER

#elif DEPLOY_UNIT_RETRIEVER_STRATEGY == SHELL_BASED_RETRIEVER
	result = initKevRuntime(&shellBasedRetriever);
#endif
	if (result != 0) {
		printf("Runtime initialization error\n");
		PROCESS_EXIT();
	}
	
	/* let's register core components */
	REGISTER_KEV_TYPES_NOW();

	printf("Kevoree server started !\n");

	/* create built-in types */
	for (i = 0 ; i < sizeof(built_in_instances)/sizeof(struct Built_In_Instance); i++) {
		// skip instance if it is disabled
		if (!built_in_instances[i].enabled) continue;
		
		/* install instance */
		do {
			static struct etimer et;
			/* Listen for announcements every one second. */
			etimer_set(&et, CLOCK_SECOND * 1);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			instTmp = NULL;
			createInstance(built_in_instances[i].type_name, built_in_instances[i].instance_name, &instTmp);
		} while (instTmp == NULL);
		printf("The instance %s is located at %p\n", built_in_instances[i].instance_name, instTmp);
		startInstance(built_in_instances[i].instance_name);
	}

	while(1) {

		PROCESS_WAIT_EVENT();
		if (ev == serial_line_event_message) {
			if (!strcmp(data, "ls")) {
				if(cfs_opendir(&dir, ".") == 0) {
					while(cfs_readdir(&dir, &dirent) != -1) {
						printf("File: %s (%ld bytes)\n",
								dirent.name, (long)dirent.size);
					}
					cfs_closedir(&dir);
				}
			}
			else if (!strcmp(data, "pushModel")) {
				//notifyNewModel(NULL);
				process_post(&shellGroupP, NEW_MODEL_IN_JSON, NULL);
			}
			else if (strstr(data, "createInstance") == data) {
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
				printf("\tThe created instance has address %p\n", ins);
			}
			else if (!strcmp(data, "format")) {
				/* format the flash */
				printf("Formatting\n");
				printf("It takes around 3 minutes\n");
				printf("...\n");

				fdFile = cfs_coffee_format();
				printf("Formatted with result %ld\n", fdFile);
			}
			else if (strstr(data, "cat") == data) {
				int n, jj;
				char* tmp = strstr(data, " ");
				tmp++;
				fdFile = cfs_open(tmp, CFS_READ);
				if (fdFile < 0) printf("error opening the file %s\n", tmp);
				while ((n = cfs_read(fdFile, buf, 60)) > 0) {
					for (jj = 0 ; jj < n ; jj++) printf("%c", (char)buf[jj]);
				}
				printf("DONE WITH CAT\n");
				cfs_close(fdFile);
				if (n!=0)
					printf("Some error reading the file\n");
			}
			else if (strstr(data, "rm") == data) {
				int n, jj;
				char* tmp = strstr(data, " ");
				tmp++;
				cfs_remove(tmp);
			}
			else if (strstr(data, "startInstance") == data) {
				filename = strstr(data, " ");
				filename++;
				startInstance(filename);
			}
			else if (strstr(data, "stopInstance") == data) {
				filename = strstr(data, " ");
				filename++;
				stopInstance(filename);
			}
			else if (strstr(data, "removeInstance") == data) {
				filename = strstr(data, " ");
				filename++;
				removeInstance(filename);
			}
			else if (strstr(data, "loadelf") == data) {
				filename = strstr(data, " ");
				filename++;
				loadElfFile(filename);
			}
			else if (strstr(data, "uploadUnit") == data) {
				char* tmp = strstr(data, " ");
				tmp++;
				fdFile = cfs_open(tmp, CFS_READ | CFS_WRITE);
				printf("Uploading deploy unit %s\n", tmp);
				processingFile = 1;
				filename = strdup(tmp);
			}
			else if (!strcmp(data, "enduploadUnit")) {
				cfs_close(fdFile);
				printf("File %s uploaded (%ld bytes)\n", filename, received);
				received = 0;
				processingFile = 0;
				// so we can notify to the other DeployUnitRetriever
				notifyDeployUnitDownloaded(filename);
				filename = 0;
			}
			else if (strstr(data, "upload") == data) {
				char* tmp = strstr(data, " ");
				tmp++;
				fdFile = cfs_open(tmp, CFS_READ | CFS_WRITE);
				printf("Uploading file %s\n", tmp);
				processingFile = 1;
			}
			else if (!strcmp(data, "endupload")) {
				cfs_close(fdFile);
				printf("File uploaded (%ld bytes)\n", received);
				received = 0;
				processingFile = 0;
			}
			else if (processingFile) {
				int n = strlen(data);
				int r = decode(data, n, buf);
				received += r;
				cfs_write(fdFile, buf, r);
			}
			else  {
				printf("%s (%lu bytes received)\n", (char*)data, received);
			}
		}
	}

	PROCESS_END();
}
