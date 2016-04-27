/*
 * deluge_rime_group.c
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
#include "cfs/cfs.h"
#include "json.h"
#include "jsonparse.h"
#include "net/uip.h"
#include "simple-udp.h"


#include "ContainerRoot.h"
#include "JSONModelLoader.h"
#include "Visitor.h"
#include "ModelCompare.h"

#include "rtkev.h"
#include "deluge-udp.h"
#include "deluge_group.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/* forward declaration */
static void* newDelugeGroup(const char* name);
static int startDelugeGroup(void* instance);
static int stopDelugeGroup(void* instance);
static int updateDelugeGroup(void* instance);
static int sendDelugeGroup(void* instance, ContainerRoot* model);

const GroupInterface DelugeRimeGroupInterface = {
		.interfaceType = GroupInstanceInterface,
		.name = DELUGE_GROUP_TYPENAME,
		.newInstance = newDelugeGroup,
		.start = startDelugeGroup,
		.stop = stopDelugeGroup,
		.update = updateDelugeGroup,
		.send = sendDelugeGroup
};

struct ModelInfo {
	uint16_t version;
	uint16_t nr_pages;
};

typedef struct  {
	/* internal values */
	struct ModelInfo info;
	ContainerRoot* lastReceivedModel;
	/* attributes */
	char* fileNameWithModel;
	uint32_t interval;
} DelugeGroup;

/* user-defined events */
static process_event_t NEW_AVAILABLE_OA_MODEL; // new over the air model (I just invented the term, :-))
static process_event_t NEW_OA_MODEL_DOWNLOADED; // the model was downloaded

/* this process handle the reception of messages */
PROCESS(delugeGroupP, "delugeGroupProcess");

/* number of pages the synchronized file should have */
static uint8_t nr_pages;

/* connection used to disseminate a model version, just the version */
static struct simple_udp_connection deluge_group_broadcast;

/* this is disgunting, but there is no way to pass a context to the receiver */
static DelugeGroup *instance;

/**
 * \brief Callback executed when a message is received with information about a proposed Kevoree model
 */
static void
model_version_recv(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
	if (datalen == sizeof(struct ModelInfo)) {
		struct ModelInfo* model_info = (struct ModelInfo*)data;
#if 0
		PRINTF("A new model proposal has been received. Version=%d, Pages=%d. Origin = ", model_info->version, model_info->nr_pages);
		uip_debug_ipaddr_print(sender_addr);
		printf("\n");
#endif
		
		if (model_info->version > instance->info.version) {
			printf("well, here is the new version Version=%d, Pages=%d\n", model_info->version, model_info->nr_pages);
		
			/* We are now aware of a new version, save it */
			instance->info = * model_info;
			
			/* notify about the new model */
			process_post(&delugeGroupP, NEW_AVAILABLE_OA_MODEL, NULL);
		}
	}
}

/**
	A new model was downloaded
 */
static void
modelDownloaded(unsigned version)
{
	process_post(&delugeGroupP, NEW_OA_MODEL_DOWNLOADED, NULL);
}


/* this process handle the reception of messages */
PROCESS_THREAD(delugeGroupP, ev, data)
{

	int fd;
	char* buf;
	uint8_t nr_pages_local;
	static struct etimer et;
	
	static struct jsonparse_state jsonState;
	static ContainerRoot * newModel;
	static uip_ipaddr_t addr;

	PROCESS_BEGIN();
	
	/* keep track of the singleton instance */
	instance = (DelugeGroup*)data;

	/* register new event types */
	NEW_AVAILABLE_OA_MODEL = process_alloc_event();
	NEW_OA_MODEL_DOWNLOADED = process_alloc_event();

	/* initialize model announcement's system */
	simple_udp_register(&deluge_group_broadcast, 34555, NULL, 34555, model_version_recv);

	/* set announcement's initial value*/
	instance->info.version = 0;
	instance->info.nr_pages = 0;
	
	/* set timer for announcements */
	etimer_set(&et, CLOCK_SECOND * instance->interval);

	while (1) {
		/* Listen for announcements every interval seconds. */
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			/* announce my model */
			uip_create_linklocal_allnodes_mcast(&addr);
  			simple_udp_sendto(&deluge_group_broadcast, &instance->info, sizeof(struct ModelInfo), &addr);
  			
			etimer_restart(&et);
		}
		else if (ev == NEW_AVAILABLE_OA_MODEL){
			/* receive the new over the air model */
			 
			/* contains the number of pages */
			nr_pages_local = instance->info.nr_pages;
			
			/* create the file with the required number of pages */
			cfs_remove(instance->fileNameWithModel);
			fd = cfs_open(instance->fileNameWithModel, CFS_WRITE);
			buf = (char*) malloc(S_PAGE);
			memset(buf, '0' , S_PAGE);
			PRINTF("Number of pages is %d\n", nr_pages_local);
			while(nr_pages_local) {
				cfs_seek(fd, 0, CFS_SEEK_END);
				cfs_write(fd, buf, S_PAGE);
				nr_pages_local--;
			}
			free(buf);
			cfs_close(fd);

			/* Deluge-based dissemination */
			if (deluge_disseminate(instance->fileNameWithModel, 0, modelDownloaded)) {
				PRINTF("ERROR: some problem waiting for new version of the file\n");
			}
			else {
				PRINTF("INFO: Waiting for new version of the file \n");
			}

		}
		else if (ev == NEW_OA_MODEL_DOWNLOADED) {
			/* deserialize the model received over the air */
			PRINTF("New model %s received in group with instance %p\n", instance->fileNameWithModel, instance);
			newModel = NULL;
			
			/* TODO: check if the file exists */
			
			/* parse model from json file */
			jsonparse_setup(&jsonState, instance->fileNameWithModel);
			newModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
			cfs_close(jsonState.fd);
			PRINTF("INFO: Deserialization finished in Deluge Group %p\n", newModel);

			/* save a reference to the new model */
			instance->lastReceivedModel = newModel;


			/* Afterwards, just call notifyNewModel */			
			if (newModel != NULL && notifyNewModel(newModel) == PROCESS_ERR_OK) {
				PRINTF("INFO: Model was successfully sent\n");
			}
			else {
				PRINTF("ERROR: The model cannot be loaded!\n");
			}
			
		}
	}

	PROCESS_END();
}


static
void* newDelugeGroup(const char* name)
{
	DelugeGroup* i = (DelugeGroup*)calloc(1, sizeof(DelugeGroup));
	return i;
}

static
int startDelugeGroup(void* instance)
{
	DelugeGroup* inst = (DelugeGroup*) instance;

	inst->fileNameWithModel = "new_model-compact.json";
	inst->interval = 10; // in second
	
	inst->lastReceivedModel = NULL;
	
	process_start(&delugeGroupP, (char*)inst);

	
	return 0;
}

static
int stopDelugeGroup(void* instance)
{
	DelugeGroup* inst = (DelugeGroup*) instance;

	process_exit(&delugeGroupP);

	return 0;
}

static
int updateDelugeGroup(void* instance)
{
	DelugeGroup* inst = (DelugeGroup*) instance;
	return 0;
}

static int
sendDelugeGroup(void* inst, ContainerRoot* model)
{
	// so, we receive a new model to distribute
	DelugeGroup* instance = (DelugeGroup*) inst;

	PRINTF("Sending with DELUGE %p %p\n", instance->lastReceivedModel, model);

	/* we don't want to resend the model as if we were the creators if we just received */
	if (instance->lastReceivedModel == model){
		return 0;
	}

	PRINTF("Sending the model through deluge\n");
	
	// TODO serialize model to a file

	// TODO calculate number of pages in the file
	
	int fd = cfs_open(instance->fileNameWithModel, CFS_READ);
	cfs_offset_t offset = cfs_seek(fd, 0, CFS_SEEK_END);
	uint16_t nPages = offset / S_PAGE;
	cfs_close(fd);
	if (offset % S_PAGE != 0) {
		
		uint16_t mod = 	offset % S_PAGE;
		PRINTF("INFO: +++++++++++++++++++++++++ %d\n", mod);
		mod = S_PAGE - mod;
		char* buf = (char*)malloc(mod);
		memset(buf, ' ', mod);
		fd = cfs_open(instance->fileNameWithModel, CFS_WRITE | CFS_APPEND);
		cfs_seek(fd, 0, CFS_SEEK_END);
		cfs_write(fd, buf, mod);
		cfs_close(fd);
		free(buf);
		nPages++;
	}

	// set my local announcement to the new version
	instance->info.version = instance->info.version + 1;
	instance->info.nr_pages = nPages;
	
	// FIXME distribute announcement to other motes
#if 0
	announcement_bump(&instance->a);
#endif

	// activate deluge
	if (deluge_disseminate(instance->fileNameWithModel, 1, NULL)) {
		PRINTF("ERROR: some problem dissemineting\n");
	}
	else {
		PRINTF("INFO: dissemineting new version of the file with version %d\n", newVersion);
	}
	
	return 0;
}
