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
#include "net/netstack.h"

#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"

#include "simple-udp.h"
#include "net/netstack.h"


#include "jsonparse.h"

#include "rtkev.h"
#include "shell_group.h"
#include "deluge_group.h"
#include "udpComponent.h"
#include "ShellBasedDeployUnitRetriever.h"
#include "NaiveUDPBasedDeployUnitRetriever.h"

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
#define DEPLOY_UNIT_RETRIEVER_STRATEGY NAIVE_UDP_BASED_RETRIEVER

#define FLASH_FORMATTED "formated.txt"
#define MAGIC_NUMBER 0xFFEEDDCC

/* used to find out if the flash is formatted */
static int
isFormatted()
{
	int r = 0;
	int fd =  cfs_open(FLASH_FORMATTED, CFS_READ);
	uint32_t x = 0;
	if (fd < 0) return r;
	if ((cfs_read(fd, &x, sizeof(uint32_t)) == sizeof(uint32_t)) && (x == MAGIC_NUMBER)) {
		r = 1;
	}
	cfs_close(fd);
	return r;
}

/* used to write down the fact hat the flash is formatted */
static void
mark_as_formatted()
{
	int fd =  cfs_open(FLASH_FORMATTED, CFS_WRITE);
	uint32_t x = MAGIC_NUMBER;
	cfs_write(fd, &x, sizeof(uint32_t));
	cfs_close(fd);
}

const char* new_model_const = "{\"eClass\": \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\": \"CtHbJw37\",\"nodes\": [{\"eClass\": \"org.kevoree.ContainerNode\",\"name\": \"node0\",\"started\": \"false\",\"metaData\": \"{\\\"x\\\":296,\\\"y\\\":167}\",\"typeDefinition\": [\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\": [],\"host\": [],\"groups\": [\"groups[group0]\"],\"dictionary\": [],\"fragmentDictionary\": [],\"components\": [{\"eClass\": \"org.kevoree.ComponentInstance\",\"name\": \"comp457\",\"started\": \"true\",\"metaData\": \"{\\\"x\\\":408,\\\"y\\\":239}\",\"typeDefinition\": [\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\": [],\"dictionary\": [{\"eClass\": \"org.kevoree.Dictionary\",\"generated_KMF_ID\": \"0.68263587262481451424775426644\",\"values\": [{\"eClass\": \"org.kevoree.DictionaryValue\",\"name\": \"time\",\"value\": \"6\"}]}],\"fragmentDictionary\": [],\"provided\": [],\"required\": []}],\"networkInformation\": [{\"eClass\": \"org.kevoree.NetworkInfo\",\"name\": \"ip\",\"values\": [{\"eClass\": \"org.kevoree.NetworkProperty\",\"name\": \"local\",\"value\": \"aaaa::0:0:5\"},{\"eClass\": \"org.kevoree.NetworkProperty\",\"name\": \"front\",\"value\": \"m3-XX.lille.iotlab.info\"}]}]}],\"typeDefinitions\": [{\"eClass\": \"org.kevoree.NodeType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"ContikiNode\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"9o86ZdvQ\",\"attributes\": []}]},{\"eClass\": \"org.kevoree.GroupType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"CoAPGroup\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"hytCmvXU\",\"attributes\": [{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"path\",\"state\": \"false\",\"datatype\": \"string\",\"defaultValue\": \"\",\"genericTypes\": []},{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"port\",\"state\": \"false\",\"datatype\": \"number\",\"defaultValue\": \"\",\"genericTypes\": []},{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"proxy_port\",\"state\": \"false\",\"datatype\": \"int\",\"defaultValue\": \"20000\",\"genericTypes\": []}]}]},{\"eClass\": \"org.kevoree.ComponentType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"hello_world\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"3dddTFpd\",\"attributes\": [{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"time\",\"state\": \"false\",\"datatype\": \"int\",\"defaultValue\": \"5\",\"genericTypes\": []}]}],\"required\": [],\"provided\": []}],\"repositories\": [{\"eClass\": \"org.kevoree.Repository\",\"url\": \"coap://[bbbb::1]:5683/libraries\"}],\"dataTypes\": [],\"libraries\": [{\"eClass\": \"org.kevoree.TypeLibrary\",\"name\": \"ContikiLib\",\"subTypes\": [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\": \"org.kevoree.TypeLibrary\",\"name\": \"Default\",\"subTypes\": [\"typeDefinitions[hello_world/0.0.1]\"]}],\"hubs\": [],\"mBindings\": [],\"deployUnits\": [{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"\",\"name\": \"kevoree-group-coap\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []},{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"org.kevoree.library.c\",\"name\": \"kevoree-contiki-node\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []},{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"kev_contiki\",\"name\": \"hello_world\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []}],\"nodeNetworks\": [],\"groups\": [{\"eClass\": \"org.kevoree.Group\",\"name\": \"group0\",\"started\": \"false\",\"metaData\": \"{\\\"x\\\":504,\\\"y\\\":259}\",\"typeDefinition\": [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"subNodes\": [\"nodes[node0]\"],\"dictionary\": [],\"fragmentDictionary\": [{\"eClass\": \"org.kevoree.FragmentDictionary\",\"name\": \"contiki-node\",\"generated_KMF_ID\": \"QoMNUckL\",\"values\": []}]}]}";

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


static char*
get_local_address(void)
{
	int i;
	uint8_t state;
	char* r = (char*)malloc(sizeof(char)*10);
	
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
				(state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			uip_ipaddr_t *ip_addr = &uip_ds6_if.addr_list[i].ipaddr;
			uint16_t a = (ip_addr->u8[14] << 8) + ip_addr->u8[15];
			sprintf(r, "%x", a);
			return r; // please, just one address, I don't care if the interface has more than one address
		}
	}
	
	return  NULL;
}

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
	
	NETSTACK_MAC.off(1);

	/* definitively we want to dynamically load modules */
	elfloader_init();
	
	/* ensure that the flash is formatted */
	if (!isFormatted()) {
		printf("Formating the flash ... this may take a while, so go for a coffee ... around three minutes\n");
		int r_format = cfs_coffee_format();
		printf("Formatting result is %d\n", r_format);
		if (r_format) {
			printf("Some error formatting ... Kevoree cannot run\n");
			PROCESS_EXIT();
		}
		else {
			mark_as_formatted();
		}
	}
	
	/* create model from constant */
	fdFile = cfs_open("new_model-compact.json", CFS_WRITE);
	cfs_write(fdFile, new_model_const, strlen(new_model_const));
	cfs_close(fdFile);
	printf("File with new model was created 000\n");
	
	/* get local address */
	char* local_address = get_local_address();
	
	/* initialize Kevoree Runtime */
	result = 1;
#if DEPLOY_UNIT_RETRIEVER_STRATEGY == NAIVE_UDP_BASED_RETRIEVER
	result = initKevRuntime(local_address, &naive_udp_retriever);
#elif DEPLOY_UNIT_RETRIEVER_STRATEGY == SMART_UDP_BASED_RETRIEVER

#elif DEPLOY_UNIT_RETRIEVER_STRATEGY == SHELL_BASED_RETRIEVER
	result = initKevRuntime(local_address, &shellBasedRetriever);
#endif

	/* free some memory */
	free(local_address);
	
	/* is everything ok? */
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
				printf("Unknown commnad => %s\n", (char*)data);
			}
		}
	}

	PROCESS_END();
}
