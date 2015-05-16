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
	void* mierda;
	/* internal values */
	ContainerRoot* lastReceivedModel;
	/* attributes */
	char* fileNameWithModel;
	uint32_t interval;
} DelugeGroup;

#define DELUGE_GROUP_ANNOUNCEMENT 128

#define MAKE_ANNOUN(P,V) (((P)<<8)|(V))
#define GET_PAGES_FROM_ANNOUN(X) ((X)>>8)
#define GET_VERSION_FROM_ANNOUN(X) ((X)&0x00FF)


static uint16_t currentValue;


static struct announcement announ_secret;

/* user-defined events */
static process_event_t NEW_AVAILABLE_OA_MODEL; // new over the air model (I just invented the term, :-))
static process_event_t NEW_OA_MODEL_DOWNLOADED; // the model was downloaded

/* this process handle the reception of messages */
PROCESS(delugeGroupP, "delugeGroupProcess");

static uint8_t nr_pages;

static void
received_announcement(struct announcement *a, const rimeaddr_t *from,
		      uint16_t id, uint16_t value)
{
	if (id != DELUGE_GROUP_ANNOUNCEMENT) return;

	
	uint8_t proposedVersion = GET_VERSION_FROM_ANNOUN(value);
	uint8_t currentVersion = GET_VERSION_FROM_ANNOUN(currentValue);
	
	//printf("OKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK proposed = %d, current = %d\n", proposedVersion, currentVersion);

	if (currentVersion < proposedVersion) {
		/* some nasty debug message */
		printf("Got announcement from %d.%d, id %d, value %d, proposedVersion %d\n",
 					from->u8[0], from->u8[1], id, value, proposedVersion);

		/* We are now aware of a new version, save it */
		announcement_set_value(a, value);
		currentValue = value;
		
		/* retransmite announcement */
		announcement_bump(a);

		/* create file with as many pages as specified in the new announced value */
		nr_pages = GET_PAGES_FROM_ANNOUN(value);
		
		/* notify about the new model */
		process_post(&delugeGroupP, NEW_AVAILABLE_OA_MODEL, &nr_pages);
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

	uint16_t nr_pages_local;
	int fd;
	char* buf;

	PROCESS_BEGIN();
	
	static struct etimer et;
	static DelugeGroup *instance;
	
	static struct jsonparse_state jsonState;
	static ContainerRoot * newModel;

	/* keep track of the singleton instance */
	instance = (DelugeGroup*)data;

	/* register new event types */
	NEW_AVAILABLE_OA_MODEL = process_alloc_event();
	NEW_OA_MODEL_DOWNLOADED = process_alloc_event();

	announcement_init();

	/* define new announcement */
	announcement_register(&announ_secret,
			DELUGE_GROUP_ANNOUNCEMENT,
			received_announcement);
	
	/* set announcement's initial value*/
	currentValue = MAKE_ANNOUN(0, 1);
	announcement_set_value(&announ_secret, currentValue);

	printf("Ok, at %p I have my instance DelugeGroup with announcament at %p and callback at %p\n", instance, &announ_secret, 
			&received_announcement);

	/* set timer for announcements */
	etimer_set(&et, CLOCK_SECOND * instance->interval);

	while (1) {
		/* Listen for announcements every interval seconds. */
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			//printf("================================================>>>>> Doing ev == PROCESS_EVENT_TIMER in %s\n", __FILE__);
			// let's check if there is some new value for the announcement
			announcement_listen(1);
			etimer_restart(&et);
		}
		else if (ev == NEW_AVAILABLE_OA_MODEL){
			/* receive the new over the air model */
			uint8_t * p_nr_pages = (uint8_t *)data; 
			/* contains the number of pages */
			nr_pages_local = *p_nr_pages;
			
			/* create the file with the required number of pages */
			cfs_remove(instance->fileNameWithModel);
			fd = cfs_open(instance->fileNameWithModel, CFS_WRITE);
			buf = (char*) malloc(S_PAGE);
			memset(buf, '0' , S_PAGE);
			printf("Number of pages is %d\n", nr_pages_local);
			while(nr_pages_local) {
				cfs_seek(fd, 0, CFS_SEEK_END);
				cfs_write(fd, buf, S_PAGE);
				nr_pages_local--;
			}
			free(buf);
			cfs_close(fd);

			/* Deluge-based dissemination */
			if (deluge_disseminate(instance->fileNameWithModel, 0/*currentVersion*/, modelDownloaded))
				printf("ERROR: some problem waiting for new version of the file\n");
			else
				printf("INFO: Waiting for new version of the file \n");

		}
		else if (ev == NEW_OA_MODEL_DOWNLOADED) {
			/* deserialize the model received over the air */

			// int fd_read;

			printf("New model %s received in group with instance %p\n", instance->fileNameWithModel, instance);
			newModel = NULL;
			//if((fd_read = cfs_open(instance->fileNameWithModel, CFS_READ)) != -1) {

			//	cfs_close(fd_read);

			jsonparse_setup(&jsonState, instance->fileNameWithModel);
			newModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
			cfs_close(jsonState.fd);
			printf("INFO: Deserialization finished in Deluge Group %p\n", newModel);
			//instance->flag = 1;
			//}

			/* save a reference to the new model */
			instance->lastReceivedModel = newModel;


			/* Afterwards, just call notifyNewModel */			
			if (newModel != NULL && notifyNewModel(newModel)== PROCESS_ERR_OK)
				printf("INFO: Model was successfully sent\n");
			else
				printf("ERROR: The model cannot be loaded!\n");
			
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
sendDelugeGroup(void* instance, ContainerRoot* model)
{
	// so, we receive a new model to distribute
	DelugeGroup* inst = (DelugeGroup*) instance;

	printf("Sending with DELUGE %p %p\n", inst->lastReceivedModel, model);

	if (inst->lastReceivedModel == model)
		return 0;

	printf("Sending the model through deluge\n");
	
	// TODO serialize model to a file

	// TODO calculate number of pages in the file
	
	int fd = cfs_open(inst->fileNameWithModel, CFS_READ);
	cfs_offset_t offset = cfs_seek(fd, 0, CFS_SEEK_END);
	uint16_t nPages = offset / S_PAGE;
	cfs_close(fd);
	if (offset % S_PAGE != 0) {
		
		uint16_t mod = 	offset % S_PAGE;
		printf("INFO: +++++++++++++++++++++++++ %d\n", mod);
		mod = S_PAGE - mod;
		char* buf = (char*)malloc(mod);
		memset(buf, ' ', mod);
		fd = cfs_open(inst->fileNameWithModel, CFS_WRITE | CFS_APPEND);
		cfs_seek(fd, 0, CFS_SEEK_END);
		cfs_write(fd, buf, mod);
		cfs_close(fd);
		free(buf);
		nPages++;
	}

	// set my local announcement to the new version
	uint8_t newVersion = GET_VERSION_FROM_ANNOUN(currentValue) + 1;
	currentValue = MAKE_ANNOUN(nPages, newVersion);
	announcement_set_value(&announ_secret, currentValue);

	// distribute announcement to other motes
	announcement_bump(&announ_secret);

	// activar deluge
	if (deluge_disseminate(inst->fileNameWithModel, 1/*newVersion*/, NULL)) {
		printf("ERROR: some problem dissemineting\n");
	}
	else printf("INFO: dissemineting new version of the file with version %d\n", newVersion);

	return 0;
}
