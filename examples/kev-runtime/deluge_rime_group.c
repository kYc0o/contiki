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

/* forward declaration */
static void* newDelugeGroup(const char* name);
static int startDelugeGroup(void* instance);
static int stopDelugeGroup(void* instance);
static int updateDelugeGroup(void* instance);
static int sendDelugeGroup(void* instance, ContainerRoot* model);

ContainerRoot *intiModel = NULL;


int fd_read;
uint32_t length;
char *jsonModel;

const GroupInterface ShellGroupInterface = {
		.name = "DelugeRimeGroupType",
		.newInstance = newDelugeGroup,
		.start = startDelugeGroup,
		.stop = stopDelugeGroup,
		.update = updateDelugeGroup,
		.send = sendDelugeGroup
};

typedef struct  {
    struct announcement announ;
	char* fileNameWithModel;
} DelugeGroup;

#define DELUGE_GROUP_ANNOUNCEMENT 128

#define MAKE_ANNOUN(P,V) (((P)<<8)|(V))
#define GET_PAGES_FROM_ANNOUN(X) ((X)>>8)
#define GET_VERSION_FROM_ANNOUN(X) ((X)|0x00FF)


static uint16_t currenValue;

static void
received_announcement(struct announcement *a, const rimeaddr_t *from,
		      uint16_t id, uint16_t value)
{
	if (id != DELUGE_GROUP_ANNOUNCEMENT) return;

	
	uint8_t proposedVersion = GET_VERSION_FROM_ANNOUN(value);
	uint8_t currentVersion = GET_VERSION_FROM_ANNOUN(currenValue);

	if (currentVersion < proposedVersion) {
		/* We have a new version, save it */
		announcement_set_value(a, value);

		/* create file with as many pages as specified in the new announced value */
		uint8_t nr_pages = GET_PAGES_FROM_ANNOUN(value);

		// TODO : create the file

		// retransmite
		announcement_bump(a);

		// some nasty debug message
		printf("Got announcement from %d.%d, id %d, value %d, our new value is %d\n",
 					from->u8[0], from->u8[1], id, value, value + 1);

		// TODO: deluge stuff	
	}
	else {
		/* keep the all previous value because it is newer */
		announcement_set_value(a, currenValue);
	}
}

/* this process handle the reception of messages */
PROCESS(delugeGroupP, "delugeGroupProcess");
PROCESS_THREAD(delugeGroupP, ev, data)
{
	static DelugeGroup *instance;

	PROCESS_BEGIN();

	instance = (DelugeGroup*)data;

	announcement_register(&instance->announ,
			DELUGE_GROUP_ANNOUNCEMENT,
			received_announcement);

	announcement_set_value(&instance->announ, MAKE_ANNOUN(1, 1));

	printf("Ok, here I have my instance %p\n", (struct DelugeGroup*)data);

	/* define new event */
	NEW_MODEL_IN_JSON = process_alloc_event();
	while (1) {
		static struct etimer et;
		/* it runs forever, waiting for some update to the model */
		PROCESS_WAIT_EVENT();
		/* Listen for announcements every 5 seconds. */
		etimer_set(&et, CLOCK_SECOND * 5);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		announcement_listen(1);
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

	printf("INFO: Sending instance %s, at %p\n", inst->fileNameWithModel, inst);

	process_start(&delugeGroupP, inst);

	printf("INFO: Sent instance %s, at %p\n", inst->fileNameWithModel, inst);

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
	
	// TODO serialize model to a file

	// TODO calculate number of pages of the file

	// TODO set my local announcement to the new version

	// distribute announcement with announcement_bump()
	announcement_bump(&inst->announ);
	
	return 0;
}
