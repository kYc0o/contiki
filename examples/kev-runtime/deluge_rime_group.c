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

#include "ContainerRoot.h"
#include "JSONModelLoader.h"
#include "Visitor.h"
#include "ModelCompare.h"

#include "rtkev.h"

#include "net/rime/announcement.h"

#include "deluge.h"

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
		.name = "DelugeRimeGroupType",
		.newInstance = newDelugeGroup,
		.start = startDelugeGroup,
		.stop = stopDelugeGroup,
		.update = updateDelugeGroup,
		.send = sendDelugeGroup
};

typedef struct  {
	/* internal values */
	struct announcement a;
	ContainerRoot* lastReceivedModel;
	/* attributes */
	char* fileNameWithModel;
	uint32_t interval;
} DelugeGroup;

#define DELUGE_GROUP_ANNOUNCEMENT 128

#define MAKE_ANNOUN(P,V) (((P)<<8)|(V))
#define GET_PAGES_FROM_ANNOUN(X) ((X)>>8)
#define GET_VERSION_FROM_ANNOUN(X) ((X)&0x00FF)

/* user-defined events */
static process_event_t NEW_AVAILABLE_OA_MODEL; // new over the air model (I just invented the term, :-))
static process_event_t NEW_OA_MODEL_DOWNLOADED; // the model was downloaded

/* this process handle the reception of messages */
PROCESS(delugeGroupP, "delugeGroupProcess");

/* number of pages the synchronized file should have */
static uint8_t nr_pages;

static void
received_announcement(struct announcement *a, const rimeaddr_t *from,
		      uint16_t id, uint16_t value)
{
	if (id != DELUGE_GROUP_ANNOUNCEMENT) return;

	
	uint8_t proposedVersion = GET_VERSION_FROM_ANNOUN(value);
	uint8_t currentVersion = GET_VERSION_FROM_ANNOUN(a->value);
	
	PRINTF("OKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK proposed = %d, current = %d\n", proposedVersion, currentVersion);

	if (currentVersion < proposedVersion) {
		/* some nasty debug message */
		PRINTF("Got announcement from %d.%d, id %d, value %d, proposedVersion %d\n",
 					from->u8[0], from->u8[1], id, value, proposedVersion);

		/* We are now aware of a new version, save it */
		announcement_set_value(a, value);
		
		/* retransmite announcement */
		announcement_bump(a);

		/* create file with as many pages as specified in the new announced value */
		nr_pages = GET_PAGES_FROM_ANNOUN(value);
		
		/* notify about the new model */
		process_post(&delugeGroupP, NEW_AVAILABLE_OA_MODEL, NULL);
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
	static DelugeGroup *instance;
	
	static struct jsonparse_state jsonState;
	static ContainerRoot * newModel;

	PROCESS_BEGIN();
	
	/* keep track of the singleton instance */
	instance = (DelugeGroup*)data;

	/* register new event types */
	NEW_AVAILABLE_OA_MODEL = process_alloc_event();
	NEW_OA_MODEL_DOWNLOADED = process_alloc_event();

	announcement_init();

	/* define new announcement */
	announcement_register(&instance->a,
			DELUGE_GROUP_ANNOUNCEMENT,
			received_announcement);
	
	/* set announcement's initial value*/
	announcement_set_value(&instance->a, MAKE_ANNOUN(0, 1));

	/* set timer for announcements */
	etimer_set(&et, CLOCK_SECOND * instance->interval);

	while (1) {
		/* Listen for announcements every interval seconds. */
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			// let's check if there is some new value for the announcement
			announcement_listen(1);
			etimer_restart(&et);
		}
		else if (ev == NEW_AVAILABLE_OA_MODEL){
			/* receive the new over the air model */
			 
			/* contains the number of pages */
			nr_pages_local = nr_pages;
			
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

			// int fd_read;

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
	DelugeGroup* i = (DelugeGroup*)malloc(sizeof(DelugeGroup));
	// probably it is good idea to zeroed the memory
	return i;
}

static
int startDelugeGroup(void* instance)
{
	DelugeGroup* inst = (DelugeGroup*) instance;

	inst->fileNameWithModel = "new_model-compact.json";
	inst->interval = 3; // in second
	
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
	uint8_t newVersion = GET_VERSION_FROM_ANNOUN(instance->a.value) + 1;
	announcement_set_value(&instance->a, MAKE_ANNOUN(nPages, newVersion));

	// distribute announcement to other motes
	announcement_bump(&instance->a);

	// activar deluge
	if (deluge_disseminate(instance->fileNameWithModel, 1/*newVersion*/, NULL)) {
		PRINTF("ERROR: some problem dissemineting\n");
	}
	else {
		PRINTF("INFO: dissemineting new version of the file with version %d\n", newVersion);
	}
	
	return 0;
}
